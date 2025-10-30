#include "SetLocationView.h"
#include "Menu.h"
#include <string>

#define FLOAT_LENGTH 8 // this is how many symbols that will be shown when u edit the location floats

#define SELECTING_AXIS 0
#define SELECTING_INDEX 1
#define SELECTING_SYMBOL 2

const std::string SetLocationView::symbols = "0123456789.";

SetLocationView::SetLocationView(Menu* menu, int& buttonFlags, std::string& longitude, std::string& latitude)
        : View(menu, buttonFlags), longitude(longitude), latitude(latitude), currentlySetting(&longitude) {
}

void SetLocationView::draw(DFRobot_RGBLCD1602* lcd) {
    switch (currentSubView) {
        case SELECTING_AXIS:
            if (isSettingLongitude) {
                lcd->setCursor(0, 0);
                lcd->printf("> Longitude");
                lcd->setCursor(0, 1);
                lcd->printf("  Latitude");
            } else {
                lcd->setCursor(0, 0);
                lcd->printf("  Longitude");
                lcd->setCursor(0, 1);
                lcd->printf("> Latitude");
            }
            break;
        case SELECTING_INDEX:
            lcd->setCursor(0, 0);
            lcd->printf("%s", currentlySetting->c_str());
            lcd->setCursor(0, 1);
            lcd->printf("                ");
            if (symbolIndex == 0) {
                lcd->setCursor(symbolIndex    , 1);
                lcd->printf("->");
            } else if (symbolIndex == FLOAT_LENGTH - 1) {
                lcd->setCursor(symbolIndex - 1, 1);
                lcd->printf("<-");
            } else {
                lcd->setCursor(symbolIndex - 1, 1);
                lcd->printf("<->");
            }
            break;
        case SELECTING_SYMBOL:
            lcd->setCursor(0, 0);
            lcd->printf("%s", currentlySetting->c_str());
            lcd->setCursor(0, 1);
            lcd->printf("                ");
            lcd->setCursor(symbolIndex, 1);
            lcd->printf("^");
            break;
    }
}

void SetLocationView::checkButtons() {
    switch (currentSubView) {
        case SELECTING_AXIS:
            if (isButtonPressed(0)) {
                menu->showView(ViewType::WEATHER);
            } else if (isButtonPressed(1) || isButtonPressed(2)) {
                isSettingLongitude = !isSettingLongitude;
                currentlySetting = isSettingLongitude ? &longitude : &latitude;
            } else if (isButtonPressed(3)) {
                currentSubView = SELECTING_INDEX;
            }
            break;
        case SELECTING_INDEX:
            if (isButtonPressed(0)) {
                currentSubView = SELECTING_AXIS;
            } else if (isButtonPressed(1)) {
                symbolIndex --;
                if (symbolIndex < 0) {
                    symbolIndex = FLOAT_LENGTH - 1;
                }
            } else if (isButtonPressed(2)) {
                symbolIndex ++;
                if (symbolIndex >= FLOAT_LENGTH) {
                    symbolIndex = 0;
                }
            } else if (isButtonPressed(3)) {
                currentSubView = SELECTING_SYMBOL;
            }
            break;
        case SELECTING_SYMBOL:
            if (isButtonPressed(0) || isButtonPressed(3)) {
                currentSubView = SELECTING_INDEX;
            } else if (isButtonPressed(1)) {
                symbolId--;
                if (symbolId < 0) {
                    symbolId = symbols.size() - 1;
                }
                (*currentlySetting)[symbolIndex] = symbols[symbolId];
            } else if (isButtonPressed(2)) {
                symbolId++;
                if (symbolId >= symbols.size()) {
                    symbolId = 0;
                }
                (*currentlySetting)[symbolIndex] = symbols[symbolId];
            }
    }
}