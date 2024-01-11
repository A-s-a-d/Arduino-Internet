#include <Ethernet.h>
#include <SPI.h>
#include <SD.h>

byte MAC[] = {0x90, 0xA2, 0xDA, 0x10, 0x0C, 0x12};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);

String reception = "";
char c;

int test_fichier(String nom){ // Fonction permettant de tester l'existence des fichiers avant de les ouvrir
  return !SD.exists(nom) ? 0 : 1; 
}

void erreur(String nom){ // Fonction permettant d'afficher un message d'erreur si un fichier n'a pas été trouvé
  Serial.print("Erreur, le fichier ");
  Serial.print(nom);
  Serial.println(" est introuvable");
}

void afficher(EthernetClient client, String nom_fichier, String type){ // Fonction permettant de répondre au requête HTTP du navigateur 
                                                                       // En fonction des arguments, on envoie une réponse personnalisé
  client.println(F("HTTP/1.1 200 OK"));                                // On envoi l'accusé de bonne réception au client
  client.print(F("Content-Type: text/"));                              // On indique le type du contenu de la réponse
  client.println(type);
  client.println(F("Refresh: 10"));                                    // La page web se rafraichit toutes les 10 s
  client.println(F("Connection: close"));                              // La connexion sera fermée en fin de réception
  client.println();                                                    // Ligne blanche imposée par le protocole HTTP
  delay(10);                                                           // La communication n'est pas instantannée, il faut attendre 

  File fichier = SD.open(nom_fichier, FILE_READ);                      // On ouvre le fichier sur la carte SD voulue, en mode lecture
  while (fichier.available()){                                         // Tant que ce fichier est disponible
    client.write(fichier.read());                                      // On demande au site web d'afficher ce qui est lu par le fichier de la carte SD
  }
  fichier.close();                                                     // Une fois que le fichier a été lu dans son entièreté, on le ferme
}

void setup() {
  Ethernet.begin(MAC, ip);                                         // Fonctions permettant de se connecter à internet
  Serial.begin(9600);
  server.begin();
  Serial.print("Adresse du server : "); 
  Serial.println(Ethernet.localIP());

  if(!SD.begin(4)){                                                // Fonction permettant d'initialisé la carte SD (port 4 de la carte) ainsi que la bibliothèque SD.h  
    Serial.println("Erreur, la carte SD n'a pas été initialisee"); 
    return;                                                        // Si la carte n'est pas initialisé, alors on arrête le programme
  }
  Serial.println("Carte SD initialisee");
  
  if(!SD.exists("Index.txt")){                                     // On teste si la carte arduino arrive à accéder au différent fichier du site web
    erreur("Index.txt");
    return;
  }
  if(!SD.exists("Index.css")){
    erreur("Index.css");
    return;
  }
  if(!SD.exists("About.txt")){
    erreur("About.txt");
    return;
  }
  if(!SD.exists("About.css")){
    erreur("About.css");
    return;
  }
  delay(1000);                                                      // On attend 1s pour que le shield puisse s'initialiser
  Serial.println("Preparation termine");
}

void loop() {
  EthernetClient client = server.available();                       
  if (client){                                                       
    reception = "";                                                   
    if (client.connected()){                                      // Si une personne est connectée sur le site web 
      Serial.println(F("Client connecte"));
      if (client.available()) c = client.read();                  // On lit la requête HTTP du site web
      while (client.available() && c != '\n'){                   // Tant qu'il y a une personne et  
        reception = reception + c;                                // que la 1ère ligne de la requête n'est pas terminé
        c = client.read();                                        // On lit la requête
      }
      Serial.println(reception);

      if (reception.startsWith("GET / HTTP/1.1")){                // Partie permettant de répondre au requête du site web
        afficher(client, "Index.txt", "html");
      }
      else if (reception.startsWith("GET /Index.css HTTP/1.1")){  
        afficher(client, "Index.css", "css");
      }
      else if (reception.startsWith("GET /About.html HTTP/1.1")){
        afficher(client, "About.txt", "html");
      }
      else if (reception.startsWith("GET /About.css HTTP/1.1")){  
        afficher(client, "About.css", "css");
      }
      else if (reception.startsWith("GET /images/CYU.png HTTP/1.1")){  
        afficher(client, "/images/Cyu.png", "png");
      }
      else if (reception.startsWith("GET /images/peakpx.jpg HTTP/1.1")){  
        afficher(client, "/images/peakpx.jpg", "jpg");
      }
      else if (reception.startsWith("GET /images/Fond_About.jpg HTTP/1.1")){  
        afficher(client, "/images/Fond_About.jpg", "jpg");
      }

      delay(1);
      client.stop(); 
    }
  }
}