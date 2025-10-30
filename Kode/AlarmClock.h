#include "DFRobot_RGBLCD1602.h"


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
    
    void update(time_t currentTime);
    void setTimer(int hour, int minute);
    void enable(bool is_on);
    void snooze();
    void mute();

    AlarmState get_AlarmClock_State() const; 
    int get_hour() const; 
    int get_minute() const;
    void set_alarm(int hour, int minute);

private:
    AlarmState state;

    time_t snoozeTimer;     
    time_t activeTimer; 

    static const int SNOOZ_DURATION = 5*60; 
    static const int AUTO_MUTE_TIMER = 10*60;

    void active();
    void deactivate();
    int hour; 
    int minute;
};

#endif //ALARMCLOCK_H
