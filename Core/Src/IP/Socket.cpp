#include "Socket.h"

#include "Debug.h"

std::vector<Socket*> Socket::_sockets;

Socket::Socket(const char* name) : Thread { name, TX_MID_PRIORITY, 1024 }, name{name}, ipHandle{&NetXDuoEthIpInstance}
{
	_sockets.push_back(this);
}

void Socket::main()
{
	SocketEvents e;

	while(true)
	{
		if (event.waitForEventFlagsAny(static_cast<SocketEvents>(0xFFFF'FFFF), e, TX_WAIT_FOREVER))
		{
			if (e & Connected)
				handleConnected();
			if (e & Disconnected)
				handleDisconnected();
			if (e & DataReceived)
				handleDataReceived();
		}
	}
}

void Socket::handleConnected()
{
	NXD_ADDRESS ip_address;
	ULONG clientPort;
    /* Accept a client socket connection.  */
	auto result = nx_tcp_server_socket_accept(&socketHandle, NX_WAIT_FOREVER);
    if (result)
    {
        nx_tcp_server_socket_unlisten(ipHandle, socketHandle.nx_tcp_socket_port);
        nx_tcp_socket_delete(&socketHandle);
		Debug::send("nx_tcp_server_socket_accept error: %u", result);
        return;
    }

    result = nxd_tcp_socket_peer_info_get(&socketHandle, &ip_address, &clientPort);
    if (result)
    {
        nx_tcp_server_socket_unaccept(&socketHandle);
        nx_tcp_server_socket_unlisten(ipHandle, socketHandle.nx_tcp_socket_port);
        nx_tcp_socket_delete(&socketHandle);
		Debug::send("nxd_tcp_socket_peer_info_get error: %u", result);
        return;
    }

    Debug::send("Peer info: IP (%s):%u", Socket::IP_TO_STRING(ip_address.nxd_ip_address.v4).data, clientPort);

    result = nx_tcp_socket_receive_notify(&socketHandle, Socket::s_DataReceived);
    if (result)
    {
        nx_tcp_server_socket_unaccept(&socketHandle);
        nx_tcp_server_socket_unlisten(ipHandle, socketHandle.nx_tcp_socket_port);
        nx_tcp_socket_delete(&socketHandle);
		Debug::send("nx_tcp_socket_receive_notify error: %u", result);
        return;
    }
}

void Socket::handleDisconnected()
{
    nx_tcp_socket_disconnect(&socketHandle, NX_WAIT_FOREVER);
    nx_tcp_server_socket_unaccept(&socketHandle);
    nx_tcp_server_socket_relisten(ipHandle, port, &socketHandle);
}

void Socket::handleDataReceived()
{
	NX_PACKET *packet_ptr;

	while (true)
	{
		auto result =  nx_tcp_socket_receive(&socketHandle, &packet_ptr, TX_NO_WAIT);

		if (result == NX_SUCCESS)
		{
			rxCallback(packet_ptr->nx_packet_prepend_ptr, packet_ptr->nx_packet_length);
			nx_packet_release(packet_ptr);
		}
		else if (result == NX_NO_PACKET)
		{
			break;
		}
		else
		{
			Debug::send("nx_tcp_socket_receive error: 0x%X", result);
			break;
		}
	}
}

Socket* Socket::findSocket(NX_TCP_SOCKET* socket)
{
	for (auto* s : _sockets)
		if (&s->socketHandle == socket)
			return s;

	return nullptr;
}

void Socket::s_rxDisconnectReceived(NX_TCP_SOCKET *socket)
{
	auto s = Socket::findSocket(socket);
	if (s)
		s->rxDisconnectReceived(socket);
	else
		Debug::send("Disconnect Failed to find Socket holder (%p)", socket);
}

void Socket::rxDisconnectReceived(NX_TCP_SOCKET* socket)
{
	Debug::send("Socket %s rxDisconnectReceived", name);
	event.post(SocketEvents::Disconnected);
}

void Socket::s_rxConnectReceived(NX_TCP_SOCKET *socket, UINT port)
{
	auto s = Socket::findSocket(socket);
	if (s)
		s->rxConnectReceived(socket, port);
	else
		Debug::send("Connect Failed to find Socket holder (%p)", socket);
}

void Socket::rxConnectReceived(NX_TCP_SOCKET* socket, UINT port)
{
	Debug::send("Socket %s rxConnectReceived", name);
	event.post(SocketEvents::Connected);
}

void Socket::s_DataReceived(NX_TCP_SOCKET* socket)
{
	auto s = Socket::findSocket(socket);
	if (s)
		s->dataNotify(socket);
	else
		Debug::send("Connect Failed to find Socket holder (%p)", socket);
}

void Socket::dataNotify(NX_TCP_SOCKET* socket)
{
	event.post(SocketEvents::DataReceived);
}

bool Socket::initialize()
{
	UINT result;
	ULONG ipStatus;

	result = nx_ip_status_check(ipHandle, NX_IP_INITIALIZE_DONE, &ipStatus, NX_IP_PERIODIC_RATE);

	if (result != NX_SUCCESS)
	{
		Debug::send("IP Not initialized: %u", result);
		return false;
	}

    /* Create a socket.  */
	result =  nx_tcp_socket_create(ipHandle, &socketHandle, (char*)name,
                                   NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 32 * 1024,
                                   NX_NULL, Socket::s_rxDisconnectReceived);

	if (result != NX_SUCCESS)
	{
		Debug::send("nx_tcp_socket_create failed: %u", result);
		return false;
	}

//	Debug::send("Socket Initialization Complete");
	return result == NX_SUCCESS;
}

void Socket::transmit(const char* message)
{
	NX_PACKET* sendPacket{nullptr};
	nx_packet_allocate(&TXMemory::getPacketPool(), &sendPacket, NX_TCP_PACKET, NX_WAIT_FOREVER);
	nx_packet_data_append(sendPacket, (void*)message, strlen(message), &TXMemory::getPacketPool(), NX_WAIT_FOREVER);
	nx_tcp_socket_send(&socketHandle, sendPacket, 1);
	nx_packet_release(sendPacket);
}

bool Socket::listen(UINT port, UINT maxConnections, DataReceivedFunction func)
{
	UINT result;
	this->rxCallback = func;
	this->port = port;
	this->maxConnections = maxConnections;

    /* Setup this thread to listen.  */
	result =  nx_tcp_server_socket_listen(ipHandle, port, &socketHandle, maxConnections, Socket::s_rxConnectReceived);

    /* Check for error.  */
    if (result)
    {
        nx_tcp_socket_delete(&socketHandle);
		Debug::send("nx_tcp_server_socket_listen failed: %u", result);
        return false;;
    }

    Debug::send("Socket listening on port %u", this->port);

//	Debug::send("Socket Listen Started");
	return result == NX_SUCCESS;
}
