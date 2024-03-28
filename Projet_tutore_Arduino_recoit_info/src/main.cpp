#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

const uint8_t PIN_LED = 13;

byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x0C, 0x12};
IPAddress ip(192, 168, 1, 177); // Adresse IP de votre carte Arduino
IPAddress server(192, 168, 1, 173); // Adresse IP du serveur web

float getStringTemp(String reception);

EthernetClient client;

void setup() {
  Serial.begin(9600);
  pinMode(A0, OUTPUT);      // Initialisation de la sortie controlant une LED
  Ethernet.begin(mac, ip);  // Initialisation du shield Ethernet avec l'adresse MAC et l'adresse IP
  delay(1000);
}

void loop() {
  if (client.connect(server, 80)) {                       // Connexion à l'ESP32
    Serial.print("Connecte à : "); Serial.println(server);
    client.println("GET /temperature HTTP/1.1");          // Envoi d'une requête de type GET pour avoir la température
    client.println();

    char c;
    int i = 0;
    String reception = "";

    while (client.connected() && i < 250) {               // Lecture de la réponse de l'ESP32
      if (client.available()) {
        c = client.read();
      }
      reception += c;
      i++;
    }
    float temperature = getStringTemp(reception);         // Recupèration de la température compris dans la réponse
    Serial.println(temperature);

    if (temperature > 19)                                 // Allumage d'une LED si la température est inférieure à 20°C
      digitalWrite(A0, HIGH);
    else
      digitalWrite(A0, LOW);

    delay(1);
    client.stop();                                        // Fermer la connexion
  } 
  else {
    Serial.println("Connection failed");
  }

  delay(3000);
}

float getStringTemp(String reception) {
  String stringTemperature = "";
  float temperature = 0;

  for(int i = 0, nbligne = 0; i < 250; i++) {            
    char c = reception[i];                                // Lecture de la réponse de l'ESP32
    if(c == '\r')                                         // S'il y a une fin de ligne, on incrémente le nombre de ligne
      nbligne++;
    if (nbligne == 6) {                                   // Si on lit la ligne comprenant la température, on la stocke dans stringTemperature
      stringTemperature += c;
    }
  }
  stringTemperature = stringTemperature.substring(2, 7);  // On garde que les caractères comprenant la température
  temperature = stringTemperature.toFloat();              // On converti le String en un float
  return temperature;
}