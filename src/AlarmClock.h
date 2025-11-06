#include "DFRobot_RGBLCD1602.h"
#include "mbed.h"


#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H

enum class AlarmState{
    Disabled, 
    Enabled,
    Active,
    Snooze
};
class AlarmClock {
public:
    AlarmClock();
    
    void update();
    void setTimer(int hour, int minute);
    void toggle();
    void snooze();
    void mute();

    AlarmState get_AlarmClock_State() const; 
    int get_hour() const; 
    int get_minute() const;
    void set_alarm(int hour, int minute);
    void start();
    void init();

private:
    bool isBuzzerInited = false;
    AlarmState state;
    PwmOut buzzer;

    time_t snoozeTimer;     
    time_t activeTimer; 

    static const int SNOOZ_DURATION = 5*60; 
    static const int AUTO_MUTE_TIMER = 10*60;

    void active();
    void deactivate();
    int hour; 
    int minute;
    int lastTriggeredMinute;

    Thread buzzer_thread; 
    EventFlags flags;

    void buzzer_task();

    static constexpr uint32_t BUZZER_ON = 1 << 0;
    static constexpr uint32_t BUZZER_STOP = 1 << 1;

    void playTone(float frequency, float duration);
    bool playMegalovania();
    bool megalovania = true;
};

#endif //ALARMCLOCK_H
