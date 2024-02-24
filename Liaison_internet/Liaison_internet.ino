#include <Ethernet.h>
#include <SPI.h>

#define BROCHE_CAPTEUR 0

byte MAC[] = {0x90, 0xA2, 0xDA, 0x10, 0x0C, 0x12};                          // Adresse MAC de la carte Arduino
IPAddress ip(192, 168, 1, 177);                                             // Adresse IP de la carte Arduino
EthernetServer server = EthernetServer(80);                               

float getTemperature() {
  return ((analogRead(BROCHE_CAPTEUR) * 5.0 / 1024.0) - 0.5) * 100.0;       // Lecture puis conversion en tension puis en température de la valeur analogique du capteur
}

void setup() {
  Serial.begin(9600);                                                       // Initialisation de la communication série
  Ethernet.begin(MAC, ip);                                                  // Initialisation de la biliothèque Ethernet.h et initialisation des paramètres internet
  server.begin();                                                           // Le serveur commence à écouter les requête 
  Serial.print("Adresse du server : "); Serial.println(Ethernet.localIP()); // Affichage de l'adresse IP de la carte Arduino
  pinMode(13, OUTPUT);                                                      //Pin du capteur
  delay(100);                                                               // On attend 1s pour que le shield puisse s'initialiser
  Serial.println("Initialisation terminee");
}

void loop() {
  EthernetClient client = server.available();
  if (client) {                                                             // Si un client (ici le serveur avec les fichiers du site web) est connécté
    client.println(F("HTTP/1.1 200 OK"));                                   // On envoi l'accusé de bonne réception au client
    client.println(F("Content-Type: text/html"));                           // On indique le type du contenu de la réponse
    client.println(F("Refresh: 10"));                                       // La page web se rafraichit toutes les 10 s
    client.println(F("Connection: close"));                                 // La connexion sera fermée en fin de réception
    client.println();                                                       // Ligne blanche imposée par le protocole HTTP
    delay(10);                                                               
                                                                            // Contenu de la réponse 
    client.println("<meta charset='UTF-8'>");                               // Meta donnée pour afficher corectement le symbole ° 
    client.print(getTemperature());                                         // Envoie de la température
    client.println("°C");   

    delay(1);   
    client.stop();                                                          // Déconnexion du serveur
  }
}
