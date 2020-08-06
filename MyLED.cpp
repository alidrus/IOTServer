// vim: syntax=cpp ai expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#include "MyLED.h"

bool MyLED::isOn() {
    return this->ledIsOn;
}

void MyLED::turnOn(bool force = false) {
    if (!this->ledIsOn || force)
    {
        digitalWrite(LED_BUILTIN, LED_ON);

        this->ledIsOn = true;
    }
}

void MyLED::turnOff(bool force = false) {
    if (this->ledIsOn || force)
    {
        digitalWrite(LED_BUILTIN, LED_OFF);

        this->ledIsOn = false;
    }
}
