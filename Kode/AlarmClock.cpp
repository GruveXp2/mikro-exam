#include "Menu.h"
#include "AlarmClock.h"
#include <ctime>

AlarmClock::AlarmClock ()
    : hour(0), minute(0), state(AlarmState::Disabled),
    snoozeTimer(0), activeTimer(0) {}

void AlarmClock::update(time_t currentTime){
    if (state == AlarmState::Disabled) return; 

    struct tm *localtm = localtime(&currentTime);
    int currentHour = localtm -> tm_hour;
    int currentMinute = localtm -> tm_min;

    if (state == AlarmState::Enabled) {
        if (currentHour > hour || (currentHour == hour && currentMinute >= minute)) {
            active();
        }
        return;
    }

    if(state == AlarmState::Active && currentTime - activeTimer >= AUTO_MUTE_TIMER){
        state = AlarmState::Enabled;
    }

    if (state == AlarmState::Snooze && currentTime >= snoozeTimer){
        active();
    }
}

void AlarmClock::active(){
    state = AlarmState::Active; 
    activeTimer = time(NULL);
}

void AlarmClock::deactivate(){
    state = AlarmState::Enabled;
}

void AlarmClock::setTimer(int hour, int minute){
    this -> hour = hour;
    this -> minute = minute;
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
    if (state == AlarmState::Active || state == AlarmState::Snooze){
        deactivate();
    }
}

AlarmState AlarmClock::get_AlarmClock_State() const {return state;}
int AlarmClock::get_hour() const {return hour;}
int AlarmClock::get_minute() const {return minute;}

