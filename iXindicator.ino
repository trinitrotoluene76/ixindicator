//iXindicator fonctionnant sur Lyligo T-Dongle-S3
//indicateur d'état commandé par l'envoi de commande via USB

// convertisseur en ligne: https://www.convert-me.com/fr/convert/time/millisecond/millisecond-to-dhms.html?u=millisecond&v=9.999.354.000
// le nombre de jour max affichable est: 4,294,967,295ms (2^32 - 1) soit 49 jours 17:02

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
const int progressBarHeight = 4;
const int progressBarMargin = 15;
unsigned long timeoutThreshold; // Déclaration de la variable timeout Threshold
bool timeout=false;//dans le mode 2 si on atteint le timeout on y reste

const int posY_L1=10;
const int posY_L2=30;
const int posY_L3=50;

int screenHeight = 0; //défini dans setup
int screenWidth = 0; //défini dans setup

// USB
USBCDC USBSerial;

// Chrono
unsigned long startTime = 0;
unsigned long elapsedTime = 0;
bool maxdays =false; //variable passe à true si le chrono dépasse 49 jours 
unsigned long currentMillis =0;
unsigned long maxMillis =4294967295-1000; // marge de sécurité d'1s (49j- 1s)
unsigned long previousMillis = 0;
const unsigned long interval = 1000; // Interval d'actualisation d'une seconde

int mode=0; //définition des modes: 0 écran d'acceuil, 1 Start chrono, 2 timeout/watchdog latché, 3 timeout nb occurences
bool refreshScreen = true; //permet de rafraichir qu'une seule fois lors d'info fixes

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
  screenHeight = tft.height();
  screenWidth = tft.width();
}//fin setup

// fonction d'affichage du temps sous différents format
// hh:mm:ss
// dd jours hh:mm
// > 49 jours
void displayTime(unsigned long elapsed, unsigned long days, bool flag = false) {
  tft.setTextColor(TFT_WHITE); // Couleur du texte
  tft.fillScreen(TFT_BLACK); // Efface l'écran
  unsigned long seconds = elapsed;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  int textSize=0;
  int posX =0;
  if (flag==false){
    if (days == 0) {
      // Affiche le temps au format "hh:mm:ss"
      unsigned long secondsRemainder = seconds % 60;
      unsigned long minutesRemainder = minutes % 60;
      unsigned long hoursRemainder = hours % 24;
      
      // Taille du texte
      textSize = tft.textWidth(String("00:00:00"));
      
      // Calcul de la position x pour centrer le texte
      posX = (screenWidth - textSize) / 2;
      tft.setCursor(posX, posY_L2); // Position y au milieu de l'écran

      // Afficher le texte centré
      tft.printf("%02lu:%02lu:%02lu", hoursRemainder, minutesRemainder, secondsRemainder);

    } else {
      // Affiche le temps au format "jj jours hh:mm"
      unsigned long hoursRemainder = hours % 24;
      unsigned long minutesRemainder = minutes % 60;
      
      // Taille du texte
      textSize = tft.textWidth(String("00 jours"));

      // Calcul de la position x pour centrer le texte
      posX = (screenWidth - textSize) / 2;
      // Calcul de la position y pour centrer le texte
      tft.setCursor(posX, posY_L1);
      tft.printf("%03lu jours", days);

      // Taille du texte
      textSize = tft.textWidth(String("00:00"));
      // Calcul de la position x pour centrer le texte
      posX = (screenWidth - textSize) / 2;
      tft.setCursor(posX, posY_L2);
      tft.printf("%02lu:%02lu", hoursRemainder, minutesRemainder);
    }
  }
  else{
    //texte:
    String message =String("> 49 jours");
    // Taille du texte
    textSize = tft.textWidth(message);
    // Calcul de la position x pour centrer le texte
    posX = (screenWidth - textSize) / 2;
    tft.setCursor(posX, posY_L1);
    tft.println(message);
  }
}//fin diplayTime

