#include "view.h"

#ifndef SETLOCATIONVIEW_H
#define SETLOCATIONVIEW_H

class SetLocationView final : public View {
public:
    explicit SetLocationView(Menu* menu, int& buttonFlags);
    virtual ~SetLocationView() = default;

    void checkButtons() override;
    void draw(DFRobot_RGBLCD1602* lcd) override;
    void update();

private:
    time_t lastUpdate = 0;
    std::string& latitude;
    std::string& longitude;
};

#endif //SETLOCATIONVIEW_H