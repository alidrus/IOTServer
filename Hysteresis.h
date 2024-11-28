// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

/*
 * The ifndef tells the compiler to only compile the following lines if it has
 * not been done before
 */
#ifndef HYSTERESIS_H
#define HYSTERESIS_H

#include "CompressorControl.h"

// Configurable limits
#define MAX_RUN_TIME 900000                   // Max compressor run time in milliseconds (900,000 ms = 15 minutes)
#define MIN_REST_TIME 180000                  // Min compressor rest time in milliseconds (180,000 ms = 3 minutes)
#define INITIAL_COOLDOWN_TIME 1200000         // Initial pre-hysteresis cooling period in milliseconds (1,200,000 ms = 20 minutes)

#define COMPRESSOR_TURN_OFF -1
#define COMPRESSOR_NO_CHANGE 0
#define COMPRESSOR_TURN_ON 1

class Hysteresis
{
    public:
        Hysteresis(float targetDewPoint = 19.0, float hysteresisBuffer = 0.5)
        {
            this->targetDewPoint = targetDewPoint;
            this->hysteresisBuffer = hysteresisBuffer;
        }

        void monitorComfort(float currentDewPoint);

        float getTargetDewPoint() const;

        void setTargetDewPoint(float targetDewPoint);

        void incrementTargetDewPoint();

        void decrementTargetDewPoint();

        bool compressorIsOn();

    protected:
        float targetDewPoint;                       // Heat index is what the temperature "feels like"

        float hysteresisBuffer;                     // Buffer to prevent rapid cycling after initial cooldown

        unsigned long acStartTime = 0;              // Track the overall start time of the A/C
};

#endif
