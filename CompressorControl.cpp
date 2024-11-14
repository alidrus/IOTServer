// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#define COMPRESSOR_CONTROL_CPP

#include <IRremote.h>

#include "CompressorControl.h"
#include "Pins.h"


void CompressorControl::turnACOnOff() {
    Serial.println("CompressorControl::turnACOnOff() executing irsend.sendRaw(txPowerToggle)");

    IRsend irsend(IR_PIN);

    irsend.sendRaw(this->txPowerToggle, sizeof(this->txPowerToggle) / sizeof(this->txPowerToggle[0]), IR_CARRIER_FREQUENCY);
}

void CompressorControl::turnCompressorOn() {
    Serial.println("CompressorControl::turnCompressorOn() executing irsend.sendRaw(txCoolingMode)");

    IRsend irsend(IR_PIN);

    irsend.sendRaw(this->txCoolingMode, sizeof(this->txCoolingMode) / sizeof(this->txCoolingMode[0]), IR_CARRIER_FREQUENCY);
}

void CompressorControl::turnCompressorOff() {
    Serial.println("CompressorControl::turnCompressorOff() executing irsend.sendRaw(txFanMode)");

    IRsend irsend(IR_PIN);

    irsend.sendRaw(this->txFanMode, sizeof(this->txFanMode) / sizeof(this->txFanMode[0]), IR_CARRIER_FREQUENCY);
}
