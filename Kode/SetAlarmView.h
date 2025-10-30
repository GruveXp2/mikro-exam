#include "View.h"
#include "AlarmClock.h"

#ifndef SETALARMVIEW_H
#define SETALARMVIEW_H

class SetAlarmView final : public View {
public:
    explicit SetAlarmView(Menu* menu, int& buttonFlags, AlarmClock& AlarmClock);
    virtual ~SetAlarmView() = default;

    void checkButtons() override;
    void draw(DFRobot_RGBLCD1602* lcd) override;
    void update();

private:
    AlarmClock& alarmClock;

    int hour;
    int minute;

    bool isSelectingDigit = true;
    int* currentlySetting;

    int currentDigit = 0;
    // ----------------
    // setting  00:00
    //  alarm   <->
};

#endif //SETALARMVIEW_H