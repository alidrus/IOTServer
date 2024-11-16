// vim: syntax=arduino autoindent expandtab tabstop=4 shiftwidth=4 softtabstop=4:

/*
 * The ifndef tells the compiler to only compile the following lines if it has
 * not been done before
 */
#ifndef HYSTERESIS_H
#define HYSTERESIS_H

// Configurable limits
#define MAX_RUN_TIME 360000                   // Max compressor run time in milliseconds (7 minutes)
#define MIN_REST_TIME 120000                  // Min compressor rest time in milliseconds (3 minutes)
#define INITIAL_COOLDOWN_TIME 900000          // Initial pre-hysteresis cooling period in milliseconds (15 minutes)

class Hysteresis
{
    public:
        Hysteresis(float targetDewPoint = 20, float hysteresisBuffer = 0.25)
        {
            this->targetDewPoint = targetDewPoint;
            this->hysteresisBuffer = hysteresisBuffer;
        }

        void monitorComfort(float currentDewPoint);

        float getTargetDewPoint() const;

        void setTargetDewPoint(unsigned long targetDewPoint);

        void incrementTargetDewPoint();

        void decrementTargetDewPoint();

    protected:
        float targetDewPoint;                       // Heat index is what the temperature "feels like"
        float hysteresisBuffer;                     // Buffer to prevent rapid cycling after initial cooldown

        bool compressorOn = false;                  // Is the compressor running?

        unsigned long compressorStartTime = 0;      // When the compressor started running
        unsigned long compressorRestStartTime = 0;  // When the compressor started resting
        unsigned long acStartTime = 0;              // Track the overall start time of the A/C
};

#endif
