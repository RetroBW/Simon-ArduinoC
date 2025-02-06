#include "pitches.h"

// I/O button assignments
const int inRedBtn = D0;
const int inGreenBtn = D1;
const int inBlueBtn = D2;
const int inYellowBtn = D3;
const int inNewGameBtn = D4;

// I/O indicator assignments
const int outRedLED = D10;
const int outGreenLED = D9;
const int outBlueLED = D8;
const int outYellowLED = D7;

const int outSpeaker = D6;

//game variables
String strDisplay = "New Game?";
int iColor[100];
int iColorIndex;
int iMaxColorIndex;
int iColorSelected;
int iColorOnTMR_PRE;
int iColorOffTMR_PRE;

//state logic variables
int NewState;
int State;

void setup() {

  // initialize input mapping
  pinMode(inYellowBtn, INPUT_PULLDOWN);
  pinMode(inGreenBtn, INPUT_PULLDOWN);
  pinMode(inRedBtn, INPUT_PULLDOWN);
  pinMode(inBlueBtn, INPUT_PULLDOWN);
  pinMode(inNewGameBtn, INPUT_PULLDOWN);

  // initialize output mapping
  pinMode(outYellowLED, OUTPUT);
  pinMode(outGreenLED, OUTPUT);
  pinMode(outRedLED, OUTPUT);
  pinMode(outBlueLED, OUTPUT);
  pinMode(outSpeaker, OUTPUT);

  //initialize state variables
  NewState = 1;
  State = 0;

  //init serial monitor
  //Serial.begin(9600);
  //while (!Serial);
  //Serial.print("Serial port initialized \n");
}

