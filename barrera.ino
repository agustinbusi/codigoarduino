#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <EEPROM.h>
#include <BigNumbers_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4); 
  
const int Trigger = 7; //  sensor
const int Echo = 8;  // sensor
const int servo = 50;  // servo
int cls                 =       250;
int encoder_position    =       0;
int encoder0PinALast    =       LOW;
int estado_temp         =       0;
int marca ;
int n                   =       0;
int suma                =       0;
int cuenta              =       0;
volatile int posicion   =       0;
int anterior            =       0;


Servo servoMotor;

#define encoder0PinA            2  // pin A de l'encodeur
#define encoder0PinB            3  // pin B de l'encodeur
#define encoder0Click           4  // click de l'encodeur

void setup() {

  Serial.begin(9600);//iniciailzamos la comunicación
  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(servo, OUTPUT); //servo
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0
  // Iniciamos el servo para que empiece a trabajar con el pin 9
  servoMotor.attach(servo);
  // Inicializamos al ángulo 0 el servomotor
  servoMotor.write(90); // 
  lcd.init();  // inicializa la pantalla LCD
  lcd.clear(); // clear display
  lcd.backlight();
  attachInterrupt(digitalPinToInterrupt(encoder0PinA),encoder,LOW);
  lee_memoria(); 
  pinMode (encoder0PinA, INPUT);
  pinMode (encoder0PinB, INPUT);
  pinMode (encoder0Click, INPUT);

  digitalWrite(encoder0PinB, HIGH);
  digitalWrite(encoder0PinA, HIGH);
  digitalWrite(encoder0Click, HIGH);

  lcd.setCursor(0,0); lcd.print("  TOMAS BOSSOLETTI  ");
  lcd.setCursor(0,1); lcd.print("       BARRERA      "); 
  lcd.setCursor(0,2); lcd.print("Ver.(0.1) - 10/2023 ");
  lcd.setCursor(0,3);
  for( int i=0 ; i<20 ; i++ ) { lcd.print("."); delay(150);} //250 PAUSA
}

void loop()
{
MenuPrincipal();
}

void MenuPrincipal() {
        lcd.clear();
        lcd.setCursor(0, 0);lcd.print("BARRERA");
        lcd.setCursor(13, 0);
        lcd.backlight();
        lcd.setCursor(1, 1);lcd.print("INICIAR"); 
        lcd.setCursor(1, 2);lcd.print("PROGRAMACION");
        lcd.setCursor(0, 1);lcd.print(">"); delay(cls);encoder_position = 0;encoder0PinALast = LOW;
        while (digitalRead(encoder0Click) == LOW) { }
        while (true) {
                if ( estado_temp == 1 ) { MenuPrincipal(); }
                int n = getPosition(1, 3);
                if ( digitalRead(encoder0Click) == LOW) {
                        switch (n) {
                        case 1: inicia();        break;
                        case 2: programacion();  break;
                        } delay(cls);}}
}

void inicia() {
lcd.clear();
lcd.setCursor(0, 0);lcd.print("TOMAS BOSSOLETTI");
lcd.setCursor(0, 1);lcd.print("PASADAS : 0");
lcd.setCursor(0, 2);lcd.print("TOPE : " );
lcd.setCursor(7, 2);lcd.print( cuenta );
lcd.setCursor(0, 3);lcd.print("INICIADO");
int pasada = 1;

while (1) {
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  
  Serial.print("Distancia: ");
  Serial.print(d);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();
  delay(200);          //Hacemos una pausa de 100ms

if ( digitalRead(encoder0Click) == LOW) { 
  suma++;
  if ( suma  ==  5 ) { 
       lcd.backlight(); suma = 0; 
       lcd.setCursor(0, 3);lcd.print("OPERACION CANCELADA.");
       while (digitalRead(encoder0Click) == LOW) { }
       MenuPrincipal(); 
    } 
 } else  { suma = 0; } 


if (pasada - 1 != cuenta) {
  
if (d >= 22) {
  servoMotor.write(90);
}
else 
{   
  lcd.setCursor(10, 1);lcd.print(pasada++);
  servoMotor.write(180);
  delay(1000);  
}
        
        

 } else  { lcd.setCursor(0, 3);lcd.print("FINALIZADO.");servoMotor.write(90); }  
}
}
void programacion() {
        lcd.clear();
        lcd.setCursor(0, 0);lcd.print("TOMAS BOSSOLETTI");
        lcd.setCursor(1, 1);lcd.print( cuenta );
        lcd.setCursor(4, 1);lcd.print( "CANTIDADES" );
        lcd.setCursor(0, 2);lcd.print("Gire para - + "); 
        lcd.setCursor(0, 3);lcd.print("Pulsar para volver");
        delay(cls);
        while (digitalRead(encoder0Click) == LOW) { }
        while (1) {
          if (posicion != anterior ) {
          anterior = posicion; 
          Serial.println(posicion);
         if (cuenta + posicion != 0)
          cuenta = cuenta + posicion; 
          posicion = 0;
//          {
          lcd.setCursor(1, 1);lcd.print( "   " );
          lcd.setCursor(1, 1);lcd.print( cuenta );}
//          }

  
       if ( digitalRead(encoder0Click) == LOW) { graba_memoria(); MenuPrincipal(); }  
  }  
}




void encoder() {
static unsigned long ultimaInterrupcion  = 0; 
unsigned long tiempoInterrupcion  = millis(); 
if ( tiempoInterrupcion - ultimaInterrupcion > 5 )
{
  if (digitalRead(encoder0PinB) == HIGH)
  { posicion++; }
  else
  { posicion--; }
ultimaInterrupcion = tiempoInterrupcion ;
}
}

int getPosition(int first, int last) {
        delay(10);
        int x = digitalRead(encoder0PinA);
        if ((encoder0PinALast == LOW) && (x == HIGH)) {
                if (digitalRead(encoder0PinB) == LOW) {
                        encoder_position--;
                        if (encoder_position <= first) {
                                encoder_position = first;
                        }
                        setCurs(encoder_position);
                } else {
                        encoder_position++;
                        if (encoder_position >= last) {
                                encoder_position = last;
                        }
                        setCurs(encoder_position);
                }

        }
        encoder0PinALast = x;
        return encoder_position;
}

void lee_memoria() {
  cuenta = EEPROM.read(0);
 }
 
void graba_memoria() {
  EEPROM.write(0,cuenta);
 }

int setCurs(int p) {
        lcd.setCursor(1, 0);
        lcd.print("");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
        if (p > 3)
        {p = p - 4;}
        lcd.setCursor(0, p);
        lcd.print(">");
        return 0;
}
