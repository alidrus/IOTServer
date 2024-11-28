// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

/*
 * The ifndef tells the compiler to only compile the following lines if it has
 * not been done before
 */
#ifndef COMPRESSOR_CONTROL_H
#define COMPRESSOR_CONTROL_H

#include <Arduino.h>

class CompressorControl {
    public:
        static void initialize();

        static void toggleACOnOff();

        static void turnOn();

        static void turnOff();

        static bool isOn;

        static unsigned long runStartTime;

        static unsigned long restStartTime;

    protected:
        static constexpr uint16_t txPowerToggle[138] = {
            9788, 9676, 9812, 9680, 4652, 2408, 432, 296, 436, 868,
            436, 868, 432, 296, 432, 872, 432, 296, 436, 296,
            432, 296, 432, 300, 432, 296, 432, 868, 436, 296,
            432, 296, 436, 868, 432, 300, 432, 296, 436, 292,
            432, 872, 432, 296, 436, 296, 432, 296, 432, 296,
            436, 868, 436, 296, 432, 868, 436, 296, 432, 872,
            432, 296, 432, 300, 432, 292, 436, 296, 432, 300,
            432, 296, 432, 296, 436, 296, 432, 296, 436, 864,
            436, 296, 436, 296, 432, 296, 432, 300, 432, 296,
            432, 296, 436, 296, 432, 868, 436, 296, 432, 296,
            436, 296, 432, 296, 432, 296, 436, 1100, 204, 292,
            436, 296, 432, 868, 436, 296, 432, 300, 432, 868,
            432, 300, 432, 868, 436, 868, 432, 868, 436, 296,
            432, 868, 436, 868, 436, 20048, 4652
        };

        static constexpr uint16_t txFanMode[138] = {
            9788, 9676, 9816, 9676, 4656, 2404, 436, 296, 436, 864,
            436, 868, 436, 296, 432, 868, 436, 328, 404, 288,
            440, 296, 432, 296, 436, 292, 436, 868, 436, 296,
            432, 296, 436, 864, 436, 296, 436, 296, 436, 324,
            404, 864, 436, 868, 436, 296, 432, 868, 436, 868,
            436, 296, 432, 296, 436, 864, 436, 296, 436, 864,
            440, 292, 436, 296, 432, 296, 436, 328, 400, 328,
            404, 292, 436, 296, 432, 296, 436, 296, 432, 868,
            436, 296, 432, 296, 436, 292, 436, 292, 436, 296,
            436, 296, 432, 296, 436, 864, 436, 296, 436, 328,
            400, 296, 436, 868, 432, 868, 436, 868, 436, 292,
            436, 296, 432, 872, 432, 328, 404, 292, 436, 868,
            436, 296, 432, 868, 436, 296, 432, 868, 436, 868,
            436, 292, 436, 868, 436, 20044, 4656
        };

        static constexpr uint16_t txCoolingMode[138] = {
            9788, 9680, 9812, 9680, 4652, 2408, 432, 300, 428, 868,
            436, 868, 436, 300, 428, 872, 432, 296, 432, 300,
            428, 300, 432, 300, 432, 868, 432, 300, 428, 300,
            432, 300, 428, 872, 432, 300, 432, 296, 432, 300,
            428, 300, 432, 868, 432, 300, 432, 300, 428, 300,
            428, 872, 432, 300, 432, 868, 432, 300, 432, 868,
            432, 300, 432, 300, 432, 296, 432, 296, 432, 300,
            432, 296, 432, 300, 428, 300, 432, 296, 432, 872,
            432, 300, 428, 300, 432, 300, 428, 300, 428, 300,
            432, 300, 428, 300, 432, 868, 432, 300, 432, 296,
            432, 300, 428, 304, 428, 300, 428, 872, 432, 300,
            432, 300, 428, 868, 432, 300, 432, 296, 432, 872,
            432, 300, 432, 868, 432, 300, 432, 868, 432, 300,
            432, 868, 432, 300, 432, 20052, 4652
        };
};

#endif
