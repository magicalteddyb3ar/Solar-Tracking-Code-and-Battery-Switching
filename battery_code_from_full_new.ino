int analogInput1 = A3; //Change if not connected to pin A3
int analogInput2 = A2; //Change if not connected to pin A2
float vout1 = 0.0; 
float vin1 = 0.0, vin11=0.0; 
float vout2 = 0.0; 
float vin2 = 0.0, vin22 = 0.0;
float R1 = 30180.0; //onboard resistor 1 value for battery 1
float R2 = 7460.0; //onboard resistor 2 value for battery 1
float R21 = 30030.0; //onboard resistor 1 value for battery 2
float R22 = 7480.0; // onboard resistor 2 value for battery 2
int svalue1 = 0; 
int svalue2 = 0;
double chargepct1 = 0.0; //charge pct of battery 1
double chargepct2 = 0.0; //charge pct of battery 2
int LED1B = 2, LED1R = 3, LED1Y = 4, LED1G = 5; //pins for LED's Battery 1 blue, red, yellow, green
int LED2B = 6, LED2R = 7, LED2Y = 8, LED2G = 9; //pins for LED's Battery 2
int relay1 = 11;                                //relay 1 pin
int relay2 = 13;                                //relay 2 pin
int NLswitch = 12;                              //no load switch pin
int sw;                                         //stores switch value
int Load1 = 0, Load2 = 0;                       //is load on battery 1 or 2
int LTT1 = 0, LTT2 = 0;                         //battery 1 or 2 drops below less than 10%
int delay1 = 0;                                 //variable to only allow initial delay once
int voffset1 = 0.2, voffset2 = 0.18;            //upon starting up system after unplugging batteries, look at values of batteries compared to voltmeter and adjust offset value 

void setup(){
pinMode(analogInput1, INPUT);
pinMode(analogInput2, INPUT);
pinMode(NLswitch, INPUT);
pinMode(LED1B ,OUTPUT);
pinMode(LED1R ,OUTPUT);
pinMode(LED1Y ,OUTPUT);
pinMode(LED1G ,OUTPUT);
pinMode(LED2B ,OUTPUT);
pinMode(LED2R ,OUTPUT);
pinMode(LED2Y ,OUTPUT);
pinMode(LED2G ,OUTPUT);
pinMode(relay1 ,OUTPUT);
pinMode(relay2 ,OUTPUT);
Serial.begin(9600);
}

