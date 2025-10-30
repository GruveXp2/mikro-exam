#include "ClimateView.h"
#include "Menu.h"
#include <cstdio>

ClimateView::ClimateView(Menu* menu, int& buttonFlags)
        : View(menu, buttonFlags), i2c(PB_11, PB_10), sensor(&i2c)  {
    sensor.init(nullptr);
    sensor.enable();
    printf("sensor was inited\n");
}

void ClimateView::draw(DFRobot_RGBLCD1602* lcd) {

    sensor.get_temperature(&temperature);
    sensor.get_humidity(&humidity);

    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->printf("Temp: %.1f", temperature);
    //printf("\nTemp: %.1f\n", temperature);
    lcd->setCursor(0, 1);
    lcd->printf("Humidity: %.1f%%", humidity);
    //printf("Humidity: %.1f%%\n", humidity);
}

void ClimateView::checkButtons() {
    if (isButtonPressed(1)) {
        menu->prevView();
    } else if (isButtonPressed(2)) {
        menu->nextView();
    }
}
