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
    

private:
    size_t scrollingIndex = 5; 
    size_t headlineIndex = 0;
    static const int lcdWidth = 16;
    
    NetworkInterface* network;
    std::vector<std::string> headlines;

    Thread update_thread;
    Mutex newsFeed_mutex;

    void update();
    void thread_task();
};

#endif //NEWSFEED_H