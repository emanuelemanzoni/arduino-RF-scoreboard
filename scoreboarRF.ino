//inspired by https://github.com/iliketomakestuff/BLEScoreBoard
//Emanuele Manzoni

#include <RCSwitch.h> //libreria per RF

#include <Adafruit_NeoPixel.h>      //std adafruit stuff
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN            3
#define NUMPIXELS      60

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);   //pixels will our adafruit obj
uint32_t colorList[] = { pixels.Color(255, 0, 0), pixels.Color(0 , 255, 0)  }; // here you can set colors

const int characterOffset = 15;
int charLayout[10][15] = {           //every row is one character on the panel
  {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1}, //0
  {1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1}, //1
  {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1}, //2
  {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1}, //3
  {1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1}, //4
  {1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1}, //5
  {1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}, //6
  {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1}, //7
  {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}, //8
  {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1} //9
};


RCSwitch mySwitch = RCSwitch();     //std rcswitch stuff
unsigned long code0 = 16432;       //the 4 different codes of the remote
unsigned long code1 = 1235;
unsigned long code2 = 1236;
unsigned long code3 = 1237;

int score[2] = {0, 0};     //score var: 0 for Left, 1 for Right


void setup() {
  //Serial.begin(9600);
  pixels.begin();
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  resetScore();
  Serial.print("boot ok");
}





void loop() {
  if (mySwitch.available()) {
    long remoteVal = mySwitch.getReceivedValue();
    if (remoteVal == code0) {
      leftAdd();
      while (mySwitch.available()) {
        mySwitch.resetAvailable();
        delay(100);
      }
    } else if (remoteVal == code1) {
      rightAdd();
      while (mySwitch.available()) {
        mySwitch.resetAvailable();
        delay(100);
      }
    } else if (remoteVal == code2) {
      leftSubtract();
      while (mySwitch.available()) {
        mySwitch.resetAvailable();
        delay(100);
      }
    } else if (remoteVal == code3) {
      rightSubtract();
      while (mySwitch.available()) {
        mySwitch.resetAvailable();
        delay(100);
      }
    }

    mySwitch.resetAvailable();  //reset for any other captured signals
  }

  if (!digitalRead(5)) {        //left add with button
    leftAdd();
    while (!digitalRead(5))
      delay(50);
  }
  if (!digitalRead(6)) {        //right add with button
    rightAdd();
    while (!digitalRead(6))
      delay(50);
  }
  if (!digitalRead(10)) {       //reset the score with button
    int counter10 = 0;
    while (!digitalRead(10)) {
      counter10++;
      if (counter10 == 30) {
        resetScore();
        resetSound();
      } else {
        delay(50);
      }
    }
  }
  noTone(7);
}


void toggleLocation(int team, int location, int ledState) {
  if (ledState == 0) {
    pixels.setPixelColor(location, pixels.Color(0, 0, 0)); //spegne il led, colore nero
  } else {
    pixels.setPixelColor(location, colorList[team]);
  }
}

void setNumber(int team, int teamScore) {     //displays the score
  int teamOffset = team * characterOffset * 2;
  int ones = (teamScore % 10);
  int tens = ((teamScore / 10) % 10);
  for (int i = 0; i < 15; i++) {
    toggleLocation(team, i + teamOffset, charLayout[tens][i]);
    toggleLocation(team, i + teamOffset + characterOffset, charLayout[ones][i]);
  }

  pixels.show();
}

void setScore(int team, int val) {    //update the score and calls the display function
  int teamScore = score[team];
  if (val == 0) {
    teamScore = 0;
  } else {
    teamScore += val;
    if (teamScore < 0) {
      teamScore = 0;
    }
  }
  score[team] = teamScore;
  setNumber(team, teamScore);
}

void leftAdd() {
  setScore(0, 1);
  tone(7, 2400);
}

void rightAdd() {
  setScore(1, 1);
  tone(7, 1900);
}

void leftSubtract() {
  setScore(0, -1);
}

void rightSubtract() {
  setScore(1, -1);
}

void resetScore() {
  setScore(0, 0);
  setScore(1, 0);
}

void resetSound() {
  tone(7, 4000, 100);
  delay(100);
  tone(7, 3000, 100);
  delay(100);
  tone(7, 2000, 100);
  delay(100);
}
