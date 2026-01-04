#include "socket/socket_util.h"

bool GameNet::SocketUtil::StaticInit() {
#if _WIN32
  WSADATA wsaData;
  int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (res != NO_ERROR) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: WSAStartUp failed\n", __FUNCTION__);
    return false;
  }
#endif
  return true;
}

void GameNet::SocketUtil::StaticCleanUp() {
#if _WIN32
  WSACleanup();
#endif
}

int GameNet::SocketUtil::GetLastError() {
#if _WIN32
  return WSAGetLastError();
#else
  return errno;
#endif
}
