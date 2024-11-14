// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#define COMPRESSOR_CONTROL_CPP

#include <IRremote.h>

#include "CompressorControl.h"
#include "Pins.h"

void CompressorControl::turnACOnOff() {
    IRsend irsend(IR_PIN);
    irsend.sendRaw(this->txPowerToggle, sizeof(this->txPowerToggle) / sizeof(this->txPowerToggle[0]), IR_CARRIER_FREQUENCY);
}

void CompressorControl::turnCompressorOn() {
    IRsend irsend(IR_PIN);
    irsend.sendRaw(this->txCoolingMode, sizeof(this->txCoolingMode) / sizeof(this->txCoolingMode[0]), IR_CARRIER_FREQUENCY);
}

void CompressorControl::turnCompressorOff() {
    IRsend irsend(IR_PIN);
    irsend.sendRaw(this->txFanMode, sizeof(this->txFanMode) / sizeof(this->txFanMode[0]), IR_CARRIER_FREQUENCY);
}
