#include "DFRobot_RGBLCD1602.h"
#include "View.h"

#ifndef TIMEVIEW_H
#define TIMEVIEW_H

class TimeView final : public View {
public:
    explicit TimeView(Menu* menu, int& buttonFlags);
    virtual ~TimeView() = default;

    void checkButtons() override;
    void draw(DFRobot_RGBLCD1602* lcd) override;

private:
    int subView = 0;
    time_t lastUpdate = 0;
};

#endif //TIMEVIEW_H