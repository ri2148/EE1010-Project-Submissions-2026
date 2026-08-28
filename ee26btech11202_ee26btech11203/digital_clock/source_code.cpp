// Connected to IC7447 BCD Input Pins
const int PIN_A = 2;
const int PIN_B = 3;
const int PIN_C = 4;
const int PIN_D = 5;

//Connected to Displays 
const int Display[]= { 6,7,8,9,10,11 };

//Buttons
const int NEXT_BTN = A1;
const int DEC_BTN = A2 ;
const int INC_BTN = A3 ;
const int PAUSE_BTN =A4;

//Some Required variables
bool paused = false;

int selectedDigit = 0 ;

unsigned long lastBlink = 0 ;
bool blinkOn = true;

//Some Button STate variables
int lastPauseState = LOW;
int lastNextState = LOW;
int lastIncState = LOW ;
int lastDecState = LOW ;

//Display Binary Variables
int W1=0, X1=0, Y1=0, Z1=0;
int W2=0, X2=0, Y2=0 ;
int W3=0, X3=0, Y3=0, Z3=0 ;
int W4=0, X4=0, Y4=0;
int W5=0, X5=0, Y5=0, Z5=0;
int W6=0, X6=0, Y6=0;
//Time tracking variable
unsigned long PreviousMilliSec = 0;

//Display Function.
void DisplayDigit(int A, int B, int C, int D, int Displaypin){
  for (int i = 0 ; i < 6; i++) {
    digitalWrite(Display[i], LOW);
   }
  digitalWrite(PIN_A,A);
  digitalWrite(PIN_B , B);
  digitalWrite(PIN_C, C);
  digitalWrite(PIN_D , D);
  digitalWrite( Displaypin, HIGH );
}

//Increment function
void incrementDigit(int d){
  int A, B, C, D;
  switch(d){
    case 0:// Sec Ones(0->1...->9->0)
      A = !W1;
      B = (W1 && !X1 && !Z1) || (!W1 && X1);
      C = (!X1 && Y1) || (!W1 && Y1) || (W1 && X1 && !Y1);
      D = (!W1 && Z1) || (W1 && X1 && Y1);
      W1 = A; X1 = B; Y1 = C; Z1 = D;
      break;

    case 1:{ // Sec Tens(0->1...->5->0)
      A = !W2;
      B = (W2 && !X2 && !Y2) || (!W2 && X2);
      C = (W2 && X2) || (!W2 && !X2 && Y2);
      W2 = A; X2 = B; Y2 = C;
    } break;

    case 2:// Min Ones(0->1...->9->0)
      A = !W3;
      B = (W3 && !X3 && !Z3) || (!W3 && X3);
      C = (!X3 && Y3) || (!W3 && Y3) || (W3 && X3 && !Y3);
      D = (!W3 && Z3) || (W3 && X3 && Y3);
      W3 = A; X3 = B; Y3 = C; Z3 = D;
      break;

    case 3:// Min Tens(0->1...->5->0)
      A = !W4;
      B = (W4 && !X4 && !Y4) || (!W4 && X4);
      C = (W4 && X4) || (!W4 && !X4 && Y4);
      W4 = A; X4 = B; Y4 = C;
      break;

    case 4:// Hour Ones
      if (X6 == 1) {//When Hour Tens =2.
        if (Y5 || Z5){// IF Hour TEns >3 -> Reset to 0.
          W5 = 0; X5 = 0; Y5 = 0; Z5 = 0;
        } else{// 2->3,3->0.
          A = !W5;
          B = (W5 && !X5) || (!W5 && X5);
          W5 = A; X5 = B; Y5 = 0; Z5 = 0;
        }
       } else {//when Hour tens <2. => 1->2...->9->0->1.
        A = !W5;
        B = (W5 && !X5 && !Z5) || (!W5 && X5);
        C = (!X5 && Y5) || (!W5 && Y5) || (W5 && X5 && !Y5);
        D = (!W5 && Z5) || (W5 && X5 && Y5);
        W5 = A; X5 = B; Y5 = C; Z5 = D;
       }
       break;

    case 5:// Hour TEns
      if (X6 == 0 && W6 == 0){//0->1.
        W6 = 1; X6 = 0; Y6 = 0;
      } else if (X6 == 0 && W6 == 1) {//1->2
        W6 = 0; X6 = 1; Y6 = 0;
        if(Y5 || Z5) {//When Hour Tens=2,if Hour Ones>3 reset to 3.
          W5 = 1; X5 = 1; Y5 = 0; Z5 = 0;
        }
      } else {//If any other value than 0,1,2 changes to 0.
        W6 = 0; X6 = 0; Y6 = 0;
      }
      break;
  }
}

