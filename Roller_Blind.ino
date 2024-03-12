// DRV8255 PINS
const int dirPin1 = A0;
const int stepPin1 = A1;
const int dirPin2 = A2;
const int stepPin2 = A3;
const int dirPin3 = A4;
const int stepPin3 = A5;
const int dirPin4 = 6;
const int stepPin4 = 7;

// Endstop Pins
const int endStop1 = 8;
const int endStop2 = 9;
const int endStop3 = 10;
const int endStop4 = 11;

const int enablePin = 12;

// Booleans for rollUP()
bool endStop1Done = false;
bool endStop2Done = false;
bool endStop3Done = false;
bool endStop4Done = false;

// 433hz Remote pins
const int remotePinA = 4;
const int remotePinB = 2;
const int remotePinC = 5;
const int remotePinD = 3;
const int blindLengthInSteps = 2600;

const int desiredPositionMode2 = blindLengthInSteps * 0.5;
const int desiredPositionMode3 = blindLengthInSteps * 0.75;

bool setDirection = false;
//  Blind Modes
int mode = 0;
int currentPosition = 0;
int memorie = 0;

void setup()
{
  // Motor Disabled
  digitalWrite(enablePin, HIGH);
	// Declare pins as Outputs
	pinMode(dirPin1, OUTPUT);
	pinMode(stepPin1, OUTPUT);
	pinMode(dirPin2, OUTPUT);
  pinMode(stepPin2, OUTPUT);
	pinMode(dirPin3, OUTPUT);
  pinMode(stepPin3, OUTPUT);
	pinMode(dirPin4, OUTPUT);
  pinMode(stepPin4, OUTPUT);
  
  // Enable PCIE2 (Port D) for CHANGE Interrupts for Remote
  PCICR |= B00000100;
  // Enable PCINT18 PCINT19 PCINT20 PCINT21(Pins D2, D3, D4, D5)
  PCMSK2 |= B10111100;

  Serial.begin(9600);

  currentPosition = 0;
  setDirection = false;
  //rollUP();

}
void loop()
{
  Serial.print(currentPosition);

  if(mode == 1){                                  // A on Remote  (Blind Fully Down)
    Serial.println(" Mode1");  
    rollDown(blindLengthInSteps);
  }
  if(mode == 2){                                      // B on Remote  (Blind 50% Down)
    Serial.println(" Mode2");                            
    rollDown(desiredPositionMode2);
  }
  if(mode == 3){                                      // C on Remote  (Blind 75% Down)
    Serial.println(" Mode3");                               
    rollDown(desiredPositionMode3);
  }
  if(mode == 4){                                      // D on Remote  (Blind Fully Up)
    Serial.println(" Mode4");                                       
    rollUP();
  }
}

