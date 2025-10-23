#include "Network.h"
#include <cstdio>
#include <cstring>
#include <string>

bool Network::openSocket(TLSSocket *socket, NetworkInterface *network) {
    printf("Opening TLS socket.......");fflush(stdout);
    int result = socket->open(network);
    if (result != NSAPI_ERROR_OK) {
        printf("failed (%d)\n", result);
        return false;
    }
    printf("Success\n");
    return true;
}

bool Network::setCert(TLSSocket *socket, const char *cert) {
    printf("Setting cert.............");fflush(stdout);
    int result = socket->set_root_ca_cert(cert);
    if (result != NSAPI_ERROR_OK) {
        printf("failed (%d)\n", result);
        socket->close();
        return false;
    }
    printf("Success\n");
    return true;
}

bool Network::connectToHost(TLSSocket *socket, NetworkInterface* network, const char *hostName, uint16_t port) {
    int hostNameLen = strlen(hostName);
    size_t dots = max(9 - hostNameLen, 2);
    std::string dotStr(dots, '.');
    printf("Connecting to %s%s", hostName, dotStr.c_str());fflush(stdout);
    socket->set_hostname(hostName);
    SocketAddress address;
    int result = network->gethostbyname(hostName, &address);
    if (result != NSAPI_ERROR_OK) {
        printf("DNS lookup failed (%d)\n", result);
        socket->close();
        return false;
    }
    printf("Success\n");

    address.set_port(443);
    result = socket->connect(address);
    if (result != NSAPI_ERROR_OK) {
        printf("failed (%d)\n", result);
        socket->close();
        return false;
    }
    printf("Success\n");
    return true;
}

std::string Network::createHTTPReq(std::string host, std::string path) {
    std::string httpRequest =
        "GET " + path + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "User-Agent: Mikrokontroller-eksamen/1.0\r\n" +
        "Connection: close\r\n\r\n";
        return httpRequest;
}