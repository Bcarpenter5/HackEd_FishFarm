#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#define ssid "FishFarmNetwork"
#define password "123456789"
#define RXp2 16
#define TXp2 17

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

float heat = 111;
float pH = 222;
int waterLevel = 333;
int pump = 0;


int heatMax = 30;
int heatMin = 10;
int phMax = 12;
int phMin = 2;


String data;
uint lastIndex;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(1000);

  server.on("/", handle_OnConnect);
  server.on("/getInitialData", HTTP_GET, handleGetInitialData);
  server.on("/storeNumber", HTTP_GET, handleStoreNumber);
  server.on("/getSensorData", HTTP_GET, handleGetSensorData);
  server.on("/pumpOn", HTTP_GET, handlePumpOn);
  server.on("/pumpOff", HTTP_GET, handlePumpOff);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {
  CheckUART();
  server.handleClient();
}

void CheckUART() {
  if(Serial2.available() > 0) {
    data = Serial2.readString();
    Serial.println(data);
    lastIndex = 0;
    
    for(uint i = 0; i < data.length(); i++) {
      if(data[i] == '\n') {
        switch(data.substring(lastIndex, i-1)[0]) {
          case '1':
            pH = data.substring(lastIndex+2, i-1).toInt()/100.0;
            break;
          case '2':
            heat = data.substring(lastIndex+2, i-1).toInt()/100.0;
            break;
          case '3':
            waterLevel = data.substring(lastIndex+2, i-1).toInt();
            break;
          case '4':
            pump = data.substring(lastIndex+2, i-1).toInt();
            break;
        }
        lastIndex = i + 1;
      }
    }
    lastIndex = 0;
  }
}

