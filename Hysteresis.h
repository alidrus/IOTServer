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
#define INITIAL_COOLDOWN_TIME 1200000         // Initial pre-hysteresis cooling period in milliseconds (20 minutes)

class Hysteresis
{
    public:
        Hysteresis(float heatIndexTarget = 26, float hysteresisBuffer = 0.5)
        {
            this->heatIndexTarget = heatIndexTarget;
            this->hysteresisBuffer = hysteresisBuffer;
        }

        void monitorComfort(float currentHeatIndex);

        float getHeatIndexTarget() const;

        void setHeatIndexTarget(unsigned long heatIndexTarget);

        void incrementHeatIndexTarget();

        void decrementHeatIndexTarget();

    protected:
        float heatIndexTarget;                   // "Feels-like" target
        float hysteresisBuffer = 0.5;                 // Buffer to prevent rapid cycling after initial cooldown

        bool compressorOn = false;

        unsigned long compressorStartTime = 0;
        unsigned long compressorRestStartTime = 0;
        unsigned long acStartTime = 0; // Track the overall start time of the A/C
};

#endif
