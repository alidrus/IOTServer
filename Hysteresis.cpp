// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#define HYSTERESIS_CPP

#include "CompressorControl.h"
#include "Hysteresis.h"

void Hysteresis::monitorComfort(float currentHeatIndex) {
    CompressorControl compressorController;

    const unsigned long currentTime = millis(); // Function to get current time in seconds

    if (this->acStartTime == 0) {
        this->acStartTime = currentTime;
    }

    // Determine if hysteresis should be applied
    bool applyHysteresis = (currentTime - this->acStartTime >= INITIAL_COOLDOWN_TIME);

    // Check if compressor should be turned ON
    if (currentHeatIndex > this->targetHeatIndex && !this->compressorOn && (currentTime - this->compressorRestStartTime >= MIN_REST_TIME)) {
        // Turn compressor ON if heat index is above target and rest time has passed
        this->compressorOn = true;
        this->compressorStartTime = currentTime;
        compressorController.turnCompressorOn(); // Send cooling mode signal
    }

    // Check if compressor should be turned OFF
    if (this->compressorOn
            && ((applyHysteresis && currentHeatIndex <= this->targetHeatIndex - this->hysteresisBuffer) // Apply hysteresis after cooldown time
                || (!applyHysteresis && currentHeatIndex <= this->targetHeatIndex)                // No hysteresis during initial cooldown
                || (currentTime - this->compressorStartTime >= MAX_RUN_TIME)))                    // Max runtime safety cutoff
    {
        // Turn compressor OFF if any of the above conditions are met
        this->compressorOn = false;
        this->compressorRestStartTime = currentTime; // Start rest timer
        compressorController.turnCompressorOff(); // Send fan mode signal
    }
}

float Hysteresis::getTargetHeatIndex() const {
    return this->targetHeatIndex;
}

void Hysteresis::setTargetHeatIndex(unsigned long targetHeatIndex) {
    this->targetHeatIndex = targetHeatIndex;
}

void Hysteresis::incrementTargetHeatIndex() {
    this->targetHeatIndex += 0.5;
}

void Hysteresis::decrementTargetHeatIndex() {
    this->targetHeatIndex -= 0.5;
}
