#include "Menu.h"
#include <cstdio>
#include <memory>
#include "View.h"
#include "ClimateView.h"
#include "TimeView.h"
#include "WeatherView.h"
#include "NewsFeed.h"


Menu::Menu(DFRobot_RGBLCD1602& lcd, int& buttonFlags, NetworkInterface* network, const std::string& longitude, const std::string& latitude):
        lcd(&lcd) {
    views.push_back(std::make_unique<TimeView>(this, buttonFlags, alarmClock));
    views.push_back(std::make_unique<ClimateView>(this, buttonFlags));
    views.push_back(std::make_unique<WeatherView>(this, buttonFlags, network, longitude, latitude));
    views.push_back(std::make_unique<NewsFeed>(this, buttonFlags, network));
}

void Menu::draw() {
    views.at(currentViewIndex)->draw(lcd);
}

void Menu::nextView() {
    lcd->clear();
    currentViewIndex++;
    if (currentViewIndex >= views.size()) currentViewIndex = 0;
}

void Menu::prevView() {
    lcd->clear();
    currentViewIndex--;
    if (currentViewIndex < 0) currentViewIndex = views.size() - 1;
}

void Menu::checkButtons() {
    views.at(currentViewIndex)->checkButtons();
}
