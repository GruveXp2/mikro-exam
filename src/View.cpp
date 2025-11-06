#include "View.h"
#include "Menu.h"

View::View(Menu* menu, int& buttonFlags) {
    this->menu = menu;
    this->buttonFlags = &buttonFlags;
}

bool View::isButtonPressed(int buttonIntex) {
    return *buttonFlags & (1 << buttonIntex);
}