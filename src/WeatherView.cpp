#include "WeatherView.h"
#include "ViewType.h"
#include "Menu.h"
#include <cstdio>
#include "Network.h"
#include <ctime>

WeatherView::WeatherView(Menu* menu, int& buttonFlags, NetworkInterface* network,
        const std::string& longitude, const std::string& latitude, EventFlags& locationFlag)
        : View(menu, buttonFlags), network(network), longitude(longitude),
        latitude(latitude), update_thread(osPriorityNormal), locationFlag(locationFlag) {
            
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

// Function to draw the weather_symbol and the air_temp
void WeatherView::draw(DFRobot_RGBLCD1602* lcd) {
    weather_mutex.lock();
    lcd->setCursor(0, 0);
    lcd->printf("%s", lastCondition.c_str());

    lcd->setCursor(0, 1);
    lcd->printf("%.f degrees",temp);
    weather_mutex.unlock();
}

// Thread task to fetch the weather api
void WeatherView::thread_task(){
    while(1){
        printf("Fetching weather api\n");
        fflush(stdout);
        update();
        locationFlag.wait_any_for(Menu::FLAG_UPDATED_LOCATION, 15min); // next update in 15min unless the user wants to update now (by changing longitude and latidude)
    }
}


void WeatherView::update() {
    Network::networkAccess.acquire(); // only 1 thread can use network at the same time to save memory

    printf("========== Fetching Weather ==========\n");
    TLSSocket* socket = new TLSSocket();
    std::string weatherHost = "api.met.no";
    std::string weatherPath = "/weatherapi/locationforecast/2.0/compact?lat=" + longitude + "&lon=" + latitude;

    if ( // connect to them, if it fails just return
        !Network::openSocket(socket, network) ||
        !Network::setCert(socket, SSL_CA_PEM1) ||
        !Network::connectToHost(socket, network, weatherHost.c_str(), 443)
        ) return;

    std::string httpRequest = Network::createHTTPReq(weatherHost, weatherPath);

    nsapi_size_t bytesToSend = httpRequest.length();
    nsapi_size_or_error_t sentBytes = 0;
    const char* sendPtr = httpRequest.c_str();

    // Loops as long there are more data
    while (bytesToSend) {
        sentBytes = socket->send(sendPtr, bytesToSend);
        if (sentBytes < 0) { // Cheks if the sent bytes are negative, if so breaks the loop
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


    static char buffer[BUFFER_SIZE];  // Define the buffer with a set size
    memset(buffer, 0, BUFFER_SIZE);   // Clears the buffer
    std::string shorten_json;
    bool found_array_start= false;
    // Index where the JSON array starts and ends
    size_t json_start = std::string::npos;
    size_t json_end = std::string::npos;

    while (true) {
        // Receving data
        int received_bytes = socket->recv(buffer, BUFFER_SIZE);
        if (received_bytes <= 0) break;  // Brekas if the byte's sent is negative or 0

        shorten_json.append(buffer, received_bytes);

        if (!found_array_start) {
            size_t firstTimeseries= shorten_json.find("\"timeseries\":[{"); // Look for the start of the timeseries
            if (firstTimeseries!= std::string::npos) {
                json_start = shorten_json.find("[{", firstTimeseries); // Find where the actual array begins
                if (json_start != std::string::npos) {
                    found_array_start= true;
                    shorten_json = shorten_json.substr(json_start); // Keeps only the portion of the array we want
                }
            } else {
                // If timeseries are not found yet and the buffer is too big, keep only the last 256
                if (shorten_json.size() > 512) {
                    shorten_json = shorten_json.substr(shorten_json.size() - 256);
                }
                continue;
            }
        }

        // If start found, look for end }}}}
        if (found_array_start) {
            json_end = shorten_json.find("}}}}");
            if (json_end != std::string::npos) {
                json_end += 4; // move past }}}}

                // Extract the JSON snippet of timeseries first object
                std::string timeseriesChunk = shorten_json.substr(0, json_end);

                // Add closing bracket for timeseries array
                timeseriesChunk += "]";

                // Wraps the timeseries in closing paranthesis to make a complete JSON - object
                std::string minimalJson = "{\"timeseries\":" + timeseriesChunk + "}";

                // Parse JSON now
                json jsonData = json::parse(minimalJson, nullptr, false);
                if (jsonData.is_discarded()) {
                    printf("JSON parse error\n");
                    return;
                }

                // Extract the infromastion from the JSON, like air_temp and humidity
                if (jsonData.contains("timeseries") && !jsonData["timeseries"].empty()) {
                    auto& firstEntry = jsonData["timeseries"][0]; // Takes the first entry of the timeseries

                    // Get air_temperature
                    if (firstEntry.contains("data") &&
                        firstEntry["data"].contains("instant") &&
                        firstEntry["data"]["instant"].contains("details") &&
                        firstEntry["data"]["instant"]["details"].contains("air_temperature")) {

                        weather_mutex.lock(); // stop main thread from reading temp when this thread modifies it
                        this->temp = firstEntry["data"]["instant"]["details"]["air_temperature"];
                        weather_mutex.unlock();
                        printf("Air temperature: %.2f°C\n", temp);
                    } else {
                        printf("Missing expected air_temperature key\n");
                    }  

                    // Get weather_symbol
                    if (firstEntry["data"].contains("next_1_hours") &&
                        firstEntry["data"]["next_1_hours"].contains("summary") &&
                        firstEntry["data"]["next_1_hours"]["summary"].contains("symbol_code")) {

                        weather_mutex.lock(); // stop main thread from reading lastCondition when this thread modifies it
                        this-> lastCondition = firstEntry["data"]["next_1_hours"]["summary"]["symbol_code"];
                        weather_mutex.unlock();
                        printf("Weather symbol code: %s\n", lastCondition.c_str());
                    } else {
                        printf("Missing next_1_hours symbol_code\n");
                    }

                } else {
                    printf("Missing timeseries array in JSON\n");
                }

                // Clear of stored JSAON data to free up memory
                jsonData.clear();
                shorten_json.clear();
                timeseriesChunk.clear();
                minimalJson.clear();
                break;  
            }
        }
    }
    // Close network socket and free up its memory, avoiding memory leak
    socket->close();
    delete socket;
    
    
    printf("======= Success =======\n");
    Network::networkAccess.release();
}

// Button check to move between views
void WeatherView::checkButtons() {
    if (isButtonPressed(1)) {
        menu->prevView();
    } else if (isButtonPressed(2)) {
        menu->nextView();
    } else if (isButtonPressed(3)) {
        menu->showView(ViewType::SET_LOCATION);
    }
}
