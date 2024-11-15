// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#define COMPRESSOR_CONTROL_CPP
#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE

#include "Pins.h"
#include "CompressorControl.h"

#include <IRremote.hpp>

CompressorControl::CompressorControl() {
    IrSender.begin();
}

void CompressorControl::turnACOnOff() {
    Serial.println("CompressorControl::turnACOnOff() executing IrSender.sendRaw(txPowerToggle)");

    IrSender.sendRaw(this->txPowerToggle, sizeof(this->txPowerToggle) / sizeof(this->txPowerToggle[0]), NEC_KHZ);
}

void CompressorControl::turnCompressorOn() {
    Serial.println("CompressorControl::turnCompressorOn() executing IrSender.sendRaw(txCoolingMode)");

    IrSender.sendRaw(this->txCoolingMode, sizeof(this->txCoolingMode) / sizeof(this->txCoolingMode[0]), NEC_KHZ);
}

void CompressorControl::turnCompressorOff() {
    Serial.println("CompressorControl::turnCompressorOff() executing IrSender.sendRaw(txFanMode)");

    IrSender.sendRaw(this->txFanMode, sizeof(this->txFanMode) / sizeof(this->txFanMode[0]), NEC_KHZ);
}
