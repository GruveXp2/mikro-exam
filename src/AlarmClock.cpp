#include "Menu.h"
#include "AlarmClock.h"
#include <cstdint>
#include <cstdio>
#include <ctime>

AlarmClock::AlarmClock ()
    : hour(0), minute(0), state(AlarmState::Disabled),
    snoozeTimer(0), activeTimer(0), buzzer(D12), lastTriggeredMinute(-1) {
    }

void AlarmClock::buzzer_task(){
    buzzer.period(0.001f);
    buzzer.write(0);
    bool beep = true;

    while(true){
        flags.wait_any(BUZZER_ON);
        while (!(flags.get() & BUZZER_STOP)) {
            buzzer.write(beep ? 0.5f : 0.0f);
            beep = !beep;
            flags.wait_any_for(BUZZER_STOP, 250ms, false); // If the alarm is stopped, skip waiting and turn off immediately
        }   
        buzzer.write(0);
        flags.clear(BUZZER_ON | BUZZER_STOP);
    }
}


void AlarmClock::update(){
    if (state == AlarmState::Disabled) return; 
    
    // Get current hour and minute 
    time_t currentTime = time(NULL);  
    struct tm *localtm = localtime(&currentTime);
    int currentHour = localtm -> tm_hour;
    int currentMinute = localtm -> tm_min;

    if (state == AlarmState::Enabled) {
        if (currentHour == hour && currentMinute == minute && lastTriggeredMinute != currentMinute) { // Turns on the alarm if the time is the same
            active();
            lastTriggeredMinute = currentMinute; // Won't trigger/reactivate for the same minute
        }
    }

    if(state == AlarmState::Active){
        if (currentTime - activeTimer >= AUTO_MUTE_TIMER){ // Automute checker 
            deactivate();
        }
    }

    if (state == AlarmState::Snooze) { 
        if (currentTime >= snoozeTimer){  // Snooze checker
            active();   
        }
    }
}

// This section is functions that sets the alarm state: Active, Deaactivate, Mute, and Snooze 
// Based on the alarm state, it sets the eventflag for the buzzer to Buzzer_on and Buzzer_stop
void AlarmClock::active(){
    flags.clear(BUZZER_STOP);
    state = AlarmState::Active; 
    activeTimer = time(NULL);
    flags.set(BUZZER_ON);
}

void AlarmClock::deactivate(){
    state = AlarmState::Enabled;
    flags.set(BUZZER_STOP);
}

void AlarmClock::snooze(){
    if (state == AlarmState::Active){
        snoozeTimer = time(NULL) + SNOOZ_DURATION;
        state = AlarmState::Snooze;
        flags.set(BUZZER_STOP);
    }
}

void AlarmClock::mute(){
    if (state == AlarmState::Active || state == AlarmState::Snooze){
        state = AlarmState::Enabled;
        flags.set(BUZZER_STOP);
    }
}

// Function to set the alarm time
void AlarmClock::setTimer(int hour, int minute){
    this -> hour = hour;
    this -> minute = minute;
    if (state == AlarmState::Disabled) {
        toggle();
    }
}

// Toggles between enabled and disabled 
void AlarmClock::toggle() {
    if (state == AlarmState::Disabled) {
        state = AlarmState::Enabled;
        if (!isBuzzerInited) {
            init();
            isBuzzerInited = true;
        }
    } else state = AlarmState::Disabled;
}

// Function to initilize the buzzer_thread
void AlarmClock::init() {
    buzzer_thread.start(callback(this, &AlarmClock::buzzer_task));
}

// Getters to display the alarm clock and a getter for the AlarmClockState (Used in button checking and timeView)
AlarmState AlarmClock::get_AlarmClock_State() const {return state;}
int AlarmClock::get_hour() const {return hour;}
int AlarmClock::get_minute() const {return minute;}
