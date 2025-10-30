#include "DFRobot_RGBLCD1602.h"
#include "View.h"
#include "AlarmClock.h"

#ifndef TIMEVIEW_H
#define TIMEVIEW_H

class TimeView final : public View {
public:
    explicit TimeView(Menu* menu, int& buttonFlags, AlarmClock& alarmClock);
    virtual ~TimeView() = default;

    void checkButtons() override;
    void draw(DFRobot_RGBLCD1602* lcd) override;

private:
    time_t lastUpdate = 0;
    AlarmClock& alarmClock;

};

#endif //TIMEVIEW_H