let heatIndexTarget = 27; // "Feels-like" target
let hysteresisBuffer = 0.5; // Buffer to prevent rapid cycling after initial cooldown
let compressorOn = false;
let compressorStartTime = 0;
let compressorRestStartTime = 0;
let acStartTime = 0; // Track the overall start time of the A/C

// Configurable limits
const MAX_RUN_TIME = 360; // Max compressor run time in seconds (e.g., 6 minutes)
const MIN_REST_TIME = 120; // Min compressor rest time in seconds (e.g., 2 minutes)
const INITIAL_COOLDOWN_TIME = 1800; // 30 minutes in seconds

function turnACOnOff() {
    // Handle the A/C power signal
    acStartTime = getCurrentTime(); // Reset the A/C start time
    // Send signal to turn A/C on or off
}

function monitorComfort() {
    const currentTime = getCurrentTime(); // Function to get current time in seconds
    let currentHeatIndex = calculateHeatIndex(temperature, humidity);

    // Determine if hysteresis should be applied
    let applyHysteresis = (currentTime - acStartTime >= INITIAL_COOLDOWN_TIME);

    // Check if compressor should be turned ON
    if (currentHeatIndex > heatIndexTarget && !compressorOn && (currentTime - compressorRestStartTime >= MIN_REST_TIME)) {
        // Turn compressor ON if heat index is above target and rest time has passed
        compressorOn = true;
        compressorStartTime = currentTime;
        turnCompressorOn(); // Send cooling mode signal
    }

    // Check if compressor should be turned OFF
    if (compressorOn && (
        (applyHysteresis && currentHeatIndex <= heatIndexTarget - hysteresisBuffer) || // Apply hysteresis after cooldown time
        (!applyHysteresis && currentHeatIndex <= heatIndexTarget) ||                   // No hysteresis during initial cooldown
        (currentTime - compressorStartTime >= MAX_RUN_TIME)                              // Max runtime safety cutoff
    )) {
        // Turn compressor OFF if any of the above conditions are met
        compressorOn = false;
        compressorRestStartTime = currentTime; // Start rest timer
        turnCompressorOff(); // Send fan mode signal
    }
}
