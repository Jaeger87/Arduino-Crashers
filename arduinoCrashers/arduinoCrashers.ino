/*
  Arduino crashers

  Arduino crashers è un gioco realizzato utilizzando un arduino, una stampante termica, due bottoni per quanto riguarda l'essenziale
  6 led e 3 servomotori per la parte "estetica".

  Nel gioco il giocatore interpreta il ruolo di un cavaliere che deve salvare una principessa, per farlo dovrà combattere contro il capo dei barbari.

  La storia del gioco viene raccontata testualmente tramite la stampante termica, il giocatore dovrà prendere delle decisioni nel gioco
  utilizzando i due bottoni verde e rosso. i 3 servo motori hanno un piccolo sistema che mostra i personaggi in pose diverse che cambiano
  a seconda di quello che succede nel gioco. I led rappresentano le vite del giocatore e del nemico da battere, se il giocatore perde tutti i led (3 vite)
  Perde e il gioco termina in game over. Altrimenti se elimina le vite dell'avversario il giocatore vince e salva la principessa.

  Per il circuito fare riferimento alla relazione e allo schema fritzing

  created 2019
  by Andrea Rosati a.k.a. Jaeger87 <https://github.com/Jaeger87>
*/

//Caricamento librerie.
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include <Servo.h>
#define TX_PIN 4 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 3 // Arduino receive   GREEN WIRE   labeled TX on printer
SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

//Dichiarazione degli oggetti per l'utilizzo dei servo motori
Servo heroServo;
Servo enemyServo;
Servo princessServo;

//Pin usati dai servo
const int heroPin = 8;
const int enemyPin = 9;
const int princessPin = 10;
//Pin usati dai due bottoni, yes è il verde, no il rosso
const int yesButtonPin = 5;
const int noButtonPin = 6;
//Le posizioni che possono assumere i servo, sono uguali per tutti e tre i servo.
const int basePosition = 10;
const int walkPosition = 65;
const int defensePosition = 120;
const int attackPosition = 170;
//I delay per aspettare che la stampante termica abbia finito di stampare (o quasi).
const int shortDelayPrinter = 800;
const int longDelayPrinter = 1500;
const int veryLongDelayPrinter = 4500;
//pin dei led divisi per personaggio
int heroLifeLedPin[3] = {11, 12, 13};
int enemyLifeLedPin[3] = {7, 15, 14};
//variabili per gestire la vita dei personaggi
int heroLife = 3;
const int maxEnemyLife = 3;
int enemyLife = maxEnemyLife;
//Qui ci sono alcune variabili per gestire alcune dinamiche del gioco quali la gestione dello scudo e se un personaggio si trova in una posizione di vantaggio.
bool enemyAdvantageP = false;
bool playerAdvantageP = false;
const int maxEnemyShield = 7;
const int maxHeroShield = 3;
int heroShield = maxHeroShield;
int enemyShield = maxEnemyShield;

//Dichiarazione dei capitoli della storia (stati in cui si può trovare il gioco)
enum  story {
  SETUP, INIT, GOINGTOSAVEPRINCESS, GOINGEQUALLY, ENEMYCAMP, HEROICENTRY, STEALTHENTRY, ADVANTAGEPOSITION, ENEMYDEFENSE, ENEMYPREPARETOATTACK, ENEMYMOVE,
  ENEMYATTACK, YOUDIE, YOUWIN
};
story chapter = INIT;


void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);  // Initialize SoftwareSerial
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
  princessServo.attach(10, 0, 180); // è speciale il servo XD (mezzo scassato)

  //I personaggi iniziano il gioco nella base position che significa non visibili
  heroServo.write(basePosition);
  enemyServo.write(basePosition);
  princessServo.write(basePosition);

  enemyAdvantageP = false;
  playerAdvantageP = false;
  heroLife = 3;
  enemyLife = maxEnemyLife;
  heroShield = maxHeroShield;
  enemyShield = maxEnemyShield;


  delay(4000);
  changeChapter(walkPosition, basePosition, basePosition, INIT);//Il gioco così passa allo stato iniziale.
}

