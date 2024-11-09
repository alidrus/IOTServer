float heatIndexTarget = 27;                 // "Feels-like" target
float hysteresisBuffer = 0.5;               // Buffer to prevent rapid cycling after initial cooldown
bool compressorOn = false;
unsigned long compressorStartTime = 0;
unsigned long compressorRestStartTime = 0;
unsigned long acStartTime = getCurrentTime(); // Track the overall start time of the A/C

// Configurable limits
#define MAX_RUN_TIME 360                    // Max compressor run time in seconds (e.g., 6 minutes)
#define MIN_REST_TIME 120                   // Min compressor rest time in seconds (e.g., 2 minutes)
#define INITIAL_COOLDOWN_TIME 1800;         // 30 minutes in seconds
