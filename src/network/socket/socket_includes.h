#pragma once

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#undef UNICODE
#undef _UNICODE
#include "WS2tcpip.h"
#include "WinSock2.h"
#include "Windows.h"

typedef int socklen_t;
#else
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef int SOCKET;
const int NO_ERROR = 0;
const int INVALID_SOCKET = -1;
const int WSAECONNRESET = ECONNRESET;
const int WSAEWOULDBLOCK = EAGAIN;
const int SOCKET_ERROR = -1;
#endif

#include <cassert>
#include <deque>
#include <format>
#include <list>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "gamenet/core/logger/logger.h"

namespace GameNet {
	
enum SocketAddressFamily { INET = AF_INET, INET6 = AF_INET6 };

}
