/*
 * Ethernet.cpp
 *
 *  Created on: Jun 27, 2024
 *      Author: mpeterson
 */


#include "EthernetThread.h"

EthernetThread* EthernetThread::instance {nullptr};

LWIP_MEMPOOL_DECLARE(RX_POOL, ETH_RX_BUFFER_CNT, sizeof(RxBuff_t), "Zero-copy RX PBUF pool");
__attribute__((section(".Rx_PoolSection"))) extern u8_t memp_memory_RX_POOL_base[];

extern "C" void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *eth)
{
	EthernetThread::getInstance().postInterfaceMessage(EthernetThread::InterfaceIDs::Ethernet_RX_Callback);
}

extern "C" void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *eth)
{
	EthernetThread::getInstance().txComplete();
}

extern "C" void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *eth)
{
	EthernetThread::getInstance().postInterfaceMessage(EthernetThread::InterfaceIDs::IT_Error);
}

extern "C" void pbuf_free_custom(struct pbuf *p)
{
  struct pbuf_custom* custom_pbuf = (struct pbuf_custom*)p;
  LWIP_MEMPOOL_FREE(RX_POOL, custom_pbuf);
}

extern "C" void HAL_ETH_RxAllocateCallback(uint8_t **buff)
{
  struct pbuf_custom *p = reinterpret_cast<struct pbuf_custom*>(LWIP_MEMPOOL_ALLOC(RX_POOL));
  if (p)
  {
    /* Get the buff from the struct pbuf address. */
    *buff = (uint8_t *)p + offsetof(RxBuff_t, buff);
    p->custom_free_function = pbuf_free_custom;
    /* Initialize the struct pbuf.
    * This must be performed whenever a buffer's allocated because it may be
    * changed by lwIP or the app, e.g., pbuf_free decrements ref. */
    pbuf_alloced_custom(PBUF_RAW, 0, PBUF_REF, p, *buff, ETH_RX_BUFFER_SIZE);
  }
  else
  {
//    RxAllocStatus = RX_ALLOC_ERROR;
    *buff = NULL;
  }
}

extern "C" void HAL_ETH_RxLinkCallback(void **pStart, void **pEnd, uint8_t *buff, uint16_t Length) {
	struct pbuf **ppStart = (struct pbuf**) pStart;
	struct pbuf **ppEnd = (struct pbuf**) pEnd;
	struct pbuf *p = NULL;

	if (buff >= memp_memory_RX_POOL_base && buff < memp_memory_RX_POOL_base + sizeof(memp_memory_RX_POOL_base))
	{
		/* Get the struct pbuf from the buff address. */
		p = (struct pbuf*) (buff - offsetof(RxBuff_t, buff));
		p->next = NULL;
		p->tot_len = 0;
		p->len = Length;

		/* Chain the buffer. */
		if (!*ppStart) {
			/* The first buffer of the packet. */
			*ppStart = p;
		} else {
			/* Chain the buffer to the end of the packet. */
			(*ppEnd)->next = p;
		}
		*ppEnd = p;

		/* Update the total length of all the buffers of the chain. Each pbuf in the chain should have its tot_len
		 * set to its own length, plus the length of all the following pbufs in the chain. */
		for (p = *ppStart; p != NULL; p = p->next) {
			p->tot_len += Length;
		}
	}
	else
	{
		Debug::getInstance().send("HAL_ETH_RxLinkCallback Fail");
	}
}

extern "C" void HAL_ETH_TxFreeCallback(uint32_t * buff)
{
  pbuf_free((struct pbuf *)buff);
}

void EthernetThread::main()
{
//	tcpip_init(NULL, NULL);
	lwip_init();
	LWIP_MEMPOOL_INIT(RX_POOL);

	networkInterface.initialize();
	dhcpFineTimer.start(500);
	dhcpCourseTimer.start(60000);

	EthernetThread::InterfaceMessage message{};
	while (true)
	{
		queue.pop(message);
		switch(message.id)
		{
		case DHCP_Coarse_Periodic:
			dhcp_coarse_tmr();
			break;
		case DHCP_Fine_Periodic:
			dhcpProcess();
			break;
		case Ethernet_RX_Callback:
			readInputData();
			break;
		case IT_Error:
			errorPostTimer.start(1);
			readInputData();
			break;
		case Handle_IT_Error: {
			auto error = HAL_ETH_GetDMAError(ethHandle);
			if ((error & ETH_DMACSR_RBU) == ETH_DMACSR_RBU)
				ethHandle->Instance->DMACRDTPR = 0;
			ethHandle->Instance->DMACSR = error;
			printf("ETH ERROR: %08X\r\n", error);
			break;
		}
		}
	}
}

void EthernetThread::dhcpProcess() {
	dhcp_fine_tmr();

	switch (dhcpState) {
	case DHCPState::Start:
		if (networkInterface.isLinkUp()) {
	      ip_addr_set_zero_ip4(&networkInterface.getHandle()->ip_addr);
	      ip_addr_set_zero_ip4(&networkInterface.getHandle()->netmask);
	      ip_addr_set_zero_ip4(&networkInterface.getHandle()->gw);
	      if (dhcp_start(networkInterface.getHandle()) == ERR_OK)
	    	  dhcpState = DHCPState::WaitAddress;
		}
		break;
	case DHCPState::WaitAddress:
		if (dhcp_supplied_address(networkInterface.getHandle()))
			dhcpState = DHCPState::Live;
		break;
	case DHCPState::Live:
		if (!networkInterface.isLinkUp()){
			dhcpState = DHCPState::Start;
		}
		break;
	default:
		dhcpState = Start;
		break;
	}
}

void EthernetThread::readInputData()
{
	HAL_StatusTypeDef halStatus;
	struct pbuf* pBuf = nullptr;
	do
	{
		halStatus = HAL_ETH_ReadData(ethHandle, (void **)&pBuf);
		if (pBuf && networkInterface.getHandle()->input(pBuf, networkInterface.getHandle()) != ERR_OK)
			pbuf_free(pBuf);
	} while (halStatus == HAL_OK);
}

void EthernetThread::txComplete()
{
	networkInterface.txComplete();
}

void EthernetThread::postInterfaceMessage(EthernetThread::InterfaceMessage& message)
{
	queue.push(message);
}

void EthernetThread::postInterfaceMessage(EthernetThread::InterfaceIDs message)
{
	InterfaceMessage m;
	m.id = message;
	queue.push(m);
}

