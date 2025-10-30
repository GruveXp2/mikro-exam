#include <vector>
#include <memory>

#include "DFRobot_RGBLCD1602.h"

#ifndef MENU_H
#define MENU_H

//class View; // fikser sirkulær include pga Menu og Views må vite om hverandre
#include "View.h" // 👈 MÅ inkluderes her
#include "AlarmClock.h"
#include "ViewType.h"

class Menu {
    // Holder styr på alle views, og hvilken view som er oppe
public:
    Menu(DFRobot_RGBLCD1602& lcd, int& buttonFlags, NetworkInterface* netowork, std::string& longitude, std::string& latitude);
    virtual ~Menu() = default;

    void draw();
    void nextView();
    void prevView();
    void showView(ViewType view);
    void checkButtons();
    AlarmClock& getAlarmClock() {return alarmClock;}
protected:
    int currentViewIndex = 0;
    std::vector<std::unique_ptr<View>> views;

    DFRobot_RGBLCD1602* lcd;
    AlarmClock alarmClock;
    std::string& longitude;
    std::string& latitude;
};

#endif //MENU_H