// vim: syntax=cpp autoindent smartindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

/*
 * This include is needed so that Arduino data types are recognized by the
 * compiler.
 */
#include <Arduino.h>

#define LED_BUILTIN 2
#define LED_ON HIGH
#define LED_OFF LOW

class MyLED
{
    public:
        bool isOn();
        void turnOn(bool force);
        void turnOff(bool force);

    protected:
        bool ledIsOn = false;
};
