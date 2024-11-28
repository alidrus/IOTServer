// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE

#include "Pins.h"
#include "CompressorControl.h"

#include <IRremote.hpp>

bool CompressorControl::isOn = false;

unsigned long CompressorControl::runStartTime = 0;

unsigned long CompressorControl::restStartTime = 0;

void CompressorControl::initialize() {
    IrSender.begin();
}

void CompressorControl::toggleACOnOff() {
    Serial.println("CompressorControl::toggleACOnOff() executing IrSender.sendRaw(txPowerToggle)");

    IrSender.sendRaw(CompressorControl::txPowerToggle, sizeof(CompressorControl::txPowerToggle) / sizeof(CompressorControl::txPowerToggle[0]), NEC_KHZ);

    CompressorControl::isOn = false;
}

void CompressorControl::turnOn() {
    Serial.println("CompressorControl::turnOn() executing IrSender.sendRaw(txCoolingMode)");

    IrSender.sendRaw(CompressorControl::txCoolingMode, sizeof(CompressorControl::txCoolingMode) / sizeof(CompressorControl::txCoolingMode[0]), NEC_KHZ);

    CompressorControl::isOn = true;

    CompressorControl::runStartTime = millis();
}

void CompressorControl::turnOff() {
    Serial.println("CompressorControl::turnOff() executing IrSender.sendRaw(txFanMode)");

    IrSender.sendRaw(CompressorControl::txFanMode, sizeof(CompressorControl::txFanMode) / sizeof(CompressorControl::txFanMode[0]), NEC_KHZ);

    CompressorControl::isOn = false;

    CompressorControl::restStartTime = millis();
}
