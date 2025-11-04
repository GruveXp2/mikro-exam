#include "Menu.h"
#include "AlarmClock.h"
#include <cstdio>
#include <ctime>

AlarmClock::AlarmClock ()
    : hour(0), minute(0), state(AlarmState::Disabled),
    snoozeTimer(0), activeTimer(0), buzzer(D12) {}

void AlarmClock::buzzer_task(){
    buzzer.period(0.001f);
    buzzer.write(0);
    bool beep = true;

    while(true){
        flags.wait_any(BUZZER_ON);
        printf("The current flags is: %d\n", flags.get());
        while (!(flags.get() & BUZZER_STOP)) {
            buzzer.write(beep ? 0.5f : 0.0f);
            beep = !beep;
            flags.wait_any_for(BUZZER_STOP, 250ms); // hvis noen skrur av alarmen så skippes venting og skrur av med en gang
        }
        printf("The irritating beeping is now finally over\n");
        buzzer.write(0);
        flags.clear(BUZZER_ON | BUZZER_STOP);
    }
}

void AlarmClock::init() {
    buzzer_thread.start(callback(this, &AlarmClock::buzzer_task));
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
    if (state == AlarmState::Disabled) {
        toggle();
    }
}

void AlarmClock::toggle() {
    if (state == AlarmState::Disabled) {
        state = AlarmState::Enabled;
        if (!isBuzzerInited) {
            init();
            isBuzzerInited = true;
        }
    } else state = AlarmState::Disabled;
}

void AlarmClock::snooze(){
    printf("its now gonna snooze\n");
    if (state == AlarmState::Active){
        snoozeTimer = time(NULL) + SNOOZ_DURATION;
        state = AlarmState::Snooze;
        flags.set(BUZZER_STOP);
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