void rollDown(int setPosition){
  if(!setDirection){
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, LOW);
    digitalWrite(dirPin3, HIGH);
    digitalWrite(dirPin4, HIGH);
    digitalWrite(enablePin, LOW);
    setDirection = true;
  }
  
	if(currentPosition < setPosition){
    currentPosition++;
    // slow = 2000, fast = 500
		digitalWrite(stepPin1, HIGH);
		delayMicroseconds(500); 
		digitalWrite(stepPin1, LOW);
		delayMicroseconds(500);

    digitalWrite(stepPin2, HIGH);
		delayMicroseconds(500); 
		digitalWrite(stepPin2, LOW);
		delayMicroseconds(500);

    digitalWrite(stepPin3, HIGH);
		delayMicroseconds(500); 
		digitalWrite(stepPin3, LOW);
		delayMicroseconds(500);

    digitalWrite(stepPin4, HIGH);
		delayMicroseconds(500); 
		digitalWrite(stepPin4, LOW);
		delayMicroseconds(500);
	}else{
    mode = 0;
    setDirection = false;
    digitalWrite(enablePin, HIGH);
  }
}
void rollUP(){
  if(!setDirection){
	digitalWrite(dirPin1, HIGH);
	digitalWrite(dirPin2, HIGH);
	digitalWrite(dirPin3, LOW);
	digitalWrite(dirPin4, LOW);
	digitalWrite(enablePin, LOW);
  setDirection = true;
  }
	if(!endStop1Done || !endStop2Done || !endStop3Done || !endStop4Done){
    currentPosition--;
    if (currentPosition < 0){
      currentPosition = 0;
    }
    
    if(!endStop1Done){
      if(digitalRead(endStop1) == LOW){
        digitalWrite(stepPin1, HIGH);
        delayMicroseconds(500); 
        digitalWrite(stepPin1, LOW);
        delayMicroseconds(500);
      }else{
        endStop1Done = true;
      }
    }else{

    }
    if(!endStop2Done){
      if(digitalRead(endStop2) == LOW){
        digitalWrite(stepPin2, HIGH);
        delayMicroseconds(500); 
        digitalWrite(stepPin2, LOW);
        delayMicroseconds(500);
      }else{
        endStop2Done = true;
      }
    }
    if(!endStop3Done){
      if(digitalRead(endStop3) == LOW){
        digitalWrite(stepPin3, HIGH);
        delayMicroseconds(500); 
        digitalWrite(stepPin3, LOW);
        delayMicroseconds(500);
      }else{
        endStop3Done = true;
      }
    }
    if(!endStop4Done){
      if(digitalRead(endStop4) == LOW){
        digitalWrite(stepPin4, HIGH);
        delayMicroseconds(500); 
        digitalWrite(stepPin4, LOW);
        delayMicroseconds(500);
      }else{
        endStop4Done = true;
      }
    }
  }
  if(endStop1Done && endStop2Done && endStop3Done && endStop4Done){
    digitalWrite(enablePin, HIGH);  
    endStop1Done = false;
    endStop2Done = false;
    endStop3Done = false;
    endStop4Done = false;
    currentPosition = 0;
    mode = 0;
    setDirection = false;
  }
}

ISR (PCINT2_vect)
{
  //Pin triggered the ISR on a Falling pulse
  if (digitalRead(remotePinA) == HIGH) {
    memorie = 1;
  }
  else if (digitalRead(remotePinB) == HIGH) {
    memorie = 2;
  }
  else if (digitalRead(remotePinC) == HIGH) {
    memorie = 3;
  }
  else if (digitalRead(remotePinD) == HIGH) {
    memorie = 4;
  }

  if (digitalRead(remotePinA) == LOW && memorie == 1) {
    memorie = 0;
    setDirection = false;
    if(currentPosition >= blindLengthInSteps){
      mode = 0;
      Serial.print(" A apasat insa mod: ");
      Serial.println(mode);
    }else if(mode == 1){
      mode = 0;
      digitalWrite(enablePin, HIGH);
      Serial.println(" A disabled");
    }else{
      mode = 1;
      Serial.println(" A");
    }
  }else if(digitalRead(remotePinB) == LOW && memorie == 2){
    memorie = 0;
    setDirection = false;
    if(currentPosition >= desiredPositionMode2){
      mode = 0;
      digitalWrite(enablePin, HIGH);
      Serial.print(" B apasat insa mod: ");
      Serial.println(mode);
    }else if(mode == 2){
      mode = 0;
      digitalWrite(enablePin, HIGH);
      Serial.println(" B disabled");
    }else{
      mode = 2;
      Serial.println(" B");
    }
  }else if(digitalRead(remotePinC) == LOW && memorie == 3){
    memorie = 0;
    setDirection = false;
    if(currentPosition >= desiredPositionMode3){
      mode = 0;
      digitalWrite(enablePin, HIGH);
      Serial.print(" C apasat insa mod: ");
      Serial.println(mode);
    }else if(mode == 3){
      mode = 0;
      digitalWrite(enablePin, HIGH);
      Serial.println(" C disabled");
    }else{
      mode = 3;
      Serial.println(" C");
    }
  }else if(digitalRead(remotePinD) == LOW && memorie == 4){
    memorie = 0;
    setDirection = false;
    if(currentPosition <= 0){
      mode = 0;
      digitalWrite(enablePin, HIGH);
      Serial.print(" D apasat insa mod: ");
      Serial.println(mode);
    }else if(mode == 4){
      mode = 0;
      digitalWrite(enablePin, HIGH);
      Serial.println(" D disabled");
    }else{
      mode = 4;
      Serial.println(" D");
    }
  }
}