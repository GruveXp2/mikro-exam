#include "Menu.h"
#include <cstdio>
#include <memory>
#include "View.h"
#include "ClimateView.h"
#include "TimeView.h"
#include "WeatherView.h"
#include "NewsFeed.h"
#include "SetLocationView.h"


Menu::Menu(DFRobot_RGBLCD1602& lcd, int& buttonFlags, NetworkInterface* network, std::string& longitude, std::string& latitude):
        lcd(&lcd), latitude(latitude), longitude(longitude) {
    views.push_back(std::make_unique<TimeView>(this, buttonFlags, alarmClock));
    views.push_back(std::make_unique<ClimateView>(this, buttonFlags));
    views.push_back(std::make_unique<WeatherView>(this, buttonFlags, network, longitude, latitude));
    views.push_back(std::make_unique<NewsFeed>(this, buttonFlags, network));
    views.push_back(std::make_unique<SetLocationView>(this, buttonFlags, longitude, latitude));
}

void Menu::draw() {
    views.at(currentViewIndex)->draw(lcd);
}

void Menu::nextView() {
    lcd->clear();
    currentViewIndex++;
    if (currentViewIndex >= static_cast<int>(ViewType::COUNT)) currentViewIndex = 0;
}

void Menu::prevView() {
    lcd->clear();
    currentViewIndex--;
    if (currentViewIndex < 0) currentViewIndex = static_cast<int>(ViewType::COUNT) - 1;
}

void Menu::showView(ViewType view) {
    currentViewIndex = static_cast<int>(view);
    if (currentViewIndex > static_cast<int>(ViewType::COUNT)) {
        currentViewIndex--;
    }
    printf("its now gonna switch to %d\n", currentViewIndex);
}

void Menu::checkButtons() {
    views.at(currentViewIndex)->checkButtons();
}
