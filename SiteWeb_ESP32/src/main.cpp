#include <Arduino.h>
#include <Ticker.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "ds1621.h"
#include "rtc3231.h"

using namespace std;

const int MONITOR_SPEED = 115200;
const uint8_t LED_WIFI_PIN = A0;
const uint8_t LED_WEB_PIN = A1;
const uint16_t DELAY = 1000;
const uint8_t ADRESSE_TEMPERATURE = 0x49; 
const uint8_t ADRESSE_TEMPS = 0x68; 
const uint8_t SIZE_OF_GRAPH = 20;

const char ssid[] = "Bbox-4F4DBAC4";
const char pwd[] = "hRTvZUKfa45wNU1qw7";

const char *PARAM_BUTTON = "state";
const char *PARAM_VALUE = "slider_value";
const char *PARAM_CITY = "cityName";

const String API_KEY = "&appid=4ed4771a297b8f8173a1f45d7da47789";
const String URL = "http://api.openweathermap.org/data/2.5/weather?q=";
const String UNITS = "&units=metric";
const String LANG = "&lang=fr";

const String datas = "98";

String City = "Paris,FR";
String Filename = "";
float gtemperature;
float temperatureTab[20];

void action();
void setRoutes();
void connect(WiFiEvent_t event, WiFiEventInfo_t info);
void disconnect(WiFiEvent_t event, WiFiEventInfo_t info);
void got_Ip(WiFiEvent_t event, WiFiEventInfo_t info);
String getDataFromAPI();
bool initSPIFFS();
String httpGetRequest(const char *url);
String CreateFilename();
void WriteFile(float temperature, String Filename);
void ReadFile(String Filename);
void PrintFile(String Filename);
uint8_t CountLinesInFile(String Filename);
void PrintTemperatureTab();

Rtc3231 Temps;
Ds1621 Temperature;
AsyncWebServer server(80);
Ticker timer(action, DELAY, 0, MILLIS);

void setup()
{
  Serial.begin(MONITOR_SPEED);
  pinMode(LED_WIFI_PIN, OUTPUT);
  pinMode(LED_WEB_PIN, OUTPUT);

  if (!Temperature.begin(ADRESSE_TEMPERATURE) || !Temps.begin(ADRESSE_TEMPS) || !initSPIFFS())
  {
    Serial.println("Erreur initialisation des composants");
    return;
  }

  timer.start();
  Filename = CreateFilename();

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

String CreateFilename() {
  String TempsFile;
  TempsFile += Temps.getHour(); TempsFile += "_"; 
  TempsFile += Temps.getMinute(); TempsFile += "_"; 
  TempsFile += Temps.getSecond();

  String Filename = "/Temperature_" + TempsFile + ".txt";

  return Filename;
}

void WriteFile(float temperature, String Filename) {
  String temperatureStr = (String)temperature + '\n';

  File file = SPIFFS.open(Filename, FILE_APPEND);

  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }

  file.print(temperatureStr);
  file.close();
}

void ReadFile(String Filename) {
  uint8_t cptTab = 0;
  String temperatureStr = "";
  char c;

  File file = SPIFFS.open(Filename, FILE_READ);

  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  while(file.available() && cptTab <= 20){
    c = file.read();
    if(c == '\n') {
      temperatureTab[cptTab] = temperatureStr.toFloat();
      cptTab++;
    }
    else
      temperatureStr += c;
  }

  file.close(); 
}

void PrintTemperatureTab() {
  Serial.print("Tab content : ");
  for(int i = 0; i < 20; i++) {
    Serial.println(temperatureTab[i]);
  }
}

void PrintFile(String Filename) {

  File file = SPIFFS.open(Filename, FILE_READ);

  if(!file){
      Serial.println("Failed to open file for reading");
      return;
  }

  Serial.print("Fichier : "); Serial.println(Filename);
  Serial.println("Content : ");

  while(file.available()){
    Serial.write(file.read());
  }
  file.close(); 
}

uint8_t CountLinesInFile(String Filename)
{
  File file = SPIFFS.open(Filename, FILE_READ);

  uint8_t LigneCpt = 0;
  char c;

  file.seek(0);               // Move file pointer to beginning
  while (file.available()) {
    c = file.read();
    if (c == '\n')
      LigneCpt++;
  }

  file.close();
  return LigneCpt;
}

bool initSPIFFS()
{
  bool res = SPIFFS.begin();

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
  temperatureTab[CountLinesInFile(Filename)] = gtemperature;

  if(CountLinesInFile(Filename) >= 20) {

    Filename = CreateFilename();
  }
  
  WriteFile(gtemperature, Filename);
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

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/styles.css", "text/css"); });

  server.on("/temp.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/temp.css", "text/css"); });

  server.on("/temperature.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/temperature.html", "text/html"); });

  server.on("/about.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/about.html", "text/html"); });
  
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
          { request->send(200, "text/plain", (String)gtemperature); });
  
  server.on("/graphique", HTTP_GET, [](AsyncWebServerRequest *request)
    {   
        Serial.println("Requete : graphique");
        String tableauStr = "[";

        PrintTemperatureTab();

        for (int i = 0; i < SIZE_OF_GRAPH; ++i) {
          tableauStr += String(temperatureTab[i]);
          if (i < SIZE_OF_GRAPH - 1)
              tableauStr += ","; // Ajouter une virgule entre les éléments
        }
        tableauStr +="]";
        request->send(200, "text/plain", tableauStr); });

  server.on("/cityinfos", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if(request->hasParam(PARAM_CITY))
                City = request->getParam(PARAM_CITY)->value();
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