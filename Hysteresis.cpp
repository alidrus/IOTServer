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

    int compressorAction = COMPRESSOR_NO_CHANGE;

    if (this->acStartTime == 0) {
        this->acStartTime = currentTime;
    }

    // Determine if hysteresis should be applied
    bool applyHysteresis = (currentTime - this->acStartTime > INITIAL_COOLDOWN_TIME);

    // Check if compressor should be turned ON
    if (currentDewPoint > (this->targetDewPoint + this->hysteresisBuffer)
            && !this->compressorIsOn
            && (((currentTime - this->compressorRestStartTime) > MIN_REST_TIME) || !applyHysteresis))
    {
        // Turn compressor ON if heat index is above target and rest time has passed
        this->compressorIsOn = true;
        this->compressorRunStartTime = currentTime;
        compressorAction = COMPRESSOR_TURN_ON;
    }

    // Check if compressor should be turned OFF
    if (this->compressorIsOn
            && ((applyHysteresis && currentDewPoint < (this->targetDewPoint - this->hysteresisBuffer))  // Apply hysteresis after cooldown time
                || (!applyHysteresis && currentDewPoint < this->targetDewPoint)                         // No hysteresis during initial cooldown
                || ((currentTime - this->compressorRunStartTime) > MAX_RUN_TIME)))                       // Max runtime safety cutoff
    {
        // Turn compressor OFF if any of the above conditions are met
        this->compressorIsOn = false;
        this->compressorRestStartTime = currentTime; // Start rest timer
        compressorAction = COMPRESSOR_TURN_OFF;
    }

    // Failsafe to prevent compressor run-on:
    // Compressor was supposedly turned off but the dew point is a lot lower than the threshold
    if (!this->compressorIsOn && currentDewPoint < (this->targetDewPoint - (1.5 * this->hysteresisBuffer)))
    {
        // Turn compressor OFF and reset compressor rest start time
        this->compressorIsOn = false;
        this->compressorRestStartTime = currentTime; // Start rest timer
        compressorAction = COMPRESSOR_TURN_OFF;
    }
    // Failsafe to prevent overheating:
    // Compressor was supposedly turned on but the dew point is a lot higher than the threshold
    else if (this->compressorIsOn && currentDewPoint > (this->targetDewPoint + (1.5 * this->hysteresisBuffer)))
    {
        // Turn compressor ON and reset compressor run start time
        this->compressorIsOn = true;
        this->compressorRunStartTime = currentTime;
        compressorAction = COMPRESSOR_TURN_ON;
    }

    // Run compressor control action only once
    switch(compressorAction) {
        case COMPRESSOR_TURN_OFF:
            compressorController.turnCompressorOff(); // Send fan mode signal
            break;
        case COMPRESSOR_TURN_ON:
            compressorController.turnCompressorOn(); // Send cooling mode signal
            break;
    }
}

float Hysteresis::getTargetDewPoint() const {
    Serial.println("Hysteresis::getTargetDewPoint()");

    return this->targetDewPoint;
}

void Hysteresis::setTargetDewPoint(float targetDewPoint) {
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

bool Hysteresis::getCompressorIsOn() {
    return this->compressorIsOn;
}
