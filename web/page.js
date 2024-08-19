// this object will hold the current status of the LED as obtained from the
// AJAX call
let statusObj = {
    ledIsOn: false,
    ajaxCallInProgress: false
};

const ids = {
    refreshInterval: null,
};

// update the LED button and LED state on the page
const updateStatus = function (responseText) {
    statusObj.ledIsOn = JSON.parse(responseText).ledIsOn;

    const buttonObj = document.getElementById('ledButton');

    const ledStateObj = document.getElementById('ledState');

    if (statusObj.ledIsOn) {
        buttonObj.classList.add('turn-off');
        buttonObj.innerText = 'Turn OFF';

        ledStateObj.classList.add('is-on');
        ledStateObj.innerText = 'LED is ON';
    } else {
        buttonObj.classList.remove('turn-off');
        buttonObj.innerText = 'Turn ON';

        ledStateObj.classList.remove('is-on');
        ledStateObj.innerText = 'LED is OFF';
    }
};

// query the state of the LED from the server or turn it on/off
const ajaxCall = function (statusOnly = false) {
    let url = '/led';

    if (!statusOnly)
        url += ('/' + (statusObj.ledIsOn ? '0' : '1'));

    const xhttp=new XMLHttpRequest();

    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200)
            updateStatus(this.responseText);
    };

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

        ajaxCall(true);

        statusObj.ajaxCallInProgress = false;
    }, 100);
};

// executed before page unloads
window.onbeforeunload = () => {
    clearInterval(ids.refreshInterval);
};

// vim: syntax=javascript expandtab tabstop=4 shiftwidth=4 softtabstop=4:
