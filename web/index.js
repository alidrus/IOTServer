// vim: syntax=javascript expandtab tabstop=4 shiftwidth=4 softtabstop=4:

// this object will hold the current status of the LED as obtained from the
// AJAX call
const statusObj = {
    environment: null,
    ajaxCallInProgress: false,
    temperatureAdjustmentInProgress: false,
    thermostatSetting: null,
};

const ids = {
    refreshInterval: null,
};

const csEmoji = {
    'OK': '😊🌡️',
    'Too Hot': '😓☀️',
    'Too Cold': '🥶❄️' ,
    'Too Dry': '💨🌵',
    'Too Humid': '😓💧',
    'Hot and Humid': '😓💦',
    'Hot and Dry': '😓🌵',
    'Cold and Humid': '🥶💧',
    'Cold and Dry': '🥶🌵',
    'Unknown': '🤨🧐',
};

const coEmoji = [
    'Compressor is off ⚪',
    'Compressor is on ⚡',
];

// update the LED button and LED state on the page
const updateStatus = function (responseText) {
    statusObj.environment = JSON.parse(responseText);

    if (statusObj.environment === null) {
        return;
    }

    console.log(statusObj.environment);

    if (Object.hasOwn(statusObj.environment, 'ts')) {
        const tsObj = document.getElementById('thermostatSetting');
        tsObj.innerText = statusObj.environment.ts + '°C';
        statusObj.thermostatSetting = statusObj.environment.ts;
    }

    if (Object.hasOwn(statusObj.environment, 't')) {
        const tObj = document.getElementById('temperatureReading');
        tObj.innerText = statusObj.environment.t + '°C';
    }

    if (Object.hasOwn(statusObj.environment, 'h')) {
        const hObj = document.getElementById('humidityReading');
        hObj.innerText = statusObj.environment.h + '%';
    }

    if (Object.hasOwn(statusObj.environment, 'hi')) {
        const hiObj = document.getElementById('heatIndexReading');
        hiObj.innerText = statusObj.environment.hi + '°C';
    }

    if (Object.hasOwn(statusObj.environment, 'dp')) {
        const dpObj = document.getElementById('dewPointReading');
        dpObj.innerText = statusObj.environment.dp + '°C';
    }

    if (Object.hasOwn(statusObj.environment, 'cs')) {
        const csObj = document.getElementById('comfortStatusEmoji');
        const cstObj = document.getElementById('comfortStatusText');
        csObj.innerText = Object.hasOwn(csEmoji, statusObj.environment.cs) ? csEmoji[statusObj.environment.cs] : '?';
        cstObj.innerText = statusObj.environment.cs;
    }

    if (Object.hasOwn(statusObj.environment, 'co')) {
        const coObj = document.getElementById('compressorState');
        coObj.innerText = coEmoji[statusObj.environment.co ? 1 : 0];
    }
};

// query the state of the LED from the server or turn it on/off
const ajaxCall = () => {
    const url = '/environment';

    const xhttp=new XMLHttpRequest();

    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200)
            updateStatus(this.responseText);
    };

    xhttp.open('GET', url, true);

    xhttp.send();
};

const temperatureAdjustmentFlag = (state) => {
};

// send temperature up command to the server
const temperatureUp = () => {
    if (statusObj.temperatureAdjustmentInProgress) {
        return;
    }

    statusObj.temperatureAdjustmentInProgress = true;

    const url = '/temperature/up';

    const xhttp=new XMLHttpRequest();

    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            console.log('Increasing temperature successful');
        }

        statusObj.temperatureAdjustmentInProgress = false;
    };

    xhttp.open('GET', url, true);

    xhttp.send();

    if (statusObj.thermostatSetting !== null && typeof statusObj.thermostatSetting === 'number') {
        const tsObj = document.getElementById('thermostatSetting');
        tsObj.innerText = (statusObj.thermostatSetting + 0.5) + '°C';
    }
};

// send temperature down command to the server
const temperatureDown = () => {
    if (statusObj.temperatureAdjustmentInProgress) {
        return;
    }

    statusObj.temperatureAdjustmentInProgress = true;

    const url = '/temperature/down';

    const xhttp=new XMLHttpRequest();

    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            console.log('Lowering temperature successful');
        }

        statusObj.temperatureAdjustmentInProgress = false;
    };

    xhttp.open('GET', url, true);

    xhttp.send();

    if (statusObj.thermostatSetting !== null && typeof statusObj.thermostatSetting === 'number') {
        const tsObj = document.getElementById('thermostatSetting');
        tsObj.innerText = (statusObj.thermostatSetting - 0.5) + '°C';
    }
};

// executed upon page loads
window.onload = () => {
    ids.refreshInterval = setInterval(() => {
        // Skip if there is already an ajax call in progress
        if (statusObj.ajaxCallInProgress) {
            return;
        }

        statusObj.ajaxCallInProgress = true;

        ajaxCall();

        statusObj.ajaxCallInProgress = false;
    }, 2000);
};

// executed before page unloads
window.onbeforeunload = () => {
    clearInterval(ids.refreshInterval);
};

// Expose temperatureUp and temperatureDown to the global scope
window.temperatureUp = temperatureUp;
window.temperatureDown = temperatureDown;