bool defenseHero = false;
void loop() {
  delay(40);//Ci sta anche alla fine, però alcune volte si può uscire dal loop senza passare per il delay finale. È qui per precauzione.
  switch (chapter)
  {
    /*
       Il capitolo iniziale del gioco, qui il giocatore deve scegliere se andare a salvare la principessa o non farlo.
       È una scelta fasulla, non cambia nulla nel gioco qualsiasi decisione prenda, ma aiuta il giocatore a capire come funzionerà il gioco.
    */
    case INIT:
      {
        printer.println(F("Oh noo!\nThe princess was kidnapped by\nthe barbarian's boss URZUNTUM!\nWhat you wanna do?\n\n"));
        delay(shortDelayPrinter);
        printer.println(F("Press Green to accept the quest\nPress Red to stay at home and\nplay Castle Crushers.\n\n"));
        delay(longDelayPrinter);
        bool acceptQuest = waitButtonAndReturnYesButton();
        if (acceptQuest)
          changeChapter(walkPosition, basePosition, basePosition, GOINGTOSAVEPRINCESS);
        else
          changeChapter(walkPosition, basePosition, basePosition, GOINGEQUALLY);
        break;
      }
    /*
       Captolo successivo all'INIT. qui il giocatore ha scelto di salvare la principessa. la stampante aggiorna il giocatore della sua scelta e
       Si passa al capitolo successivo.
    */
    case GOINGTOSAVEPRINCESS:
      {
        printer.println(F("Fuck yeah! let'go to kick some\nbarbarians ass and save\nthe princess!\n\n"));
        delay(longDelayPrinter);
        changeChapter(walkPosition, basePosition, walkPosition, ENEMYCAMP);
        break;
      }
    /*
       Captolo successivo all'INIT. Il giocatore aveva scelto di rimanere a casa ma viene costretto ad andare a salvare la principessa.
       Si passa al capitolo successivo.
    */
    case GOINGEQUALLY:
      {
        printer.println(F("Are you serious?\nHow i suppose to continue the\nstory if you choose to stay\nat Home?\nTake your sword and shield\nand go to save the princess\nidiot.\n\n"));
        delay(longDelayPrinter);
        changeChapter(walkPosition, basePosition, walkPosition, ENEMYCAMP);
        break;
      }
    /*
       In questo capitolo il giocatore arriva al campo nemico dove la pricipessa è tenuta prigioniera.
       Gli viene chiesto se vuole fare un'entrata eroica sfondando la porta o silenziosa.
       Nel primo caso il giocatore entrerà e troverà il nemico in posizione difensiva, nel secondo sorprenderà il nemico.
       La seconda opzione è più conveniente.
    */
    case ENEMYCAMP:
      {
        printer.println(F("You are in front the enemy camp\nFrom your position you can see\nthe princess, they bound her on a pale\nWho knows what will be her end\nif you don't save her!\n"
                          "Now, you have to take a decision\n\nYou can break through the enemy gates and go forward with your\nstrenght or use your stealth\nskills and try to enter without\nthe guards noticing.\n"
                          "Press green if you are brave\nPress Red if you believe\nto be smart.\n\n"));
        delay(800);
        if (waitButtonAndReturnYesButton())
          changeChapter(walkPosition, defensePosition, walkPosition, HEROICENTRY);
        else
          changeChapter(walkPosition, walkPosition, walkPosition, STEALTHENTRY);
        break;
      }
    /*
      Capitolo dell'entrata eroica, qui il giocatore trova il nemico in posizione difensiva.
      Dal punto di vista del gameplay questo capitolo è identico a ENEMYDEFENSE. cambia solo la scritta iniziale stampata.
      Le scelte sono se attaccare il nemico subito o provare ad ottenere una posizione vantaggiosa.
    */
    case HEROICENTRY:
      {
        printer.println(F("Once you enter in the camp you\nkill quickly two guards and the\nothers soldiers run away\nfrom fear,\n"
                          "When you reach the princess\nURZUNTUM appears in his defence\nposition. What you gonna do?\n"
                          "Press Green to attack\nPress Red to try to go round\nURZUNTUM and place at his\nshoulders.\n\n"));
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
    /*
       Capitolo del giocatore se ha scelto di entrare silenziosamente nel campo nemico. Il capitolo è identico a ADVANTAGEPOSITION con la scritta iniziale diversa
       Qui il giocatore deve scegliere se attaccare il nemico direttamente o sullo scudo, la differenza è che direttamente c'è il rischio di mancarlo.
    */
    case STEALTHENTRY:
      {
        printer.println(F("Like a shadow in the night\nyou enter in the enemy camp and nobody noticed\nsilently you kill all the guards\n"
                          "and enter where they keep the\nprincess, when you enter the\nroom you see the princess\nbut is not alone\nURZUNTUM is with she.\n"
                          "You come closer and place\non URZUNTUM shoulders, where you\nwant to attack?\n"
                          "Press green to direct attack\nURZUNTUM body\nPress Red to attack URUZUNTUM on\nhis shield\n\n"));

        if (waitButtonAndReturnYesButton())
        {
          heroServo.write(attackPosition);
          delay(longDelayPrinter);
          if (hitEnemy())
          {
            changeChapter(basePosition, basePosition, attackPosition, YOUWIN);
            return;
          }

          changeChapter(attackPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
        }
        else
        {
          procedureAttackEnemyShield();
        }
        break;
      }
    /*
       In questo capitolo il giocatore si trova in una posizione di vantaggio e può attaccare il nemico.
       Qui il giocatore deve scegliere se attaccare il nemico direttamente o sullo scudo, la differenza è che direttamente c'è il rischio di mancarlo.
    */
    case ADVANTAGEPOSITION:
      {
        playerAdvantageP = false;
        printer.println(F("It's the best chance to attack!\nPress green to direct attack\nURZUNTUM body\nPress Red to attack URUZUNTUM on\nhis shield\n\n"));
        if (waitButtonAndReturnYesButton())
        {
          heroServo.write(attackPosition);
          delay(longDelayPrinter);
          if (hitEnemy())
          {
            changeChapter(basePosition, basePosition, attackPosition, YOUWIN);
            return;
          }

          changeChapter(attackPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
        }
        else
        {
          procedureAttackEnemyShield();
        }
        break;
      }
    /*
       In questo capitolo il nemico è in una posizione difensiva.
       Le scelte sono se attaccare il nemico subito o provare ad ottenere una posizione vantaggiosa.
    */
    case ENEMYDEFENSE:
      {
        printer.println(F("URZUNTUM raise his shield and\ngets defensive\nPress Green to attack\nPress Red to try to go round\nURZUNTUM and place at\nhis shoulders.\n\n"));
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
    /*
       In questo capitolo il nemico ha deciso di muoversi per prendere una posizione di vantaggio per il suo attacco.
       Il giocatore può provare a colpirlo al volo oppure muoversi annullando così il vantaggio di posizione del nemico.
    */
    case ENEMYMOVE:
      {
        printer.println(F("URZUNTUM starts to move\nhe want for sure to take a good position for his attack.\n"
                          "Press Green to move and cancel\nhis advantage\nPress red to do a really\nquick attack.\n\n"));
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
    /*
       In questo capitolo il nemico si prepara ad attaccare.
       Il giocatore deve decidere se parare o evitare il colpo.
    */
    case ENEMYPREPARETOATTACK:
      {
        printer.println(F("URZUNTUM is ready to attack you at any moment.\nWhat you gonna do?\n\n"
                          "Press green to raise\nyour shield\nPress red to try to dodge\nthe hit\n\n"));
        if (waitButtonAndReturnYesButton())
        {
          printer.println(F("You decided to parry the hit\nhope that the shield resist...\n\n"));
          defenseHero = true;
          delay(veryLongDelayPrinter);
          changeChapter(defensePosition, attackPosition, walkPosition, ENEMYATTACK);
        }
        else
        {
          printer.println(F("A brave decision\nlet's see if it was even quick..\n\n"));
          delay(veryLongDelayPrinter);
          changeChapter(walkPosition, attackPosition, walkPosition, ENEMYATTACK);
        }
        break;
      }
    /*
       In questo capitolo il nemico attacca.
       Il giocatore qui non prende decisioni, viene calcolato se subisce danno in base alla scelta fatta in ENEMYPREPARETOATTACK
       Dopo l'attacco il nemico prende una decisione tra difesa, preparaattacco o muoversi.
    */
    case ENEMYATTACK:
      {
        printer.println(F("URZUNTUM uses his shield to\nattack you,\nit's a really speed and powerful\nattack\n\n\n"));
        delay(longDelayPrinter);
        if (defenseHero)
        {
          if (procedureParring())
          {
            changeChapter(basePosition, walkPosition, walkPosition, YOUDIE);
            return;
          }
          procedureUrzuntumChoice(defensePosition);
        }
        else
        {

          if (procedureDodge())
          {
            changeChapter(basePosition, walkPosition, walkPosition, YOUDIE);
            return;
          }
          procedureUrzuntumChoice(walkPosition);
        }
        enemyAdvantageP = false;
        break;
      }
    /*
       Capitolo del gameover.
       Si arriva qui se si ha perso tutte le vite, il gioco finisce e si chiede al giocatore di premere un tasto qualsiasi per ricominciare.
    */
    case YOUDIE:
      {
        delay(veryLongDelayPrinter);
        printer.println(F("So, you die!\nI suppose that is a thing that\ncould happened but honestly\ni programmed this game\nin a way "
                          "to avoid this, so\ncongratulations|\n\n\nI almost forgot to tell you that\nright now URZUNTUM is playing\nwith your dead body\n\n(it is so cute)\n\n"
                          "Your adventure is over, now\nit's time to cut your paper\nand find a good place to\nconserve it\nPress any button to restart the Adventure.\n\n\n\n\n\n---------\n\n"));
        delay(longDelayPrinter);
        waitButtonAndReturnYesButton();
        changeChapter(basePosition, basePosition, basePosition, SETUP);
        break;
      }
    /*
       Capitolo finale
       Si arriva qui se si uccide il nemico, il gioco finisce e si chiede al giocatore di premere un tasto qualsiasi per ricominciare.
    */
    case YOUWIN:
      {
        delay(veryLongDelayPrinter);
        printer.println(F("You made it!\n\nYou killed URZUNTUM and set free the princess\nShe is so happy that she kiss you\n(but you are not really smart\nand you kept your helmet on head)\n\n\n"
                          "Your adventure is over, now it's time to cut\nyour paper and find a good place to conserve it\nPress any button to restart the Adventure.\n\n\n\n\n\n---------\n\n"));

        waitButtonAndReturnYesButton();
        changeChapter(basePosition, basePosition, basePosition, SETUP);
        break;
      }
    /*
       Qui il gioco viene ripristinato allo stato iniziale.
    */
    case SETUP:
      {
        setup();
        delay(veryLongDelayPrinter);
        break;
      }

    default:
      {
        break;
      }
  }

  delay(50);
}

/*
   Questa funzione crea un loop in attesa che l'utente prema un tasto
   Un tasto è premuto se l'utente prima lo preme e poi lo rilascia.
   In questo modo se cambia idea all'ultimo (prima di rilasciare il tasto) può premere al volo l'altro pulsante.
*/
bool waitButtonAndReturnYesButton()
{
  bool waitingForChoices = true;
  int yesButtonState = 1;
  int noButtonState = 1;
  int oldYesButtonState = 1;
  int oldNoButtonState = 1;
  Serial.println("While bottoni");
  while (waitingForChoices)
  {
    yesButtonState = digitalRead(yesButtonPin);
    noButtonState = digitalRead(noButtonPin);



    if (yesButtonState != oldYesButtonState)
    {
      if (yesButtonState == HIGH)
      {
        Serial.println("YES!");
        return true;
      }
    }
    if (noButtonState != oldNoButtonState)
    {
      if (noButtonState == HIGH)
      {
        Serial.println("NO!");
        return false;
      }
    }
    oldYesButtonState = yesButtonState;
    oldNoButtonState = noButtonState;
    delay(60);
  }
}

/*
   Funzione per cambiare capitolo, oltre al cambio capitolo vengono spostati (se serve) i servo motori dei personaggi
   Facendogli cambiare stato.
*/
void changeChapter(int hero, int enemy, int princess, story newChapter)
{
  heroServo.write(hero);
  enemyServo.write(enemy);
  princessServo.write(princess);
  chapter = newChapter;
  return;
}

/*
   Toglie una vita al giocatore, spegne un led e controlla se è morto.
*/
bool heroLoseLife()
{
  heroLife--;
  digitalWrite(heroLifeLedPin[heroLife], LOW);
  if (heroLife == 0)
    return true;

  return false;
}

/*
   Toglie una vita al nemico, spegne un led e controlla se è morto.
*/
bool enemyLoseLife()
{
  enemyLife--;
  digitalWrite(enemyLifeLedPin[enemyLife], LOW);
  if (enemyLoseLife == 0)
    return true;

  return false;
}

const int riskPoint = 8;
const int successPointHitShield = 16;
/*
   Funzione che viene chiamata quando il giocatore colpisce lo scudo nemico.
   In base a quanto è danneggiato lo scudo nemico c'è una probabilità che il colpo ferisca il nemico oltre a rovinargli lo scudo.
*/
bool hitEnemyShield()//return true if you kill the enemy
{
  long randNumber = random(100);
  if (randNumber < 10 + (maxEnemyShield - enemyShield) * (successPointHitShield - (maxEnemyShield - enemyShield)))
  {
    printer.println(F("Your hit is such power that not only\ndamage URZUNTUM's shield\nbut even hits him.\n\n"));
    delay(shortDelayPrinter);
    return enemyLoseLife();
  }
  printer.println(F("Your hit lashes out URZUNTUM's\nshield, you don't hit your\nenemy but at least you\nhave damaged his shield.\n\n"));
  return false;
}


const int heroLifePointHit = 27;
const int enemyLifePointHit = 6;
/*
   Funzione che viene chiamata se il giocatore attacca direttamente il nemico.
   In base alla vita del giocatore e del nemico viene calcolata la probabilità di mancare o colpire il nemico.
   Difficile mancarlo qui, ma possibile.
*/
bool hitEnemy()//return true if you kill the enemy
{
  long randNumber = random(100);
  if (randNumber < 16 + (heroLife * heroLifePointHit) - (enemyLife * enemyLifePointHit))
  {
    printer.println(F("Yes, you hit URZUNTUM."));
    delay(shortDelayPrinter);
    return enemyLoseLife();
  }
  printer.println(F("OH NOO\n You miss URZUNTUM and now the barbarian boss\nis ready to attack you in an advantage position!\n\n"));
  delay(shortDelayPrinter);
  enemyAdvantageP = true;
  return false;
}

/*
   Qui dopo aver colpito lo scudo nemico, viene calcolato se un pezzo di scudo cade e colpisce il giocatore in testa facendogli perdere una vita.
   Fa rosicare se capita e non è così difficile che capiti.
*/
bool enemyShieldHurtYou()//return true if the hero lose a life
{
  if (enemyShield > 0)
    enemyShield--;
  long randNumber = random(100);

  if (randNumber < (maxEnemyShield - enemyShield) * riskPoint)
  {
    printer.println(F("Unfortunatly a piece of the\nshield fall down and hit\nyour head wounding you.\n\n"));
    delay(shortDelayPrinter);
    return heroLoseLife();
  }
  return false;
}

const int walkLifePoints = 19;
const int walkShieldPoint = 4;
/*
  Qui il giocatore tenta di mettersi in una posizione favorevole per colpire il nemico.
*/
bool smartWalk()
{
  long randNumber = random(100);
  if (randNumber < (walkLifePoints * heroLife) - (walkShieldPoint * heroShield))
  {
    playerAdvantageP = true;
    printer.println(F("You move very fast and\nURZUNTUM stays in defense\nposition."));
    delay(shortDelayPrinter);
    return true;
  }
  printer.println(F("You are not so fast\nURZUNTUM understand your move\nand walks to cancel\nyour advantage.\n\n"));
  return false;
}

const int pointsEnemyMove = 28;
/*
   La procedura che viene chiamata quando il giocatore decide di attaccare il nemico sullo scudo
*/
void procedureAttackEnemyShield()
{
  heroServo.write(attackPosition);
  delay(shortDelayPrinter);
  if (hitEnemyShield())
  {
    changeChapter(basePosition, basePosition, attackPosition, YOUWIN);
    return;
  }
  else if (enemyShieldHurtYou())
  {
    changeChapter(basePosition, walkPosition, walkPosition, YOUDIE);
    return;
  }

  delay(shortDelayPrinter);

  long randNumber = random(100);
  if (randNumber < enemyLife * pointsEnemyMove)
    changeChapter(attackPosition, walkPosition, walkPosition, ENEMYMOVE);
  else
    changeChapter(attackPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
}

/*
   Qui il nemico decide cosa fare dopo che è stato in posizione difensiva.
*/
void procedureUrzuntumDecision(int heroPosition)
{
  printer.println(F("What will gonna do URZUNTUM...\n\n"));
  delay(shortDelayPrinter);

  long randNumber = random(100);
  if (randNumber < enemyLife * pointsEnemyMove)
    changeChapter(heroPosition, walkPosition, walkPosition, ENEMYMOVE);
  else
    changeChapter(heroPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
}
/*
   Funzione che viene chiamata se il giocatore decide di eseguire la camminata "furba".
*/
void procedureSmartWalk()
{
  heroServo.write(walkPosition);
  delay(shortDelayPrinter);
  if (smartWalk())
    changeChapter(walkPosition, defensePosition, walkPosition, ADVANTAGEPOSITION);
  else
    changeChapter(walkPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
}

const int quickLifePoints = 6;
const int quickShieldPoint = 2;
const int quickEnemyLifePoints = 1;
const int quickEnemyShieldPoint = 6;
/*
   Qui viene calcolato se l'attacco rapido quando il nemico si muove va a buon fine
   Molto difficile che succeda.
*/
void procedureQuickAttack()
{
  heroServo.write(attackPosition);
  delay(shortDelayPrinter);
  enemyAdvantageP = true;
  long randNumber = random(100);
  if (randNumber < (heroLife * quickLifePoints) - (enemyLife * quickEnemyLifePoints) - (heroShield * quickShieldPoint) + (enemyShield * quickEnemyShieldPoint))
  {
    printer.println(F("Oh WOW *_*\nYou really hit URZUNTUM\nYou are very speed but most of\nall lucky\n\n(believe me, i programmed\nthis game).\n\n"));
    delay(shortDelayPrinter);
    if (enemyLoseLife())
    {
      changeChapter(basePosition, basePosition, attackPosition, YOUWIN);
      return;
    }
  }
  printer.println(F("As was to be expected you miss\nURZUNTUM and now is in\na advantage position\n\n"));
  delay(longDelayPrinter);
  changeChapter(attackPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
}

const int parringPoint = 29;
/*
   Qui viene calcolato se il colpo del nemico è parato con successo
   In qualsiasi caso lo scudo è danneggiato
*/
bool procedureParring()
{
  defenseHero = false;


  long randNumber = random(100);
  int malusPosition = 0;
  if (enemyAdvantageP)
    malusPosition += 31;
  if (randNumber < 9 + (heroShield * parringPoint) - malusPosition)
  {
    printer.println(F("The shield protected you.\nBut off course has suffered\ndamages.\n\n\n"));
    if (heroShield > 0)
      heroShield--;
    return false;
  }
  if (heroShield > 0)
    heroShield--;
  printer.println(F("The URZUNTUM hit is such power\nthat your shield was not able to\nprotect you\n\n\n"));
  delay(shortDelayPrinter);
  return heroLoseLife();
}

const int dodgeShieldPoint = 12;
const int baseDodgePoints = 16;
/*
   Qui viene calcolato se la schivata del giocatore è andata a buon fine.
   Più si è sani, più lo scudo è danneggiato e più il nemico e ferito e più si ha probabilità.
*/
bool procedureDodge()
{
  printer.println(F("Urzuntum raise up his shield up\nhis head and with all of\nhis strenght hits the ground\n\nyou...\n\n\n\n\n"));
  delay(shortDelayPrinter);
  long randNumber = random(100);
  int malusPosition = 0;
  if (enemyAdvantageP)
    malusPosition += 31;
  if (randNumber < (baseDodgePoints + (maxHeroShield - heroShield) * dodgeShieldPoint) - malusPosition + (maxEnemyLife - enemyLife) * 8)
  {
    printer.println(F("WAS ABLE TO DODGE\n\nIt's incredible, you did not\nreceive any damage!\n\n"));
    return false;
  }
  printer.println(F("Was not enough fast to dodge it :(\n\nthis hit wounded you.\n\n\n"));
  delay(shortDelayPrinter);
  return heroLoseLife();
}

/*
   Qui il nemico decide cosa fare dopo aver attaccato
   Più è ferito e più e probabile che decida di riattaccare.
*/
void procedureUrzuntumChoice(int heroPosition)
{
  long randNumber = random(100);
  if (randNumber < 18 + (maxEnemyLife - enemyLife) * 10)
  {
    changeChapter(heroPosition, walkPosition, walkPosition, ENEMYMOVE);
    return;
  }
  if (randNumber < 8 + (maxEnemyLife - enemyLife) * 16)
  {
    changeChapter(heroPosition, walkPosition, walkPosition, ENEMYPREPARETOATTACK);
    return;
  }
  changeChapter(heroPosition, defensePosition, walkPosition, ENEMYDEFENSE);
}
