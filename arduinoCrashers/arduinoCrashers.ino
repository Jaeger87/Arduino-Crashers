#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include <Servo.h>
#define TX_PIN 4 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 3 // Arduino receive   GREEN WIRE   labeled TX on printer
SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

Servo myservo;

Servo heroServo;
Servo enemyServo;
Servo princessServo;

const int heroPin = 8;
const int enemyPin = 9;
const int princessPin = 10;

const int yesButtonPin = 5;
const int noButtonPin = 6;

const int basePosition = 0;
const int walkPosition = 90;
const int defensePosition = 180;
const int attackPosition = 270;

int heroLifeLedPin[3] = {11, 12, 13};
int enemyLifeLedPin[3] = {14, 15, 16};

int heroLife = 3;
int enemyLife = 3;
bool shieldHealthy = true;

enum  story {
  SETUP, INIT, GOINGTOSAVEPRINCESS, GOINGEQUALLY, HEROICENTRY, STEALTHENTRY, ENEMYDEFENSE, ENEMYSURPISED, ENEMYATTACHED, YOUDIE, YOUWIN
};
story chapter = INIT;
bool havetoPrint = true;

void setup() {
  Serial.begin(9600);

  // This line is for compatibility with the Adafruit IotP project pack,
  // which uses pin 2 as a spare grounding point.  You only need this if
  // wired up the same way (w/3-pin header into pins 4/3/2):
  pinMode(2, OUTPUT); digitalWrite(2, LOW);

  // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  mySerial.begin(19200);  // Initialize SoftwareSerial
  //Serial1.begin(19200); // Use this instead if using hardware serial
  printer.begin();        // Init printer (same regardless of serial type)
  pinMode(yesButtonPin, INPUT);
  pinMode(noButtonPin, INPUT);

  pinMode(heroLifeLedPin[0], OUTPUT);
  pinMode(heroLifeLedPin[1], OUTPUT);
  pinMode(heroLifeLedPin[2], OUTPUT);

  digitalWrite(heroLifeLedPin[0], HIGH);
  digitalWrite(heroLifeLedPin[1], HIGH);
  digitalWrite(heroLifeLedPin[2], HIGH);

  pinMode(enemyLifeLedPin[0], OUTPUT);
  pinMode(enemyLifeLedPin[1], OUTPUT);
  pinMode(enemyLifeLedPin[2], OUTPUT);

  digitalWrite(enemyLifeLedPin[0], HIGH);
  digitalWrite(enemyLifeLedPin[1], HIGH);
  digitalWrite(enemyLifeLedPin[2], HIGH);

  heroServo.attach(8);
  enemyServo.attach(9);
  princessServo.attach(10);



  changeChapter(walkPosition, basePosition, basePosition, INIT);


}

void loop() {
  switch (chapter)
  {
    case INIT:
      {
        havetoPrint = false;
        printer.println(F("Oh noo!\n The princess was kidnapped by the barbarian's boss  URZUNTUM!\n What you wanna do?"));
        printer.println(F("Press Green to accept the quest\nPress Red to stay at home and play castle crushers."));
        delay(200);
        bool acceptQuest = waitButtonAndReturnYesButton();
        if (acceptQuest)
          changeChapter(walkPosition, basePosition, basePosition, GOINGTOSAVEPRINCESS);
        else
          changeChapter(walkPosition, basePosition, basePosition, GOINGEQUALLY);
        break;
      }

    case GOINGTOSAVEPRINCESS:
      {
        break;
      }

    case GOINGEQUALLY:
      {
        break;
      }

    case HEROICENTRY:
      {
        break;
      }

    case STEALTHENTRY:
      {
        break;
      }

    case ENEMYSURPISED:
      {
        break;
      }

    case ENEMYDEFENSE:
      {
        break;
      }

    case YOUDIE:
      {
        break;
      }

    case YOUWIN:
      {
        break;
      }

    case SETUP:
      {
        setup();
        break;
      }

    default:
      {
        break;
      }

  }

  delay(50);
}


void waitButtonAndReturnYesButton()
{
  bool waitingForChoices = true;
  bool oldyesButtonState = false;
  int yesButtonState = 0;
  int noButtonState = 0;
  int oldYesButtonState = 0;
  int oldNoButtonState = 0;
  while (waitingForChoices)
  {
    yesButtonState = digitalRead(yesButtonPin);
    noButtonState = digitalRead(noButtonPin);

    if (yesButtonState != oldYesButtonState)
    {
      if (yesButtonState == HIGH)
        return true;
    }
    if (noButtonState != oldNoButtonState)
    {
      if (noButtonState == HIGH)
        return false;
    }
    oldYesButtonState = yesButtonState;
    oldNoButtonState = noButtonState;
    delay(50);
  }


}

void changeChapter(int hero, int enemy, int princess, story newChapter)
{
  heroServo.write(hero);
  enemyServo.write(enemy);
  princessServo.write(princess);
  chapter = newChapter;
  return;
}

bool heroLoseLife()
{
  heroLife--;
  digitalWrite(heroLifeLedPin[heroLife], LOW);
  if (heroLife == 0)
    return true;

  return false;
}

bool enemyLoseLife()
{
  enemyLife--;
  digitalWrite(enemyLifeLedPin[enemyLife], LOW);
  if (enemyLoseLife == 0)
    return true;

  return false;
}
