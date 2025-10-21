#include "mbed.h"
#include <cstdio>
#include <json.hpp>
#include <string>
#include "TLSSocket.h"
#include "mbed-trace/mbed_trace.h"
#include "DFRobot_RGBLCD1602.h"
using json = nlohmann::json;

#include "Menu.h"

#define TRACE_GROUP "main"
#define BUFFER_SIZE 4000
#define REFRESH_RATE 50ms



std::string latitude = "";
std::string longitude = "";

double unix_time = 0; 


static BufferedSerial serial_port(USBTX, USBRX);

const char* SSL_CA_PEM = 
"-----BEGIN CERTIFICATE-----\n"
"MIICnzCCAiWgAwIBAgIQf/MZd5csIkp2FV0TttaF4zAKBggqhkjOPQQDAzBHMQsw\n"
"CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"
"MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMjMxMjEzMDkwMDAwWhcNMjkwMjIwMTQw\n"
"MDAwWjA7MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZp\n"
"Y2VzMQwwCgYDVQQDEwNXRTEwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARvzTr+\n"
"Z1dHTCEDhUDCR127WEcPQMFcF4XGGTfn1XzthkubgdnXGhOlCgP4mMTG6J7/EFmP\n"
"LCaY9eYmJbsPAvpWo4H+MIH7MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggr\n"
"BgEFBQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQU\n"
"kHeSNWfE/6jMqeZ72YB5e8yT+TgwHwYDVR0jBBgwFoAUgEzW63T/STaj1dj8tT7F\n"
"avCUHYwwNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzAChhhodHRwOi8vaS5wa2ku\n"
"Z29vZy9yNC5jcnQwKwYDVR0fBCQwIjAgoB6gHIYaaHR0cDovL2MucGtpLmdvb2cv\n"
"ci9yNC5jcmwwEwYDVR0gBAwwCjAIBgZngQwBAgEwCgYIKoZIzj0EAwMDaAAwZQIx\n"
"AOcCq1HW90OVznX+0RGU1cxAQXomvtgM8zItPZCuFQ8jSBJSjz5keROv9aYsAm5V\n"
"sQIwJonMaAFi54mrfhfoFNZEfuNMSQ6/bIBiNLiyoX46FohQvKeIoJ99cx7sUkFN\n"
"7uJW\n"
"-----END CERTIFICATE-----\n";

I2C lcdI2C(D14, D15);
DFRobot_RGBLCD1602 lcd(&lcdI2C, RGB_ADDRESS_V10_7BIT);

InterruptIn button0(D9, PullUp);
InterruptIn button1(D8, PullUp);
InterruptIn button2(D3, PullUp);
InterruptIn button3(D2, PullUp);

int buttonFlags = 0;
Timer debounceTimer;

void handleButtonPress(int buttonIndex) {
    if (debounceTimer.elapsed_time() > 100ms) {
        buttonFlags |= (1 << buttonIndex);
        debounceTimer.reset();
    }
}

