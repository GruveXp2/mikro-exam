#include "View.h"
#include "AlarmClock.h"

#ifndef SETALARMVIEW_H
#define SETALARMVIEW_H


// The menu will look like this:
// ----------------
// setting  00:00
//  alarm   <->
//
// <-> is the cursor, ^ means you are setting a number on a secific digit

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
};

#endif //SETALARMVIEW_H