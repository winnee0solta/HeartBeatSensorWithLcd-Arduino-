//Import necessary library
#include <Wire.h>
#include <LiquidCrystal.h>
#define pulsePin A0

//LCD pins initial setup
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//variables for Beat Sensor
int rate[10];
unsigned long sampleCounter = 0;
unsigned long lastBeatTime = 0;
unsigned long lastTime = 0, N;
int BPM = 0;
int IBI = 0;
int P = 512;
int T = 512;
int thresh = 512;
int amp = 100;
int Signal;
boolean Pulse = false;
boolean firstBeat = true;
boolean secondBeat = true;
boolean QS = false;



void setup(void)
{
  Serial.begin(9600);
  Serial.println("Started" );
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
}

void loop(void)
{
  if (QS == true) {
    Serial.println("BPM: " + String(BPM));
    QS = false;
  } else if (millis() >= (lastTime + 2)) {
    readPulse();
    lastTime = millis();
  }
}


//Read beat pulse from sensor
void readPulse() {

  Signal = analogRead(pulsePin);
  sampleCounter += 2;
  int N = sampleCounter - lastBeatTime;

  detectSetHighLow();

  if (N > 250) {
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3) )
      pulseDetected();
  }

  if (Signal < thresh && Pulse == true) {
    Pulse = false;
    amp = P - T;
    thresh = amp / 2 + T;
    P = thresh;
    T = thresh;
  }

  if (N > 2500) {
    thresh = 512;
    P = 512;
    T = 512;
    lastBeatTime = sampleCounter;
    firstBeat = true;
    secondBeat = true;
  }

}

void detectSetHighLow() {
  if (Signal < thresh && N > (IBI / 5) * 3) {
    if (Signal < T) {
      T = Signal;
    }
  }
  if (Signal > thresh && Signal > P) {
    P = Signal;
  }
}

//Fires when Pulse is detected
void pulseDetected() {
  Pulse = true;
  IBI = sampleCounter - lastBeatTime;
  lastBeatTime = sampleCounter;

  if (firstBeat) {
    firstBeat = false;
    return;
  }
  if (secondBeat) {
    secondBeat = false;
    for (int i = 0; i <= 9; i++) {
      rate[i] = IBI;
    }
  }

  word runningTotal = 0;

  for (int i = 0; i <= 8; i++) {
    rate[i] = rate[i + 1];
    runningTotal += rate[i];
  }

  rate[9] = IBI;
  runningTotal += rate[9];
  runningTotal /= 10;
  BPM = 60000 / runningTotal;
  QS = true;

  //print BPM to console
  Serial.println(BPM );


  //Print BPM to LCD
  lcd.setCursor(0, 0);
  lcd.print(" Heart Monitor ");
  lcd.setCursor(0, 1);
  lcd.print("          ");
  lcd.setCursor(0, 1);
  lcd.print(BPM);

}
