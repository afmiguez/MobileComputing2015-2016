#include "projecto_ir.h"
#include <IRremote.h>
IRsend irsend;
int act1 = LOW;
int prev1 = LOW;
int act2 = LOW;
int prev2 = LOW;
int thres1 = 100;
int thres2 = 100;
stateIds actStateId = BEGIN, prevStateId = BEGIN;
int countPeople = 0;
int stateLed = LOW;
int flowState = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(SENSOR1, INPUT);
  pinMode(SENSOR2, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(9600);
}

void printStates() {
  Serial.print("prev1=");
  Serial.print(prev1);
  Serial.print("act1=");
  Serial.print(act1);
  Serial.print("prev2=");
  Serial.print(prev2);
  Serial.print("act2=");
  Serial.print(act2);
  Serial.println();
}

void printStateName(stateIds state) {
  if (state == BEGIN) {
    Serial.print("BEGIN ");
    return;
  }
  if (state == IN1) {
    Serial.print("IN1 ");
    return;
  }
  if (state == IN2) {
    Serial.print("IN2 ");
    return;
  }
  if (state == IN1IN2) {
    Serial.print("IN1+IN2 ");
    return;
  }
  if (state == IN2IN1) {
    Serial.print("IN2+IN1 ");
    return;
  }
  if (state == OUT1IN2) {
    Serial.print("OUT1+IN2 ");
    return;
  }
  if (state == OUT2IN1) {
    Serial.print("OUT2+IN1 ");
    return;
  }
  if (state == OUT1) {
    Serial.print("OUT1 ");
    return;
  }
  if (state == OUT2) {
    Serial.print("OUT2 ");
    return;
  }
}

void printValues() {
  Serial.print("value1= ");
  Serial.print(analogRead(SENSOR1));
  Serial.print("value2= ");
  Serial.print(analogRead(SENSOR2));
  Serial.println("");
}

void updateState() {
  int value1 = analogRead(SENSOR1);
  int value2 = analogRead(SENSOR2);
  prev1 = act1;
  prev2 = act2;
  if (value1 > thres1) {
    act1 = HIGH;
    digitalWrite(LED1, HIGH);
  } else {
    act1 = LOW;
    digitalWrite(LED1, LOW);
  }
  if (value2 > thres2) {
    act2 = HIGH;
    digitalWrite(LED2, HIGH);
  } else {
    act2 = LOW;
    digitalWrite(LED2, LOW);
  }
}

stateIds getActualState() {
  //retorna o valor do id do estado atual
  if (!prev1 && act1 && !prev2 && !act2 && actStateId == BEGIN) {
    return IN1;
  } if (prev1 && act1 && !prev2 && act2 && actStateId == IN1) {
    return IN1IN2;
  } if (prev1 && !act1 && prev2 && act2 && actStateId == IN1IN2) {
    return OUT1IN2;
  } if (!prev1 && !act1 && prev2 && !act2 && actStateId == OUT1IN2) {
    return OUT2;
  } if (!prev1 && !act1 && !prev2 && act2 && actStateId == BEGIN) {
    return IN2;
  } if (!prev1 && act1 && prev2 && act2 && actStateId == IN2) {
    return IN2IN1;
  } if (prev1 && act1 && prev2 && !act2 && actStateId == IN2IN1) {
    return OUT2IN1;
  } if (prev1 && !act1 && !prev2 && !act2 && actStateId == OUT2IN1) {
    return OUT1;
  } if (!prev1 && !act1 && !prev2 && !act2 ) {
    return BEGIN;
  } else {
    return NAS;
  }
}

void triggerStateEvents() {

  //checks if happened either an entry or an exit
  if ((prevStateId == OUT1IN2 && actStateId == OUT2) || (prevStateId == OUT2IN1 && actStateId == OUT1)) {
    //there was an entry
    if (prevStateId == OUT1IN2 && actStateId == OUT2) {
      countPeople++;
      prevStateId = BEGIN;
      actStateId = BEGIN;
    }
    //there was an exit
    else {
      if (countPeople > 0) {
        countPeople--;
      }
      prevStateId = BEGIN;
      actStateId = BEGIN;
    }
    flowState = HIGH;
    //prints the actual number of people
    if (countPeople == 1) {
      Serial.print("There is ");
      Serial.print(countPeople);
      Serial.println(" person");
    } else {
      Serial.print("There are ");
      Serial.print(countPeople);
      Serial.println(" people");
    }
  }
}
//turns off the led tape
void turnOff() {
  irsend.sendNEC(BUTTON_OFF, 32);
}
//turns on the led tape
void turnOn() {
  irsend.sendNEC(BUTTON_ON, 32);
}
void changeWhite() {
  irsend.sendNEC(BUTTON_WHITE, 32);
}
void changeBlue() {
  irsend.sendNEC(BUTTON_BLUE, 32);
}
void changeGreen() {
  irsend.sendNEC(BUTTON_GREEN, 32);
}


void loop() {
  //get new sensor values
  updateState();
  //update the machine state
  prevStateId = actStateId;
  actStateId = getActualState();

  //avoid weird states (Not A State)
  if (actStateId == NAS) {
    actStateId = prevStateId;
  }
  //checks entry or exit
  triggerStateEvents();

  //if there's at least one person and the led tape was off
  if (countPeople > 0) {
    if (!stateLed) {
      turnOn();
      stateLed = HIGH;
      delay(100);
    }
    //checks if there was a change in the room
    if (flowState) {
      if (countPeople % 3 == 0) {
        changeBlue();
      } else if (countPeople % 3 == 1) {
        changeWhite();
      } if (countPeople % 3 == 2) {
        changeGreen();
      }
      flowState = LOW;
    }
  }
  //if there's none in the room
  else if (countPeople <= 0) {
    if (stateLed) {
      turnOff();
      stateLed = LOW;
    }
  }
}
