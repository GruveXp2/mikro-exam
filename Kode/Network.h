#include "DFRobot_RGBLCD1602.h"
#include <string>

#ifndef NETWORK_H
#define NETWORK_H

class Network {
public:
    static bool openSocket(TLSSocket* socket, NetworkInterface* network);
    static bool setCert(TLSSocket* socket, const char* cert);
    static bool connectToHost(TLSSocket* socket, NetworkInterface* network, const char* hostName, uint16_t port);
    static std::string createHTTPReq(std::string host, std::string path);
};

#endif //NETWORK_H