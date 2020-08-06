let statusObj = {
    ledIsOn: false
};
const updateStatus = function (responseText) {
    statusObj.ledIsOn = JSON.parse(responseText).ledIsOn;
    const buttonObj = document.getElementById('ledButton');
    const ledStateObj = document.getElementById('ledState');
    if (statusObj.ledIsOn) {
        buttonObj.classList.add('turn-off');
        buttonObj.innerText = 'OFF';
        ledStateObj.classList.add('is-on');
        ledStateObj.innerText = 'LED is ON';
    } else {
        buttonObj.classList.remove('turn-off');
        buttonObj.innerText = 'ON';
        ledStateObj.classList.remove('is-on');
        ledStateObj.innerText = 'LED is OFF';
    }
};
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
window.onload = function() {
    ajaxCall(true);
};
