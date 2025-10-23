#include "DFRobot_RGBLCD1602.h"
#include "View.h"
#include "HTS221Sensor.h"

#ifndef CLIMATEVIEW_H
#define CLIMATEVIEW_H

class ClimateView final : public View {
public:
    explicit ClimateView(Menu* menu, int& buttonFlags);
    virtual ~ClimateView() = default;

    void checkButtons() override;
    void draw(DFRobot_RGBLCD1602* lcd) override;
private:
    DevI2C i2c;
    HTS221Sensor sensor; // HTS221 instance
    float temperature; 
    float humidity;
};

#endif //CLIMATEVIEW_H