String getHTML(){
  String htmlCode = "";
  htmlCode += "<!DOCTYPE html>\n";
  htmlCode += "<html lang='en'>\n";
  htmlCode += "<head>\n";
  htmlCode += "    <meta charset='UTF-8'>\n";
  htmlCode += "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
  htmlCode += "    <title>Fish Farm</title>\n";
  htmlCode += "</head>\n";
  htmlCode += "<body>\n";
  htmlCode += "    <header>\n";
  htmlCode += "        <h1>Welcome to your Fish Farm!</h1>\n";
  htmlCode += "    </header>\n";
  htmlCode += "    <main>\n";
  htmlCode += "        <div id='dataDiv'>\n";
  htmlCode += "            <h3>Data</h3>\n";
  htmlCode += "            <section>\n";
  htmlCode += "                <p class='boldP'>Water Temp: <span id='tempDsp'>---</span>°C</p>\n";
  htmlCode += "\n";
  htmlCode += "                <section>\n";
  htmlCode += "                    <p>Min:<br><span id='minTempDsp'>000</span>°C</p>\n";
  htmlCode += "                    <progress id='tempProg' value='0' max='100'></progress>\n";
  htmlCode += "                    <p>Max:<br><span id='maxTempDsp'>000</span>°C</p>\n";
  htmlCode += "                </section>\n";
  htmlCode += "                \n";
  htmlCode += "\n";
  htmlCode += "                <p class='boldP'>Fish Water pH: <span id='phDsp'>---</span></p>\n";
  htmlCode += "\n";
  htmlCode += "                <section>\n";
  htmlCode += "                    <p>Min:<br><span id='minPhDsp'>000</span></p>\n";
  htmlCode += "                    <progress id='phProg' value='0' max='100'></progress>\n";
  htmlCode += "                    <p>Max:<br><span id='maxPhDsp'>000</span></p>\n";
  htmlCode += "                </section>\n";
  htmlCode += "                \n";
  htmlCode += "\n";
  htmlCode += "                <p class='boldP'>Water Level: <span id='lvlDsp'>---</span></p>\n";
  htmlCode += "\n";
  htmlCode += "                <p class='boldP'>Pump Status: <span id='pumpDsp'>---</span></p>\n";
  htmlCode += "            </section>\n";
  htmlCode += "        </div>\n";
  htmlCode += "\n";
  htmlCode += "        <div id='statusDiv'>\n";
  htmlCode += "            <h3>Status Messages</h3>\n";
  htmlCode += "            <section id='statusMsgs'></section>\n";
  htmlCode += "        </div>\n";
  htmlCode += "\n";
  htmlCode += "        <div id='controlDiv'>\n";
  htmlCode += "            <h3>Control</h3>\n";
  htmlCode += "            <section>\n";
  htmlCode += "                <p class='boldP'>-Heat Range-</p>\n";
  htmlCode += "                <section>\n";
  htmlCode += "                    <label>Min:</label> <input type='number' id='heatMin'>\n";
  htmlCode += "                    <button id='heatMinSubmit' onclick='sendXHR(\"heatMin\")'>Submit</button>\n";
  htmlCode += "                </section>\n";
  htmlCode += "                <section>\n";
  htmlCode += "                    <label>Max:</label> <input type='number' id='heatMax'>\n";
  htmlCode += "                    <button id='heatMaxSubmit' onclick='sendXHR(\"heatMax\")'>Submit</button>\n";
  htmlCode += "                </section>\n";
  htmlCode += "                \n";
  htmlCode += "                <p class='boldP'>-pH Range-</p>\n";
  htmlCode += "                <section>\n";
  htmlCode += "                    <label>Min:</label> <input type='number' id='phMin'>\n";
  htmlCode += "                    <button id='phMinSubmit' onclick='sendXHR(\"phMin\")'>Submit</button>\n";
  htmlCode += "                </section>\n";
  htmlCode += "                <section>\n";
  htmlCode += "                    <label>Max:</label> <input type='number' id='phMax'>\n";
  htmlCode += "                    <button id='phMaxSubmit' onclick='sendXHR(\"phMax\")'>Submit</button>\n";
  htmlCode += "                </section>\n";
  htmlCode += "            </section>\n";
  htmlCode += "        </div>\n";
  htmlCode += "        <div id='pumpControl'>\n";
  htmlCode += "            <h3>Pump Control</h3>\n";
  htmlCode += "            <section>\n";
  htmlCode += "                <button type='button' id='pumpOn' onclick='PumpOnHandler()'>ON</button>\n";
  htmlCode += "                <button type='button' id='pumpOff' onclick='PumpOffHandler()'>OFF</button>\n";
  htmlCode += "            </section>\n";
  htmlCode += "        </div>\n";
  htmlCode += "    </main>\n";
  htmlCode += "    <footer>\n";
  htmlCode += "        <h5>HackEd 2024</h5>\n";
  htmlCode += "        <h6>Brandon Carpenter | Andreas Delfin | Cole Yaremko | Liam Moore | Cyler LaRocque</h6>\n";
  htmlCode += "    </footer>\n";
  htmlCode += "\n";
  htmlCode += "    <style>\n";
  htmlCode += "        html{\n";
  htmlCode += "            background: linear-gradient(90deg, #00C9FF 0%, #92FE9D 100%);\n";
  htmlCode += "        }\n";
  htmlCode += "        html, button{\n";
  htmlCode += "            font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;\n";
  htmlCode += "        }\n";
  htmlCode += "        button{\n";
  htmlCode += "            font-weight: bolder;\n";
  htmlCode += "        }\n";
  htmlCode += "        h1, h3, h4, h5, h6{\n";
  htmlCode += "            text-align: center;\n";
  htmlCode += "        }\n";
  htmlCode += "        h1{\n";
  htmlCode += "            font-size: 40px;\n";
  htmlCode += "        }\n";
  htmlCode += "        h3{\n";
  htmlCode += "            border: 2px solid black;\n";
  htmlCode += "            border-radius: 10px;\n";
  htmlCode += "            padding: 3px;\n";
  htmlCode += "            margin-top: 8px;\n";
  htmlCode += "            background-color: rgba(255,255,255,0.3);\n";
  htmlCode += "        }\n";
  htmlCode += "        h5, h6{\n";
  htmlCode += "            margin: 5px;\n";
  htmlCode += "        }\n";
  htmlCode += "        .boldP{\n";
  htmlCode += "            font-weight: bolder;\n";
  htmlCode += "        }\n";
  htmlCode += "        p, label{\n";
  htmlCode += "            text-align: center;\n";
  htmlCode += "            padding: 10px;\n";
  htmlCode += "            margin: 2px;\n";
  htmlCode += "            font-size: large;\n";
  htmlCode += "        }\n";
  htmlCode += "        label{\n";
  htmlCode += "            font-size: larger;\n";
  htmlCode += "        }\n";
  htmlCode += "        header, footer{\n";
  htmlCode += "            border: 3px solid black;\n";
  htmlCode += "            border-radius: 10px;\n";
  htmlCode += "            background-color: rgba(255,255,255,0.3);\n";
  htmlCode += "        }\n";
  htmlCode += "        footer{\n";
  htmlCode += "            position: fixed;\n";
  htmlCode += "            bottom: 0;\n";
  htmlCode += "            width: 100%;\n";
  htmlCode += "        }\n";
  htmlCode += "        main{\n";
  htmlCode += "            display: grid;\n";
  htmlCode += "            margin-left: 10%;\n";
  htmlCode += "            margin-right: 10%;\n";
  htmlCode += "            grid-template-columns: repeat(3, 1fr);\n";
  htmlCode += "            grid-gap: 25px;\n";
  htmlCode += "        }\n";
  htmlCode += "        #dataDiv > section{\n";
  htmlCode += "            display: grid;\n";
  htmlCode += "            grid-template-columns: 1fr;\n";
  htmlCode += "        }\n";
  htmlCode += "        #dataDiv > section > section{\n";
  htmlCode += "            display: flex;\n";
  htmlCode += "            align-items: center;\n";
  htmlCode += "            justify-content: center;\n";
  htmlCode += "        }\n";
  htmlCode += "        progress{\n";
  htmlCode += "            height: 50%;\n";
  htmlCode += "        }\n";
  htmlCode += "        input{\n";
  htmlCode += "            width: 30%;\n";
  htmlCode += "        }\n";
  htmlCode += "        #statusDiv{\n";
  htmlCode += "\n";
  htmlCode += "        }\n";
  htmlCode += "\n";
  htmlCode += "        .goodMessage{\n";
  htmlCode += "            border: 2px solid green;\n";
  htmlCode += "            background-color: lightgreen;\n";
  htmlCode += "            border-radius: 15px;\n";
  htmlCode += "        }\n";
  htmlCode += "        .badMessage{\n";
  htmlCode += "            border: 2px solid red;\n";
  htmlCode += "            background-color: rgb(255, 157, 146);\n";
  htmlCode += "            border-radius: 15px;\n";
  htmlCode += "        }\n";
  htmlCode += "\n";
  htmlCode += "        #controlDiv{\n";
  htmlCode += "            display: flex;\n";
  htmlCode += "            flex-direction: column;\n";
  htmlCode += "        }\n";
  htmlCode += "        #controlDiv > section > section{\n";
  htmlCode += "            display: flex;\n";
  htmlCode += "            align-items: center;\n";
  htmlCode += "            justify-content: space-evenly;\n";
  htmlCode += "            height: 50px;\n";
  htmlCode += "            \n";
  htmlCode += "        }\n";
  htmlCode += "        button, input{\n";
  htmlCode += "            height: 30px;\n";
  htmlCode += "            font-size: 16px;\n";
  htmlCode += "        }\n";
  htmlCode += "        button {\n";
  htmlCode += "            border: none;\n";
  htmlCode += "            --c:  rgba(255,255,255,0.5); /* the color*/\n";
  htmlCode += "            \n";
  htmlCode += "            box-shadow: 0 0 0 .1em inset var(--c); \n";
  htmlCode += "            --_g: linear-gradient(var(--c) 0 0) no-repeat;\n";
  htmlCode += "            background: \n";
  htmlCode += "                var(--_g) calc(var(--_p,0%) - 100%) 0%,\n";
  htmlCode += "                var(--_g) calc(200% - var(--_p,0%)) 0%,\n";
  htmlCode += "                var(--_g) calc(var(--_p,0%) - 100%) 100%,\n";
  htmlCode += "                var(--_g) calc(200% - var(--_p,0%)) 100%;\n";
  htmlCode += "            background-size: 50.5% calc(var(--_p,0%)/2 + .5%);\n";
  htmlCode += "            outline-offset: .1em;\n";
  htmlCode += "            transition: background-size .4s, background-position 0s .4s;\n";
  htmlCode += "        }\n";
  htmlCode += "        button:hover {\n";
  htmlCode += "            --_p: 100%;\n";
  htmlCode += "            transition: background-position .4s, background-size 0s;\n";
  htmlCode += "        }\n";
  htmlCode += "        button:active {\n";
  htmlCode += "            box-shadow: 0 0 9e9q inset #0009; \n";
  htmlCode += "            background-color: var(--c);\n";
  htmlCode += "            color: #fff;\n";
  htmlCode += "        }\n";
  htmlCode += "        #pumpControl{\n";
  htmlCode += "            grid-column: 2;\n";
  htmlCode += "            text-align: center;\n";
  htmlCode += "        }\n";
  htmlCode += "        #pumpControl > section{\n";
  htmlCode += "            display: flex;\n";
  htmlCode += "            align-items: center;\n";
  htmlCode += "            justify-content: space-around;\n";
  htmlCode += "        }\n";
  htmlCode += "        #pumpControl > section > button{\n";
  htmlCode += "            width: 40%;\n";
  htmlCode += "        }\n";
  htmlCode += "        main > div{\n";
  htmlCode += "            border: 2px solid black;\n";
  htmlCode += "            border-radius: 10px;\n";
  htmlCode += "            padding: 10px;\n";
  htmlCode += "            padding-top: 0;\n";
  htmlCode += "            margin: 10px;\n";
  htmlCode += "            background-color: rgba(0, 0, 0, 0.15);\n";
  htmlCode += "        }\n";
  htmlCode += "        \n";
  htmlCode += "    </style>\n";
  htmlCode += "\n";
  htmlCode += "    <script>\n";
  htmlCode += "        // Fetch the sensor data every 3 seconds to show data in real time\n";
  htmlCode += "        const interval = setInterval(fetchData, 500);\n";
  htmlCode += "\n";
  htmlCode += "        // heat and ph max-min variables\n";
  htmlCode += "        var HeatMax = 0;\n";
  htmlCode += "        var HeatMin = 0;\n";
  htmlCode += "        var PHMax = 0;\n";
  htmlCode += "        var PHMin = 0;\n";
  htmlCode += "\n";
  htmlCode += "        var pumpFlag = 0;\n";
  htmlCode += "\n";
  htmlCode += "        window.onload = function() {\n";
  htmlCode += "            // create an XMLHttpRequest object to initialize our max and min variables\n";
  htmlCode += "            var xhr = new XMLHttpRequest();\n";
  htmlCode += "            xhr.open('GET', '/getInitialData', true);\n";
  htmlCode += "            xhr.onreadystatechange = function() {\n";
  htmlCode += "                if(xhr.readyState == 4 && xhr.status == 200){\n";
  htmlCode += "                    // data is ready to be used\n";
  htmlCode += "                    var data = JSON.parse(xhr.responseText);\n";
  htmlCode += "                    console.log(data);\n";
  htmlCode += "                    document.getElementById('tempDsp').innerHTML = data.heat.toFixed(2);\n";
  htmlCode += "                    document.getElementById('phDsp').innerHTML = data.pH.toFixed(2);\n";
  htmlCode += "\n";
  htmlCode += "                    if(data.waterLevel == 0){\n";
  htmlCode += "                        document.getElementById('lvlDsp').innerHTML = 'Low';\n";
  htmlCode += "                    }\n";
  htmlCode += "                    else{\n";
  htmlCode += "                        document.getElementById('lvlDsp').innerHTML = 'Good';\n";
  htmlCode += "                    }\n";
  htmlCode += "\n";
  htmlCode += "                    pumpFlag = data.pump;\n";
  htmlCode += "\n";
  htmlCode += "                    if(pumpFlag == 0){\n";
  htmlCode += "                        document.getElementById('pumpDsp').innerHTML = 'OFF';\n";
  htmlCode += "                    }\n";
  htmlCode += "                    else{\n";
  htmlCode += "                        document.getElementById('pumpDsp').innerHTML = 'ON';\n";
  htmlCode += "                    }\n";
  htmlCode += "\n";
  htmlCode += "                    document.getElementById('heatMax').placeholder = data.heatMax;\n";
  htmlCode += "                    document.getElementById('heatMin').placeholder = data.heatMin;\n";
  htmlCode += "                    document.getElementById('phMax').placeholder = data.phMax;\n";
  htmlCode += "                    document.getElementById('phMin').placeholder = data.phMin;\n";
  htmlCode += "\n";
  htmlCode += "                    document.getElementById('minTempDsp').innerHTML = data.heatMin;\n";
  htmlCode += "                    document.getElementById('maxTempDsp').innerHTML = data.heatMax;\n";
  htmlCode += "                    document.getElementById('minPhDsp').innerHTML = data.phMin;\n";
  htmlCode += "                    document.getElementById('maxPhDsp').innerHTML = data.phMax;\n";
  htmlCode += "\n";
  htmlCode += "                    HeatMax = data.heatMax;\n";
  htmlCode += "                    HeatMin = data.heatMin;\n";
  htmlCode += "                    PHMax = data.phMax;\n";
  htmlCode += "                    PHMin = data.phMin;\n";
  htmlCode += "                    \n";
  htmlCode += "                    var tempProg = document.getElementById('tempProg');\n";
  htmlCode += "                    tempProg.max = HeatMax - HeatMin;\n";
  htmlCode += "                    tempProg.value = data.heat.toFixed(2) - HeatMin;\n";
  htmlCode += "\n";
  htmlCode += "                    var phProg = document.getElementById('phProg');\n";
  htmlCode += "                    phProg.max = PHMax - PHMin;\n";
  htmlCode += "                    phProg.value = data.pH.toFixed(2) - PHMin;\n";
  htmlCode += "                }\n";
  htmlCode += "            };\n";
  htmlCode += "            xhr.send();\n";
  htmlCode += "\n";
  htmlCode += "            fetchData();\n";
  htmlCode += "        };\n";
  htmlCode += "\n";
  htmlCode += "        function fetchData(){\n";
  htmlCode += "            // create xmlhttprequest object to use to ge7t data\n";
  htmlCode += "            var xhr = new XMLHttpRequest();\n";
  htmlCode += "\n";
  htmlCode += "            // setup the request type and the URL of the req\n";
  htmlCode += "            xhr.open('GET', '/getSensorData', true);\n";
  htmlCode += "\n";
  htmlCode += "            // callback function for when the request returns the data\n";
  htmlCode += "            xhr.onreadystatechange = function(){\n";
  htmlCode += "                if(xhr.readyState == 4 && xhr.status == 200){\n";
  htmlCode += "                    // data is ready to be used\n";
  htmlCode += "                    var data = JSON.parse(xhr.responseText);\n";
  htmlCode += "                    document.getElementById('tempDsp').innerHTML = data.heat.toFixed(2);\n";
  htmlCode += "                    document.getElementById('phDsp').innerHTML = data.pH.toFixed(2);\n";
  htmlCode += "                    pumpFlag = data.pump;\n";
  htmlCode += "\n";
  htmlCode += "                    if(data.waterLevel == 0){\n";
  htmlCode += "                        document.getElementById('lvlDsp').innerHTML = 'Low';\n";
  htmlCode += "                    }\n";
  htmlCode += "                    else{\n";
  htmlCode += "                        document.getElementById('lvlDsp').innerHTML = 'Good';\n";
  htmlCode += "                    }\n";
  htmlCode += "\n";
  htmlCode += "                    if(pumpFlag == 0){\n";
  htmlCode += "                        document.getElementById('pumpDsp').innerHTML = 'OFF';\n";
  htmlCode += "                    }\n";
  htmlCode += "                    else{\n";
  htmlCode += "                        document.getElementById('pumpDsp').innerHTML = 'ON';\n";
  htmlCode += "                    }\n";
  htmlCode += "\n";
  htmlCode += "                    document.getElementById('tempProg').value = data.heat.toFixed(2) - HeatMin;\n";
  htmlCode += "                    document.getElementById('phProg').value = data.pH.toFixed(2) - PHMin;\n";
  htmlCode += "\n";
  htmlCode += "                    var str = \"\";\n";
  htmlCode += "\n";
  htmlCode += "                    // check temperature status\n";
  htmlCode += "                    if(data.heat > HeatMax){\n";
  htmlCode += "                        str += \"<p class='badMessage'>Water Temperature is too hot!</p>\";\n";
  htmlCode += "                    }\n";
  htmlCode += "                    else if(data.heat < HeatMin){\n";
  htmlCode += "                        str += \"<p class='badMessage'>Water Temperature is too cold!</p>\";\n";
  htmlCode += "                    }\n";
  htmlCode += "                    else{\n";
  htmlCode += "                        str += \"<p class='goodMessage'>Water Temperature is Optimal</p>\";\n";
  htmlCode += "                    }\n";
  htmlCode += "\n";
  htmlCode += "                    // check ph status\n";
  htmlCode += "                    if(data.pH > PHMax){\n";
  htmlCode += "                        str += \"<p class='badMessage'>pH Level is too high!</p>\";\n";
  htmlCode += "                    }\n";
  htmlCode += "                    else if(data.pH < PHMin){\n";
  htmlCode += "                        str += \"<p class='badMessage'>pH Level is too low!</p>\";\n";
  htmlCode += "                    }\n";
  htmlCode += "                    else{\n";
  htmlCode += "                        str += \"<p class='goodMessage'>pH Level is Optimal</p>\";\n";
  htmlCode += "                    }\n";
  htmlCode += "\n";
  htmlCode += "                    // check water level status\n";
  htmlCode += "                    if(data.waterLevel == 1){\n";
  htmlCode += "                        str += \"<p class='goodMessage'>Water Level is Optimal</p>\";\n";
  htmlCode += "                    }\n";
  htmlCode += "                    else{\n";
  htmlCode += "                        str += \"<p class='badMessage'>Water Level Low!</p>\";\n";
  htmlCode += "                    }\n";
  htmlCode += "\n";
  htmlCode += "                    document.getElementById('statusMsgs').innerHTML = str;\n";
  htmlCode += "                }\n";
  htmlCode += "            };\n";
  htmlCode += "\n";
  htmlCode += "            // send the request\n";
  htmlCode += "            xhr.send();\n";
  htmlCode += "\n";
  htmlCode += "        };\n";
  htmlCode += "\n";
  htmlCode += "        function sendXHR(varName){\n";
  htmlCode += "            // The value that we want to send back to the ESP32\n";
  htmlCode += "            var varVal = document.getElementById(varName).value;\n";
  htmlCode += "            document.getElementById(varName).value = '';\n";
  htmlCode += "\n";
  htmlCode += "            // store the value in the designated variable, check for logic irregularity\n";
  htmlCode += "            //  with max and min values\n";
  htmlCode += "            switch(varName){\n";
  htmlCode += "                case \"heatMax\":\n";
  htmlCode += "                    HeatMax = varVal;\n";
  htmlCode += "                    if(HeatMax <= HeatMin){\n";
  htmlCode += "                        HeatMin = parseInt(HeatMax, 10) - 1;\n";
  htmlCode += "                    }\n";
  htmlCode += "                    break;\n";
  htmlCode += "                case \"heatMin\":\n";
  htmlCode += "                    HeatMin = varVal;\n";
  htmlCode += "                    if(HeatMin >= HeatMax){\n";
  htmlCode += "                        HeatMax = parseInt(HeatMin, 10) + 1;\n";
  htmlCode += "                    }\n";
  htmlCode += "                    break;\n";
  htmlCode += "                case \"phMax\":\n";
  htmlCode += "                    PHMax = varVal;\n";
  htmlCode += "                    if(PHMax <= PHMin){\n";
  htmlCode += "                        PHMin = parseInt(PHMax, 10) - 1;\n";
  htmlCode += "                    }\n";
  htmlCode += "                    break;\n";
  htmlCode += "                case \"phMin\":\n";
  htmlCode += "                    PHMin = varVal;\n";
  htmlCode += "                    if(PHMin >= PHMax){\n";
  htmlCode += "                        PHMax = parseInt(PHMin, 10) + 1;\n";
  htmlCode += "                    }\n";
  htmlCode += "                    break;\n";
  htmlCode += "            }\n";
  htmlCode += "\n";
  htmlCode += "            // update the progress bars with the new values\n";
  htmlCode += "            var tempProg = document.getElementById('tempProg');\n";
  htmlCode += "            tempProg.max = HeatMax - HeatMin;\n";
  htmlCode += "            tempProg.value = parseInt(document.getElementById('tempDsp').innerHTML, 10) - HeatMin;\n";
  htmlCode += "\n";
  htmlCode += "            var phProg = document.getElementById('phProg');\n";
  htmlCode += "            phProg.max = PHMax - PHMin;\n";
  htmlCode += "            phProg.value = parseInt(document.getElementById('phDsp').innerHTML, 10) - PHMin;\n";
  htmlCode += "\n";
  htmlCode += "            // set the placeholders for each one in-case of changes\n";
  htmlCode += "            document.getElementById('heatMax').placeholder = HeatMax;\n";
  htmlCode += "            document.getElementById('heatMin').placeholder = HeatMin;\n";
  htmlCode += "            document.getElementById('phMax').placeholder = PHMax;\n";
  htmlCode += "            document.getElementById('phMin').placeholder = PHMin;\n";
  htmlCode += "\n";
  htmlCode += "            document.getElementById('minTempDsp').innerHTML = HeatMin;\n";
  htmlCode += "            document.getElementById('maxTempDsp').innerHTML = HeatMax;\n";
  htmlCode += "            document.getElementById('minPhDsp').innerHTML = PHMin;\n";
  htmlCode += "            document.getElementById('maxPhDsp').innerHTML = PHMax;\n";
  htmlCode += "\n";
  htmlCode += "            // Create an XMLHttpRequest object\n";
  htmlCode += "            var xhr = new XMLHttpRequest();\n";
  htmlCode += "\n";
  htmlCode += "            // Specify the type of request and the URL\n";
  htmlCode += "            xhr.open('GET', '/storeNumber?heatMax='+HeatMax+'&heatMin='+HeatMin+'&phMax='+PHMax+'&phMin='+PHMin, true);\n";
  htmlCode += "\n";
  htmlCode += "            // Send the request\n";
  htmlCode += "            xhr.send();\n";
  htmlCode += "        }\n";
  htmlCode += "        function PumpOnHandler(){\n";
  htmlCode += "            // Create an XMLHttpRequest object to turn the pump on\n";
  htmlCode += "            var xhr = new XMLHttpRequest();\n";
  htmlCode += "            xhr.open('GET', '/pumpOn', true);\n";
  htmlCode += "            xhr.send();\n";
  htmlCode += "        }\n";
  htmlCode += "\n";
  htmlCode += "        function PumpOffHandler(){\n";
  htmlCode += "            // Create an XMLHttpRequest object to turn the pump on\n";
  htmlCode += "            var xhr = new XMLHttpRequest();\n";
  htmlCode += "            xhr.open('GET', '/pumpOff', true);\n";
  htmlCode += "            xhr.send();\n";
  htmlCode += "        }\n";
  htmlCode += "\n";
  htmlCode += "    </script>\n";
  htmlCode += "</body>\n";
  htmlCode += "</html>";

  return htmlCode;
}

