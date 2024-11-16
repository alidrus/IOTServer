// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

/*
 * The ifndef tells the compiler to only compile the following lines if it has
 * not been done before
 */
#ifndef HYSTERESIS_H
#define HYSTERESIS_H

// Configurable limits
#define MAX_RUN_TIME 600000                   // Max compressor run time in milliseconds (600,000 ms = 10 minutes)
#define MIN_REST_TIME 180000                  // Min compressor rest time in milliseconds (180,000 ms = 3 minutes)
#define INITIAL_COOLDOWN_TIME 1200000         // Initial pre-hysteresis cooling period in milliseconds (1,200,000 ms = 20 minutes)

class Hysteresis
{
    public:
        Hysteresis(float targetDewPoint = 20.0, float hysteresisBuffer = 0.25)
        {
            this->targetDewPoint = targetDewPoint;
            this->hysteresisBuffer = hysteresisBuffer;
        }

        void monitorComfort(float currentDewPoint);

        float getTargetDewPoint() const;

        void setTargetDewPoint(float targetDewPoint);

        void incrementTargetDewPoint();

        void decrementTargetDewPoint();

    protected:
        float targetDewPoint;                       // Heat index is what the temperature "feels like"
        float hysteresisBuffer;                     // Buffer to prevent rapid cycling after initial cooldown

        bool compressorIsOn = false;                // Is the compressor running?

        unsigned long compressorRunStartTime = 0;   // When the compressor started running
        unsigned long compressorRestStartTime = 0;  // When the compressor started resting
        unsigned long acStartTime = 0;              // Track the overall start time of the A/C
};

#endif
