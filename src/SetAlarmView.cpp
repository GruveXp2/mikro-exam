#include <string>

#include "SetAlarmView.h"
#include "Menu.h"
#include "AlarmClock.h"

#define TIME_LENGTH 4 // 4 symbols for time, hhmm

SetAlarmView::SetAlarmView(Menu* menu, int& buttonFlags, AlarmClock& alarmClock)
        : View(menu, buttonFlags), alarmClock(alarmClock), hour(alarmClock.get_hour()), minute(alarmClock.get_minute()), isSelectingDigit(true) {
}

void SetAlarmView::draw(DFRobot_RGBLCD1602* lcd) {
    lcd->setCursor(0, 0);
    lcd->printf("setting  %02d:%02d", hour, minute);

    if (isSelectingDigit) {
        lcd->setCursor(0, 1);
        lcd->printf(" alarm          "); // extra spaces to remove the <->
        // <-> symbolizes teh cursor, you can move left or right to select a different digit
        // if no digis is to the left, the < isnt gonna be shown. same for the right
        if (currentDigit == 0) {
            lcd->setCursor(9    , 1);
            lcd->printf("->");
        } else if (currentDigit == 3) {
            lcd->setCursor(12, 1);
            lcd->printf("<-");
        } else {
            lcd->setCursor(7 + currentDigit * 2, 1); // math to make it skip the : between hour and minute
            lcd->printf("<->");
        }
    } else {
        lcd->setCursor(0, 1);
        lcd->printf("                ");
        int position = 9 + currentDigit; // start of hh:mm
        if (currentDigit > 1) position++; // move 1 more to the right if currentDigit is 2 or 3, bc then it means were changing the minutes which is after the ':'
        lcd->setCursor(position, 1);
        lcd->printf("^");
    }
}

void SetAlarmView::checkButtons() {
    if (isSelectingDigit) { // selecting a digits with the cursor
        if (isButtonPressed(0)) { // save alarm and return to time view
            alarmClock.setTimer(hour, minute);
            menu->showView(ViewType::TIME);
        } else if (isButtonPressed(1)) { // move cursor to the left
            currentDigit --;
            if (currentDigit < 0) { // wrap around
                currentDigit = TIME_LENGTH - 1;
            }
        } else if (isButtonPressed(2)) { // move cursor to the right
            currentDigit ++;
            if (currentDigit >= TIME_LENGTH) { // wrap around
                currentDigit = 0;
            }
        } else if (isButtonPressed(3)) { // selects the digit the cursor is pointing to
            isSelectingDigit = !isSelectingDigit;
        }
    } else { // selecting a number for the current digit
        if (isButtonPressed(0) || isButtonPressed(3)) { // save digit and return to moving cursor
            isSelectingDigit = !isSelectingDigit;
            return;
        }
        int change = isButtonPressed(1) ? -1 : 1; // shoud the number go up or down
        bool editingHour = currentDigit < 2; // first 2 digits is hour, last 2 is minutes
        bool editingTens = currentDigit % 2 == 0;

        if (editingHour) {
            int tens = hour / 10;
            int ones = hour % 10;

            if (editingTens) {
                tens = (tens + change + 3) % 3; // the hours can be only 0,1,2, bc 24 is max
                if (tens == 2 && ones > 3) tens = 1; // if hour is 9 and tens--, it wont go to 29 but instead 19
            } else {
                ones = (ones + change + 10) % 10;
                if (tens == 2 && ones > 3) ones = 3; // if hour is 20 and ones--, it wont go to 29 but instead 23
            }
            hour = tens * 10 + ones;
        } else {
            int tens = minute / 10;
            int ones = minute % 10;

            if (editingTens) {
                tens = (tens + change + 6) % 6; // 0–5
            } else {
                ones = (ones + change + 10) % 10;
            }

            minute = tens * 10 + ones;
        }
    }
}