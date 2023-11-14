#include <Ethernet.h>
#include <SPI.h>

byte MAC[] = {0x90, 0xA2, 0xDA, 0x10, 0x0C, 0x12};
IPAddress ip(192, 168, 1, 177); //adresse IP local, il faut la chercher sur google
EthernetServer server(80);

float getTemperature() {
  int brocheCapteur = 0; //Correspond à la broche A0 de la carte Arduino
  float valeur = analogRead(brocheCapteur); // Lecture de la valeur transmise par le capteur
  valeur = valeur*5.0/1024; // Conversion de la valeur en une tension utilisable
  valeur = (valeur - 0.5)/0.01; // ou *100 
  Serial.print(valeur); Serial.println(" °C"); //Conversion de la tension en une température. D'après la datasheet, la courbe de la tension en fonction de la température possède un coefficient directeur de 10 mV/°C et un offset de 0.5 V
  return valeur;
}

void setup() {
  Ethernet.begin(MAC, ip);
  Serial.begin(9600);
  server.begin();
  Serial.print("Adresse du server : "); Serial.println(Ethernet.localIP());
  pinMode(13, OUTPUT); //Pin du capteur
  delay(1000); // On attend 1s pour que le shield puisse s'initialiser
}

void loop() {
  EthernetClient client = server.available();
  if (client) { // s'il y a un client
    Serial.println("new client");
    boolean currentLineIsBlank = true; // an http request ends with a blank line
    while (client.connected()) { // Tant que le client est sur la page web
      if (client.available()) { // S'il est disponible
        char c = client.read(); //
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.print("Temperature : ");
          client.print(getTemperature());
          client.println("°C ");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
