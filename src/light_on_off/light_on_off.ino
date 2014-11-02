int incomingByte = 0;   // for incoming serial data
int led1 = 4;
int led2 = 5;
boolean state1 = false;
boolean state2 = false;

void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        pinMode(led1, OUTPUT);
        pinMode(led2, OUTPUT);
      }

void loop() {

        // send data only when you receive data:
        if (Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();
                //Serial.println(incomingByte);
                if (incomingByte == '1') {
                      Serial.println("foo");
                  
                      if (state1 == false) {
                        state1 = true;
                      } else {
                        state1 = false; 
                      }
                      digitalWrite(led1, state1);
                      
                } else if (incomingByte == '2') {
                
                     Serial.println("bar");
                
                    if (state2 == false){
                      state2 = true;
                    } else {
                      state2 = false;
                    }   
               
                    digitalWrite(led2, state2); 
                }
        }
        
}
 
