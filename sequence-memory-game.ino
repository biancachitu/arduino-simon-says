#include <avr/io.h>
#include <avr/interrupt.h>
#include <TimerOne.h>

const int btnRed = 2;
const int btnYlw = 3;
const int btnGreen = 4;
const int btnBlue = 5;

const int ledRed = 7;
const int ledYlw = 8;
const int ledGreen = 9;
const int ledBlue = 10;
 

int start = 0;
int waitForInput = 0;
int currentLevel = 1;
int nrLevels = 8;

int randNr;
int randSeqNr = 0;

volatile bool correct = false;

int seqArray[8];
int userInputSeq[8];

int buttonState[] = {0, 0, 0, 0};

volatile unsigned long millisCounter = 0;
volatile unsigned long lastMillis = 0;
volatile unsigned long lastMillisLeds = 0;
volatile unsigned long lastMillisGameOver = 0;

volatile byte state;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
volatile int currentLed;

void setup() {
  Serial.begin(9600);

  pinMode(btnRed, INPUT);
  pinMode(btnYlw, INPUT);
  pinMode(btnGreen, INPUT);
  pinMode(btnBlue, INPUT);

  pinMode(ledRed, OUTPUT);
  pinMode(ledYlw, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);

  randomSeed(analogRead(0));

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
}

void loop() {
  if (start == 0) {
    generateSequence();
    start = 1;
  }

  if (waitForInput == 0) {
    ledsLightUp();
    waitForInput = 1;
  }

  checkUserInput();
}

void generateSequence() {
  for (int i = 0; i < nrLevels; i++) {
    randNr = random(1, 200);
    if (randNr <= 50)
      randSeqNr = 7;
    else if (randNr > 50 && randNr <= 100)
      randSeqNr = 8;
    else if (randNr > 100 && randNr <= 150)
      randSeqNr = 9;
    else if (randNr <= 200)
      randSeqNr = 10;

    seqArray[i] = randSeqNr;
  }
}

void notDelay(int interval) {
  while(true){
    currentMillis = millis();

    Serial.println(currentMillis - previousMillis);
    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      break;
    }
  }
}

void ledsLightUp() {
    notDelay(400);
    for (int i = 0; i < currentLevel; i++) {
      digitalWrite(seqArray[i], HIGH);

      notDelay(600);

      digitalWrite(seqArray[i], LOW);

      notDelay(400);
    }   
}

void checkUserInput() {
  int buttonChange = 0;
  int i = 0;
  int j = 0;

  while (j < currentLevel) {
    while (buttonChange == 0) {
      for (i = 0; i < 4; i++) {
        buttonState[i] = digitalRead(i + 2);
        buttonChange = buttonChange + buttonState[i];
      }
    }

    for (i = 0; i < 4; i++) {
      if (buttonState[i] == HIGH) {
        
        digitalWrite(i + 7, HIGH);
        delay(200);
        digitalWrite(i + 7, LOW);

        waitForInput = 0;
        userInputSeq[j] = i + 7;
        buttonState[i] = LOW;
        buttonChange = 0;
      }
    }

    if (userInputSeq[j] == seqArray[j]) {
      j++;
      correct = true;
    } else {
      correct = false;
      i = 4;
      j = currentLevel;
      waitForInput = 0;
    }
  }

  if (correct == false) {
    delay(300);
    i = 0;
    start = 0;
    currentLevel = 1;
    gameOver(2000);
    delay(500);
    start = 0;
  }

  if (correct == true) {
    currentLevel++;
    waitForInput = 0;
  }

  if (currentLevel == nrLevels) {
    delay(500);
    winnerWinnerChickenDinner(2000);
    start = 0;
    currentLevel = 1;
  }
}

void winnerWinnerChickenDinner(int duration) {
  int waveDelay = 100;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < currentLevel; j++) {
      digitalWrite(seqArray[j], HIGH);
      delay(waveDelay);
      digitalWrite(seqArray[j], LOW);
    }
  }
}

void gameOver(int duration) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(i+7, HIGH);
  }

  delay(duration);

  for (int i = 0; i < 4; i++) {
    digitalWrite(i+7, LOW);
  }
}