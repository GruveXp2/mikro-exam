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
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
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

    socket->set_hostname("api.ipgeolocation.io");

    SocketAddress address;
    network->gethostbyname("api.ipgeolocation.io", &address);
    address.set_port(443);

    printf("Connecting to ip_geolocation...... ");fflush(stdout);
    if ((r = socket->connect(address)) != NSAPI_ERROR_OK) {
        printf("TLS socket connect failed (%d) Bruh\n", r);
        socket->close();
        return 1;
    }
    printf("Success\n");
    lcd.setCursor(0, 0);
    lcd.printf("Connect #####---");

    const char httpRequest[] = "GET /v2/timezone?apiKey=b0c2c1c553244be58e7bf18736e8106e HTTP/1.1\r\n"
                                "Host: api.ipgeolocation.io\r\n"
                                "Connection: close\r\n"
                                "\r\n";


    nsapi_size_t bytesToSend = strlen(httpRequest);
    nsapi_size_or_error_t sentBytes = 0;

    printf("\nSending HTTP request: \n%s", httpRequest);

    while (bytesToSend) {
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


    char static buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int remainingBytes = BUFFER_SIZE;
    int receivedBytes = 0;

    printf("Starting to receive\n");
    while (remainingBytes > 0) {
        nsapi_size_or_error_t result = socket->recv(buffer + receivedBytes, remainingBytes);
        if (result == 0) {
            break; 
        }
        if (result < 0) {
            printf("Receive failed: %d\n", result);
            return 1;
        }
        receivedBytes += result;
        remainingBytes -= result;
    }
    buffer[receivedBytes] = '\0';  

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

    printf("Json:\n \"%s\" \n End:",json_str);

    json jsonData = json::parse(json_str, nullptr, false);  

    if (jsonData.is_discarded()) {
        printf("JSON parse error\n");
        return 1;
    }

    data.clear();
    bufferStr.clear();
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

    jsonData.clear();
    socket->close();
    delete socket;

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