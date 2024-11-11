// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#include <IRremote.h>

#define IR_CARRIER_FREQUENCY  38

#include "CompressorControl.h"

int irPin = 4;

IRsend irsend(irPin);

void turnACOnOff() {
    irsend.sendRaw(txPowerToggle, sizeof(txPowerToggle) / sizeof(txPowerToggle[0]), IR_CARRIER_FREQUENCY);
}

void turnCompressorOn() {
    irsend.sendRaw(txCoolingMode, sizeof(txCoolingMode) / sizeof(txCoolingMode[0]), IR_CARRIER_FREQUENCY);
}

void turnCompressorOff() {
    irsend.sendRaw(txFanMode, sizeof(txFanMode) / sizeof(txFanMode[0]), IR_CARRIER_FREQUENCY);
}
