/*
 * NetworkInterface.cpp
 *
 *  Created on: Jun 27, 2024
 *      Author: mpeterson
 */

#include <NetworkInterface.h>

ETH_HandleTypeDef* NetworkInterface::ethHandle{nullptr};
ETH_TxPacketConfigTypeDef* NetworkInterface::ethTxConfig{nullptr};
ETH_BufferTypeDef* NetworkInterface::txBuffer {nullptr};
size_t NetworkInterface::txBufferCount {0};
BinaryLock NetworkInterface::txLock {"NetIFTXLock"};

extern "C"
{

int32_t ETH_PHY_IO_Init(void);
int32_t ETH_PHY_IO_DeInit (void);
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal);
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal);
int32_t ETH_PHY_IO_GetTick(void);

lan8742_Object_t LAN8742;
lan8742_IOCtx_t  LAN8742_IOCtx = {ETH_PHY_IO_Init,
                               ETH_PHY_IO_DeInit,
                               ETH_PHY_IO_WriteReg,
                               ETH_PHY_IO_ReadReg,
                               ETH_PHY_IO_GetTick};

extern ETH_HandleTypeDef heth;

/*******************************************************************************
                       PHI IO Functions
*******************************************************************************/
/**
  * @brief  Initializes the MDIO interface GPIO and clocks.
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
int32_t ETH_PHY_IO_Init(void)
{
  /* We assume that MDIO GPIO configuration is already done
     in the ETH_MspInit() else it should be done here
  */

  /* Configure the MDIO Clock */
  HAL_ETH_SetMDIOClockRange(&heth);

  return 0;
}

/**
  * @brief  De-Initializes the MDIO interface .
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
int32_t ETH_PHY_IO_DeInit (void)
{
  return 0;
}

/**
  * @brief  Read a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  pRegVal: pointer to hold the register value
  * @retval 0 if OK -1 if Error
  */
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal)
{
  if(HAL_ETH_ReadPHYRegister(&heth, DevAddr, RegAddr, pRegVal) != HAL_OK)
  {
    return -1;
  }

  return 0;
}

/**
  * @brief  Write a value to a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  RegVal: Value to be written
  * @retval 0 if OK -1 if Error
  */
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal)
{
  if(HAL_ETH_WritePHYRegister(&heth, DevAddr, RegAddr, RegVal) != HAL_OK)
  {
    return -1;
  }

  return 0;
}

/**
  * @brief  Get the time in millisecons used for internal PHY driver process.
  * @retval Time value
  */
int32_t ETH_PHY_IO_GetTick(void)
{
  return osKernelGetTickCount();
}

/**
  * @brief
  * @retval None
  */
void ethernet_link_check_state(struct netif *netif)
{
  ETH_MACConfigTypeDef MACConf = {0};
  int32_t PHYLinkState = 0U;
  uint32_t linkchanged = 0U, speed = 0U, duplex =0U;

  PHYLinkState = LAN8742_GetLinkState(&LAN8742);

  if(netif_is_link_up(netif) && (PHYLinkState <= LAN8742_STATUS_LINK_DOWN))
  {
    HAL_ETH_Stop(&heth);
    netif_set_down(netif);
    netif_set_link_down(netif);
  }
  else if(!netif_is_link_up(netif) && (PHYLinkState > LAN8742_STATUS_LINK_DOWN))
  {
    switch (PHYLinkState)
    {
    case LAN8742_STATUS_100MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      linkchanged = 1;
      break;
    case LAN8742_STATUS_100MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      linkchanged = 1;
      break;
    case LAN8742_STATUS_10MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      linkchanged = 1;
      break;
    case LAN8742_STATUS_10MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      linkchanged = 1;
      break;
    default:
      break;
    }

    if(linkchanged)
    {
      /* Get MAC Config MAC */
      HAL_ETH_GetMACConfig(&heth, &MACConf);
      MACConf.DuplexMode = duplex;
      MACConf.Speed = speed;
      HAL_ETH_SetMACConfig(&heth, &MACConf);
      if (HAL_ETH_Start_IT(&heth) != HAL_OK)
    	  printf("big fail");
      netif_set_up(netif);
      netif_set_link_up(netif);
    }
  }
}

}

