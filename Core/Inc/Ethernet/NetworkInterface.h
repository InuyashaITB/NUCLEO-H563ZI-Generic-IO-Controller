/*
 * NetworkInterface.h
 *
 *  Created on: Jun 27, 2024
 *      Author: mpeterson
 */

#ifndef INC_ETHERNET_NETWORKINTERFACE_H_
#define INC_ETHERNET_NETWORKINTERFACE_H_

#include <algorithm>

#include "lan8742.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/ip_addr.h"
#include "lwip/etharp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"

#include "netif/ethernet.h"
#include "netif/etharp.h"

#include "stm32h563xx.h"
#include "stm32h5xx_hal.h"
#include "stm32h5xx_hal_eth.h"

#include <BinaryLock.h>

#define ETH_RX_BUFFER_SIZE            1000U
#define ETH_RX_BUFFER_CNT             12U
#define ETH_TX_BUFFER_MAX             ((ETH_TX_DESC_CNT) * 2U)

struct RxBuff_t
{
  struct pbuf_custom pbuf_custom;
  uint8_t buff[(ETH_RX_BUFFER_SIZE + 31) & ~31] __ALIGNED(32);
};

class NetworkInterface
{
public:
	NetworkInterface(ETH_HandleTypeDef* heth, ETH_TxPacketConfigTypeDef* ethTxConfig, size_t txCount)
	{
		NetworkInterface::ethHandle = heth;
		NetworkInterface::ethTxConfig = ethTxConfig;
		NetworkInterface::txBuffer = new ETH_BufferTypeDef[txBufferCount = txCount];
	}

	void initialize();
	bool isLinkUp();
	bool isUp();

	void txComplete();

	struct netif* getHandle() { return &gnetif; }
protected:
private:
	static err_t s_EthernetInterfaceInitialize(struct netif* netif);
	static err_t s_EthernetInterfaceInput(struct pbuf* p, struct netif* inp);
	static void s_EthernetLinkStatusUpdated(struct netif* net);
	static err_t s_LowLevelOutput(struct netif *netif, struct pbuf *p);

	struct netif gnetif;

	static ETH_HandleTypeDef* ethHandle;
	static ETH_TxPacketConfigTypeDef* ethTxConfig;

	static ETH_BufferTypeDef* txBuffer;
	static size_t txBufferCount;
	static BinaryLock txLock;
};



#endif /* INC_ETHERNET_NETWORKINTERFACE_H_ */