void loop()
{
if (delay1>0){    //controls the initial delay upon startup (only happens once)
  delay(15000);
  delay1 = 1;
}
for(int i=0; i<1000; i++)   //finds voltage of the battery 1000 times and averages it 
  {
  svalue1 = analogRead(analogInput1); //this reads the value from the sensor
  vout1 = (svalue1 * 5.0) / 1024.0;   //calculates the voltage to the arduino
  vin11 = vout1 / (R2/(R1+R2))+voffset1;  //used voltage divider rule to find the voltage from battery 1
  vin1 = vin1 + vin11;                //sums all 1000 values measured from the battery
  svalue2 = analogRead(analogInput2); //this reads the value from the sensor
  vout2 = (svalue2 * 5.0) / 1024.0;   
  vin22 = vout2 / (R22/(R21+R22))+voffset2;
  vin2 = vin2 + vin22;
  delay(10);
  }

vin1 = vin1/1000;   //averages vin1 after the for loop
Serial.print("Vin1: ");
Serial.println(vin1,2); // prints the voltage

vin2 = vin2/1000;   //averages vin2 after for loop
Serial.print("Vin2: ");
Serial.println(vin2, 2);

if (chargepct2 > chargepct1 && (chargepct1<10 && chargepct2>90))      //ensures load relay only switches to battery 2 when battery 1 is less than 10% and battery 2 is charged past 90%
{
  digitalWrite(relay1, LOW);
  delay(100);
  Load1 = 0;
}

if (chargepct1 > chargepct2 && (chargepct1>90 && chargepct2<10))    //ensures load relay only switches to battery 2 when battery 2 is less than 10% and battery 1 is charged past 90%
{
  digitalWrite(relay1, HIGH);
  delay(100);
  Load1 = 1;
  Load2 = 0;
}

if (chargepct2 > chargepct1 && (chargepct1<10 && chargepct2>90))    //ensures charge relay switches to battery 1 when its below 10% and battery 2 has been charged to 90%
{
  Serial.println("relay 2 high");
  digitalWrite(relay2, HIGH); 
  delay(100);
}

if (chargepct1 > chargepct2 && (chargepct1>90 && chargepct2<10))  //ensures charge relay switches to battery 2 when its below 10% and battery 1 has been charged to 90%
{
  Serial.println("relay 2 low");
  digitalWrite(relay2, LOW); 
  delay(100);
}
sw = digitalRead(NLswitch);       //read load switch
if(sw == HIGH)                    //when load switch is on high set load values to 1
  {
    Load1 = 1;
    Load2 = 1;
    Serial.println("sw on");
  }
else if(sw == LOW)               //if switch off set load values to 0
  {
    Load1 = 0;
    Load2 = 0;
    Serial.println("sw off");
  }

if(Load2 == 0)                  //if load 2 is not being used 
  {
  if(LTT2 == 1)                 // if load 2 reached less than 10%
    {
    chargepct2 = chargepct2;    //store the charge pct 2 value it ended on 
    Load1 = 1;                  //since battery 1 is now being used, set load1 variable to 1
    if(sw == 0){Load1 = 0;}        
    }
  else if(vin2 > 10 && vin2< 12.9)  //charge percent calculation using lithium iron phosphate voltage levels at no load and resting conditions between 10 and 12.9 volts
    {
    chargepct2 = 6.36*vin2 - 64.57;
    }
  else if (vin2 > 12.9 && vin2< 13.6)   //charge percent calculation using lithium iron phosphate voltage levels at no load and resting conditions between 12.9 and 13.6 volts
    {
    chargepct2 = 127.27*vin2 - 1616.9;
    }
  else if(vin2<10){chargepct2 = 0;}     //below 10v, battery is at 0%
  else if (vin2>13.6){chargepct2 = 100;}  //higher than 13.6v at rest, battery at 100%
  }
if(Load2 == 1)      //if inverter reads around 113W (max load from the smart grid) use this math to estimate the battery charge based on experimatal data
  {
    if(LTT2 == 1)
    {
    chargepct2 = chargepct2;        
    }
    else if(vin2 > 12 && vin2<12.67)    
    {
      chargepct2 = 2E-24*exp(4.6769*vin2);
    }
    else if(vin2<=12){chargepct2 = 0;}
    else if (vin2>=12.67){chargepct2 = 100;}
  }

if(Load1 == 0)    //same as above but for batter 1 
  {
  if(LTT1 == 1)
    {
    chargepct1 = chargepct1;        
    }
  else if(vin1 > 10 && vin1< 12.9)
    {
    chargepct1 = 6.36*vin1 - 64.57;
    }
  else if (vin1 > 12.9 && vin1< 13.3)
    {
    chargepct1 = 127.27*vin1 - 1616.9;
    }
  else if(vin1<10){chargepct1 = 0;}
  else if (vin1>13.3){chargepct1 = 100;}
  }
if(Load1 == 1)
  {
    if(vin1 > 12.39 && vin1<12.64)
    {
      chargepct1 = 252.92*vin1 -3105.7;
    }
    else if(vin1<=12.39&&vin1>11.65){chargepct1 = 1E-17*exp(3.42*vin1);}
    else if(vin1<11.65){chargepct1 = 0;}
    else if (vin1>=12.54){chargepct1 = 695.65*vin1-8650.9;}
    if(chargepct1>100){chargepct1 = 100;}
  }

if(chargepct1<10)
  {
    LTT1 = 1;
  }
else if (vin1>13.35)
  {
    LTT1 = 0;
  }
if(chargepct2<10)
  {
    LTT2 = 1;
  }
else if (vin2>13.35)
  {
    LTT2 = 0;
  }  

Serial.println();
Serial.println(chargepct1,2); // prints the voltage
Serial.println(chargepct2,2); // prints the voltage*

for(int count=0; count<4; count++)            //turn on LED's based on charge percent for 14 cycles
  {
  for(int i=0; i<14; i++)
    {
    if(chargepct1>0 && chargepct1<25)
      {
        digitalWrite(LED1R,0);
        digitalWrite(LED1Y,0);
        digitalWrite(LED1G,0);
        digitalWrite(LED1B,1);
        delay(100);
        digitalWrite(LED1B,0);
        delay(100);
      }
    else if(chargepct1>=25 && chargepct1<50) 
      {
        digitalWrite(LED1B,1);
        digitalWrite(LED1R,1);
        digitalWrite(LED1Y,0);
        digitalWrite(LED1G,0);
        delay(100);
        digitalWrite(LED1R,0);
        delay(100);
      }
    else if(chargepct1>=50 && chargepct1<75)
    {
      digitalWrite(LED1B,1);
      digitalWrite(LED1R,1);
      digitalWrite(LED1Y,1);
      digitalWrite(LED1G,0);
      delay(100);
      digitalWrite(LED1Y,0);
      delay(100);
    }
    else if(chargepct1>=75 && chargepct1<95) 
    {
      digitalWrite(LED1B,1);
      digitalWrite(LED1R,1);
      digitalWrite(LED1Y,1);
      digitalWrite(LED1G,1);
      delay(100);
      digitalWrite(LED1G,0);
      delay(100);
    }
    else if(chargepct1>=95)
    {
      digitalWrite(LED1B,1);
      digitalWrite(LED1R,1);
      digitalWrite(LED1Y,1);
      digitalWrite(LED1G,1);  
    }

    else{
      digitalWrite(LED1B,0);
      digitalWrite(LED1R,0);
      digitalWrite(LED1Y,0);
      digitalWrite(LED1G,0);
    }

if(chargepct2>0 && chargepct2<25){
digitalWrite(LED2R,0);
digitalWrite(LED2Y,0);
digitalWrite(LED2G,0);
digitalWrite(LED2B,1);
delay(100);
digitalWrite(LED2B,0);
delay(100);
}
else if(chargepct2>=25 && chargepct2<50){
digitalWrite(LED2B,1);
digitalWrite(LED2R,1);
digitalWrite(LED2Y,0);
digitalWrite(LED2G,0);
delay(100);
digitalWrite(LED2R,0);
delay(100);
}
else if(chargepct2>=50 && chargepct2<75){
digitalWrite(LED2B,1);
digitalWrite(LED2R,1);
digitalWrite(LED2Y,1);
digitalWrite(LED2G,0);
delay(100);
digitalWrite(LED2Y,0);
delay(100);
}
else if(chargepct2>=75 && chargepct2<95){
digitalWrite(LED2B,1);
digitalWrite(LED2R,1);
digitalWrite(LED2Y,1);
digitalWrite(LED2G,1);
delay(100);
digitalWrite(LED2G,0);
delay(100);
}
else if(chargepct2>=95){
digitalWrite(LED2B,1);
digitalWrite(LED2R,1);
digitalWrite(LED2Y,1);
digitalWrite(LED2G,1);  
}
else{
digitalWrite(LED2B,0);
digitalWrite(LED2R,0);
digitalWrite(LED2Y,0);
digitalWrite(LED2G,0);
}
}
delay(1000);
}
}