#include "TimeView.h"
#include "Menu.h"
#include <cstdio>

// subwievs
#define MAIN 0
#define SET_ALARM 1
#define SET_HOUR 2
#define SET_MINUTES 3

TimeView::TimeView(Menu* menu, int& buttonFlags)
    : View(menu, buttonFlags), lastUpdate(0)  {}

void TimeView::draw(DFRobot_RGBLCD1602* lcd) {
    printf("\n\nTimeView::draw\n==========\n");
    lcd->clear();
    time_t seconds = time(NULL);
    if (seconds != lastUpdate){
        lastUpdate=seconds;
        char buffer [32];
        struct tm *localtm = localtime(&seconds);
        strftime(buffer, sizeof(buffer), "%a %d %b %H:%M", localtm);

        lcd->setCursor(0, 0);
        lcd->printf("%s", buffer);
    }
}

void TimeView::checkButtons() {
    if (isButtonPressed(1)) {
        menu->prevView();
    } else if (isButtonPressed(2)) {
        menu->nextView();
    }
}

