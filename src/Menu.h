#include <vector>
#include <memory>

#include "DFRobot_RGBLCD1602.h"

#ifndef MENU_H
#define MENU_H

#include "View.h"
#include "AlarmClock.h"
#include "ViewType.h"
/**
 * this class holds all the views, shows them and has logic to switch between them
 */
class Menu {
public:
    Menu(DFRobot_RGBLCD1602& lcd, int& buttonFlags, NetworkInterface* netowork, std::string& longitude, std::string& latitude);
    virtual ~Menu() = default;

    void draw();
    void refreshScreen();

    void nextView();
    void prevView();
    void showView(ViewType view);

    void checkButtons();

    AlarmClock& getAlarmClock() {return alarmClock;}
protected:
    int currentViewIndex = 0; // which of the normal views that is currently shown. does not include the settings views
    std::vector<std::unique_ptr<View>> views;
    bool refresh = false; // if the screen should clear before next draw. Prevents unneccesary flashing

    DFRobot_RGBLCD1602* lcd;
    AlarmClock alarmClock;
    std::string& longitude;
    std::string& latitude;
};

#endif //MENU_H