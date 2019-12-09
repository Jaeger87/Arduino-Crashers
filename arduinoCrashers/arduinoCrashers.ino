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

const int maxEnemyShield = 7;
int heroShield = 3;
int enemyShield = maxEnemyShield;

enum  story {
  SETUP, INIT, GOINGTOSAVEPRINCESS, GOINGEQUALLY, ENEMYCAMP, HEROICENTRY, STEALTHENTRY, ENEMYDEFENSE, ENEMYSURPISED, ENEMYATTACHED, YOUDIE, YOUWIN
};
story chapter = INIT;

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

  heroLife = 3;
  enemyLife = 3;
  heroShield = 3;
  enemyShield = maxEnemyShield;



  changeChapter(walkPosition, basePosition, basePosition, INIT);


}

void loop() {
  switch (chapter)
  {
    case INIT:
      {
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
        printer.println(F("Fuck yeah! let'go to kick some barbarians ass and save the princess!"));
        delay(2500);
        changeChapter(walkPosition, basePosition, walkPosition, ENEMYCAMP);
        break;
      }

    case GOINGEQUALLY:
      {
        printer.println(F("Are you serious?\nHow i suppose to continue the story if you choose to stay at Home?\nTake your sword and shield and go to save the princess, idiot."));
        delay(2500);
        changeChapter(walkPosition, basePosition, walkPosition, ENEMYCAMP);
        break;
      }

    case ENEMYCAMP:
      {
        printer.println(F("You are in front the enemy camp\nFrom your position ypu can see the princess, they bound her on a pale\nWho knows what will be her end if you don't save her!\n"
                          "Now, you have to take a decision.\nYou can break through the enemy gates and go forward with your strenght\n Or use your stealth skill and try to enter without the guards noticing.\n"
                          "Press green if you are brave, Press Red if you believe to be sly."));
        delay(800);
        if (waitButtonAndReturnYesButton())
          changeChapter(walkPosition, defensePosition, walkPosition, HEROICENTRY);
        else
          changeChapter(walkPosition, walkPosition, walkPosition, STEALTHENTRY);
        break;
      }
    case HEROICENTRY:
      {
        printer.println(F("Once you enter in the camp you kill quickly two guards and the others soldiers run away from fear,\n"
                          "when you reach the princess URZUNTUM appears in his defence position. What you gonna do?\n"
                          "Press Green to attack, Press Red to try to go round URZUNTUM and place at his shoulders."));
        delay(800);
        if (waitButtonAndReturnYesButton())
        {
          if(hitEnemyShield())
          {
            changeChapter(basePosition, basePosition, attackPosition, YOUWIN);
          }
          else 
          if(enemyShieldHurtYou())
          {
            changeChapter(basePosition, walkPosition, walkPosition, YOUDIE);
          }
        }
        else
        {

        }
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


bool waitButtonAndReturnYesButton()
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
    delay(60);
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

const int riskPoint = 8;
const int successPointHitShield = 12;

bool hitEnemyShield()//return true if you kill the enemy
{
  long randNumber = random(100);
  if (randNumber < (maxEnemyShield - enemyShield) * (successPointHitShield - (maxEnemyShield - enemyShield)))
  {
    printer.println(F("Your hit is such power that not only\ndamage URZUNTUM's shield\nbut even hits him."));
    delay(700);
    return enemyLoseLife();
  }
  printer.println(F("Your hit lashes out URZUNTUM's shield\n you don't hit your enemy but at least you have damaged\nhis shield."));
  return false;
}


bool enemyShieldHurtYou()//return true if the hero lose a life
{
  if (enemyShield > 0)
    enemyShield--;
  long randNumber = random(100);

  if (randNumber < (maxEnemyShield - enemyShield) * riskPoint)
  {
    printer.println(F("Unfortunatly a piece of the shield\nfall down and hit your head wounding you."));
    delay(700);
    heroLoseLife();
    return true;
  }
  return false;
}
