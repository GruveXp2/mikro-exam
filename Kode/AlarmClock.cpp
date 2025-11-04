#include "Menu.h"
#include "AlarmClock.h"
#include <cstdio>
#include <ctime>

AlarmClock::AlarmClock ()
    : hour(0), minute(0), state(AlarmState::Disabled),
    snoozeTimer(0), activeTimer(0), buzzer(D12) {
        buzzer_thread.start(callback(this, &AlarmClock::buzzer_task));
    }

void AlarmClock::buzzer_task(){
    buzzer.period(0.001f);
    buzzer.write(0);

    while(true){
        uint32_t flags_read = flags.wait_any(BUZZER_ON | BUZZER_STOP);
        if (flags_read & BUZZER_ON){
            printf("Buzzer is buzzing");
            buzzer.write(0.5);
        }

        if (flags_read & BUZZER_STOP){
            printf("Buzzer is stopping");
            buzzer.write(0);
        }
    }
}


void AlarmClock::update(){
    if (state == AlarmState::Disabled) return; 

    time_t currentTime = time(NULL);
    struct tm *localtm = localtime(&currentTime);
    int currentHour = localtm -> tm_hour;
    int currentMinute = localtm -> tm_min;

    if (state == AlarmState::Enabled) {
        if (currentHour == hour && currentMinute == minute) {
            active();
        }
    }

    if(state == AlarmState::Active){
        if (currentTime - activeTimer >= AUTO_MUTE_TIMER){
            state = AlarmState::Enabled;
        }
    }

    if (state == AlarmState::Snooze) {
        if (currentTime >= snoozeTimer){
            active();   
        }
    }
}

void AlarmClock::active(){
    state = AlarmState::Active; 
    activeTimer = time(NULL);
    flags.set(BUZZER_ON);
}

void AlarmClock::deactivate(){
    state = AlarmState::Enabled;
    flags.set(BUZZER_STOP);
}

void AlarmClock::setTimer(int hour, int minute){
    this -> hour = hour;
    this -> minute = minute;
    state = AlarmState::Enabled;
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