//Decrement function
void decrementDigit(int d) {
  int A, B, C, D;
  switch (d) {
    case 0://Sec Ones(9->8...->0->9)
      A = !W1;
      B = (!X1 && !W1 && ((!Z1 && Y1) || (Z1 && !Y1))) || (!Z1 && W1 && X1);
      C = (!Z1 && Y1 && (X1 || W1)) || (Z1 && !X1 && !W1 && !Y1);
      D = !X1 && !Y1 && ((Z1 && W1) || (!Z1 && !W1));
      W1 = A; X1 = B; Y1 = C; Z1 = D;
      break;

    case 1://Sec Tens (5->4...->0->5)
      A = !W2;
      B = (Y2 && !X2 && !W2) || (!Y2 && X2 && W2);
      C = !X2 && ((Y2 && W2) || (!Y2 && !W2));
      W2 = A; X2 = B; Y2 = C;
      break;

    case 2://MIn Ones(9->8...->0->9)
      A = !W3;
      B = (!X3 && !W3 && ((!Z3 && Y3) || (Z3 && !Y3))) || (!Z3 && W3 && X3);
      C = (!Z3 && Y3 && (X3 || W3)) || (Z3 && !X3 && !W3 && !Y3);
      D = !X3 && !Y3 && ((Z3 && W3) || (!Z3 && !W3));
      W3 = A; X3 = B; Y3 = C; Z3 = D;
      break;

    case 3://Min Tens (5->4...->0->5)
      A = !W4;
      B = (Y4 && !X4 && !W4) || (!Y4 && X4 && W4);
      C = !X4 && ((Y4 && W4) || (!Y4 && !W4));
      W4 = A; X4 = B; Y4 = C;
      break;

    case 4://Hour Ones
      if (X6 == 1) {//when Hour tens =2.
        if (Y5 || Z5 || (W5 == 0 && X5 == 0)) {//0->3.
          W5 = 1; X5 = 1; Y5 = 0; Z5 = 0;
        } else {//3->2->1->0.
          A = !W5;
          B = (X5 && W5) || (!X5 && !W5);
          W5 = A; X5 = B; Y5 = 0; Z5 = 0;
        }
      } else {//if Hour Tens <=1. Hour Ones(9->8...->0->9)
        A = !W5;
        B = (!X5 && !W5 && ((!Z5 && Y5) || (Z5 && !Y5))) || (!Z5 && W5 && X5);
        C = (!Z5 && Y5 && (X5 || W5)) || (Z5 && !X5 && !W5 && !Y5);
        D = !X5 && !Y5 && ((Z5 && W5) || (!Z5 && !W5));
        W5 = A; X5 = B; Y5 = C; Z5 = D;
      }
      break;

    case 5://Hour Tens
      if (X6 == 0 && W6 == 0) {//0->2.
        W6 = 0; X6 = 1; Y6 = 0;
        if (Y5 || Z5) {//If Hour Tens =2,when Hour Ones > 3 reset to 3.
          W5 = 1; X5 = 1; Y5 = 0; Z5 = 0;
        }
      } else if (X6 == 0 && W6 == 1) {//1->0
        W6 = 0; X6 = 0; Y6 = 0;
      } else {//2->1
        W6 = 1; X6 = 0; Y6 = 0;
      }
      break;
  }
}

