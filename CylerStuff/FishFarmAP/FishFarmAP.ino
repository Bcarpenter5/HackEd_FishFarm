#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#define ssid "FishFarmNetwork"
#define password "123456789"

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

int heat = 0;
int pH = 0;
int waterLevel = 0;


int heatMax = 0;
int heatMin = 0;
int phMax = 0;
int phMin = 0;

void setup(){
  // setup code here to run once
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(1000);

  server.on("/", handle_OnConnect);
  server.on("/getInitialData", HTTP_GET, handleGetInitialData);
  server.on("/storeNumber", HTTP_GET, handleStoreNumber);
  server.on("/getSensorData", HTTP_GET, handleGetSensorData);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP Server Started");
}

void loop(){
  //put your main code here, to run repeatedly
  server.handleClient();
}

String getHTML(){
  String htmlCode = "<!DOCTYPE html>\n";
  htmlCode += "<html lang='en'>\n";
  htmlCode += "<head>\n";
  htmlCode += "    <meta charset='UTF-8'>\n";
  htmlCode += "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
  htmlCode += "    <title>Fish Farm</title>\n";
  htmlCode += "</head>\n";
  htmlCode += "<body>\n";
  htmlCode += "    <header>\n";
  htmlCode += "        <h1>Welcome to fish farm!</h1>\n";
  htmlCode += "    </header>\n";
  htmlCode += "    <main>\n";
  htmlCode += "        <div id='dataDiv'>\n";
  htmlCode += "            <h3>Data</h3>\n";
  htmlCode += "            <section>\n";
  htmlCode += "                <p>Water Temp: 20°C</p>\n";
  htmlCode += "                <p>Fish Water pH: 13.7</p>\n";
  htmlCode += "                <p>Water Level</p>\n";
  htmlCode += "            </section>\n";
  htmlCode += "        </div>\n";
  htmlCode += "        <div id='controlDiv'>\n";
  htmlCode += "            <h3>Control</h3>\n";
  htmlCode += "            <section>\n";
  htmlCode += "                <h3>-Heat Range-</h3>\n";
  htmlCode += "                <label>Max:</label> <input type='number' id='heatMax'>\n";
  htmlCode += "                <button id='heatMaxSubmit' onclick='sendXHR(\"heatMax\")'>Submit</button>\n";
  htmlCode += "\n";
  htmlCode += "                <label>Min:</label> <input type='number' id='heatMin'>\n";
  htmlCode += "                <button id='heatMinSubmit' onclick='sendXHR(\"heatMin\")'>Submit</button>\n";
  htmlCode += "\n";
  htmlCode += "                <h3>-pH Range-</h3>\n";
  htmlCode += "                <label>Max:</label> <input type='number' id='phMax'>\n";
  htmlCode += "                <button id='phMaxSubmit' onclick='sendXHR(\"phMax\")'>Submit</button>\n";
  htmlCode += "\n";
  htmlCode += "                <label>Min:</label> <input type='number' id='phMin'>\n";
  htmlCode += "                <button id='phMinSubmit' onclick='sendXHR(\"phMin\")'>Submit</button>\n";
  htmlCode += "            </section>\n";
  htmlCode += "        </div>\n";
  htmlCode += "        <div id='pumpControl'></div>\n";
  htmlCode += "    </main>\n";
  htmlCode += "\n";
  htmlCode += "    <script>\n";
  htmlCode += "        // Fetch the sensor data every 3 seconds to show data in real time\n";
  htmlCode += "        const interval = setInterval(fetchData, 3000);\n";
  htmlCode += "\n";
  htmlCode += "        // heat and ph max-min variables\n";
  htmlCode += "        var HeatMax;\n";
  htmlCode += "        var HeatMin;\n";
  htmlCode += "        var PHMax;\n";
  htmlCode += "        var PHMin;\n";
  htmlCode += "\n";
  htmlCode += "        window.onload = function() {\n";
  htmlCode += "            // create an XMLHttpRequest object to initialize our max and min variables\n";
  htmlCode += "            var xhr = new XMLHttpRequest();\n";
  htmlCode += "            xhr.open('GET', '/getInitialData', true);\n";
  htmlCode += "            xhr.onreadystatechange = function(){\n";
  htmlCode += "                if(xhr.readyState == 4 && xhr.status == 200){\n";
  htmlCode += "                    // data is ready to be used\n";
  htmlCode += "                    var data = xhr.responseText;\n";
  htmlCode += "                    console.log(data);\n";
  htmlCode += "                }\n";
  htmlCode += "            };\n";
  htmlCode += "            xhr.send();\n";
  htmlCode += "        };\n";
  htmlCode += "\n";
  htmlCode += "        function fetchData(){\n";
  htmlCode += "            // create an XMLHttpRequest object to get data\n";
  htmlCode += "            var xhr = new XMLHttpRequest();\n";
  htmlCode += "\n";
  htmlCode += "            // setup the request type and the URL of the req\n";
  htmlCode += "            xhr.open('GET', '/getSensorData', true);\n";
  htmlCode += "\n";
  htmlCode += "            // callback function for when the request returns the data\n";
  htmlCode += "            xhr.onreadystatechange = function(){\n";
  htmlCode += "                if(xhr.readyState == 4 && xhr.status == 200){\n";
  htmlCode += "                    // data is ready to be used\n";
  htmlCode += "                    var data = xhr.responseText;\n";
  htmlCode += "                    document.getElementById('pumpControl').innerHTML = data;\n";
  htmlCode += "                }\n";
  htmlCode += "            };\n";
  htmlCode += "\n";
  htmlCode += "            // send the request\n";
  htmlCode += "            xhr.send();\n";
  htmlCode += "        };\n";
  htmlCode += "\n";
  htmlCode += "        function sendXHR(varName){\n";
  htmlCode += "            // The value that we want to send back to the ESP32\n";
  htmlCode += "            var varVal = document.getElementById(varName).value;\n";
  htmlCode += "\n";
  htmlCode += "            // Create an XMLHttpRequest object\n";
  htmlCode += "            var xhr = new XMLHttpRequest();\n";
  htmlCode += "\n";
  htmlCode += "            // Specify the type of request and the URL\n";
  htmlCode += "            xhr.open('GET', '/storeNumber?varName='+varName+'&varVal='+varVal, true);\n";
  htmlCode += "\n";
  htmlCode += "            // Send the request\n";
  htmlCode += "            xhr.send();\n";
  htmlCode += "        }\n";
  htmlCode += "    </script>\n";
  htmlCode += "</body>\n";
  htmlCode += "</html>\n";


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
  doc["heatMax"] = heatMax;
  doc["heatMin"] = heatMin;
  doc["phMax"] = phMax;
  doc["phMin"] = phMin;

  String jsonData;
  serializeJson(doc, jsonData);
  server.send(200, "application/json", jsonData);
}

void handleStoreNumber(){
  if(server.hasArg("varVal") && server.hasArg("varName")){
    int num = server.arg("varVal").toInt();
    String name = server.arg("varName");

    if(name == "heatMax"){
      heatMax = num;
    }
    else if(name == "heatMin"){
      heatMin = num;
    }
    else if(name == "phMax"){
      phMax = num;
    }
    else if(name == "phMin"){
      phMin = num;
    }

    Serial.println("Heat Max: "+String(heatMax)+" || Heat Min: "+String(heatMin)+" || pH Max: "+String(phMax)+" || pH Min: "+String(phMin));
    server.send(200, "text/plain", "Number stored successfully");
  }
  else{
    server.send(400, "text/plain", "Bad Request");
  }
}

void handleGetSensorData(){
  server.send(200, "text/plain", String(7806999338));
}

void handle_NotFound(){
  Serial.println("Not Found");
  server.send(404, "text/plain", "Not Found");
}