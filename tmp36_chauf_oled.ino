/* Programme Capteur de Temperature, led Rouge (chauffage allumee), Led Vert (chauffage etient),
**
**------TMP36----------------------------------------------
**      _________
**      |       |
**      | TMP36 |
**      ---------
**        | | |
**      GND,A0,+5v
** 
**    A0 = Pin Analog de Arduino
**
**---------LED VERT----------------------------------------
**   
**     ledG = led Vert 
**
**                R
**         |---\/\/\/---|    |\  |
**   D6 ---|            |----|  >|----------GND
**         |---\/\/\/---|    |/  |
**                R
**
**            R = 51 ohm      R//R = Req ~= 25ohm
**
**           D6 = Pin D6 arduino
**
**
**---------LED ROUGE-----------------------------------------
**   
**     ledR = led Rouge 
**
**                R
**         |---\/\/\/---|    |\  |
**   D7 ---|            |----|  >|----------GND
**         |---\/\/\/---|    |/  |
**                R
**
**            R = 100 ohm      R//R = Req ~= 50ohm
**
**           D7 = Pin D7 arduino
**
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define brocheCapteur A0
#define ledR 6
#define ledG 7


// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


void setup() {
  Serial.begin(9600);
  pinMode(brocheCapteur, INPUT);         
  pinMode(ledR, OUTPUT);      
  pinMode(ledG, OUTPUT);      
  digitalWrite(ledG, LOW);
  digitalWrite(ledR, LOW);


  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println("Pret");
  screen_init();

}



void loop() {
  float temperature = lire_temp();
  aff_temp_serial_port(temperature);
  chauffage(temperature);
  delay(1000); 
}



float lire_temp() {
  int valeur = analogRead(brocheCapteur);  
  Serial.println(valeur);

  float voltage = valeur * (5.0 / 1023.0);  
  Serial.println(voltage, 4);

  float temperature = (voltage - 0.5) * 100.0;
  return temperature;
}



void chauffage(float temp) {
  if (temp <= 20) {
    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, LOW);
  } else {
    digitalWrite(ledR, LOW);
    digitalWrite(ledG, HIGH);
  }
  screen_temperature(temp);
}


void aff_temp_serial_port(float temp) {
  Serial.print(temp, 2);
  Serial.println(" °C");
  Serial.println();
  delay(1000);
}


void screen_init()
{
  screen_center_loading_bar(); // loading
  screen_center_pret();
  delay(2000);
  screen_Arduino_internet_text();
  delay(2000);
  screen_credit();
  delay(2000);

}

void screen_center_loading_bar()
{
    display.clearDisplay(); // Clear the display buffer
    for (int j = 28, x = 29, f = 30, g = 31, h = 32; j < 29 && x < 30 && f < 31 && g < 32 && h < 33; j++) {
        for (int i = 0; i < 128; i++) {
            display.drawPixel(i, j, SSD1306_WHITE);
            display.drawPixel(i, x, SSD1306_WHITE);
            display.drawPixel(i, f, SSD1306_WHITE);
            display.drawPixel(i, g, SSD1306_WHITE);
            display.drawPixel(i, h, SSD1306_WHITE);
            display.display();  // Update the display after drawing each pixel

            display.setTextSize(2); // Draw 2X-scale text
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(25, 0);
            display.println(F("Loading"));
            display.display();      // Show initial text
        }
    }
}

void screen_center_pret()
{
  display.clearDisplay(); // Clear the display buffer
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(40, 25);
  display.println(F("Pret"));
  display.display();      // Show initial text
}

void screen_Arduino_internet_text()
{
  display.clearDisplay(); // Clear the display buffer
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 25);
  display.println(F("Arduino \n Internet"));
  display.display();      // Show initial text
}

void screen_credit()
{
  display.clearDisplay(); // Clear the display buffer
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F(".M Asad \n.R Simon \n.M Thomas \n.M Simon"));
  display.display();      // Show initial text
}

void screen_temperature(float temp)
{
  display.clearDisplay(); // Clear the display buffer
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 0);
  char tempStr[10];
  if(temp > 0)
  {
    dtostrf(temp, 4, 2, tempStr); // 4 is the minimum width, 2 is the number of decimal places
    display.println("Temperature");
  }else
  {
    dtostrf(temp, 4, 1, tempStr); // 4 is the minimum width, 2 is the number of decimal places
    display.println("Temperature");
  }

    
  display.setCursor(0, 20);
  display.setTextSize(3); // Draw 2X-scale text
  display.println(tempStr);

  display.setTextSize(2); // Set text size back to 1X
  display.setCursor(95, 20); // Adjust the position as needed
  for (int i = 247; i < 247 + 1; i++) { // Include only the degree symbol
  display.print((char)i);
  }
  display.setTextSize(3); // Set text size back to 1X
  display.setCursor(105, 20); // Adjust the position as needed
  display.print("C");

  display.display();      // Show initial text
}



