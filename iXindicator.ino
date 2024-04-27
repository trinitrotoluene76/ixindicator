//iXindicator fonctionnant sur Lyligo T-Dongle-S3
//indicateur d'état commandé par l'envoi de commande via USB

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

// Liaison série USB
#include "USB.h"
#define HWSerial    Serial
#define BAUDRATE 115200
// Écran
#define TFT_LEDA_PIN   38 // Broche de contrôle du rétroéclairage
#define BRIGHNESS 70 //70% de luminosité

USBCDC USBSerial;

void setup() {
  // Définition de la liaison série
  HWSerial.begin(BAUDRATE);
  USBSerial.begin();
  USB.begin();

  // Définition de l'écran
  tft.init();
  tft.setRotation(1);
  int brightness = map(BRIGHNESS, 0, 100, 0, 255); // Définit le rétroéclairage à 70%
  analogWrite(TFT_LEDA_PIN, brightness); // Contrôle de la luminosité
  tft.fillScreen(TFT_BLACK); // Premier init de l'écran pour éviter la neige


  // Définition du texte
  tft.setTextColor(TFT_WHITE); // Couleur du texte
  
  //Texte d'acceuil
  tft.setTextSize(1);
  tft.setCursor(5, 5);
  tft.println("iXindicator V1.0");
  tft.setTextSize(3);
  tft.setCursor(5, 17);
  tft.println("Waiting");
  tft.setCursor(5, 45);
  tft.println("command");
  tft.setTextSize(2);
}

unsigned long startTime = 0; // Variable pour enregistrer le temps de début
unsigned long elapsedTime = 0;

void loop() {
  // Lecture des données sur la liaison série
  if (USBSerial.available() > 0) { // Vérifie s'il y a des données disponibles à lire sur l'UART0
    char receivedData[20]; // Tableau pour stocker les données reçues
    int bytesRead = USBSerial.readBytesUntil('\n', receivedData, 20); // Lit les données jusqu'à un saut de ligne ('\n') ou jusqu'à 20 octets
    receivedData[bytesRead] = '\0'; // Ajoute un terminateur de chaîne
    // USBSerial.print("Données reçues : ");
    // USBSerial.println(receivedData); // Affiche les données reçues

    if (strcmp(receivedData, "$START") == 0){
    // Démarre le chronomètre lorsque des données sont disponibles
    startTime = millis();

    // Affichage sur l'écran
    displayInProgress();
    delay(1000);
  
    // displayPassed();
    // delay(1000);
    // displayFailed();
    // delay(1000);
    }
    displayChrono();
    delay(1000);
  }
}

void displayChrono() {
 elapsedTime = millis() - startTime;

  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK); // Couleur du texte
  // Taille de l'écran
  int screenWidth = tft.width();
  
  //texte:
  String message =String(elapsedTime / 1000) + " s";
  // Taille du texte
  int textSize = tft.textWidth(message);

  // Calcul de la position x pour centrer le texte
  int posX = (screenWidth - textSize) / 2;
  // Calcul de la position y pour centrer le texte
  int posY = 54;
  tft.setCursor(posX, posY); // Position y au milieu de l'écran

  // Afficher le texte centré
  tft.println(message);  
}

void displayInProgress() {
  tft.setTextColor(TFT_BLACK); // Couleur du texte
  // Taille de l'écran
  int screenWidth = tft.width();
  int screenHeight = tft.height();
  
  //texte:
  String message = "In Progress";
  // Taille du texte
  int textSize = tft.textWidth(message);
  int textHeight = tft.fontHeight();
  // Calcul de la position x pour centrer le texte
  int posX = (screenWidth - textSize) / 2;
  // Calcul de la position y pour centrer le texte
  int posY = (screenHeight - textHeight) / 2;
  tft.setCursor(posX, posY); // Position y au milieu de l'écran
  tft.fillScreen(tft.color565(255, 255, 0)); // écran vert
  tft.setCursor(posX, posY); // Position y au milieu de l'écran
  // Afficher le texte centré
  tft.println(message);  
}

void displayPassed() {
  tft.setTextColor(TFT_BLACK); // Couleur du texte
    // Taille de l'écran
  int screenWidth = tft.width();
  int screenHeight = tft.height();
  
  //texte:
  String message = "PASSED";
  // Taille du texte
  int textSize = tft.textWidth(message);
  int textHeight = tft.fontHeight();
  // Calcul de la position x pour centrer le texte
  int posX = (screenWidth - textSize) / 2;
  // Calcul de la position y pour centrer le texte
  int posY = (screenHeight - textHeight) / 2;
  tft.setCursor(posX, posY); // Position y au milieu de l'écran
  tft.fillScreen(tft.color565(0, 255, 0)); // écran vert
  tft.setCursor(posX, posY); // Position y au milieu de l'écran
  // Afficher le texte centré
  tft.println(message);  
}
