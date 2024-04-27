// Script de recevoir un message sur la liaison série/USB (115200bds) et l'affiche sur l'écran

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
#include "USB.h"
#define HWSerial    Serial
#define TFT_LEDA_PIN   38 // Broche de contrôle du rétroéclairage
#define BAUDRATE 115200
#define BRIGHNESS 70 

USBCDC USBSerial;

void setup() {
  // définition de la liaison série
  HWSerial.begin(BAUDRATE);
  USBSerial.begin();
  USB.begin();
  USBSerial.println("Hello T-Dongle-S3");

  // définition de l'écran
  tft.init();
  tft.setRotation(1);
  int brightness=map(70,0,100,0,255); //set le rétroéclairage à 70%
  analogWrite(TFT_LEDA_PIN, BRIGHNESS); // Contrôle de la luminosité
  tft.fillScreen(TFT_BLACK);
}

void loop() { // Put your main code here, to run repeatedly:
  // laison série
  if (USBSerial.available() > 0) { // Vérifie s'il y a des données disponibles à lire sur l'UART0
    char receivedData[20]; // Tableau pour stocker les données reçues
    int bytesRead = USBSerial.readBytesUntil('\n', receivedData, 20); // Lit les données jusqu'à un saut de ligne ('\n') ou jusqu'à 20 octets
    receivedData[bytesRead] = '\0'; // Ajoute un terminateur de chaîne
    USBSerial.print("Données reçues : ");
    USBSerial.println(receivedData); // Affiche les données reçues

  // écran
  // Set "cursor" at top left corner of display (0,0) and select font 2
  // (cursor will move to next line automatically during printing with 'tft.println'
  //  or stay on the line is there is room for the text with tft.print)
  tft.setCursor(0, 0, 2);
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  // We can now plot text on screen using the "print" class
  tft.println(receivedData);
  }

  
}
