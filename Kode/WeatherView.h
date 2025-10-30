#ifndef WEATHERVIEW_H
#define WEATHERVIEW_H

#include "DFRobot_RGBLCD1602.h"
#include "View.h"
#include <cstdio>
#include <json.hpp>
#include <string>
using json = nlohmann::json;

#define BUFFER_SIZE 520

class WeatherView final : public View {
public:
    explicit WeatherView(Menu* menu, int& buttonFlags, NetworkInterface* network, const std::string& longitude, const std::string& latitude);
    virtual ~WeatherView() = default;

    void checkButtons() override;
    void draw(DFRobot_RGBLCD1602* lcd) override;
    

private:    
    NetworkInterface* network;
    std::string latitude;
    std::string longitude;

    std::string construct_url();

    float temp = 0.0f;
    std::string lastCondition = "unknown";

    Thread update_thread; 
    Mutex weather_mutex; 

    void update();
    void thread_task();
};

#endif //WEATHERVIEW_H