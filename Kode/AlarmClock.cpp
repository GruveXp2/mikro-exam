#include "Menu.h"
#include "AlarmClock.h"
#include <ctime>

AlarmClock::AlarmClock(Menu* menu, int& buttonFlags)
    : View(menu, buttonFlags), state(AlarmState::Disabled), snoozeTimer(0), activeTimer(0) {
    }

void AlarmClock::update(time_t currentTime){
    if (state == AlarmState::Disabled) return; 

    struct tm *now = localtime(&currentTime); 
    int currentHour = now -> tm_hour;
    int currentMinute = now -> tm_min; 
    
    if (state == AlarmState::Enabled){
        if (currentTime >= snoozeTimer){
            active();
        }
        return; 
    }
}

void AlarmClock::active(){
    state = AlarmState::Active; 
    activeTimer = time(NULL);
    
}

void AlarmClock::deactivate(){
    state = AlarmState::Disabled;
}

void AlarmClock::setTimer(int h, int m){
}

void AlarmClock::enable(bool is_on){
    state = is_on ? AlarmState::Enabled : AlarmState::Disabled;
}

void AlarmClock::snooze(){
    if (state == AlarmState::Active){
        snoozeTimer = time(NULL) + SNOOZ_DURATION;
        state = AlarmState::Snooze;
    }
}

void AlarmClock::mute(){
    if (state == AlarmState::Active){
        deactivate();
    }
}

void AlarmClock::checkButtons() {}
void AlarmClock::draw(DFRobot_RGBLCD1602* lcd) {}