void handle_OnConnect(){
  Serial.println("On connect");
  server.send(200, "text/html", getHTML());
}

void handleGetInitialData(){
  DynamicJsonDocument doc(256);
  // serialize the object
  doc["heat"] = heat;
  doc["pH"] = pH;
  doc["waterLevel"] = waterLevel;
  doc["pump"] = pump;
  doc["heatMax"] = heatMax;
  doc["heatMin"] = heatMin;
  doc["phMax"] = phMax;
  doc["phMin"] = phMin;

  String jsonData;
  serializeJson(doc, jsonData);
  server.send(200, "application/json", jsonData);
}

void handleStoreNumber(){
  if(server.hasArg("heatMax") && server.hasArg("heatMin") && server.hasArg("phMax") && server.hasArg("phMin")){
    int hmax = server.arg("heatMax").toInt();
    int hmin = server.arg("heatMin").toInt();
    int pmax = server.arg("phMax").toInt();
    int pmin = server.arg("phMin").toInt();

    heatMax = hmax;
    heatMin = hmin;
    phMax = pmax;
    phMin = pmin;

    Serial.println("Heat Max: "+String(heatMax)+" || Heat Min: "+String(heatMin)+" || pH Max: "+String(phMax)+" || pH Min: "+String(phMin));
    server.send(200, "text/plain", "Number stored successfully");
  }
  else{
    server.send(400, "text/plain", "Bad Request");
  }
}

void handleGetSensorData(){
  DynamicJsonDocument doc(256);
  // serialize the object
  doc["heat"] = heat;
  doc["pH"] = pH;
  doc["waterLevel"] = waterLevel;
  doc["pump"] = pump;

  String jsonData;
  serializeJson(doc, jsonData);

  server.send(200, "application/json", jsonData);
}

void handlePumpOn(){
  //UART CODE TO SEND PUMP ON MESSAGE TO NANO
  Serial2.println("4 1");
  Serial.println("Turning pump on...");
  server.send(200, "text/plain", "Pump operation successful");
}

void handlePumpOff(){
  //UART CODE TO SEND PUMP OFF MESAGE TO NANO
  Serial2.println("4 0");
  Serial.println("Turning pump off...");
  server.send(200, "text/plain", "Pump operation successful");
}

void handle_NotFound(){
  Serial.println("Not Found");
  server.send(404, "text/plain", "Not Found");
}