/*
 * Ethernet.h
 *
 *  Created on: Jun 27, 2024
 *      Author: mpeterson
 */

#ifndef INC_ETHERNET_ETHERNET_H_
#define INC_ETHERNET_ETHERNET_H_

#include <cstring>

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"

#include "stm32h563xx.h"
#include "stm32h5xx_hal.h"
#include "stm32h5xx_hal_eth.h"

#include <BinaryLock.h>
#include <Debug.h>
#include <NetworkInterface.h>
#include <Timer.h>
#include <Thread.h>
#include <QueueT.h>

extern "C"
{
extern ETH_HandleTypeDef heth;
extern ETH_TxPacketConfigTypeDef TxConfig;
extern ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
extern ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
}

class EthernetThread : public Thread
{
public:
	static EthernetThread& getInstance() { return *instance; }

	enum InterfaceIDs {
		DHCP_Fine_Periodic,
		DHCP_Coarse_Periodic,
		Ethernet_RX_Callback,
		IT_Error,
		Handle_IT_Error
	};
	struct InterfaceMessage {
		InterfaceIDs id;
	};

	EthernetThread(ETH_HandleTypeDef* heth, ETH_TxPacketConfigTypeDef* txConfig,
			ETH_DMADescTypeDef* rxDescriptors, size_t rxCount,
			ETH_DMADescTypeDef* txDescriptors, size_t txCount)
	: Thread("Ethernet", osPriorityNormal, 1024 * 2),
	  ethHandle{heth},
	  dmaRxBuffer{rxDescriptors},
	  dmaRxBufferCount{rxCount},
	  dmaTxBuffer{txDescriptors},
	  dmaTxBufferCount{txCount},
	  networkInterface{heth, txConfig, txCount}
	{
		EthernetThread::instance = this;
	}

	void txComplete();
	void postInterfaceMessage(InterfaceMessage& message);
	void postInterfaceMessage(InterfaceIDs id);

protected:
	virtual void main() override;
private:
	static EthernetThread* instance;

	void dhcpProcess();
	void readInputData();

	ETH_HandleTypeDef* ethHandle;
	ETH_DMADescTypeDef* dmaRxBuffer;
	size_t dmaRxBufferCount;
	ETH_DMADescTypeDef* dmaTxBuffer;
	size_t dmaTxBufferCount;

	NetworkInterface networkInterface;
	Queue<InterfaceMessage> queue { 10 };

	Timer dhcpFineTimer { osTimerType_t::osTimerPeriodic, [this]() { postInterfaceMessage(InterfaceIDs::DHCP_Fine_Periodic); } };
	Timer dhcpCourseTimer { osTimerType_t::osTimerPeriodic, [this]() { postInterfaceMessage(InterfaceIDs::DHCP_Coarse_Periodic); } };
	Timer errorPostTimer { osTimerType_t::osTimerOnce, [this]() { postInterfaceMessage(InterfaceIDs::Handle_IT_Error); } };
	enum DHCPState {
		Start,
		WaitAddress,
		Live
	};
	DHCPState dhcpState { Start };
};


#endif /* INC_ETHERNET_ETHERNET_H_ */