err_t NetworkInterface::s_EthernetInterfaceInitialize(struct netif *netif)
{
	netif->name[0] = 'm';
	netif->name[1] = 'p';

	netif->output = etharp_output;
	netif->linkoutput = NetworkInterface::s_LowLevelOutput;
	return ERR_OK;
}

err_t NetworkInterface::s_EthernetInterfaceInput(struct pbuf* p, struct netif* inp)
{
	return ERR_OK;
}

err_t NetworkInterface::s_LowLevelOutput(struct netif *netif, struct pbuf *p)
{
	static constexpr ETH_BufferTypeDef EMPTY_BUFFER{};
	std::fill(txBuffer, txBuffer + txBufferCount, EMPTY_BUFFER);

	size_t i = 0;
	for (struct pbuf* q = p; q != nullptr; q = q->next)
	{
		auto& thisBuffer = txBuffer[i];
		auto& prevBuffer = i > 0 ? txBuffer[i - 1] : thisBuffer;

		if (i >= txBufferCount)
			return ERR_IF;

		thisBuffer.buffer = reinterpret_cast<uint8_t*>(q->payload);
		thisBuffer.len = q->len;
		prevBuffer.next = &thisBuffer;

		if (q->next == nullptr)
			thisBuffer.next = nullptr;

		i++;
	}

	ethTxConfig->Length = p->tot_len;
	ethTxConfig->TxBuffer = txBuffer;
	ethTxConfig->pData = p;

	pbuf_ref(p);

	err_t errval = ERR_OK;
	do
	{
		if (HAL_ETH_Transmit_IT(ethHandle, ethTxConfig) == HAL_OK)
			errval = ERR_OK;
		else {
			if (HAL_ETH_GetError(ethHandle) & HAL_ETH_ERROR_BUSY)
			{
				txLock.take();
				HAL_ETH_ReleaseTxPacket(ethHandle);
				errval = ERR_BUF;
			}
			else
			{
				pbuf_free(p);
				errval = ERR_IF;
			}
		}
	} while (errval == ERR_BUF);

	return errval;
}

void NetworkInterface::s_EthernetLinkStatusUpdated(struct netif* net)
{

}

void NetworkInterface::initialize()
{
	/* Set PHY IO functions */
	LAN8742_RegisterBusIO(&LAN8742, &LAN8742_IOCtx);

	/* Initialize the LAN8742 ETH PHY */
	LAN8742_Init(&LAN8742);

	ethernet_link_check_state(&gnetif);


	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;

	IP4_ADDR(&ipaddr, 192, 168, 50, 123);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 50, 1);

//	ip_addr_set_zero_ip4(&ipaddr);
//	ip_addr_set_zero_ip4(&netmask);
//	ip_addr_set_zero_ip4(&gw);

	/* add the network interface */
//	netif_add(&gnetif, &ipaddr, &netmask, &gw, nullptr, &NetworkInterface::s_EthernetInterfaceInitialize, &NetworkInterface::s_EthernetInterfaceInput);
//	netif_add(&gnetif, &ipaddr, &netmask, &gw, nullptr, &NetworkInterface::s_EthernetInterfaceInitialize, &tcpip_input);
	netif_add(&gnetif, &ipaddr, &netmask, &gw, nullptr, &NetworkInterface::s_EthernetInterfaceInitialize, &ethernet_input);

	/*  Registers the default network interface */
	netif_set_default (&gnetif);

	gnetif.hwaddr_len = 6;
	std::copy(ethHandle->Init.MACAddr, ethHandle->Init.MACAddr+6, gnetif.hwaddr);
	gnetif.mtu = ETH_MAX_PAYLOAD;
	gnetif.flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

	NetworkInterface::s_EthernetLinkStatusUpdated(&gnetif);

    netif_set_up(&gnetif);
    netif_set_link_up(&gnetif);
}

bool NetworkInterface::isLinkUp()
{
	return netif_is_link_up(&gnetif);
}

bool NetworkInterface::isUp()
{
	return netif_is_up(&gnetif);
}

void NetworkInterface::txComplete()
{
	txLock.give();
}
