// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <memory>
#include "filesystem"

//#include <math.h>

#include <windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

// WinRT
#pragma comment(lib, "windowsapp")
#include <winrt/base.h>
#include <winrt/Windows.Storage.h> // for Logger.h
#include <winrt/Windows.Globalization.DateTimeFormatting.h> // for Logger.h
#include <winrt/Windows.Data.Xml.Dom.h> // for Notifications
#include <winrt/Windows.UI.Notifications.h> // for Notifications

// For directx
#include <unknwn.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Gaming.Input.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>

#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>

#include <xaudio2.h>

#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>

// Include folder
#include "MinHook.h"

// Libs
#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib,"libzmq-mt-sgd-4_3_3.lib")
#else
#pragma comment(lib,"libzmq-mt-s-4_3_3.lib")
#endif

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "dxguid.lib")  // for d2d1effects

#endif //PCH_H
