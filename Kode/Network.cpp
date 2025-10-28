#include "Network.h"
#include <cstdio>
#include <cstring>
#include <string>

NetworkInterface* Network::getNetworkInterFace() {
    printf("Getting network interface.. ");fflush(stdout);
    NetworkInterface *network = NetworkInterface::get_default_instance();
    if (!network) {
        printf("failed\n");
        return network;
    }
    printf("Success\n");
    return network;
}

bool Network::connectToWifi(NetworkInterface* network) {
    printf("Connecting to wifi......... ");fflush(stdout);
    int tries = 0;
    while (1) {
        tries++;
        nsapi_size_or_error_t result = network->connect();
        if (result == -3011) {
            printf("\rConnecting to wifi......... failed (%d)", tries);fflush(stdout);
            continue;
        }
        if (result != 0) {
            printf("Failed to connect to wifi: %d\n", result);
            return false;
        }
        break;
    }
    printf("\rConnecting to network...... Success (%d)\n", tries);
    return true;
}

bool Network::openSocket(TLSSocket *socket, NetworkInterface *network) {
    printf("Opening TLS socket.........");fflush(stdout);
    int result = socket->open(network);
    if (result != NSAPI_ERROR_OK) {
        printf("failed (%d)\n", result);
        return false;
    }
    printf("Success\n");
    return true;
}

bool Network::setCert(TLSSocket *socket, const char *cert) {
    printf("Setting cert...............");fflush(stdout);
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
    size_t dots = max(7 - hostNameLen, 2);
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