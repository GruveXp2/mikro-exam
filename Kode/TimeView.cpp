#include "TimeView.h"
#include "AlarmClock.h"
#include "Menu.h"
#include <cstdio>

TimeView::TimeView(Menu* menu, int& buttonFlags, AlarmClock& alarmClock)
    : View(menu, buttonFlags), alarmClock(alarmClock), lastUpdate(0) {}

void TimeView::draw(DFRobot_RGBLCD1602* lcd) {
    time_t seconds = time(NULL);
    if (seconds != lastUpdate){
        lastUpdate=seconds;
        char buffer [32];
        struct tm *localtm = localtime(&seconds);
        strftime(buffer, sizeof(buffer), "%a %d %b %H:%M", localtm);

        lcd->setCursor(0, 0);
        lcd->printf("%s", buffer);
    }

    lcd->setCursor(0, 1);

    AlarmState state = alarmClock.get_AlarmClock_State();
    int hour = alarmClock.get_hour();
    int minute = alarmClock.get_minute();

    switch(state){
        case AlarmState::Disabled:
        lcd->printf("                ");
        break; 

        case AlarmState::Enabled:
        lcd->printf("Alarm     %02d:%02d", hour, minute); // pad to fill 16 chars
        break;

        case AlarmState::Active:
            lcd->printf("Alarm (A) %02d:%02d", hour, minute);
            break;

        case AlarmState::Snooze:
            lcd->printf("Alarm (S) %02d:%02d", hour, minute);
            break;
    }
} 

void TimeView::checkButtons() {
    if (isButtonPressed(1)) {
        menu->prevView();
    } else if (isButtonPressed(2)) {
        menu->nextView();
    }
}

