
int ARead0, ARead1;
int diffNS, diffEW;
int IN1 = 9;
int IN2 = 10;
int IN3 = 11;
int IN4 = 12;
int sw=2;

int buttonUp=3;                                                        //set ports
int buttonDown=4;
int buttonLeft=5;
int buttonRight=6;

int bUp, bDown, bLeft, bRight;

int Photo1, Photo2, Photo3, Photo4, read=0;
int stop1 = 0, stop2 = 0;
void setup() {
Serial.begin(9600);
pinMode(IN1, OUTPUT);
pinMode(IN2, OUTPUT);
}
int swi = digitalRead(sw);
void loop() {
Serial.print("switch: ");
Serial.println(swi);
while(swi==0){
//int switchUD=serial.Read(0);
bUp=digitalRead(buttonUp);
bDown=digitalRead(buttonDown);
bLeft=digitalRead(buttonLeft);
bRight=digitalRead(buttonRight);
    
if(bUp==1){                       //move up
    analogWrite(A4,255);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    }

else if(bDown==1){                     //move down
    analogWrite(A4,255);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    }
else{
    analogWrite(A4,0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
}

if(bLeft==1){                    //move left
    analogWrite(A5,255);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    }
    
else if(bRight==1){                  //move right
    analogWrite(A5,255);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);   
    }
else{
    analogWrite(A5,0);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    }
    swi = digitalRead(sw);
}

while(swi==1){
ARead0 = analogRead(A0);
ARead1 = analogRead(A1);

Photo1 = 1023-ARead0;
Photo2 = 1023-ARead1;
Photo3 = ARead1;
Photo4 = ARead0;

diffNS = Photo1-Photo4;
diffEW = Photo2-Photo3;
Serial.print("Top Photoresistor: "); Serial.println(Photo1);
Serial.print("Right Photoresistor: "); Serial.println(Photo2);
Serial.print("Left Photoresistor: "); Serial.println(Photo3);
Serial.print("Bottom Photoresistor: ");Serial.println(Photo4);


if(diffNS<-250)
  {
    Serial.println("move up");
    analogWrite(A4,200);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    stop1 = 0;
  }
else if(diffNS>250)
  {
    Serial.println("move down");
    analogWrite(A4,200);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    stop1 = 0;
  }
else
  {
    Serial.println();
    analogWrite(A5,0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    stop1 = 1;
  }
if(diffEW<-250)
  {
    Serial.println("move left");
    analogWrite(A5,200);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    stop2 = 0;
  }
else if(diffEW>250)
  {
  Serial.println("move right");
  analogWrite(A5,200);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  stop2 = 0;
  }
else
  {
    Serial.println();
    analogWrite(A5,0);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    stop2 = 1;
  }
if(stop1==1 && stop2==1)
{
    delay(1000);
}

Serial.println();
delay(10);
swi = digitalRead(sw);
  }
}