int main() {
    debounceTimer.start();
    mbed_trace_init();
    serial_port.set_baud(115200);
    lcd.init();
    lcd.clear();
    lcd.setRGB(0,255,127);

    mbed_trace_init();

    printf("\n\n\nmain()\n==========\n");
    lcd.setCursor(0, 0);
    lcd.printf("Connect ---------");

    printf("Getting default instance... ");fflush(stdout);
    NetworkInterface *network = NetworkInterface::get_default_instance();
    if (!network) {
        printf("Failed to get default network interface\n");
        while (1);
    }
    printf("Success\n");
    lcd.setCursor(0, 0);
    lcd.printf("Connect #-------");


    printf("Connecting to network...... ");fflush(stdout);
    int tries = 0;
    while (1) {
        tries++;
        nsapi_size_or_error_t result = network->connect();
        if (result == -3011) {
            printf("\rConnecting to network...... failed (%d)", tries);fflush(stdout);
            continue;
        }
        if (result != 0) {
            printf("Failed to connect to network: %d\n", result);
            while (1);
        }
        break;
    }
    printf("\rConnecting to network...... Success (%d)\n", tries);
    lcd.setCursor(0, 0);
    lcd.printf("Connect ##------");

    nsapi_error_t r;

    printf("Opening tls socket......... ");fflush(stdout);
    // setting up TLS socket
    TLSSocket* socket = new TLSSocket();
    if ((r = socket->open(network)) != NSAPI_ERROR_OK) {
        printf("TLS socket open failed (%d)\n", r);
        return 1;
    }
    printf("Success\n");
    lcd.setCursor(0, 0);
    lcd.printf("Connect ###-----");

    printf("Setting certificate........ ");fflush(stdout);
    if ((r = socket->set_root_ca_cert(SSL_CA_PEM)) != NSAPI_ERROR_OK) {
        printf("TLS socket setRoot_ca_cert failed (%d)\n", r);
        return 1;
    }
    printf("Success\n");
    lcd.setCursor(0, 0);
    lcd.printf("Connect ####----");

    socket->set_hostname("ipgeolocation.io");

    SocketAddress address;
    network->gethostbyname("ipgeolocation.io", &address);
    address.set_port(443);

    printf("Connecting to ip_geolocation...... ");fflush(stdout);
    if ((r = socket->connect(address)) != NSAPI_ERROR_OK) {
        printf("TLS socket connect failed (%d) Bruh\n", r);
        return 1;
    }
    printf("Success\n");
    lcd.setCursor(0, 0);
    lcd.printf("Connect #####---");

        // Create HTTP request
    const char httpRequest[] = "GET /v2/timezone?apiKey=b0c2c1c553244be58e7bf18736e8106e HTTP/1.1\r\n"
                                "Host: api.ipgeolocation.io\r\n"
                                "Connection: close\r\n"
                                "\r\n";


    nsapi_size_t bytesToSend = strlen(httpRequest);
    nsapi_size_or_error_t sentBytes = 0;

    printf("\nSending HTTP request: \n%s", httpRequest);

    while (bytesToSend) {
        // Try to send the remaining data.
        // send() returns how many bytes were actually sent
        printf("Sendeing................... ");fflush(stdout);
        sentBytes = socket->send(httpRequest + sentBytes, bytesToSend);

        if (sentBytes < 0) {
            // Negative return values from send() are errors
            break;
        } else {
            printf("%d bytes\n", sentBytes);
        }

        bytesToSend -= sentBytes;
    }

    if (sentBytes < 0) {
      printf("Failed to send HTTP request: %d\n", sentBytes);
      while (1);
    }

    printf("Complete message sent\n\n");
    lcd.setCursor(0, 0);
    lcd.printf("Connect ######--");


    char buffer[BUFFER_SIZE];
    int remainingBytes = BUFFER_SIZE;
    int receivedBytes = 0;

    printf("Starting to receive\n");
    while (remainingBytes > 0) {
        nsapi_size_or_error_t result = socket->recv(buffer + receivedBytes, remainingBytes);
        if (result == 0) {
            break; // no more data
        }
        if (result < 0) {
            printf("Receive failed: %d\n", result);
            return 1;
        }
        receivedBytes += result;
        remainingBytes -= result;
    }
    buffer[receivedBytes] = '\0';  // Null-terminate the received data
    printf("now the socket is gonna get deleted:\n");
    socket->close();
    delete socket;
    printf("now its deleted\n");


    // Find start of JSON
    std::string data(buffer, receivedBytes);
    size_t index = data.find("\r\n\r\n");
    if (index == std::string::npos) {
        printf("No header-body separator found\n");
        return 1;
    }
    index += 4;  
    lcd.setCursor(0, 0);
    lcd.printf("Connect #######-");

    
    string bufferStr = buffer;
    size_t index1 = bufferStr.find('{');
    size_t index2 = bufferStr.find("}}}");
    buffer[index2 + 3] = '\0';
    char* json_str = buffer + index1;

    //printf("Json:\n \"%s\" \n End:",json_str);

    json jsonData = json::parse(json_str, nullptr, false);  

    if (jsonData.is_discarded()) {
        printf("JSON parse error\n");
        return 1;
    }
    lcd.setCursor(0, 0);
    lcd.printf("Connect ########");

    if (jsonData.contains("time_zone") && jsonData["time_zone"].is_object() &&
        jsonData.contains("location") && jsonData["location"].is_object()) {
            const auto& timezone = jsonData["time_zone"];
            const auto& location = jsonData["location"];
            lcd.clear();

            if (timezone.contains("date_time_unix") && timezone["date_time_unix"].is_number_float()) {
                unix_time = timezone["date_time_unix"];
                lcd.setCursor(0, 0);
                lcd.printf("Unix epoch time:");
                lcd.setCursor(0, 1);
                lcd.printf("%.3f",unix_time);
                ThisThread::sleep_for(2s);
                lcd.clear();
                } 
        

            if (location.contains("latitude") && location["latitude"].is_string() &&
                location.contains("longitude") && location["longitude"].is_string()){
                    latitude = location["latitude"];
                    longitude = location["longitude"];
                    lcd.setCursor(0, 0);
                    lcd.printf("Lat: %s", latitude.c_str());
                    lcd.setCursor(0, 1);
                    lcd.printf("Lon: %s", longitude.c_str());
                    ThisThread::sleep_for(2s);
                    lcd.clear();
                }
            

                
            if (location.contains("city") && location["city"].is_string()){
                std::string city = location["city"];
                lcd.setCursor(0, 0);
                lcd.printf("City:");
                lcd.setCursor(0, 1);
                lcd.printf("%s",city.c_str());
                ThisThread::sleep_for(2s);
                lcd.clear();
            }
            int utc_offset_sec = 0;
            int dst_saving_sec = 0;
            if (timezone.contains("offset") && timezone["offset"].is_number_integer()) {
                utc_offset_sec = timezone["offset"].get<int>() * 3600;;
                } 
                

            if (timezone.contains("dst_savings") && timezone["dst_savings"].is_number_integer()) {
                dst_saving_sec = timezone["dst_savings"].get<int>() * 3600;;
                } 
                
            printf("%i\n", dst_saving_sec);
            printf("%i\n",utc_offset_sec);
            time_t local_time = unix_time + utc_offset_sec + dst_saving_sec;

            set_time(local_time);



    } else {
        printf("JSON fields missing or wrong type\n");
        return 1;
    }
    // make the buttons work
    button0.fall([] { handleButtonPress(0); });
    button1.fall([] { handleButtonPress(1); });
    button2.fall([] { handleButtonPress(2); });
    button3.fall([] { handleButtonPress(3); });

    // Define the menus and display the first view
    Menu menu(lcd, buttonFlags, network, latitude, longitude);

    int updates = 0; 
    while (true) {
        if (buttonFlags > 0) {
            menu.checkButtons();
        }
        updates++;
        if (updates % 20 == 0 || buttonFlags > 0) { // lsden drawer sjeldnere for å spare strøm (med mindre en knapp blir trykt)
            menu.draw();
        }
        buttonFlags = 0;
        ThisThread::sleep_for(REFRESH_RATE);
    }
}