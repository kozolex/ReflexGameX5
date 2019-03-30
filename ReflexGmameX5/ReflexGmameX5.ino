#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

int led = 0, ledBefore = 20;
float timeGameAll, timeRecord = 99000;
int timeAverage;

int countRound = 5;
unsigned long  timeGameStart, timeGameEnd, timePointStart, timePointEnd ;
bool pointDecision = false;
short int i, point = 0;
short int pointAllow = 0;

void point_count() {        // Add points
  pointDecision = true;
}

void avoid_Random() {        // Avoid repeated this same led in sequence
  while (ledBefore == led) {
    if (led == 4) {
      led = 0;
    }
    else {
      led++;
    }
  }
}

void setup() {
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print( "  - - WELCOME - - ");
  lcd.setCursor(0, 1);
  lcd.print( "  REFLEX GAME 1.1   ");
  randomSeed(analogRead(A0));
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), point_count, RISING);
}

void start_routine() {            // Animation effect at start
  int timeDelay = 100;
  for (i = 3; i > 0; i--)
  {
    for (int j = 3; j < 8; j++) {
      digitalWrite(j, HIGH);
      delay(timeDelay);
    }
    for (int j = 3; j < 8; j++) {
      digitalWrite(j, LOW);
      delay(timeDelay);
    }
    lcd.setCursor(0, 1);
    lcd.print("- - - - - - - - - -");
    lcd.setCursor(9, 1);
    lcd.print(i);
    delay(4);
    for (int j = 3; j < 8; j++) {
      digitalWrite(j, LOW);
    }
    delay(300);
  }
  for (int j = 3; j < 8; j++) {
    digitalWrite(j, HIGH);
  }
  delay(300);
  for (int j = 3; j < 8; j++) {
    digitalWrite(j, LOW);
  }
  delay(300);
}

void loop() {  
  pointDecision = false;
  int timeFastReaction = 10000, timeSlowReaction = 0;
  String lineString;
  lcd.begin();
  point = 0;
  start_routine();
  lcd.setCursor(0, 1);
  lcd.print( "                    ");
  timeGameStart = millis();        
  for (i = 0; i < countRound; i++) {     //Random loop
    led = random(5);
    avoid_Random();
    pointAllow = point;
    timePointStart = millis();           //Start mesure reaction time
    digitalWrite(led + 3, HIGH);
    lcd.setCursor(0, 2);
    if (countRound - point < 10)
      lineString = "      ->  " + String(countRound - point) + " <-       ";
    else
      lineString = "      -> " + String(countRound - point) + " <-       ";

    lcd.print(lineString);
      lcd.setCursor(0,0);
      lcd.print("         Best:" + String (timeRecord/1000) + "s");
    while (pointDecision == false) {   // Wait until correct button will be push
      lcd.setCursor(0,0);
      lcd.print("Time:" + String ((millis()-timeGameStart)/1000) + "s");
    }
    digitalWrite(led + 3, LOW);
    pointDecision = false;
    timePointEnd = millis();
    point = point + 1;
    delay(50);
    // Conditions to reaction mesure
    if ((timePointEnd - timePointStart) > timeSlowReaction) { //Slowest reaction
      timeSlowReaction = timePointEnd - timePointStart;
    }
    if ((timePointEnd - timePointStart) < timeFastReaction) { // Fastest reaction
      timeFastReaction = timePointEnd - timePointStart;
    }
    ledBefore = led;      //save to memory last led to avoid repeated the same led at sequence
  }
  timeGameEnd = millis();
  timeGameAll = timeGameEnd - timeGameStart;
  timeAverage = timeGameAll / countRound;

  int k ;
  // Show the results on LCD
  int timeDelayShowResults = 1000;
  lcd.begin();
  delay(timeDelayShowResults);
  lcd.setCursor(0, 0);
  lineString = "You:" + String(timeGameAll / 1000) + " Best:" + String(timeRecord / 1000);
  lcd.print(lineString);
  delay(timeDelayShowResults);
  lcd.setCursor(0, 1);
  if (timeAverage < 1000) {
    lineString = "HI : " + String(timeFastReaction) + " ms";
  }
  else {
    lineString = "HI: " + String(timeFastReaction) + " ms";
  }
  lcd.print(lineString);
  delay(timeDelayShowResults);
  float timeRecordOld = timeRecord;
  if (timeRecord > timeGameAll)
  {
    timeRecord = timeGameAll;
  }
  lcd.setCursor(0, 2);
  if (timeAverage < 1000) {
    lineString = "LOW: " + String(timeSlowReaction) + " ms";
  }
  else {
    lineString = "LOW: " + String(timeSlowReaction) + " ms";
  }
  lcd.print(lineString);

  delay(timeDelayShowResults);
  while (pointDecision == false) {   // Wainting to next game (please press button
    digitalWrite(5, HIGH);
    lcd.setCursor(0, 3);
    lcd.print("PRESS   THE   BUTTON");
    if (timeRecord < timeRecordOld){
          lcd.setCursor(17, 1);
          lcd.print("NEW");
          lcd.setCursor(14, 2);
          lcd.print("RECORD");
    }
    
    delay(100);
    lcd.setCursor(0, 3);
    lcd.print("                    ");
        if (timeRecord < timeRecordOld){
          lcd.setCursor(17, 1);
          lcd.print("   ");
          lcd.setCursor(14, 2);
          lcd.print("      ");
    }
    digitalWrite(5, LOW);
    delay(100);
  }

}
