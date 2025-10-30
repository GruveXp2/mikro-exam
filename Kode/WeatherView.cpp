#include "WeatherView.h"
#include "Menu.h"
#include <cstdio>
#include <ctime>

WeatherView::WeatherView(Menu* menu, int& buttonFlags, NetworkInterface* network, const std::string& longitude, const std::string& latitude)
    : View(menu, buttonFlags), network(network), longitude(longitude), latitude(latitude), update_thread(osPriorityLow) {
        update_thread.start(callback(this, &WeatherView::thread_task));
    }

const char* SSL_CA_PEM1 =   
"-----BEGIN CERTIFICATE-----\n"
"MIIG5TCCBM2gAwIBAgIRANpDvROb0li7TdYcrMTz2+AwDQYJKoZIhvcNAQEMBQAw\n"
"gYgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpOZXcgSmVyc2V5MRQwEgYDVQQHEwtK\n"
"ZXJzZXkgQ2l0eTEeMBwGA1UEChMVVGhlIFVTRVJUUlVTVCBOZXR3b3JrMS4wLAYD\n"
"VQQDEyVVU0VSVHJ1c3QgUlNBIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MB4XDTIw\n"
"MDIxODAwMDAwMFoXDTMzMDUwMTIzNTk1OVowRDELMAkGA1UEBhMCTkwxGTAXBgNV\n"
"BAoTEEdFQU5UIFZlcmVuaWdpbmcxGjAYBgNVBAMTEUdFQU5UIE9WIFJTQSBDQSA0\n"
"MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEApYhi1aEiPsg9ZKRMAw9Q\n"
"r8Mthsr6R20VSfFeh7TgwtLQi6RSRLOh4or4EMG/1th8lijv7xnBMVZkTysFiPmT\n"
"PiLOfvz+QwO1NwjvgY+Jrs7fSoVA/TQkXzcxu4Tl3WHi+qJmKLJVu/JOuHud6mOp\n"
"LWkIbhODSzOxANJ24IGPx9h4OXDyy6/342eE6UPXCtJ8AzeumTG6Dfv5KVx24lCF\n"
"TGUzHUB+j+g0lSKg/Sf1OzgCajJV9enmZ/84ydh48wPp6vbWf1H0O3Rd3LhpMSVn\n"
"TqFTLKZSbQeLcx/l9DOKZfBCC9ghWxsgTqW9gQ7v3T3aIfSaVC9rnwVxO0VjmDdP\n"
"FNbdoxnh0zYwf45nV1QQgpRwZJ93yWedhp4ch1a6Ajwqs+wv4mZzmBSjovtV0mKw\n"
"d+CQbSToalEUP4QeJq4Udz5WNmNMI4OYP6cgrnlJ50aa0DZPlJqrKQPGL69KQQz1\n"
"2WgxvhCuVU70y6ZWAPopBa1ykbsttpLxADZre5cH573lIuLHdjx7NjpYIXRx2+QJ\n"
"URnX2qx37eZIxYXz8ggM+wXH6RDbU3V2o5DP67hXPHSAbA+p0orjAocpk2osxHKo\n"
"NSE3LCjNx8WVdxnXvuQ28tKdaK69knfm3bB7xpdfsNNTPH9ElcjscWZxpeZ5Iij8\n"
"lyrCG1z0vSWtSBsgSnUyG/sCAwEAAaOCAYswggGHMB8GA1UdIwQYMBaAFFN5v1qq\n"
"K0rPVIDh2JvAnfKyA2bLMB0GA1UdDgQWBBRvHTVJEGwy+lmgnryK6B+VvnF6DDAO\n"
"BgNVHQ8BAf8EBAMCAYYwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHSUEFjAUBggr\n"
"BgEFBQcDAQYIKwYBBQUHAwIwOAYDVR0gBDEwLzAtBgRVHSAAMCUwIwYIKwYBBQUH\n"
"AgEWF2h0dHBzOi8vc2VjdGlnby5jb20vQ1BTMFAGA1UdHwRJMEcwRaBDoEGGP2h0\n"
"dHA6Ly9jcmwudXNlcnRydXN0LmNvbS9VU0VSVHJ1c3RSU0FDZXJ0aWZpY2F0aW9u\n"
"QXV0aG9yaXR5LmNybDB2BggrBgEFBQcBAQRqMGgwPwYIKwYBBQUHMAKGM2h0dHA6\n"
"Ly9jcnQudXNlcnRydXN0LmNvbS9VU0VSVHJ1c3RSU0FBZGRUcnVzdENBLmNydDAl\n"
"BggrBgEFBQcwAYYZaHR0cDovL29jc3AudXNlcnRydXN0LmNvbTANBgkqhkiG9w0B\n"
"AQwFAAOCAgEAUtlC3e0xj/1BMfPhdQhUXeLjb0xp8UE28kzWE5xDzGKbfGgnrT2R\n"
"lw5gLIx+/cNVrad//+MrpTppMlxq59AsXYZW3xRasrvkjGfNR3vt/1RAl8iI31lG\n"
"hIg6dfIX5N4esLkrQeN8HiyHKH6khm4966IkVVtnxz5CgUPqEYn4eQ+4eeESrWBh\n"
"AqXaiv7HRvpsdwLYekAhnrlGpioZ/CJIT2PTTxf+GHM6cuUnNqdUzfvrQgA8kt1/\n"
"ASXx2od/M+c8nlJqrGz29lrJveJOSEMX0c/ts02WhsfMhkYa6XujUZLmvR1Eq08r\n"
"48/EZ4l+t5L4wt0DV8VaPbsEBF1EOFpz/YS2H6mSwcFaNJbnYqqJHIvm3PLJHkFm\n"
"EoLXRVrQXdCT+3wgBfgU6heCV5CYBz/YkrdWES7tiiT8sVUDqXmVlTsbiRNiyLs2\n"
"bmEWWFUl76jViIJog5fongEqN3jLIGTG/mXrJT1UyymIcobnIGrbwwRVz/mpFQo0\n"
"vBYIi1k2ThVh0Dx88BbF9YiP84dd8Fkn5wbE6FxXYJ287qfRTgmhePecPc73Yrzt\n"
"apdRcsKVGkOpaTIJP/l+lAHRLZxk/dUtyN95G++bOSQqnOCpVPabUGl2E/OEyFrp\n"
"Ipwgu2L/WJclvd6g+ZA/iWkLSMcpnFb+uX6QBqvD6+RNxul1FaB5iHY=\n"
"-----END CERTIFICATE-----\n";


