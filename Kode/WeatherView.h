#include "DFRobot_RGBLCD1602.h"
#include "View.h"
#include <cstdio>
#include <json.hpp>
#include <string>
using json = nlohmann::json;

#ifndef WEATHERVIEW_H
#define WEATHERVIEW_H
#define BUFFER_SIZE 520

class WeatherView final : public View {
public:
    explicit WeatherView(Menu* menu, int& buttonFlags, NetworkInterface* network, const std::string& longitude, const std::string& latitude);
    virtual ~WeatherView() = default;

    void checkButtons() override;
    void draw(DFRobot_RGBLCD1602* lcd) override;
    void update();

private:
    Thread updater;
    bool updating = false;
    int subView = 0;
    time_t lastUpdate = 0;
    NetworkInterface* network;

    std::string latitude;
    std::string longitude;

    std::string construct_url();

    float temp = 0.0;
    std::string lastCondition = "WeatherView";
};

#endif //WEATHERVIEW_H