//Setup Function (Runs Only one after connecting to Power or AFter Reset)
void setup() {
  pinMode(PIN_A, OUTPUT); pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT); pinMode(PIN_D, OUTPUT);

  for(int i = 0; i < 6; i++) {
    pinMode(Display[i], OUTPUT);
    digitalWrite(Display[i], LOW);
  }

  pinMode(PAUSE_BTN, INPUT);
  pinMode(NEXT_BTN, INPUT);
  pinMode(INC_BTN, INPUT);
  pinMode(DEC_BTN, INPUT);
}

//Loop Function(Executes continously after connecting to Power.)
void loop() {

//Pause Button Logic
  int pauseState = digitalRead(PAUSE_BTN);
  if (pauseState == HIGH && lastPauseState == LOW){ 
    paused = !paused;
    if(paused) {
     selectedDigit = 0;
    }
     delay(50);//Debouncing Delay
  }
  lastPauseState = pauseState;

//Next Button Logic
  int nextState = digitalRead(NEXT_BTN);
  if (paused && nextState == HIGH && lastNextState == LOW) {
    selectedDigit = (selectedDigit + 1) %6;
    delay(50);//Debouncing Delay
  }
  lastNextState= nextState;

//INcrement buttton Logic.
  int incState = digitalRead(INC_BTN);
  if(paused && incState == HIGH && lastIncState == LOW) {
    incrementDigit(selectedDigit);
    delay(50);//Debouncing Delay
  }
  lastIncState = incState;

//Decreemnt Button Logic
  int decState = digitalRead(DEC_BTN);
  if (paused&& decState == HIGH && lastDecState == LOW){
    decrementDigit(selectedDigit);
    delay(50);//Debouncing Delay
  }
  lastDecState = decState;

//Blink Setting(Current has Timeperiod= 1Sec)(Only for selectedDigit)
  if (paused && (millis() - lastBlink >= 500)) {
    blinkOn= !blinkOn ;
    lastBlink = millis();
  }

//Multiplexing Display Cycle(Contains Blink Logic for selectedDigit.)
  for (int d = 0; d < 6; d++) {
      bool ShowTime = true;
    if (paused && d ==selectedDigit && !blinkOn) {
      ShowTime = false;
    }

    if (ShowTime) {
      switch (d) {
        case 0: DisplayDigit(W1, X1, Y1,Z1, Display[0]); break;
        case 1: DisplayDigit(W2, X2, Y2, 0, Display[1]); break;
        case 2: DisplayDigit(W3, X3, Y3,Z3, Display[2]); break;
        case 3: DisplayDigit(W4, X4, Y4, 0, Display[3]); break;
        case 4: DisplayDigit(W5,X5, Y5, Z5, Display[4]); break;
        case 5: DisplayDigit(W6, X6, Y6, 0, Display[5]); break;
      }
      delay(1);//To give LED time to be recognized as ON by Human Eye(Else visible as dimly Lit)
      digitalWrite(Display[d], LOW);//Disable Display to avoid Ghosting.
    }
  }
//CLock Time Engine
  if (!paused && (millis()- PreviousMilliSec >=1000)) {
    PreviousMilliSec = millis();

    incrementDigit(0);//Increase Seconds

    if ((W1 | X1 | Y1 | Z1) == 0) {//Carry Over to Seconds Ten 
      incrementDigit(1);

      if ((W2 | X2 | Y2) == 0) {// Carry over to Minutes Ones
        incrementDigit(2);

        if ((W3 | X3 | Y3 | Z3) == 0) {// Carry over to Minutes Tens
          incrementDigit(3);

          if ((W4 | X4 | Y4) == 0) {
            if (X6 == 1) {//When Hour Tens=2
              if (W5 == 1 && X5 == 1) {//If Hour Ones =3,Change 23:59:59 -> 00:00:00
                W5 = 0; X5 = 0; Y5 = 0; Z5 = 0;
                W6 = 0; X6 = 0; Y6 = 0;
              } else {//If Hour Ones is = 1or0.
                incrementDigit(4);
              }
            } else {//Hour Tens is 0 or 1.
                incrementDigit(4);
                if ((W5 | X5 | Y5 | Z5) == 0) {//Carry to Hour Tens
                incrementDigit(5);
                }
             }
           }
        }
      }
    }
  }
}
