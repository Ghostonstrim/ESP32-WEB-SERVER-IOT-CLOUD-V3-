#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AsyncTCP.h>
const char* ssid = "MALUPONTON";
const char* password = "Paulette20";

const int verde = 33;
const int blanco = 32;
const int rojo = 27;
const int amarillo = 26;
#define DHTPIN 15     // Digital pin connected to the DHT sensor

#define DHTTYPE    DHT11     // DHT 11

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
String header; // Variable para guardar el HTTP request
String estadoSalida = "off";
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
AsyncWebServer server(80);

String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
    <meta charset='UTF-8'>
    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>
    <link rel='preconnect' href='https://fonts.googleapis.com'>
    <link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>
    <link href='https://fonts.googleapis.com/css2?family=Open+Sans:ital,wght@0,400;1,700&display=swap' rel='stylesheet'>
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
    .button{
            border:none;
            outline: none;
            padding: 10px 15px;
            cursor: pointer;
            border-radius: 5px;
            color: white;
            width: 150px;
            transition: 0.2s;
        }
        .verde{
            background-color: chartreuse;
        }
        .rojo{
            background-color: crimson;
        }
        .blanco{
            background-color: darkgrey;
        }
        .amarillo{
            background-color: yellow;
            color: black;
    
        }
        body{
            color: white;
            background: rgb(2,0,36);
            background: linear-gradient(180deg, rgba(45,81,228,1) 50%, rgba(0,212,255,1) 100%);
            display: flex;
            flex-direction: column;
            justify-content: center;
            width: 100vw;
            align-items: center;
            font-family: 'Open Sans', sans-serif;
            font-weight: 700;
            font-style: italic;
        }
  </style>
</head>
<body>
  <h1>Servidor Web ESP32</h1>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <h2>Foco Verde</h2>
    <p><a href='/onV'><button class='button verde'>ON</button></a></p>
    <p><a href='/offV'><button class='button verde'>OFF</button></a></p>
    <div></div>
    <h2>Foco Blanco</h2>
    <p><a href='/onB'><button class='button blanco'>ON</button></a></p>
    <p><a href='/offB'><button class='button blanco'>OFF</button></a></p>
    <div></div>
    <h2>Foco Rojo</h2>
    <p><a href='/onR'><button class='button rojo'>ON</button></a></p>
    <p><a href='/offR'><button class='button rojo'>OFF</button></a></p>
    <div></div>
    <h2>Foco Amarillo</h2>
    <p><a href='/onA'><button class='button amarillo'>ON</button></a></p>
    <p><a href='/offA'><button class='button amarillo'>OFF</button></a></p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var) {
  //Serial.println(var);
  if (var == "TEMPERATURE") {
    return readDHTTemperature();
  }
  else if (var == "HUMIDITY") {
    return readDHTHumidity();
  }
  return String();
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("");
  dht.begin();
  pinMode(verde, OUTPUT);
  pinMode(blanco, OUTPUT);
  pinMode(rojo, OUTPUT);
  pinMode(amarillo, OUTPUT);
  digitalWrite(verde, LOW);
  digitalWrite(blanco, LOW);
  digitalWrite(rojo, LOW);
  digitalWrite(amarillo, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);

  display.println("Conectando...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  display.clearDisplay();
  display.println(WiFi.localIP());
  display.display();
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
  // Route to set GPIO to HIGH
  server.on("/onV", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(verde, HIGH);
    request->send_P(200, "text/html", index_html, processor);
  });

    server.on("/offV", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(verde, LOW);
    request->send_P(200, "text/html", index_html, processor);
  });

    server.on("/onB", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(blanco, HIGH);
    request->send_P(200, "text/html", index_html, processor);
  });
    server.on("/offB", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(blanco, LOW);
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/onR", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(rojo, HIGH);
    request->send_P(200, "text/html", index_html, processor);
  });
    server.on("/offR", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(rojo, LOW);
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/onA", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(amarillo, HIGH);
    request->send_P(200, "text/html", index_html, processor);
  });
    server.on("/offA", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(amarillo, LOW);
    request->send_P(200, "text/html", index_html, processor);
  });
  



  //  server.on("/onV", HTTP_GET, onV);
  //  server.on("/offV", HTTP_GET, offV);
  //  server.on("/onB", HTTP_GET, onB);
  //  server.on("/offB", HTTP_GET, offB);
  //  server.on("/onR", HTTP_GET, onR);
  //  server.on("/offR", HTTP_GET, offR);
  //  server.on("/onA", HTTP_GET, onA);
  //  server.on("/offA", HTTP_GET, offA);




  // Start server
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:


}


void onV() {
  digitalWrite(verde, HIGH);
}

void offV() {
  digitalWrite(verde, LOW);
}
void onB() {
  digitalWrite(blanco, HIGH);
}
void offB() {
  digitalWrite(blanco, LOW);
}
void onR() {
  digitalWrite(rojo, HIGH);
}
void offR() {
  digitalWrite(rojo, LOW);
}
void onA() {
  digitalWrite(amarillo, HIGH);
}
void offA() {
  digitalWrite(amarillo, LOW);
}
