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
        megalovania = true; // alarm initially plays megalovania
        while (!(flags.get() & BUZZER_STOP)) {
            if (megalovania) {
                bool result = playMegalovania(); // result is false if it was stopped before finishing
                // if it wasnt stopped and finished playing (returning true), megalovania will be false and it will play regular beeping instead
                if (!result) break; else megalovania = false;
                continue;
            }
            // beep 2 times/s
            buzzer.write(beep ? 0.5f : 0.0f);
            beep = !beep;
            flags.wait_any_for(BUZZER_STOP, 250ms, false); // wait 250ms, but immediately skip waiting if alarm gets stopped
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

void AlarmClock::playTone(float frequency, float duration) {
    if (frequency == 0) {
        buzzer.write(0); // pause
    } else {
        buzzer.period(1.0 / frequency); // start tone
        buzzer.write(0.5);
    }
    wait_us(duration * 1000000);

    buzzer.write(0); // stop tone
}

bool AlarmClock::playMegalovania() {
    struct Note {
        float frequency;
        float duration;
    };

    Note melody[] = {
        {294.00, 0.125}, // D4
        {294.00, 0.125}, // D4
        {587.00, 0.125}, // D5
        {     0, 0.125}, // D5
        {440.00, 0.125}, // A4
        {     0, 0.250}, // D5
        {415.00, 0.125}, // Ab4
        {     0, 0.125}, // D5
        {392.00, 0.125}, // G4
        {     0, 0.125}, // D5
        {349.00, 0.250}, // F4
        {294.00, 0.125}, // D4
        {349.00, 0.125}, // F4
        {392.00, 0.125}  // G4
    };

    int melodySize = sizeof(melody) / sizeof(Note);
    for (int i = 0; i < melodySize; i++) {
        playTone(melody[i].frequency, melody[i].duration);
        if (flags.get() & BUZZER_STOP) return false; // return false if the alarm was muted/snoozed
    }
    return true;
}
