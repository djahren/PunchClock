int button = 13;
const int LED[] = {5, 6, 7, 9, 10, 11, 12};
const int numLED = (sizeof(LED)/sizeof(int));
int speaker = A1;
int previousState = LOW;
bool pairing = true;
int noresponsecount = 0;
unsigned long counter = 0;
unsigned long pingevery = 40000;
int pairafterfailed = 10;

void setup() {
  randomSeed(analogRead(0));
 
  pinMode(button, INPUT);
  pinMode(speaker, OUTPUT);
  for(int i = 0; i < numLED; i++){
    pinMode(LED[i], OUTPUT);
  }
  
  Serial.begin(9600);
  Serial.setTimeout(50);
}

void loop() {
  if(!pairing){
    int state = digitalRead(button);
    
    if(state != previousState){
      if(state == HIGH){
        //button pressed send boop
        Serial.println("boop");
        tone(speaker, 440, 20);
        digitalWrite(LED[3], HIGH);
        
        //wait for response
        String response = Serial.readStringUntil('\n');
  
        //if response received excute
        if(response.equals("beep")){   
          Serial.println("yay!");
          tone(speaker, 880, 20);
          ledPattern(1);
        }
        if(response.equals("nope")){   
          Serial.println(":'(");
          tone(speaker, 220, 20);
          ledPattern(2);
        }
      }
      else{
        Serial.println("no boop");
        digitalWrite(LED[3], LOW);
      }
      //delay(100);
      previousState = state;
    }
  }
  
  //pairing logic
  if(counter % pingevery == 0){ // every 100 loops
    Serial.println("it you?");
      
      //wait for response
      String response = Serial.readStringUntil('\n');

      //if response received excute
      if(response.equals("it me!")){
        pairing = false; noresponsecount = 0;
        for(int i = 0; i < numLED; i++){
          digitalWrite(LED[i], LOW);
        }         
      }
      else{
        noresponsecount++;
        if(noresponsecount >= pairafterfailed){
          pairing = true;
        }
      }
      if(pairing){
        Serial.println((counter/(pingevery)));
        for(int i = 0; i < numLED; i++){
          digitalWrite(LED[i], (counter/(pingevery)) % 2);
        }
      }
  }
  counter++;
}

void ledPattern(int pattern){
  int rest = 200;
  int ledsOn = numLED;
  switch (pattern) {
    case 1:
      //success
      digitalWrite(LED[3], HIGH);
      delay(rest);
      digitalWrite(LED[2], HIGH);
      digitalWrite(LED[4], HIGH);
      delay(rest);
      digitalWrite(LED[1], HIGH);
      digitalWrite(LED[5], HIGH);
      delay(rest);
      digitalWrite(LED[0], HIGH);
      digitalWrite(LED[6], HIGH);
      delay(rest);
      for(int i = 0; i < numLED - 1; i++){
          digitalWrite(LED[i], LOW);
          delay(rest/2);
      }
      for(int i = numLED - 1; i > -1; i--){
          digitalWrite(LED[i], HIGH);
          delay(rest/2);
          digitalWrite(LED[i], LOW);
      }
      for(int i = 1; i < 3; i++){
          digitalWrite(LED[i], HIGH);
          delay(rest/2);
          digitalWrite(LED[i], LOW);
      }
      digitalWrite(LED[3], HIGH);
      delay(rest/2);
      digitalWrite(LED[2], HIGH);
      digitalWrite(LED[4], HIGH);
      delay(rest/2);
      digitalWrite(LED[1], HIGH);
      digitalWrite(LED[5], HIGH);
      delay(rest/2);
      digitalWrite(LED[0], HIGH);
      digitalWrite(LED[6], HIGH);
      delay(rest/2);
  
      int ledState[numLED]; //create ledState array and set all values to HIGH
      for(int i = 0; i < numLED; i++){
          ledState[i] = HIGH;
      }
      
      while(ledsOn > 0){
        //pick a random LED and set it to LOW. If there are still LEDs on HIGH - continue
        int ledChoice = random(numLED); //choose random LED
        digitalWrite(LED[ledChoice], LOW); //turn off random LED
        ledState[ledChoice] = LOW; //mark LED as turned off
        
        //count remaining LEDs on
        ledsOn = 0;
        for(int i = 0; i < numLED; i++){
          ledsOn += ledState[i];
        }
  
        //pause before next
        delay(rest);
      }
      break;
      
    case 2:
      // fail
      //flash all LEDs twice
      for(int j = 1; j < 5; j++){
        for(int i = 0; i < numLED; i++){
            digitalWrite(LED[i], j % 2);
        }
        delay(rest);
      }
      break;
    default:
      //nothing
      break;
  }
}
