//Library for ESP8266 Web Server
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//Library for DHT
#include <DHT.h>

//Uncomment one of the lines below to choose DHT sensor type
//#define DHTTYPE DHT11 //DHT 11
//#define DHTTYPE DHT21 //DHT 21 (AM2301)
#define DHTTYPE DHT22 //DHT 22 (AM2302), AM2321

//Library for common data processing
#include <stdio.h>
#include <String.h>

//Web Page Content stored as text
const char main[] PROGMEM = R"=====(
<!doctype html>
<html lang='en'>
  <head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>
    <link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css'>
    <title>DEVN Temperature And Humidity Monitoring System</title>
    <style>table{margin:0 auto!important;}h1{font-size:32px;}h2{font-size:24px;}</style>
  </head>
  <body>
    <h1 class='text-center p-2 m-2'>DEVN Temperature And Humidity Monitoring System</h1>
    <div class='row p-0'>
      <div class='col-sm-12 col-md-6' id='temperature-chart-container'>
        <h2 class='text-center p-2 m-2'>Temperature</h2>
        <div class='col' id='temperature-chart-div' style='max-width:100%'>
          <p class='text-center'>Temperature Chart is loading...</p>
        </div>
      </div>
    <div class='col-sm-12 col-md-6' id='humidity-chart-container'>
      <h2 class='text-center p-2 m-2'>Humidity</h2>
      <div class='col' id='humidity-chart-div' style='max-width:100%'>
        <p class='text-center'>Humidity Chart is loading...</p>
      </div>
    </div>
  </div>
<script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>
<script type='text/javascript'>


google.charts.load('current', {
  'packages': ['gauge']
});
setInterval(function () {
  getTemperatureChart();
  getHumidityChart();
}, 1000);

function getTemperatureChart() {
  var temperatureChartHttp = new XMLHttpRequest();
  temperatureChartHttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      google.charts.setOnLoadCallback(drawTemperatureChart);
      var temperatureText = this.responseText;

      function drawTemperatureChart() {
        var temperatureData = google.visualization.arrayToDataTable([
          ['Label', 'Value'],
          ['*C', parseFloat(temperatureText)],
        ]);
        var temperatureOptions = {
          width: '400',
          height: '400',
          redFrom: 75,
          redTo: 100,
          yellowFrom: 25,
          yellowTo: 75,
          minorTicks: 5
        };
        var temperatureChart = new google.visualization.Gauge(document.getElementById('temperature-chart-div'));
        temperatureChart.draw(temperatureData, temperatureOptions);
      }
    }
  };
  temperatureChartHttp.open('GET', 'updateTemperatureChart', true); //Handle Temperature Chart on ESP8266
  temperatureChartHttp.send();
}

function getHumidityChart() {
  var humidityChartHttp = new XMLHttpRequest();
  humidityChartHttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      google.charts.setOnLoadCallback(drawHumidityChart);
      var humidityText = this.responseText;

      function drawHumidityChart() {
        var humidityData = google.visualization.arrayToDataTable([
          ['Label', 'Value'],
          ['%', parseFloat(humidityText)],
        ]);
        var humidityOptions = {
          width: '400',
          height: '400',
          redFrom: 75,
          redTo: 100,
          yellowFrom: 25,
          yellowTo: 75,
          minorTicks: 5
        };
        var humidityChart = new google.visualization.Gauge(document.getElementById('humidity-chart-div'));
        humidityChart.draw(humidityData, humidityOptions);
      }
    }
  };
  humidityChartHttp.open('GET', 'updateHumidityChart', true); //Handle Humidity Chart on ESP8266
  humidityChartHttp.send();
}

</script>
</body>
</html>
)=====";

//DHT Sensor
uint8_t DHTPin = D1;

//Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE); 

//
float Temperature;
float Humidity;

const char* ssid = "Diamond Electric VietNam 2.4GHz";
const char* password = "devietnam@2019#";

//Instantiate server at port 80 (http port)
ESP8266WebServer server(80);
/* -------------------------------------------------- */

/* ---------- Sensor Data Getting Functions --------- */

/* -------------------------------------------------- */

/* ---------- Web Server Implementation ----------*/
void wifi_and_server()
{
//Begin WiFi connection
WiFi.begin(ssid, password);
Serial.println("");

//Wait for connection
while (WiFi.status() != WL_CONNECTED) {
delay(1000);
Serial.print(".");
}

//View IP Address at COM Monitor
Serial.println("");
Serial.print("Connected to: ");
Serial.println(ssid);
Serial.print("IP address: ");
Serial.println(WiFi.localIP());

//Request handle
server.on("/", handleRoot);
server.on("/updateTemperatureChart", handleTemperatureChart);
server.on("/updateHumidityChart", handleHumidityChart);
server.begin();

//View at COM Monitor
Serial.println("Web Server Started!");
}
/* -------------------------------------------------- */

/* ---------- Server request handling functions ----------*/
//Send web page
void handleRoot() {
String page = main; //Read HTML contents
server.send(200, "text/html", page);
}

//Send temperature data to client ajax request
void handleTemperatureChart() {
String temperatureData = "";
Temperature = dht.readTemperature();
Serial.println(Temperature);
temperatureData += String(Temperature);
server.send(200, "text/plain", temperatureData);
}

//Send humidity data to client ajax request
void handleHumidityChart() {
String humidityData = "";
Humidity = dht.readHumidity();
Serial.println(Temperature);
humidityData += String(Humidity);
server.send(200, "text/plain", humidityData);
}

/* -------------------------------------------------- */

/* ---------- Setup ---------- */
void setup(void) {
Serial.begin(9600);
pinMode(DHTPin, INPUT);
dht.begin();
wifi_and_server();
}
/* -------------------------------------------------- */

/* ---------- Run repeatly ---------- */
void loop(void) {
server.handleClient();
}
/* -------------------------------------------------- */
