#ifndef APPLICATION_USER_CORE_INC_IP_SOCKET_H_
#define APPLICATION_USER_CORE_INC_IP_SOCKET_H_

#include <functional>
#include <vector>

#include "BinaryLock.h"
#include "QueueT.h"
#include "SocketEvent.h"
#include "Thread.h"
#include "ThreadXMemory.h"

#include "app_netxduo.h"

extern "C"
{
extern NX_IP NetXDuoEthIpInstance;
}

struct IPString {
	char data[16];
};
using DataReceivedFunction = std::function<void(uint8_t*, size_t)>;

class Socket : protected Thread
{
public:
	Socket(const char* name);

	static IPString IP_TO_STRING(ULONG ip) {
		IPString res;
		sprintf(res.data, "%lu.%lu.%lu.%lu",
				(ip >> 24) & 0xFF,
				(ip >> 16) & 0xFF,
				(ip >> 8) & 0xFF,
				ip & 0xFF);
		return res;
	};

	bool initialize();
	void transmit(const char* message);
	bool listen(UINT port, UINT maxConnections, DataReceivedFunction func);

	static std::vector<Socket*>& getAllSockets() { return _sockets; }
protected:
	virtual void main() override;
private:
	void handleConnected();
	void handleDisconnected();
	void handleDataReceived();

	void rxDisconnectReceived(NX_TCP_SOCKET* socket);
	void rxConnectReceived(NX_TCP_SOCKET* socket, UINT port);
	void dataNotify(NX_TCP_SOCKET* socket);

	const char* name;
	NX_IP* ipHandle;
	NX_TCP_SOCKET socketHandle;

	DataReceivedFunction rxCallback;
	ULONG port;
	UINT maxConnections;

	SocketEvent event{};

	static std::vector<Socket*> _sockets;
	static Socket* findSocket(NX_TCP_SOCKET* socket);
	static void s_rxDisconnectReceived(NX_TCP_SOCKET* socket);
	static void s_rxConnectReceived(NX_TCP_SOCKET* socket, UINT port);
	static void s_DataReceived(NX_TCP_SOCKET* socket);
};



#endif /* APPLICATION_USER_CORE_INC_IP_SOCKET_H_ */
