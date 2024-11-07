static const char* indexHtml = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><link rel=\"icon\" href=\"data:,\"><title>LED Control</title><meta charset=\"UTF-8\"><link rel=\"preconnect\" href=\"https://fonts.googleapis.com\"><link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin><link href=\"https://fonts.googleapis.com/css2?family=Economica:ital,wght@0,400;0,700;1,400;1,700&display=swap\" rel=\"stylesheet\"><style>.economica-regular{font-family:Economica,sans-serif;font-weight:400;font-style:normal}.economica-bold{font-family:Economica,sans-serif;font-weight:700;font-style:normal}.economica-regular-italic{font-family:Economica,sans-serif;font-weight:400;font-style:italic}.economica-bold-italic{font-family:Economica,sans-serif;font-weight:700;font-style:italic}.columns{display:flex;flex-direction:row;justify-content:center;align-items:flex-start;flex-wrap:wrap}.columns>div{margin:20px;border:2px solid #339;border-radius:8px}html{font-family:Economica,sans-serif;font-weight:400;font-style:normal}caption{color:#36c;font-family:Economica,sans-serif;font-weight:700;font-style:normal;font-size:18pt;text-align:center}th{font-family:Economica,sans-serif;font-weight:700;font-style:normal;font-size:12pt;text-align:right;width:80px}td{font-family:Economica,sans-serif;font-weight:400;font-style:normal;font-size:12pt;text-align:center;width:80px}.button-cell{text-align:center!important}.comfort-status-emoji{text-align:center;font-size:36pt;width:200px!important;border:0!important}.thermostat-setting{font-size:18pt}</style></head><body class=\"economica-regular\"><div class=\"columns\"><div><table><caption>Environment</caption><tr><th>Temperature:</th><td><span id=\"temperatureReading\"></span></td></tr><tr><th>Feels Like:</th><td><span id=\"heatIndexReading\"></span></td></tr><tr><th>Humidity:</th><td><span id=\"humidityReading\"></span></td></tr><tr><th>Dew Point:</th><td><span id=\"dewPointReading\"></span></td></tr></table></div><div id=\"comfortStatusEmoji\" class=\"comfort-status-emoji\"></div><div><table><caption>Thermostat</caption><tr><th rowspan=\"2\"><span id=\"thermostatSetting\" class=\"thermostat-setting\"></span></th><td class=\"button-cell\"><button id=\"upButton\" class=\"control-switch\" onclick=\"temperatureUp()\">▲</button></td></tr><tr><td class=\"button-cell\"><button id=\"downButton\" class=\"control-switch\" onclick=\"temperatureDown()\">▼</button></td></tr></table></div></div><script>let statusObj={environment:null,ajaxCallInProgress:!1},ids={refreshInterval:null},csEmoji={OK:\"😊🌡️\",TooHot:\"😓☀️\",TooCold:\"🥶❄️\",TooDry:\"💨🌵\",TooHumid:\"😓💧\",HotAndHumid:\"😓💦\",HotAndDry:\"😓🌵\",ColdAndHumid:\"🥶💧\",ColdAndDry:\"🥶🌵\"},updateStatus=function(t){statusObj.environment=JSON.parse(t),null!==statusObj.environment&&(console.log(statusObj.environment),Object.hasOwn(statusObj.environment,\"ts\")&&(document.getElementById(\"thermostatSetting\").innerText=statusObj.environment.ts+\"°C\"),Object.hasOwn(statusObj.environment,\"t\")&&(document.getElementById(\"temperatureReading\").innerText=statusObj.environment.t+\"°C\"),Object.hasOwn(statusObj.environment,\"h\")&&(document.getElementById(\"humidityReading\").innerText=statusObj.environment.h+\"%\"),Object.hasOwn(statusObj.environment,\"hi\")&&(document.getElementById(\"heatIndexReading\").innerText=statusObj.environment.hi+\"°C\"),Object.hasOwn(statusObj.environment,\"dp\")&&(document.getElementById(\"dewPointReading\").innerText=statusObj.environment.dp+\"°C\"),Object.hasOwn(statusObj.environment,\"cs\"))&&(document.getElementById(\"comfortStatusEmoji\").innerText=Object.hasOwn(csEmoji,statusObj.environment.cs)?csEmoji[statusObj.environment.cs]:\"?\")},ajaxCall=()=>{var t=new XMLHttpRequest;t.onreadystatechange=function(){4==this.readyState&&200==this.status&&updateStatus(this.responseText)},t.open(\"GET\",\"/environment\",!0),t.send()},temperatureUp=()=>{console.log(\"up!\")},temperatureDown=()=>{console.log(\"down!\")};window.onload=()=>{ids.refreshInterval=setInterval(()=>{statusObj.ajaxCallInProgress||(statusObj.ajaxCallInProgress=!0,ajaxCall(),statusObj.ajaxCallInProgress=!1)},2e3)},window.onbeforeunload=()=>{clearInterval(ids.refreshInterval)}</script></body></html>";
