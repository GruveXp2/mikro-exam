#include "DFRobot_RGBLCD1602.h"
#include "View.h"
#include <cstdio>
#include <string>
#include <vector>


#ifndef NEWSFEED_H
#define NEWSFEED_H
#define BUFFER_SIZE 520

class NewsFeed final : public View {
public:
    explicit NewsFeed(Menu* menu, int& buttonFlags, NetworkInterface* network);
    virtual ~NewsFeed() = default;

    void checkButtons() override;
    void draw(DFRobot_RGBLCD1602* lcd) override;
    void update();

private:    
    time_t lastUpdate = 0;
    NetworkInterface* network;
    std::vector<std::string> headlines;
};

#endif //NEWSFEED_H