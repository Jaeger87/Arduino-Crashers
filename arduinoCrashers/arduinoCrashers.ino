#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include <Servo.h>
#define TX_PIN 6 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 5 // Arduino receive   GREEN WIRE   labeled TX on printer
SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

Servo myservo;

Servo heroServo;
Servo enemyServo;
Servo princessServo;

const int heroPin = 8;
const int enemyPin = 9;
const int princessPin = 10;

const int yesButtonPin = 3;
const int noButtonPin = 4;

const int basePosition = 0;
const int walkPosition = 90;
const int defensePosition = 180;
const int attackPosition = 270;

int heroLife[3] = {11, 12, 13};
int enemyLife[3] = {14, 15, 16};
enum  story {
  INIT, GOINGTOSAVEPRINCESS, GOINGEQUALLY, HEROICENTRY, STEALTHENTRY, ENEMYDEFENSE, ENEMYSURPISED, ENEMYATTACHED, YOUDIE, YOUWIN
};
story chapter = INIT;


void setup() {
  Serial.begin(9600);

  // This line is for compatibility with the Adafruit IotP project pack,
  // which uses pin 2 as a spare grounding point.  You only need this if
  // wired up the same way (w/3-pin header into pins 5/6/2):
  pinMode(2, OUTPUT); digitalWrite(2, LOW);

  // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  mySerial.begin(19200);  // Initialize SoftwareSerial
  //Serial1.begin(19200); // Use this instead if using hardware serial
  printer.begin();        // Init printer (same regardless of serial type)
  pinMode(yesButtonPin, INPUT);
  pinMode(noButtonPin, INPUT);

  pinMode(heroLife[0], OUTPUT);
  pinMode(heroLife[1], OUTPUT);
  pinMode(heroLife[2], OUTPUT);

  digitalWrite(heroLife[0], HIGH);
  digitalWrite(heroLife[1], HIGH);
  digitalWrite(heroLife[2], HIGH);

  pinMode(enemyLife[0], OUTPUT);
  pinMode(enemyLife[1], OUTPUT);
  pinMode(enemyLife[2], OUTPUT);

  digitalWrite(enemyLife[0], HIGH);
  digitalWrite(enemyLife[1], HIGH);
  digitalWrite(enemyLife[2], HIGH);

  heroServo.attach(8);
  enemyServo.attach(9);
  princessServo.attach(10);
}

void loop() {
  switch (chapter)
  {
    case INIT:
      {
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

    default:
      {
        break;
      }

  }

  delay(50);
}



void changeChapter(int hero,int enemy,int princess,story newChapter)
{

  return;
}
