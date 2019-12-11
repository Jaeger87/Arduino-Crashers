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

const int longDelayPrinter = 3500;

int heroLifeLedPin[3] = {11, 12, 13};
int enemyLifeLedPin[3] = {14, 15, 16};

int heroLife = 3;
int enemyLife = 3;

bool enemyAdvantageP = false;
bool playerAdvantageP = false;
const int maxEnemyShield = 7;
int heroShield = 3;
int enemyShield = maxEnemyShield;

enum  story {
  SETUP, INIT, GOINGTOSAVEPRINCESS, GOINGEQUALLY, ENEMYCAMP, HEROICENTRY, STEALTHENTRY, ADVANTAGEPOSITION, ENEMYDEFENSE, ENEMYPREPARETOATTACK, ENEMYMOVE,
  ENEMYATTACK, YOUDIE, YOUWIN
};
story chapter = INIT;


const int pointsEnemyMove = 28;

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


  heroServo.write(basePosition);
  enemyServo.write(basePosition);
  princessServo.write(basePosition);

  enemyAdvantageP = false;
  playerAdvantageP = false;
  heroLife = 3;
  enemyLife = 3;
  heroShield = 3;
  enemyShield = maxEnemyShield;


  delay(4000);
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
                          "Press Green to attack\nPress Red to try to go round URZUNTUM\nand place at his shoulders."));
        delay(longDelayPrinter);
        if (waitButtonAndReturnYesButton())
        {
          procedureAttackEnemyShield();
        }
        else
        {
          procedureSmartWalk();
        }
        break;
      }

    case STEALTHENTRY:
      {
        printer.println(F("Like a shadow in the night you enter in the enemy camp and nobody noticed\nsilently ypou kill all the guards\n"
                          "and enter where they keep the princess, when you enter the room\nyou see the princess but is not alone\nURZUNTUM is with she.\n"
                          "You come closer and place\non URZUNTUM shoulders, where you want to attack?"
                          "Press green to direct attack URZUNTUM body\nPress Red to attack URUZUNTUM on his shield"));

        if (waitButtonAndReturnYesButton())
        {
          heroServo.write(attackPosition);
          delay(longDelayPrinter);
          if (hitEnemy())
          {
            changeChapter(basePosition, basePosition, attackPosition, YOUWIN);
          }

          changeChapter(attackPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
        }
        else
        {
          procedureAttackEnemyShield();
        }
        break;
      }

    case ADVANTAGEPOSITION:
      {
        playerAdvantageP = false;
        printer.println(F("It's the best chance to attack!\nPress green to direct attack URZUNTUM body\nPress Red to attack URUZUNTUM on his shield"));
        if (waitButtonAndReturnYesButton())
        {
          heroServo.write(attackPosition);
          delay(longDelayPrinter);
          if (hitEnemy())
          {
            changeChapter(basePosition, basePosition, attackPosition, YOUWIN);
          }

          changeChapter(attackPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
        }
        else
        {
          procedureAttackEnemyShield();
        }
        break;
      }

    case ENEMYDEFENSE:
      {
        printer.println(F("URZUNTUM raise his shield and\ngets defensive\nPress Green to attack\nPress Red to try to go round URZUNTUM\nand place at his shoulders."));
        delay(longDelayPrinter);
        if (waitButtonAndReturnYesButton())
        {
          procedureAttackEnemyShield();
        }
        else
        {
          procedureSmartWalk();
        }
        break;
      }
    case ENEMYMOVE:
      {
        printer.println(F("URZUNTUM starts to move\nhe want for sure to take a good position\nfor his attack.\n"
                          "Press Green to move and cancel his advantage\nPress red to do a really quick attack."));
        delay(longDelayPrinter);
        if (waitButtonAndReturnYesButton())
        {
          changeChapter(walkPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
        }
        else
        {
          procedureQuickAttack();
        }
        break;
      }
    case ENEMYPREPARETOATTACK:
      {
        break;
      }
    case ENEMYATTACK:
      {
        break;
      }

    case YOUDIE:
      {
        printer.println(F("So, you die!\nI suppose that is a thing that could\nhappen but honestly i programmed this game\nin a way"
                          "to avoid this, so\ncongratulations|\n\n\nI almost forgot to tell you that right now URZUNTUM is playing with you dead body\n it is so cute)\n."
                          "Your adventure is over, now is the time to cut\nyour paper and find a good place to conserve it\n Press any button to restart the Adventure.\n\n\n\n\n\n---------\n\n"));
        delay(3000);
        waitButtonAndReturnYesButton();
        changeChapter(basePosition, basePosition, basePosition, SETUP);
        break;
      }

    case YOUWIN:
      {
        delay(5000);
        printer.println(F("You made it!\n\nYou killed URZUNTUM and set free the princess\nShe is so happy that she kiss you\n(but you are not really smart\nand you kept your helmet on head)\n\n\n"
                          "Your adventure is over, now is the time to cut\nyour paper and find a good place to conserve it\n Press any button to restart the Adventure.\n\n\n\n\n\n---------\n\n"));
        
        waitButtonAndReturnYesButton();
        changeChapter(basePosition, basePosition, basePosition, SETUP);
        break;
      }

    case SETUP:
      {
        setup();
        delay(6000);
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

const int heroLifePointHit = 24;
const int enemyLifePointHit = 7;
bool hitEnemy()//return true if you kill the enemy
{
  long randNumber = random(100);
  if (randNumber < (heroLife * heroLifePointHit) - (enemyLife * enemyLifePointHit))
  {
    printer.println(F("Yes, you hit URZUNTUM."));
    delay(700);
    return enemyLoseLife();
  }
  printer.println(F("OH NOO\n You miss URZUNTUM and now the barbarian boss\nis ready to attack when you are unbalanced."));
  delay(1150);
  enemyAdvantageP = true;
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

const int walkLifePoints = 16;
const int walkShieldPoint = 4;
bool smartWalk()
{
  long randNumber = random(100);
  if (randNumber < (walkLifePoints * heroLife) - (walkShieldPoint * heroShield))
  {
    playerAdvantageP = true;
    printer.println(F("You move very fast and\nURZUNTUM stays in defense position."));
    delay(1200);
    return true;
  }
  printer.println(F("You are not so fast\n URZUNTUM understand your move and walks to\ncancel your advantage."));
  return false;
}

void procedureAttackEnemyShield()
{
  heroServo.write(attackPosition);
  delay(1500);
  if (hitEnemyShield())
  {
    changeChapter(basePosition, basePosition, attackPosition, YOUWIN);
  }
  else if (enemyShieldHurtYou())
  {
    changeChapter(basePosition, walkPosition, walkPosition, YOUDIE);
  }

  delay(2000);

  long randNumber = random(100);
  if (randNumber < enemyLife * pointsEnemyMove)
    changeChapter(attackPosition, walkPosition, walkPosition, ENEMYMOVE);
  else
    changeChapter(attackPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
}

void procedureSmartWalk()
{
  heroServo.write(walkPosition);
  delay(100);
  if (smartWalk())
    changeChapter(walkPosition, defensePosition, walkPosition, ADVANTAGEPOSITION);
  else
    changeChapter(walkPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
}

const int quickLifePoints = 5;
const int quickShieldPoint = 2;
const int quickEnemyLifePoints = 1;
const int quickEnemyShieldPoint = 5;
void procedureQuickAttack()
{
  heroServo.write(attackPosition);
  delay(100);
  enemyAdvantageP = true;
  long randNumber = random(100);
  if (randNumber < (heroLife * quickLifePoints) - (enemyLife * quickEnemyLifePoints) - (heroShield * quickShieldPoint) + (enemyShield * quickEnemyShieldPoint))
  {
    printer.println(F("Oh WOW *_*\nYou really hit URZUNTUM\nYou are very speed but most of all lucky\n\n(believe me, i programmed this game)."));
    delay(700);
    if (enemyLoseLife())
    {
      changeChapter(basePosition, basePosition, attackPosition, YOUWIN);
      return;
    }
  }
  printer.println(F("As was to be expected you miss URZUNTUM\nand now is in a advantage position"));
  delay(1150);
  changeChapter(attackPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
}
