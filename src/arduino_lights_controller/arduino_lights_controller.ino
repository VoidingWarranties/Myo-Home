int incomingByte = 0;  // for incoming serial data
int relay1 = 4;
int relay2 = 5;
boolean state1 = false;
boolean state2 = false;

void setup() {
  Serial.begin(9600);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if (incomingByte == '1') {
      Serial.println("Toggling light 1");
      if (state1 == false) {
        state1 = true;
      } else {
        state1 = false;
      }
      digitalWrite(led1, state1);
    } else if (incomingByte == '2') {
      Serial.println("Toggling light 2");
      if (state2 == false){
        state2 = true;
      } else {
        state2 = false;
      }
      digitalWrite(led2, state2);
    }
  }
}
 
