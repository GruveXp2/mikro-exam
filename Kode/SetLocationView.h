#include "View.h"

#ifndef SETLOCATIONVIEW_H
#define SETLOCATIONVIEW_H

class SetLocationView final : public View {
public:
    explicit SetLocationView(Menu* menu, int& buttonFlags, std::string& longitude, std::string& latitude);
    virtual ~SetLocationView() = default;

    void checkButtons() override;
    void draw(DFRobot_RGBLCD1602* lcd) override;
    void update();

private:
    std::string& longitude;
    std::string& latitude;

    int currentSubView = 0;

    bool isSettingLongitude = true;
    std::string* currentlySetting;

    int symbolIndex = 0;
    int symbolId = 0;
    static const std::string symbols;
};

#endif //SETLOCATIONVIEW_H