void WeatherView::draw(DFRobot_RGBLCD1602* lcd) {
    weather_mutex.lock();
    lcd->setCursor(0, 0);
    lcd->printf("%s", lastCondition.c_str());

    lcd->setCursor(0, 1);
    lcd->printf("%.f degrees",temp);
    weather_mutex.unlock();
}

void WeatherView::thread_task(){
    while(1){
        printf("Fetching weather api\n");
        fflush(stdout);
        weather_mutex.lock();
        update();
        weather_mutex.unlock();
        ThisThread::sleep_for(15min); //Sleep the thread for 15 min before update
    }
}


void WeatherView::update() {
    TLSSocket* socket = new TLSSocket();
    if (socket->open(network) != NSAPI_ERROR_OK) {
        printf("TLS socket open failed\n");
        return;
    }

    if (socket->set_root_ca_cert(SSL_CA_PEM1) != NSAPI_ERROR_OK) {
        printf("Setting root CA cert failed\n");
        socket->close();
        return;
    }

    socket->set_hostname("api.met.no");
    SocketAddress address;
    if (network->gethostbyname("api.met.no", &address) != NSAPI_ERROR_OK) {
        printf("DNS lookup failed\n");
        socket->close();
        return;
    }
    address.set_port(443);

    if (socket->connect(address) != NSAPI_ERROR_OK) {
        printf("TLS socket connect failed\n");
        socket->close();
        return;
    }

    std::string weatherHost = "api.met.no";
    std::string weatherPath = "/weatherapi/locationforecast/2.0/compact?lat=" + longitude + "&lon=" + latitude;

    std::string httpRequest =
        "GET " + weatherPath + " HTTP/1.1\r\n" +
        "Host: " + weatherHost + "\r\n" +
        "User-Agent: MyWeatherClient/1.0\r\n" +
        "Connection: close\r\n\r\n";

    nsapi_size_t bytesToSend = httpRequest.length();
    nsapi_size_or_error_t sentBytes = 0;
    const char* sendPtr = httpRequest.c_str();

    while (bytesToSend) {
        sentBytes = socket->send(sendPtr, bytesToSend);
        if (sentBytes < 0) {
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

    static char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    std::string Shorten_Json;
    bool Start_string_found= false;
    size_t Start_of_the_array = std::string::npos;
    size_t End_of_the_array = std::string::npos;

    while (true) {
        int r = socket->recv(buffer, BUFFER_SIZE);
        if (r <= 0) break;

        Shorten_Json.append(buffer, r);

        if (!Start_string_found) {
            size_t pos = Shorten_Json.find("\"timeseries\":[{");
            if (pos != std::string::npos) {
                Start_of_the_array = Shorten_Json.find("[{", pos);
                if (Start_of_the_array != std::string::npos) {
                    Start_string_found= true;
                    Shorten_Json = Shorten_Json.substr(Start_of_the_array);
                }
            } else {
                // Not found yet, keep reading
                if (Shorten_Json.size() > 512) {
                    Shorten_Json = Shorten_Json.substr(Shorten_Json.size() - 256);
                }
                continue;
            }
        }

        // If start found, look for end "}}}}"
        if (Start_string_found) {
            End_of_the_array = Shorten_Json.find("}}}}");
            if (End_of_the_array != std::string::npos) {
                End_of_the_array += 4; // move past "}}}}"

                // Extract the JSON snippet of timeseries first object
                std::string timeseriesChunk = Shorten_Json.substr(0, End_of_the_array);

                // Add closing bracket for timeseries array
                timeseriesChunk += "]";

                // Wrap in root object
                std::string minimalJson = "{\"timeseries\":" + timeseriesChunk + "}";

                printf("Extracted minimal JSON:\n%s\n", minimalJson.c_str());

                // Parse JSON now
                json jsonData = json::parse(minimalJson, nullptr, false);
                if (jsonData.is_discarded()) {
                    printf("JSON parse error\n");
                    return;
                }

                if (jsonData.contains("timeseries") && !jsonData["timeseries"].empty()) {
                    auto& firstEntry = jsonData["timeseries"][0];

                    if (firstEntry.contains("data") &&
                        firstEntry["data"].contains("instant") &&
                        firstEntry["data"]["instant"].contains("details") &&
                        firstEntry["data"]["instant"]["details"].contains("air_temperature")) {

                        this->temp = firstEntry["data"]["instant"]["details"]["air_temperature"];
                        printf("Air temperature: %.2f°C\n", temp);
                    } else {
                        printf("Missing expected air_temperature key\n");
                    }  

                    if (firstEntry["data"].contains("next_1_hours") &&
                        firstEntry["data"]["next_1_hours"].contains("summary") &&
                        firstEntry["data"]["next_1_hours"]["summary"].contains("symbol_code")) {

                        this-> lastCondition = firstEntry["data"]["next_1_hours"]["summary"]["symbol_code"];
                        printf("Weather symbol code: %s\n", lastCondition.c_str());
                    } else {
                        printf("Missing next_1_hours symbol_code\n");
                    }

                } else {
                    printf("Missing expected air_temperature key\n");
                }

                jsonData.clear();
                Shorten_Json.clear();
                timeseriesChunk.clear();
                minimalJson.clear();
                break;  
            }
        }
    }
    socket->close();
    delete socket;
}


void WeatherView::checkButtons() {
    if (isButtonPressed(1)) {
        menu->prevView();
    } else if (isButtonPressed(2)) {
        menu->nextView();
    } else if (isButtonPressed(3)) {
        update();
    }
}
