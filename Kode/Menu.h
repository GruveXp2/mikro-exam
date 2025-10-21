#include <vector>
#include <memory>

#include "DFRobot_RGBLCD1602.h"

#ifndef MENU_H
#define MENU_H

//class View; // fikser sirkulær include pga Menu og Views må vite om hverandre
#include "View.h" // 👈 MÅ inkluderes her

class Menu {
    // Holder styr på alle views, og hvilken view som er oppe
public:
    Menu(DFRobot_RGBLCD1602& lcd, int& buttonFlags, NetworkInterface* netowork, const std::string& longitude, const std::string& latitude);
    virtual ~Menu() = default;

    void draw();
    void nextView();
    void prevView();
    void checkButtons();
protected:
    int currentViewIndex = 0;
    std::vector<std::unique_ptr<View>> views;
    DFRobot_RGBLCD1602* lcd;
};

#endif //MENU_H