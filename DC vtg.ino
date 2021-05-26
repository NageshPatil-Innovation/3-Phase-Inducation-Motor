#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#else
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>S
#endif


// Variables to store temperature values
String Voltage = "";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Replace with your network credentials
const char* ssid = "moto";
const char* password = "sonu143ranu";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//read voltage
String readVoltage()
{
  // Call sensors.requestVoltage() to issue a global temperature and Requests to all devices on the bus
  int sensorValue = analogRead(A0);

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 50V):

  float Vtg1 = sensorValue * (5.0 / 1024.0) * 10;
  Serial.print("Voltage1: ");
  Serial.println(Vtg1);
  return String(Vtg1);
}

//web page code
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<style>
table, th, td {
  border: 1px solid black;
  border-collapse: collapse;
}
th, td {
  padding: 5px;
  text-align: left;
width:60px; 
text-align:center;
font-size:19px; 
}
.dot {
  height: 60px;
  width: 60px;
  background-color:violet;
  border-radius: 50%;
  display: inline-block;
}
.button {
  background-color: #4CAF50; /* Green */
  border: none;
  color: white;
  padding: 15px 32px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  margin: 4px 2px;
  cursor: pointer;
}
</style>
</head>
<body>

<center>  
<h1 style="border:orange; border-width:5px; border-style:solid;">3 Phase Motor Controlling Dashboard </h1>  
</center> 
<table style="width:28%;height:552px;border: 2px solid black;">
  <tr>
    <th colspan="2">1 Phase Reading</th> 
  </tr>
  <tr>
    <th>Parameter</th>
    <th>Value</th>
  </tr>
  <tr>
    <td>Voltage (V)</td>
    <td>
      <span id="vtg">%VOLTAGE%</span>
    </td>
  </tr>
  <tr>
    <td>Current (Amp)</td>
    <td></td>
  </tr>
    <td>Power (Watt)</td>
    <td></td>
  </tr>
  <tr>
    <th colspan="2">2 Phase Reading</th>
  </tr>
  <tr>
    <th>Parameter</th>
    <th>Value</th>
  </tr>
  <tr>
    <td>Voltage (V)</td>
    <td></td>
  </tr>
  <tr>
    <td>Current (Amp)</td>
    <td></td>
  </tr>
    <td>Power (Watt)</td>
    <td></td>
  </tr>
  <tr>
    <th colspan="2">3 Phase Reading</th>
  </tr>
  <tr>
    <th>Parameter</th>
    <th>Value</th>
  </tr>
  <tr>
    <td>Voltage (V)</td>
    <td></td>
  </tr>
  <tr>
    <td>Current (Amp)</td>
    <td></td>
    </tr>
    <td>Power (Watt)</td>
    <td></td>
  </tr>
</table>

<table style="width:40%;height:450px;margin-left:30%;margin-top:-512px;">
  <tr>
    <th colspan="3">3 Phase Status</th>
  </tr>
   <tr>
    <th>1 Phase</th>
    <th>2 Phase</th>
    <th>3 Phase</th>
  </tr> 
  <tr>
    <td><span class="dot"></span></td>
    <td><span class="dot"></span></td>
    <td><span class="dot"></span></td>
  </tr>
  
</table>

<table style="width:28%;height:550px;margin-left:72%;margin-top:-494px;">
  <tr>
    <th colspan="1">Motor Mode</th>
  </tr>
  <tr rowspan="2">
    <td colspan="1"><button class="button">ON</button><h5 style="font-size:19px;">Click here to ON Motor</h5></td>
  </tr>
   <tr rowspan="2">
    <td colspan="1"><button class="button">OFF</button><h5 style="font-size:19px;">Click here to OFF Motor</h5></td>
  </tr>
  <tr>
    <th>Motor Status</th>
  </tr>
  <tr>
  <td><span class="dot"></span></td>
  </tr>
</table>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("vtg").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/vtg", true);
  xhttp.send();
}, 10000) ;
</script>
</html>
)rawliteral";


void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();

  Voltage = readVoltage();

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/vtg", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", Voltage.c_str());
  });

  // Start server
  server.begin();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    Voltage = readVoltage();
    lastTime = millis();
  }
}
