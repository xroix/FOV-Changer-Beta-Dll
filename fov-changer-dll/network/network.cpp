#include "../pch.h"
#include "network.h"


//void NetworkThread(Network* network)
//{
//	std::cout << "Network Thread started" << std::endl;
//
//	int iResult;
//
//	// Initialize Winsock
//	iResult = WSAStartup(MAKEWORD(2, 2), &network->wsaData);
//	if (iResult != 0)
//	{
//		printf("WSAStartup failed: %d\n", iResult);
//		return;
//	}
//	network->wsaDataInitialized = true;
//
//	struct addrinfo* result = NULL,
//				   * ptr    = NULL,
//					 hints;
//
//	ZeroMemory(&hints, sizeof(hints));
//	hints.ai_family = AF_UNSPEC;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_protocol = IPPROTO_TCP;
//
//	#define DEFAULT_PORT "27015"
//
//	// Resolve the server address and port
//	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
//	if (iResult != 0)
//	{
//		printf("getaddrinfo failed: %d\n", iResult);
//		WSACleanup();
//		return;
//	}
//
//	SOCKET ConnectSocket = INVALID_SOCKET;
//	
//	// Attempt to connect to the first address returned by
//	// the call to getaddrinfo
//	ptr = result;
//
//	// Create a SOCKET for connecting to server
//	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
//		ptr->ai_protocol);
//
//	if (ConnectSocket == INVALID_SOCKET)
//	{
//		printf("Error at socket(): %ld\n", WSAGetLastError());
//		freeaddrinfo(result);
//		WSACleanup();
//		return;
//	}
//
//	std::cout << "Network Thread closed" << std::endl;
//}

//void NetworkThread(Network* network)
//{
//    /*printf("Connecting to hello world server…\n");
//    void* context = zmq_ctx_new();
//    void* requester = zmq_socket(context, ZMQ_REQ);
//    zmq_connect(requester, "tcp://localhost:1325");
//
//    printf("Connected\n");
//
//    char buffer[10];
//    printf("Sending Hello");
//    zmq_send(requester, "Hello", 5, 0);
//    zmq_recv(requester, buffer, 10, 0);
//    printf("Received World");
// 
//    zmq_close(requester);
//    zmq_ctx_destroy(context);*/
//    zmq::context_t ctx;
//    zmq::socket_t sock(ctx, zmq::socket_type::rep);
//    sock.bind("tcp://127.0.0.1:4234");
//
//    while (true)
//    {
//        std::cout << "Hello, why isn't this working?" << std::endl;
//        zmq::message_t msg;
//        const auto ret = sock.recv(msg, zmq::recv_flags::none);
//
//        std::cout << "Got " << msg << std::endl;
//
//        sock.send(zmq::str_buffer("Hello, world"), zmq::send_flags::none);
//    }
//    
//}

bool Network::startThread()
{
	//networkThread = std::thread(NetworkThread, this);
	return true;
}

void Network::onStop()
{
	if (wsaDataInitialized)
	{
		WSACleanup();
		std::cout << "Cleaned up WSA!" << std::endl;
	}
}