//Affiche soit "passed" soit "passed + chrono sous différentes formes cf displayTime"
void displayPassed(unsigned long elapsed=0, unsigned long days=0, bool flag = false) {
  
  if (refreshScreen==true) {
    unsigned long seconds = elapsed;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    int textSize=0;
    int posX =0;
    String message =String("");
    tft.fillScreen(tft.color565(0, 255, 0)); // écran vert
    tft.setTextColor(TFT_BLACK); // Couleur du texte

    if (flag==false){
      if (days == 0) {
        if (elapsed>0){
          // Affiche le temps au format "hh:mm:ss"
          unsigned long secondsRemainder = seconds % 60;
          unsigned long minutesRemainder = minutes % 60;
          unsigned long hoursRemainder = hours % 24;
          
          // Taille du texte
          textSize = tft.textWidth(String("00:00:00"));
          
          // Calcul de la position x pour centrer le texte
          posX = (screenWidth - textSize) / 2;
          tft.setCursor(posX, posY_L2); // Position y au milieu de l'écran

          // Afficher le texte centré
          tft.printf("%02lu:%02lu:%02lu", hoursRemainder, minutesRemainder, secondsRemainder);
        }//fin du if elapse time >0
      } else {
        // Affiche le temps au format "jj jours hh:mm"
        unsigned long hoursRemainder = hours % 24;
        unsigned long minutesRemainder = minutes % 60;
        // Taille du texte
        textSize = tft.textWidth(String("00 jours"));
        // Calcul de la position x pour centrer le texte
        posX = (screenWidth - textSize) / 2;
        // Calcul de la position y pour centrer le texte
        tft.setCursor(posX, posY_L1);
        tft.printf("%03lu jours", days);
        // Taille du texte
        textSize = tft.textWidth(String("00:00"));
        // Calcul de la position x pour centrer le texte
        posX = (screenWidth - textSize) / 2;
        tft.setCursor(posX, posY_L2);
        tft.printf("%02lu:%02lu", hoursRemainder, minutesRemainder);
      }
    }
    else{
      //texte:
      message =String("> 49 jours");
      // Taille du texte
      textSize = tft.textWidth(message);
      // Calcul de la position x pour centrer le texte
      posX = (screenWidth - textSize) / 2;
      tft.setCursor(posX, posY_L1);
      tft.println(message);
    }
    //texte:
    message = "PASSED";
    // Taille du texte
    textSize = tft.textWidth(message);
    posX = (screenWidth - textSize) / 2;
    tft.setCursor(posX, posY_L3); // Position y au milieu de l'écran
    // Afficher le texte centré
    tft.println(message);
    refreshScreen=false;
  }//fin du if refreshScreen==true
}//fin du displayPassed

//Affiche soit "failed" soit "failed + chrono sous différentes formes cf displayTime"
void displayFailed(unsigned long elapsed=0, unsigned long days=0, bool flag = false) {
  
  if (refreshScreen==true) {
    unsigned long seconds = elapsed;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    int textSize=0;
    int posX =0;
    String message =String("");
    tft.fillScreen(tft.color565(255, 0, 0)); // écran rouge
    tft.setTextColor(TFT_BLACK); // Couleur du texte

    if (flag==false){
      if (days == 0) {
        if (elapsed>0){
          // Affiche le temps au format "hh:mm:ss"
          unsigned long secondsRemainder = seconds % 60;
          unsigned long minutesRemainder = minutes % 60;
          unsigned long hoursRemainder = hours % 24;
          
          // Taille du texte
          textSize = tft.textWidth(String("00:00:00"));
          
          // Calcul de la position x pour centrer le texte
          posX = (screenWidth - textSize) / 2;
          tft.setCursor(posX, posY_L2); // Position y au milieu de l'écran

          // Afficher le texte centré
          tft.printf("%02lu:%02lu:%02lu", hoursRemainder, minutesRemainder, secondsRemainder);
        }//fin du if elapse time >0
      } else {
        // Affiche le temps au format "jj jours hh:mm"
        unsigned long hoursRemainder = hours % 24;
        unsigned long minutesRemainder = minutes % 60;
        // Taille du texte
        textSize = tft.textWidth(String("00 jours"));
        // Calcul de la position x pour centrer le texte
        posX = (screenWidth - textSize) / 2;
        // Calcul de la position y pour centrer le texte
        tft.setCursor(posX, posY_L1);
        tft.printf("%03lu jours", days);
        // Taille du texte
        textSize = tft.textWidth(String("00:00"));
        // Calcul de la position x pour centrer le texte
        posX = (screenWidth - textSize) / 2;
        tft.setCursor(posX, posY_L2);
        tft.printf("%02lu:%02lu", hoursRemainder, minutesRemainder);
      }
    }
    else{
      //texte:
      message =String("> 49 jours");
      // Taille du texte
      textSize = tft.textWidth(message);
      // Calcul de la position x pour centrer le texte
      posX = (screenWidth - textSize) / 2;
      tft.setCursor(posX, posY_L1);
      tft.println(message);
    }
    //texte:
    message = "FAILED";
    // Taille du texte
    textSize = tft.textWidth(message);
    posX = (screenWidth - textSize) / 2;
    tft.setCursor(posX, posY_L3); // Position y au milieu de l'écran
    // Afficher le texte centré
    tft.println(message);
    refreshScreen=false;
  }//fin du if refreshScreen==true
}//fin du displayFailed

