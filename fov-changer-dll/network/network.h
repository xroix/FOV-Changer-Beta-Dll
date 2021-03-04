#pragma once

#include "../pch.h"
#include <cppzmq/zmq.hpp>


class Network
{
public:
	std::thread networkThread;
	WSADATA wsaData;
	bool wsaDataInitialized = false;

	bool startThread();

	void onStop();
};