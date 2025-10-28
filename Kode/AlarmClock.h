#include "DFRobot_RGBLCD1602.h"
#include "View.h"
#include "HTS221Sensor.h"

#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H

enum class AlarmState{
    Disabled, 
    Enabled,
    Active,
    Snooze
};

class AlarmClock final : public View {
public:
    explicit AlarmClock(Menu* menu, int& buttonFlags);
    virtual ~AlarmClock() = default;

    void checkButtons() override;
    void draw(DFRobot_RGBLCD1602* lcd) override;
    void update(time_t currentTime);
    void setTimer(int h, int m);
    void enable(bool is_on);
    void snooze();
    void mute();
private:
    AlarmState state;

    time_t snoozeTimer;     
    time_t activeTimer; 

    static const int SNOOZ_DURATION = 5*60; 
    static const int AUTO_MYTE_TIMER = 10*60;

    void active();
    void deactivate();
};

#endif //ALARMCLOCK_H