//mode d'accueil waiting command
void mode0() {
  if (refreshScreen==true) {
    // Définition du texte
    tft.setTextColor(TFT_WHITE); // Couleur du texte
    tft.fillScreen(TFT_BLACK); // on efface tout ce qui était affiché avant
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
  }//fin du if
}//fin du mode 0

//mode Start latché: affiche une progress barre puis "refresh time>x s at hh:mm:ss" sur fond rouge
//nécessite de rafraichir la barre avec de nouvelles commandes START, sinon on reste dans le mode.
//Pour sortir du mode, il faut débrancher ou envoyer une commande STOP
void mode2(unsigned long timeoutThreshold) {  
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;
  int textSize=0;
  int posX =0;

  if (elapsedTime <= timeoutThreshold) {
    int progressBarWidth = map(elapsedTime, 0, timeoutThreshold, 0, screenWidth - 2 * progressBarMargin);
    
    // Dessine la barre de progression
    tft.fillRect(screenWidth - progressBarMargin - progressBarWidth, (screenHeight - progressBarHeight) *2/ 3, progressBarWidth, progressBarHeight, 0x7BEF);
  } else {
    if(refreshScreen==true && timeout==false){
      // La progression est terminée
      tft.fillScreen(tft.color565(255, 0, 0)); // écran rouge
      tft.setTextColor(TFT_BLACK); // Couleur du texte
      String message="refresh time";
      // Taille du texte
      textSize = tft.textWidth(message);
      // Calcul de la position x pour centrer le texte
      posX = (screenWidth - textSize) / 2;
      tft.setCursor(posX, posY_L1); // Position y au milieu de l'écran
      tft.println(message);
      message="123s at:";
      // Taille du texte
      textSize = tft.textWidth(message);
      // Calcul de la position x pour centrer le texte
      posX = (screenWidth - textSize) / 2;
      tft.setCursor(posX, posY_L2); // Position y au milieu de l'écran
      tft.printf(">%02lus", timeoutThreshold/1000);
      startTime=0; // on réinitialise le chrono
      refreshScreen=false;
      timeout=true;
    }//fin du if refresh
  }//fin du else
}//fin du mode2
void loop() {
  // Lecture des données sur la liaison série
  if (USBSerial.available() > 0) { // Vérifie s'il y a des données disponibles à lire sur l'UART0
    char receivedData[20]; // Tableau pour stocker les données reçues
    int bytesRead = USBSerial.readBytesUntil('\n', receivedData, 20); // Lit les données jusqu'à un saut de ligne ('\n') ou jusqu'à 20 octets
    receivedData[bytesRead] = '\0'; // Ajoute un terminateur de chaîne
    // USBSerial.print("Données reçues : ");
    // USBSerial.println(receivedData); // Affiche les données reçues

    // Recherche de la sous-chaîne "$START pour les modes 2 et 3"
    char *startPtr = strstr(receivedData, "$START,");
    if (startPtr != NULL) {
      int m; // mode
      // Analyse de la sous-chaîne à partir de "$START" (m=mode 2 ou 3. Timeout (s))
      int result = sscanf(startPtr, "$START,%d,%lu", &m, &timeoutThreshold);
      //si le paterne match on extrait les données
      if (result == 2 && timeout==false) {
        refreshScreen=true;// on arme le refresh (il passe à false si timeout)
        startTime = millis(); // Démarre le chronomètre
        timeoutThreshold=timeoutThreshold*1000; //conversion du timeout en ms
        tft.fillScreen(TFT_BLACK); // on efface tout ce qui était affiché avant
        // on initialise la barre en blanc
        tft.fillRect(progressBarMargin, (screenHeight - progressBarHeight) *2/ 3, screenWidth - progressBarMargin*2, progressBarHeight, TFT_WHITE);
        if(m==2){
          mode=2;
        }
        if(m==3){
          mode=3;
        }
      }//fin du if result==2
    }//fin du if startPtr

    //Si c'est juste $START (chrono uniquement)
    if ((strcmp(receivedData, "$START") == 0) && timeout==false){
          // Démarre le chronomètre lorsque des données sont disponibles
      startTime = millis(); //démarre le chrono en récupérant le temps courrant
      mode=1;
    }//fin du if $START
    if (strcmp(receivedData, "$STOP") == 0){
      mode=0;
      refreshScreen=true;
      timeout=false;
    }//fin du if $STOP
    if ((strcmp(receivedData, "$PASS") == 0) && timeout==false){
      mode=4;
      refreshScreen=true;
    }//fin du if $PASS
    if ((strcmp(receivedData, "$FAIL") == 0) && timeout==false){
      mode=5;
      refreshScreen=true;
    }//fin du if $FAIL
  }//fin du if USBSerial

  switch(mode) {
    case 1: //chrono qui défille
      //si on n'a pas atteint le nb de jour max affichable on affiche le compteur, sinon on sature à "> 49 jours"
      if(maxdays==false && millis()< maxMillis){
      currentMillis = millis();
      }
      else{
        currentMillis=maxMillis;
        maxdays=true;
      }
      // on rafraichit l'écran que toutes les secondes (=interval)
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        unsigned long secondsElapsed = (currentMillis - startTime) / 1000;
        unsigned long days = secondsElapsed / 86400; // Nombre de jours écoulés
        unsigned long secondsRemainder = secondsElapsed % 86400; // Secondes restantes après le dernier jour complet
        elapsedTime = secondsRemainder; // Met à jour le temps écoulé en prenant en compte les secondes restantes après le dernier jour complet
        // si on s'approche de 2^32-1 (i.e la limite encodable alors on sature l'affichage)
        if (maxdays==false) {
          displayTime(elapsedTime, days);
        }else{
          displayTime(0, 49, true);
          }//fin du else
      }//fin du if
      break;
    case 2:
      mode2(timeoutThreshold);
      break;
    case 3:
      // mode3(timeoutThreshold);//TODO
      break;
    case 4: //mode 4 : PASSED avec l'affichage du chrono arreté
      //si on n'a pas atteint le nb de jour max affichable on affiche le compteur, sinon on sature à "> 49 jours"
      if(maxdays==false && millis()< maxMillis){
      currentMillis = millis();
      }
      else{
        currentMillis=maxMillis;
        maxdays=true;
      }
      //si on est dans le mode sans chrono (pas d'appel avec $START)
      if (startTime==0){
        displayPassed(); // on affiche uniquement "PASSED" sans chrono
      }else{
      unsigned long secondsElapsed = (currentMillis - startTime) / 1000;
      unsigned long days = secondsElapsed / 86400; // Nombre de jours écoulés
      unsigned long secondsRemainder = secondsElapsed % 86400; // Secondes restantes après le dernier jour complet
      elapsedTime = secondsRemainder; // Met à jour le temps écoulé en prenant en compte les secondes restantes après le dernier jour complet
      // si on s'approche de 2^32-1 (i.e la limite encodable alors on sature l'affichage)
      if (maxdays==false) {
        displayPassed(elapsedTime, days);
      }else{
        displayPassed(0, 49, true); //chrono à 0, 49 jours, maxdays=true
        }//fin du else
      }//fin du else starttime !=0
      break;
    case 5: //mode 4 : FAILED avec l'affichage du chrono arreté
      //si on n'a pas atteint le nb de jour max affichable on affiche le compteur, sinon on sature à "> 49 jours"
      if(maxdays==false && millis()< maxMillis){
      currentMillis = millis();
      }
      else{
        currentMillis=maxMillis;
        maxdays=true;
      }
      //si on est dans le mode sans chrono (pas d'appel avec $START)
      if (startTime==0){
        displayFailed(); // on affiche uniquement "PASSED" sans chrono
      }else{
      unsigned long secondsElapsed = (currentMillis - startTime) / 1000;
      unsigned long days = secondsElapsed / 86400; // Nombre de jours écoulés
      unsigned long secondsRemainder = secondsElapsed % 86400; // Secondes restantes après le dernier jour complet
      elapsedTime = secondsRemainder; // Met à jour le temps écoulé en prenant en compte les secondes restantes après le dernier jour complet
      // si on s'approche de 2^32-1 (i.e la limite encodable alors on sature l'affichage)
      if (maxdays==false) {
        displayFailed(elapsedTime, days);
      }else{
        displayFailed(0, 49, true); //chrono à 0, 49 jours, maxdays=true
        }//fin du else
      }//fin du else starttime !=0
      break;
    default:
      mode0();
      startTime=0; // on réinitialise le chrono
      refreshScreen=false;
      break;
  }//fin du switch
}//fin du loop

