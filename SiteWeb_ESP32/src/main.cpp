#include <Arduino.h>
#include <Ticker.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "ds1621.h"
#include "rtc3231.h"

const int MONITOR_SPEED = 115200;
const uint8_t LED_WIFI_PIN = A0;
const uint8_t LED_WEB_PIN = A1;
const uint16_t DELAY = 1000;
const uint8_t ADRESSE_TEMPERATURE = 0x49; 

const char ssid[] = "Bbox-4F4DBAC4";
const char pwd[] = "hRTvZUKfa45wNU1qw7";

// const char ssid[] = "Bbox-451FD75D";
// const char pwd[] = "rCL45nb39Zn3ZxF6V4";

// const char ssid[] = "Linksys01370";
// const char pwd[] = "3fanq5w4pb";

const char *PARAM_BUTTON = "state";
const char *PARAM_VALUE = "slider_value";
const char *PARAM_CITY = "cityName";

const String API_KEY = "&appid=4ed4771a297b8f8173a1f45d7da47789";
const String URL = "http://api.openweathermap.org/data/2.5/weather?q=";
const String UNITS = "&units=metric";
const String LANG = "&lang=fr";

const String datas = "98";

String City = "Paris,FR";
float gtemperature;

void action();
void setRoutes();
void connect(WiFiEvent_t event, WiFiEventInfo_t info);
void disconnect(WiFiEvent_t event, WiFiEventInfo_t info);
void got_Ip(WiFiEvent_t event, WiFiEventInfo_t info);
String getDataFromAPI();
bool initSPIFFS();
String httpGetRequest(const char *url);

Rtc3231 Temps;
Ds1621 Temperature;
AsyncWebServer server(80);
Ticker timer(action, DELAY, 0, MILLIS);

void setup()
{
  Serial.begin(MONITOR_SPEED);
  pinMode(LED_WIFI_PIN, OUTPUT);
  pinMode(LED_WEB_PIN, OUTPUT);

  if (!Temperature.begin(ADRESSE_TEMPERATURE) || !initSPIFFS())
  {
    Serial.println("Erreur initialisation des composants");
    return;
  }

  timer.start();

  WiFi.mode(WIFI_STA);
  WiFi.onEvent(connect, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(got_Ip, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(disconnect, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.begin(ssid, pwd);
}

void loop()
{
  timer.update();
}

bool initSPIFFS()
{
  bool res = false;
  res = SPIFFS.begin();

  if (!res)
    Serial.println("Error Init SPIFFS");
  else
  {
    Serial.println("Init SPIFFS successful");

    File root = SPIFFS.open("/");
    File file = root.openNextFile();

    while (file)
    {
      Serial.print("File: ");
      Serial.println(file.name());
      file.close();
      file = root.openNextFile();
    }
  }

  return res;
}

void action()
{
  gtemperature = Temperature.getTemperature();
}

void setRoutes()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  server.on("/justgage.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/justgage.js", "text/javascript"); });

  server.on("/raphael.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/raphael.min.js", "text/javascript"); });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/script.js", "text/javascript"); });

  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/w3.css", "text/css"); });

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/styles.css", "text/css"); });

  server.on("/temperature.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/temperature.html", "text/html"); });

  server.on("/about.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/about.html", "text/html"); });

  server.on("/button", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if(request->hasParam(PARAM_BUTTON)) 
    {
      Serial.println("Request : Button");
      String state = request->getParam(PARAM_BUTTON)->value();
      if(state == "1")
        digitalWrite(LED_WEB_PIN, HIGH);
      else
        digitalWrite(LED_WEB_PIN, LOW);
    } request->send(200); });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
            { Serial.println("Request : Temperature");
              request->send(200, "text/plain", (String)gtemperature); 
            });
  
  server.on("/graphique", HTTP_GET, [](AsyncWebServerRequest *request)
    { 
        Serial.println("Request : graphique");

        // Exemple de tableau de données
        int tableau[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
        int tailleTableau = sizeof(tableau) / sizeof(tableau[0]);

        // Convertir le tableau en une chaîne de caractères
        String tableauStr = "";
        for (int i = 0; i < tailleTableau; ++i) {
            tableauStr += String(tableau[i]);
            if (i < tailleTableau - 1) {
                tableauStr += ","; // Ajouter une virgule entre les éléments
            }
        }

        // Envoyer la réponse HTTP avec les données du tableau
        request->send(200, "text/plain", tableauStr);
    });

  server.on("/cityinfos", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Serial.println("Request : API");
              if(request->hasParam(PARAM_CITY))
              {
                City = request->getParam(PARAM_CITY)->value();
              }
              request->send(200, "text/plain", getDataFromAPI()); });
}

String getDataFromAPI()
{
  String jsonBuffer = "";
  String API = "" + URL + City + API_KEY + UNITS + LANG;
  jsonBuffer = httpGetRequest(API.c_str());
  return jsonBuffer;
}

String httpGetRequest(const char *url)
{
  WiFiClient client;
  HTTPClient http;
  String payload = "";

  http.begin(client, url);
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0)
  {
    Serial.print("HTTP code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }

  http.end();
  return payload;
}

void connect(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.printf("\nConnected to: %s\n", ssid);
  digitalWrite(LED_WIFI_PIN, HIGH);
}

void disconnect(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Disconnected");
  WiFi.begin(ssid, pwd);
}

void got_Ip(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
  setRoutes();
}