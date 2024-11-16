// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#define HYSTERESIS_CPP

#include "CompressorControl.h"
#include "Hysteresis.h"

void Hysteresis::monitorComfort(float currentDewPoint) {
    Serial.print("Hysteresis::monitorComfort(");
    Serial.print(currentDewPoint);
    Serial.println(")");

    CompressorControl compressorController;

    const unsigned long currentTime = millis(); // Function to get current time in seconds

    if (this->acStartTime == 0) {
        this->acStartTime = currentTime;
    }

    // Determine if hysteresis should be applied
    bool applyHysteresis = (currentTime - this->acStartTime >= INITIAL_COOLDOWN_TIME);

    // Check if compressor should be turned ON
    if (currentDewPoint > this->targetDewPoint && !this->compressorOn && (currentTime - this->compressorRestStartTime >= MIN_REST_TIME)) {
        // Turn compressor ON if heat index is above target and rest time has passed
        this->compressorOn = true;
        this->compressorStartTime = currentTime;
        compressorController.turnCompressorOn(); // Send cooling mode signal
    }

    // Check if compressor should be turned OFF
    if (this->compressorOn
            && ((applyHysteresis && currentDewPoint <= this->targetDewPoint - this->hysteresisBuffer) // Apply hysteresis after cooldown time
                || (!applyHysteresis && currentDewPoint <= this->targetDewPoint)                // No hysteresis during initial cooldown
                || (currentTime - this->compressorStartTime >= MAX_RUN_TIME)))                    // Max runtime safety cutoff
    {
        // Turn compressor OFF if any of the above conditions are met
        this->compressorOn = false;
        this->compressorRestStartTime = currentTime; // Start rest timer
        compressorController.turnCompressorOff(); // Send fan mode signal
    }
}

float Hysteresis::getTargetDewPoint() const {
    Serial.println("Hysteresis::getTargetDewPoint()");

    return this->targetDewPoint;
}

void Hysteresis::setTargetDewPoint(unsigned long targetDewPoint) {
    Serial.print("Hysteresis::setTargetDewPoint(");
    Serial.print(targetDewPoint);
    Serial.println(")");

    this->targetDewPoint = targetDewPoint;
}

void Hysteresis::incrementTargetDewPoint() {
    Serial.println("Hysteresis::incrementTargetDewPoint()");

    this->targetDewPoint += 0.5;
}

void Hysteresis::decrementTargetDewPoint() {
    Serial.println("Hysteresis::decrementTargetDewPoint()");

    this->targetDewPoint -= 0.5;
}
