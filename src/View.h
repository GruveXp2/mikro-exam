#include "DFRobot_RGBLCD1602.h"


#ifndef VIEW_H
#define VIEW_H

class Menu;


class View {
    // Holder styr på alle visninger/"screens".
    // Hver view viser info om noe, feks klima, posisjon, alarm, etc
public:
    View(Menu* menu, int& buttonFlags);
    virtual ~View() = default;

    virtual void checkButtons() {}; // her kan man sjekke om knapper er trykt
    virtual void draw(DFRobot_RGBLCD1602* lcd) {}; // viser tekst fra viewen på skjermen
    bool isButtonPressed(int buttonIntex); // sjekker om en knapp er trykt
protected:
    int* buttonFlags; // de 4 første bitsa tilsvarer de 4 knappene
    Menu* menu;
};

#endif //VIEW_H