void loop() {
  // Serial.println(micros());
  // new state logic
  switch (NewState) {
    case 1:
      SetLEDState(D7, HIGH);
      SetLEDState(D8, HIGH);
      SetLEDState(D9, HIGH);
      SetLEDState(D10, HIGH);
      delay(200);
      SetLEDState(D7, LOW);
      SetLEDState(D8, LOW);
      SetLEDState(D9, LOW);
      SetLEDState(D10, LOW);
      break;
  }
  // state logic
  switch (State) {
    case 1:
      { // state 1: lost
        if (digitalRead(inNewGameBtn) == HIGH)
        {
          //transition from state 1 to state 2
          iMaxColorIndex = 1;
          iColorOnTMR_PRE = 1000;
          iColorOffTMR_PRE = 500;
          strDisplay = "Displaying Color Sequence";
          iColorIndex = 1;
          switch(rand_num()) {
            case 1:
              iColor[iColorIndex] = D10;
              break;
            case 2:
              iColor[iColorIndex] = D9;
              break;
            case 3:
              iColor[iColorIndex] = D8;
              break;
            case 4:
              iColor[iColorIndex] = D7;
              break;
          }
          SetLEDState(iColor[iColorIndex], HIGH);
          NewState = 2;
          // Serial.print("New State 2, ColorOUT: ");
          // Serial.print(iColor[iColorIndex]);
          // Serial.print("\n");
        }
        else {
          SetLEDState(1, digitalRead(inRedBtn));
          SetLEDState(2, digitalRead(inGreenBtn));
          SetLEDState(3, digitalRead(inBlueBtn));
          SetLEDState(4, digitalRead(inYellowBtn));
        }
        break;
      }
    case 2: { // state 2: display next color
        //transition from state 2 to state 3
        delay(iColorOnTMR_PRE);
        SetLEDState(iColor[iColorIndex], LOW);
        iColorIndex++;
        NewState = 3;
        // Serial.print("New State 3 \n");
        break;
      }
    case 3: { // state 3: check display color index
        if (iColorIndex <= iMaxColorIndex) {
          //transition from state 3 to state 2
          delay(iColorOffTMR_PRE);
          if (iColorIndex == iMaxColorIndex) {
            switch(rand_num()) {
              case 1:
                iColor[iColorIndex] = D10;
                break;
              case 2:
                iColor[iColorIndex] = D9;
                break;
              case 3:
                iColor[iColorIndex] = D8;
                break;
              case 4:
                iColor[iColorIndex] = D7;
                break;
            }
          }
          SetLEDState(iColor[iColorIndex], HIGH);
          // Serial.print("New State 2, ColorOUT: ");
          // Serial.print(iColor[iColorIndex]);
          // Serial.print("\n");
          NewState = 2;
        }
        else {
          //transition from state 3 to state 4
          iColorIndex = 1;
          strDisplay = "Enter Color Sequence";
          iColorSelected = 0;
          NewState = 4;
          // Serial.print("New State 4 \n");
        }
        break;
      }
    case 4: { // state 4: get next color button press
        if (digitalRead(inRedBtn) == HIGH) {
          iColorSelected = 1;
        }
        else if (digitalRead(inGreenBtn) == HIGH) {
          iColorSelected = 2;
        }
        else if (digitalRead(inBlueBtn) == HIGH) {
          iColorSelected = 3;
        }
        else if (digitalRead(inYellowBtn) == HIGH) {
          iColorSelected = 4;
        }
        if (iColorSelected > 0) {
          delay(100);
          //transition from state 4 to state 5
          SetLEDState(iColorSelected, HIGH);
          NewState = 5;
          // Serial.print("New State 5, iColorSelected = ");
          // Serial.print(iColorSelected);
          // Serial.print("\n");
        }
        break;
      }
    case 5: { // state 5: wait for color button release
        if ((digitalRead(inYellowBtn) == LOW) &&
        (digitalRead(inGreenBtn) == LOW) &&
        (digitalRead(inRedBtn) == LOW) &&
        (digitalRead(inBlueBtn) == LOW)) {
          SetLEDState(iColorSelected, LOW);
          delay(100);
          if (iColorSelected == iColor[iColorIndex]) {
            //transition from state 5 to state 6
            // Serial.print("New State 6, ");
            // Serial.print("ColorIN = ");
            // Serial.print(iColor[iColorIndex]);
            // Serial.print("\n");
            iColorIndex++;
            NewState = 6;
          }
          else {
            //transition from state 5 to state 8
            strDisplay = "You Lose";
            NewState = 8;
            // Serial.print("New State 8, ");
            // Serial.print("ColorIN = ");
            // Serial.print(iColor[iColorIndex]);
            // Serial.print("\n");
          }
        }
        break;
      }
    case 6: { // state 6: check button color index
        if (iColorIndex <= iMaxColorIndex) {
          iColorSelected = 0;
          NewState = 4;
          // Serial.print("New State 4 \n");
        }
        else {
          //transition from state 6 to state 7
          iMaxColorIndex++;
          iColorIndex = 1;
          iColorOnTMR_PRE *= 0.9;
          iColorOffTMR_PRE *= 0.9;
          NewState = 7;
          // Serial.print("New State 7 \n");
        }
        break;
      }
    case 7: { // state 7: round won
        //transition from state 7 to state 2
        strDisplay = "Displaying Color Sequencce";
        NewState = 2;
        delay(500);
        SetLEDState(iColor[iColorIndex], HIGH);
        // Serial.print("New State 2, ColorOUT: ");
        // Serial.print(iColor[iColorIndex]);
        // Serial.print("\n");
        break;
      }
    case 8: { // state 8: game lost
        //transition from state 8 to state 1
        for(int i=0; i<9; i++){
          SetLEDState(D7, i % 2);
          SetLEDState(D8, i % 2);
          SetLEDState(D9, i % 2);
          SetLEDState(D10, i % 2);
          delay(200);
        }
        NewState = 1;
        // Serial.print("New State 1 \n");
        break;
      }
    default: { // invalid state
        //transition from invalid state to state 1
        strDisplay = "invalid state";
        delay(2000);
        strDisplay = "New Game?";
        NewState = 1;
        // Serial.print("New State 1 \n");
      }
  }

  //state transition logic
  if (NewState != 0) {
    State = NewState;
    NewState = 0;
  }
}

long rand_num() {
  long micro = micros();
  // Serial.print("micros() = ");
  // Serial.print(micro);
  // Serial.print(", random = ");
  // Serial.print((micro % 4) + 1);
  // Serial.print("\n");
  return (micro % 4) + 1;
}

void SetLEDState(int iLEDColor, int iState) {
  //mute speaker when turning off LED
  if(not iState) noTone(outSpeaker);
  switch (iLEDColor) {
    case 1: { // Red
      if(iState) tone(outSpeaker, NOTE_C5, 1000);
      digitalWrite(outRedLED, iState);
      //Serial.print("outRedLED (3) = ");
      //Serial.print(iState);
      //Serial.print("\n");
      break;
    }
    case 2: { // Green
      if(iState) tone(outSpeaker, NOTE_D5, 1000);
      digitalWrite(outGreenLED, iState);
      //Serial.print("outGreenLED (2) = ");
      //Serial.print(iState);
      //Serial.print("\n");
      break;
    }
    case 3: { // Blue
      if(iState) tone(outSpeaker, NOTE_E5, 1000);
       digitalWrite(outBlueLED, iState);
      //Serial.print("outBlueLED (4) = ");
      //Serial.print(iState);
      //Serial.print("\n");
      break;
    }
    case 4: { // Yellow
       if(iState) tone(outSpeaker, NOTE_F5, 1000);
      digitalWrite(outYellowLED, iState);
      //Serial.print("outYellowLED (1) = ");
      //Serial.print(iState);
      //Serial.print("\n");
      break;
    }
  }
}

