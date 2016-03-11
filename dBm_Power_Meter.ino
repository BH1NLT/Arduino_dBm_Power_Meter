#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

char v[20];
String vStr;
char d[10];
String dBm;
char m[30];
String mW;

void setup() {
  pinMode(A0,INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.noDisplay();
  delay(500);
  lcd.display();
}

void loop() {
  dtostrf(analogRead(A0)*(5./1023.), 1, 7, v);
  vStr = String(v);
  dBm = getdBm(v);
  float mW = getmW(v);
  String Wtype = "fW"; // femtowatt
  // determine the multiple
  // picowatt  .000000X
  // nanowatt  .00000X
  // microwatt .0000X
  // milliwatt .000X whole number to 999.9999 = milliwatts
  // watt      X.    whole numbers > 1000.0+  = Watt
  // kilowatt  X000. whole numbers > 10000.0+ = KWatt
  // https://www.safaribooksonline.com/library/view/fiber-optic-video/9780240804880/034_9780080506302_appG.html
  //float mWd = strtod(mW);
  float mWd = mW;
  if(mWd >= 0.000000000999 && mWd <= 0.000000999) {   // picowatt
    Wtype = "pW"; // x100000
    mWd = mWd * 1000000000;
  } else if(mWd >= 0.000000999 && mWd <= 0.000999) {  // nanowatt
    Wtype = "nW"; // x10000
    mWd = mWd * 1000000;
  } else if(mWd >= 0.000999 && mWd <= 0.999) {        // microwatt
    Wtype = "uW"; // x1000
    mWd = mWd * 1000;
  } else if(mWd >= 0.999 && mWd <= 999.9) {           // milliwatt
    Wtype = "mW";                                    
  } else if(mWd >= 999.99) {                          // Watt
    Wtype = "W"; // /1000
    mWd = mWd / 1000;
  }
  //Serial.print(mWd);  
  //Serial.println(Wtype);  
  
  //
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(dBm+"dBm");
  lcd.setCursor(0, 1);
  dtostrf(mWd,1,3,m);
  lcd.print(m+Wtype+" "+vStr+"v");  
  delay(1000);
}

void set_zero() {
  // input 0dBm to this device.
  // store the offset in memory (if we can't store it perm).
  // read the voltage when we know we're inputting 0dBm and that will be the 
  // baseline for all other readings used within get_dBm() function instead of 0.6
  // which is probably incorrect.
  //
  //   float dBm = (volts - 0.6) / 0.025;
  //
  
}
void set_attenuator_inline(float add_dBm) {
  // add_dBm =  3.0 = 3db  attenuation
  //           10.0 = 10db 
  //           30.0 = 30db
  //           etc...
  //
  // !!! NOT 3, 10, 30 !!!
  //
  // because we're inputing a stronger signal which is then being attenuated
  //
  // If we input +34dBm the ZX47-60 can only handle safely about 5dBm so we add inline
  // physically at this devices input, a 30dBm Attenuator.  What we would see without 
  // using this function would +4dBm.  This function is intended to correct for this 
  // additional attenuation and provide an accurate reading based on un-attenuated 
  // signal on the LCD disply.  This way the user won't have to mentally calculate 
  // the offset/difference while using the device if they're set this value in code.
  // 
  // The best way to accomplish this would be to use a button and an obtical encoder
  // and create a menu for them to make changes to the code on the fly.
  // 
  // Initially we'll just test by hard coding this and uploading the change to the Arduino.
  // However, the I'd prefer to have some sort of menu to allow for on-the-fly changes like this.
  //
  
}

float getdBm(char v[20]) {
  // Max Volts 2.1 / 65 dBm range = 0.0323 v/dBm
  float volts = atof(v);
  float dBm = (volts - 0.6012) / 0.025;
  if(dBm < 0) {
     dBm = abs(dBm);
  } else {
     dBm = dBm * -1;
  }
  return dBm;
}

float getmW(char v[20]) {
  //Serial.print(v);
  //Serial.print(" ");  
  
  float dBm = getdBm(v);
  //Serial.print(dBm);
  //Serial.print(" ");  
  
  // The formula for converting dBm to mW is:
  // P(Watts) = 10 ^ (( P(dBm) - 30) / 10)
  // so to get mW is to / 1000;
  
  float dBmDiv = dBm / 10;
  //Serial.print(dBmDiv);
  //Serial.print(" ");    
  
  float PdBm = pow(10,dBmDiv);  
  char PdBmChr[20];
  dtostrf(PdBm, 1, 10, PdBmChr);
  String PdBmStr = String(PdBmChr);
  //Serial.println(PdBmStr);  

  return strtod(PdBmChr,NULL);
}

