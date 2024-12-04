// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

#include "Hysteresis.h"

void Hysteresis::monitorComfort(float currentDewPoint) {
    Serial.print("Hysteresis::monitorComfort(");
    Serial.print(currentDewPoint);
    Serial.println(")");

    const unsigned long currentTime = millis(); // Function to get current time in seconds

    if (this->acStartTime == 0) {
        this->acStartTime = currentTime;
    }

    // Determine if hysteresis should be applied
    bool applyHysteresis = (currentTime - this->acStartTime > INITIAL_COOLDOWN_TIME);

    // Check if compressor should be turned ON
    if (currentDewPoint > (this->targetDewPoint + this->hysteresisBuffer)
            && !CompressorControl::isOn
            && (((currentTime - CompressorControl::restStartTime) > MIN_REST_TIME) || !applyHysteresis))
    {
        // Turn compressor ON if heat index is above target and rest time has passed
        CompressorControl::turnOn();

        return;
    }

    // Check if compressor should be turned OFF
    if (CompressorControl::isOn
            && ((applyHysteresis && currentDewPoint < (this->targetDewPoint - this->hysteresisBuffer))  // Apply hysteresis after cooldown time
                || (!applyHysteresis && currentDewPoint < this->targetDewPoint)                         // No hysteresis during initial cooldown
                || ((currentTime - CompressorControl::runStartTime) > MAX_RUN_TIME)))                   // Max runtime safety cutoff
    {
        // Turn compressor OFF if any of the above conditions are met
        CompressorControl::turnOff();

        return;
    }

    // Failsafe to prevent compressor run-on:
    // Compressor was supposedly turned off but the dew point is a lot lower than the threshold
    if (!CompressorControl::isOn && currentDewPoint < (this->targetDewPoint - (1.4 * this->hysteresisBuffer)))
    {
        // Turn compressor OFF and reset compressor rest start time
        CompressorControl::turnOff();

        return;
    }

    // Failsafe to prevent overheating:
    // Compressor was supposedly turned on but the dew point is a lot higher than the threshold
    if (CompressorControl::isOn && currentDewPoint > (this->targetDewPoint + (1.4 * this->hysteresisBuffer)))
    {
        // Turn compressor ON and reset compressor run start time
        CompressorControl::turnOn();

        return;
    }
}

float Hysteresis::getTargetDewPoint() const {
    Serial.println("Hysteresis::getTargetDewPoint()");

    return this->targetDewPoint;
}

float Hysteresis::getHysteresisBuffer() const {
    Serial.println("Hysteresis::getHysteresisBuffer()");

    return this->hysteresisBuffer;
}

void Hysteresis::setTargetDewPoint(float targetDewPoint) {
    Serial.print("Hysteresis::setTargetDewPoint(");
    Serial.print(targetDewPoint);
    Serial.println(")");

    this->targetDewPoint = targetDewPoint;
}

void Hysteresis::incrementTargetDewPoint() {
    Serial.println("Hysteresis::incrementTargetDewPoint()");

    this->targetDewPoint += 0.1;
}

void Hysteresis::decrementTargetDewPoint() {
    Serial.println("Hysteresis::decrementTargetDewPoint()");

    this->targetDewPoint -= 0.1;
}

bool Hysteresis::compressorIsOn() {
    return CompressorControl::isOn;
}
