static const char* indexHtml = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><link rel=\"icon\" href=\"data:,\"><title>LED Control</title><meta charset=\"UTF-8\"><link rel=\"preconnect\" href=\"https://fonts.googleapis.com\"><link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin><link href=\"https://fonts.googleapis.com/css2?family=Economica:ital,wght@0,400;0,700;1,400;1,700&display=swap\" rel=\"stylesheet\"><style>.economica-regular{font-family:Economica,sans-serif;font-weight:400;font-style:normal}.economica-bold{font-family:Economica,sans-serif;font-weight:700;font-style:normal}.economica-regular-italic{font-family:Economica,sans-serif;font-weight:400;font-style:italic}.economica-bold-italic{font-family:Economica,sans-serif;font-weight:700;font-style:italic}.columns{display:flex;flex-direction:row;justify-content:center;align-items:flex-start;flex-wrap:wrap}.columns>div{margin:20px;border:2px solid #339;border-radius:8px;flex-grow:1}.comfort-status-emoji{border:0!important;flex-grow:0!important}.comfort-status-emoji>div{text-align:center;font-size:14pt}.comfort-status-emoji>div:first-child{text-align:center;font-size:36pt}html{font-family:Economica,sans-serif;font-weight:400;font-style:normal}table{width:98%}caption{color:#36c;font-family:Economica,sans-serif;font-weight:700;font-style:normal;font-size:18pt;text-align:center}th{font-family:Economica,sans-serif;font-weight:700;font-style:normal;font-size:14pt;text-align:right;width:50%}td{font-family:Economica,sans-serif;font-weight:500;font-style:normal;font-size:14pt;text-align:left;width:50%}.button-cell{text-align:center!important}.thermostat-setting{font-size:24pt}.control-switch{font-size:20pt;padding:3px}</style></head><body class=\"economica-regular\"><div class=\"columns\"><div><table><caption>Thermostat</caption><tr><th rowspan=\"2\"><span id=\"thermostatSetting\" class=\"thermostat-setting\"></span></th><td class=\"button-cell\"><button id=\"upButton\" class=\"control-switch\" onclick=\"temperatureUp()\">▲</button></td></tr><tr><td class=\"button-cell\"><button id=\"downButton\" class=\"control-switch\" onclick=\"temperatureDown()\">▼</button></td></tr></table></div><div class=\"comfort-status-emoji\"><div id=\"comfortStatusEmoji\"></div><div id=\"comfortStatusText\"></div></div><div><table><caption>Environment</caption><tr><th>Temperature:</th><td><span id=\"temperatureReading\"></span></td></tr><tr><th>Feels Like:</th><td><span id=\"heatIndexReading\"></span></td></tr><tr><th>Humidity:</th><td><span id=\"humidityReading\"></span></td></tr><tr><th>Dew Point:</th><td><span id=\"dewPointReading\"></span></td></tr></table></div></div><script>// this object will hold the current status of the LED as obtained from the
// AJAX call
const statusObj = {
    environment: null,
    ajaxCallInProgress: false
    temperatureAdjustmentInProgress: false
};

const ids = {
    refreshInterval: null,
};

const csEmoji = {
    'OK': '😊🌡️',
    'TooHot': '😓☀️',
    'TooCold': '🥶❄️' ,
    'TooDry': '💨🌵',
    'TooHumid': '😓💧',
    'HotAndHumid': '😓💦',
    'HotAndDry': '😓🌵',
    'ColdAndHumid': '🥶💧',
    'ColdAndDry': '🥶🌵',
};

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
        cstObj.innerText = statusObj.environment.cs.replace(/([A-Z])/g, ' $1').trim();
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

const temperatureUp = () => {
    if (statusObj.temperatureAdjustmentInProgress) {
        return;
    }

    const url = '/temperature/up';

    const xhttp=new XMLHttpRequest();

    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            updateStatus(this.responseText);
        }

        statusObj.temperatureAdjustmentInProgress = false;
    };

    statusObj.temperatureAdjustmentInProgress = true;

    xhttp.open('GET', url, true);

    xhttp.send();
};

const temperatureDown = () => {
    if (statusObj.temperatureAdjustmentInProgress) {
        return;
    }

    const url = '/temperature/down';

    const xhttp=new XMLHttpRequest();

    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            updateStatus(this.responseText);
        }

        statusObj.temperatureAdjustmentInProgress = false;
    };

    statusObj.temperatureAdjustmentInProgress = true;

    xhttp.open('GET', url, true);

    xhttp.send();
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
};</script></body></html>";
