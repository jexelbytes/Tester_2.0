#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <String.h>

#define ANCHO 128
#define ALTO 64
#define OLED_RESET 4
Adafruit_SSD1306 oled(ANCHO, ALTO, &Wire, OLED_RESET);

#define potenciometro A3
#define Button 10
#define mideCap   A2
#define r120k 7
#define r12k 6
#define r910 9
#define GND 8

// 1005.49

byte Menu = 0;
const float res = 12000.0F; 

//****************************************************************************************************** SETUP
void setup() {
   Serial.begin(9600);
   Wire.begin();
   oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop() {
  switch(Menu){
      case 0:
        menu();
      break;
      case 1:
        capacitancia2();
      break;
      case 2:
        Osciloscope();
      break;
      case 3:
      PWM();
      break;    
      case 4:
        resistencia();
      break;
  }
}

//****************************************************************************************************** CONTROLES
int pospotenciometro(){
  return analogRead(potenciometro);
}

byte divpotenciometro(byte div){
  return div - ((analogRead(potenciometro) / (1024/(div))));
}

bool pulseButton(){
    if(digitalRead(Button) == HIGH){
      tone(13, 800, 50);
      while(digitalRead(Button) == HIGH){delay(10);}
      delay(100);
      return true;
    }else{
      return false;
    }
  }

//****************************************************************************************************** MENU
void menu(){
 //--------------------> Init
  int Selector = 0;
  oled.setTextSize(1);
  pinMode(GND, OUTPUT);
  digitalWrite(GND, LOW);
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  pinMode(Button, INPUT);
  pinMode(r12k, INPUT);
  pinMode(r910, INPUT);
  pinMode(r120k, INPUT);
  pinMode(mideCap, INPUT);

  //--------------------> RUN
  while(Menu == 0){
    oled.clearDisplay();
    oled.setTextColor(WHITE);
    
    Selector = divpotenciometro(4);
  
    oled.setCursor(10, 10 * Selector);
    oled.print(">");
    oled.setCursor(20, 10);
    oled.print("Capacitancia");
    oled.setCursor(20, 20);
    oled.print("Osciloscope");
    oled.setCursor(20, 30);
    oled.print("PWM");
    oled.setCursor(20, 40);
    oled.print("Resistencia");

    if(pulseButton()){
      Menu = Selector;
      return;
    }
    
    oled.display();
  }
}

//****************************************************************************************************** CAPACITANCIA

void capacitancia2(){
 //------------------------------------> Init
  pinMode(r910, INPUT);
  pinMode(r12k, OUTPUT); 
  unsigned long iniPeriodo = 0;
  unsigned long transcurrido = 0;
  float Cx  = 0;
  float Cx0  = 0;
  String nom = "uF";
  float tpm = 0;

 //------------------------------------> RUN
  while(Menu == 1){
    oled.clearDisplay();
    oled.setTextColor(WHITE);
    
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.print("Capacitancia!");
    
    pinMode(r120k, INPUT);
    pinMode(r12k, OUTPUT); digitalWrite(r12k, HIGH);
    
    if(analogRead(mideCap) < 645){
      
      digitalWrite(r12k, LOW);          
      pinMode(r910, OUTPUT);            
      digitalWrite(r910, LOW);
      while(analogRead(mideCap) > 0){}
      
      pinMode(r910, INPUT);
      
      digitalWrite(r12k, HIGH);
      iniPeriodo = micros();
      while(analogRead(mideCap) < 645){}
      tpm = analogRead(mideCap);
      transcurrido = micros() - iniPeriodo - 116;
      
      if(tpm > 645){
        transcurrido = ((transcurrido / tpm) * 645) - 114;
      }
      
      Cx = ((float)transcurrido / 12000.0f);
      
      if(Cx < 0.000f || Cx > 20000){
        Cx = 0;
      }
      
      digitalWrite(r12k, LOW);
      pinMode(r12k, INPUT);
    
    }else
    {
      digitalWrite(r12k, LOW);            
      pinMode(r910, OUTPUT);            
      digitalWrite(r910, LOW);
      while(analogRead(mideCap) > 0){}  // --------------------------------------->DESCARGA
      
      pinMode(r910, INPUT);
      pinMode(r12k, INPUT); // ----------------------------------------------->INICIALIZACION
      pinMode(r120k, OUTPUT);
      
      digitalWrite(r120k, HIGH);
      iniPeriodo = micros();
      while(analogRead(mideCap) < 645){} // -------------------------------------->CARGA
      
      tpm = analogRead(mideCap);
      transcurrido = micros() - iniPeriodo - 116;
      
      if(tpm > 645){
        transcurrido = ((transcurrido / tpm) * 645) - 114;
      }
      
      Cx = ((float)transcurrido / 120000.0f);

      if(Cx > 20000){
      Cx = 0; 
      }
      
      digitalWrite(r120k, LOW);
      pinMode(r120k, INPUT);
    }


    
    pinMode(r910, OUTPUT);            
    digitalWrite(r910, LOW);
    while(analogRead(mideCap) > 0){}
    pinMode(r910, INPUT);

    if(Cx < 1.0f){
      Cx*= 1000.0f;
      nom = "nF";
    }else{
      nom = "uF";
    }

    oled.setCursor(5, 30);
    oled.setTextSize(2);
    oled.print(Cx);
    oled.setCursor(91 , 34);
    oled.setTextSize(3);
    oled.print(nom);
    
    oled.drawLine(91, 50, 91, 59, WHITE);
    oled.drawLine(92, 50, 92, 59, WHITE);
    oled.drawLine(93, 50, 93, 59, WHITE);
    
    oled.setCursor(5, 20);
    oled.setTextSize(1);
    oled.print(Cx0);
    oled.print(nom);

    if(Cx != 0){
      Cx0 = Cx;
      }
    
    oled.setCursor(100, 10);
    oled.print(">");
    
    oled.setCursor(110, 10);
    oled.print("Esc");

    if(pulseButton()){
      Menu = 0;
    }
    
    oled.display();
  }  
}

//****************************************************************************************************** RESISTENCIA
void resistencia(){
  pinMode(GND, OUTPUT);
  digitalWrite(GND, HIGH);
  
  float r1 = 0;
  float vout = 0;
  String nomr = "Ohm";
  
  while(Menu == 4){
    oled.clearDisplay();
    oled.setTextColor(WHITE);
      
    pinMode(r910, INPUT);
    pinMode(r120k, INPUT);
    pinMode(r12k, OUTPUT);
    digitalWrite(r12k, LOW);
    
    oled.setCursor(0, 0);
    oled.print("Resistencia!");
  
    vout = analogRead(mideCap);
    vout = 0.004887585532746f * vout;
    r1 = res * ((5 / vout) - 1);
    
    nomr= "Ohm";
    
    if(r1 > 6000.0f){
      r1 = r1/1000;
      nomr= "Kohm";
      
      if(r1 > 50.0f){
        pinMode(r910, INPUT);
        pinMode(r12k, INPUT);
        pinMode(r120k, OUTPUT);
        digitalWrite(r120k, LOW);
        vout = analogRead(mideCap);
        vout = 0.004887585532746f * vout;
        r1 = 120000.0f * ((5 / vout) - 1);
        r1 = r1/1000;
      }
    }else if(r1 > 100.0f && r1 < 2000){
      pinMode(r12k, INPUT);
      pinMode(r910, OUTPUT);
      digitalWrite(r910, LOW);
      vout = analogRead(mideCap);
      vout = 0.004887585532746f * vout;
      r1 = 910.0f * ((5 / vout) - 1);
      nomr= "Ohm";
    }

    oled.setCursor(5, 30);
    oled.setTextSize(2);
    oled.print(r1);
    oled.setTextSize(1);
    oled.print(nomr);
    oled.setCursor(0, 55);
    oled.print("100~300k");
    oled.setCursor(100, 10);
    oled.print(">");
    oled.setCursor(110, 10);
    oled.print("Esc");

    byte t = 0;
    
    if(r1 < 300 && r1 > 0){
      tone(13, 1000);
    }


    pinMode(r910, INPUT);
    pinMode(r120k, INPUT);
    pinMode(r12k, OUTPUT);
    digitalWrite(r12k, LOW);
    
    while(t < 20){
      vout = analogRead(mideCap);
      vout = 0.004887585532746f * vout;
      r1 = 12000 * ((5 / vout) - 1);
      
      if(r1 = 0 || r1 > 2000){
        noTone(13);
      }
      
      if(pulseButton()){
        Menu = 0;
        delay(300);
        noTone(13);
        return;
      }
      
      delay(10);
      t++;
    }
    
    oled.display();
  }
}
//****************************************************************************************************** Osciloscope

void Osciloscope(){

  byte lon = 128;
  byte arr[lon];
  int a = 0;
  float V = 0.0f;
  float H = 0.0f;
  byte h = 0;
  bool Sc = false;
  byte P = 0;
  int fff=0;
  
  while(Menu == 2){
    byte y0 = 51;
    byte s = 1;
    int xD = 0;
    byte M = 255;
    float T = 0.0f;
  
    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setTextSize(1);

    T = micros();
    for(byte i=0; i<lon; i++){  // REALIZA LA LECTURA DE LA FRECUENCIA
      arr[i]= analogRead(A1)/4;
      delayMicroseconds(fff);
    }
    
    T= ((micros() - T)/lon)/1000;
    
    for(byte i=0; i<lon; i++){  // OBTINE EL VALOR MAS ALTO PARA CALCULAR EL VOLTAGE
      if(arr[i] < M){
        M=arr[i];
      }
    }
    
    V = ((((255 - M) - 128) * 0.0195312f) + V)/2;  // CALCULAR EL VOLTAGE

    for(byte i=0; i<lon; i++){  // OBTIENE EL VALOR 0 PARA ACOMODAR LA GRAFICA
      if(arr[i] == 127 && arr[i+2] > 127){
        P=i;
        break;
      }else if(arr[i] < 127 && arr[i+2] > 127){
        P=i;
        break;
      }else if(arr[i] == 127 && arr[i+2] == 127){
        P=i;
        break;
      }
    }
    
    if(h == 1){
      byte P0 = 0;
      
      for(byte i=P+3; i<lon; i++){
        if(arr[i] == 128 && arr[i+2] > 128){
          P0= i - P;
          break;
        }else if(arr[i] < 128 && arr[i+2] > 128){
          P0= i - P;
          break;
        }
      }
      
      if(V > 0){
        H = 1000/(P0*T);
      }else{
        H=0;
      }
      oled.setCursor(74, 56);
      oled.print(H);
      oled.print("Hz");
    }else if(h == 2){
      oled.setCursor(98, 56);
      oled.print(V);
      oled.print("v");
    }
    
    if(V < 1.0f){
      if(V < 0.5f){
        oled.setCursor(0, 10);
        oled.print("0.5");
        oled.setCursor(0, 56);
        oled.print("0.5");
        s=3;
      }else{
        oled.setCursor(0, 10);
        oled.print("1.0");
        oled.setCursor(0, 56);
        oled.print("1.0");
        s=2;
      }
    }else{
        oled.setCursor(0, 10);
        oled.print("2.5");
        oled.setCursor(0, 56);
        oled.print("2.5");
      s=1;
    }
    
    
    oled.setCursor(0, 33);
    oled.print("0");
    
    y0 = mag(arr[0],s);
    
    oled.drawLine(10, 8, 127, 8, WHITE);    // TOP
    oled.drawLine(16, 22, 19, 22, WHITE);
    oled.drawLine(10, 36, 19, 36, WHITE);  // MIDLE - LEFT
    oled.drawLine(16, 49, 19, 49, WHITE);
    oled.drawLine(19, 9, 19, 63, WHITE);    // LEFT
    
    if(P+108 < lon){
      xD = P+108;
    }else{
      xD = lon;
    }
    
    for(byte i= P; i<xD ; i++){
      oled.drawLine(i+19-P, y0, i+19-P, mag(arr[i],s), WHITE);
      y0 = mag(arr[i], s);
    }

    a = pospotenciometro();

    if(Sc){
      oled.setCursor(0, 0);
      oled.print("->");
      fff = a;
      a = 3;
    }else{
      a = (a/342) +1;
      if(a==0){
        a=1;
      }
      oled.setCursor(132 - (23*a), 0);
      oled.print("~");
    }
    
    oled.setCursor(116, 0);
    oled.print("Ex");
    oled.setCursor(93, 0);
    oled.print("Sw");
    oled.setCursor(70, 0);
    oled.print("Sc");
    oled.setCursor(13, 0);
    oled.print(fff/10);
    
    if(pulseButton()){
      switch(a){
        case 1:{
            Menu = 0;
            delay(100);
          break;
        }
        case 2:{
          if(h > 1){
            h=0;
          }else{
            h++;
          }
            delay(100);
          break;
        }
        case 3:{
            Sc= !Sc;
            delay(100);
          break;
        }
        case 4:{
            delay(100);
          break;
        }
        case 5:{
            delay(100);
          break;
        }
      }
    }
    oled.display();
  }
}

byte mag(byte F, byte s){
  switch(s){
    case 1:
      return (F/4.74074074074074f)+10;
    break;
    case 2:
      return (F*0.4)-14;
    break; 
    case 3:
      return (F*0.8)-65;
    break; 
  }
}

//****************************************************************************************************** INDUCTANCIA
void Inductancia(){
  
  pinMode(GND, OUTPUT);
  digitalWrite(GND, LOW);
  while(Menu == 3){
    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setTextSize(1);


    
    oled.setCursor(100, 10);
    oled.print(">");
    oled.setCursor(110, 10);
    oled.print("Esc");
    
    oled.setCursor(10, 30);
    oled.print("Falta poco!");
    
    
    oled.display();
    
    if(pulseButton()){
        Menu = 0;
        delay(300);
        noTone(13);
        return;
    }
  }
}

void PWM(){
  
  pinMode(GND, OUTPUT);
  pinMode(r910, OUTPUT);
  digitalWrite(GND, LOW);
  
  while(Menu == 3){
    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setTextSize(1);


    
    oled.setCursor(100, 10);
    oled.print(">");
    oled.setCursor(110, 10);
    oled.print("Esc");

    float valor = pospotenciometro()/10.23f;
    
    oled.setCursor(10, 10);
    oled.print(valor);
    analogWrite(r910,pospotenciometro()/4);
    valor +=17;
    
    oled.drawLine(17, 30, valor, 30, WHITE);
    oled.drawLine(valor, 30, valor, 60, WHITE);
    oled.drawLine(valor, 60, 117, 60, WHITE);
    
    oled.display();
    
    if(pulseButton()){
        Menu = 0;
        delay(300);
        noTone(13);
        return;
    }
  }
}
