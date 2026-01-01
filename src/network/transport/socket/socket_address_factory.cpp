#include "socket_address_factory.h"

GameNet::SocketAddressOpt GameNet::SocketAddressFactory::CreateIPv4FromString(
    const std::string& str) {
  auto pos = str.find_last_of(':');
  std::string host, service;
  if (pos != std::string::npos) {
    host = str.substr(0, pos);
    service = str.substr(pos + 1);
  } else {
    host = str;
    service = "0";
  }

  addrinfo hint;
  memset(&hint, 0, sizeof(hint));
  hint.ai_family = AF_INET;

  // Convert domain name to IP address.
  // getaddrinfo() blocks the operation until it
  // receives the resolved IP addresses from the DNS server.
  addrinfo* result{nullptr};
  int err = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
  if (err != 0 && result != nullptr) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: getaddrinfo failed\n",
                __FUNCTION__);
    freeaddrinfo(result);
    return std::nullopt;
  }

  while (!result->ai_addr && result->ai_next) {
    result = result->ai_next;
  }

  if (!result->ai_addr) {
    freeaddrinfo(result);
    return std::nullopt;
  }

  auto toRet = std::make_optional<SocketAddress>(*result->ai_addr);

  freeaddrinfo(result);
  return toRet;
}