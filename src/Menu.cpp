#include "Menu.h"
#include <cstdio>
#include <memory>
#include "View.h"
#include "ClimateView.h"
#include "TimeView.h"
#include "WeatherView.h"
#include "NewsFeed.h"
#include "SetLocationView.h"
#include "SetAlarmView.h"


Menu::Menu(DFRobot_RGBLCD1602& lcd, int& buttonFlags, NetworkInterface* network, std::string& longitude, std::string& latitude):
        lcd(&lcd), latitude(latitude), longitude(longitude) {
    views.push_back(std::make_unique<TimeView>(this, buttonFlags, alarmClock));
    views.push_back(std::make_unique<ClimateView>(this, buttonFlags));
    views.push_back(std::make_unique<WeatherView>(this, buttonFlags, network, longitude, latitude));
    views.push_back(std::make_unique<NewsFeed>(this, buttonFlags, network));
    views.push_back(std::make_unique<SetLocationView>(this, buttonFlags, longitude, latitude));
    views.push_back(std::make_unique<SetAlarmView>(this, buttonFlags, alarmClock));
    lcd.setRGB(0,255,127);
}

void Menu::draw() {
    if (refresh) {
        lcd->clear(); // only clear if needed, like when switching views
        refresh = false;
    }
    views.at(currentViewIndex)->draw(lcd);
}

void Menu::refreshScreen() {
    refresh = true;
}

void Menu::nextView() {
    refresh = true;
    currentViewIndex++;
    if (currentViewIndex >= static_cast<int>(ViewType::COUNT)) currentViewIndex = 0;
}

void Menu::prevView() {
    refresh = true;
    lcd->clear();
    currentViewIndex--;
    if (currentViewIndex < 0) currentViewIndex = static_cast<int>(ViewType::COUNT) - 1;
}

void Menu::showView(ViewType view) {
    refresh = true;
    currentViewIndex = static_cast<int>(view);
    if (currentViewIndex > static_cast<int>(ViewType::COUNT)) {
        // for special views for changing settings
        // since ViewType::COUNT isnt its own view, all ViewTypes after will have 1 higher value than their actual index in the views vector
        currentViewIndex--;
        lcd->setRGB(255,255,0);
    } else {
        lcd->setRGB(0,255,127);
    }
}

void Menu::checkButtons() {
    views.at(currentViewIndex)->checkButtons();
}
