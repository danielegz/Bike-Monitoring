#include <SoftwareSerial.h>
#include <string.h> 
SoftwareSerial SIM808(7, 8); //Seleccionamos los pines 7 como Rx y 8 como Tx
byte buffer[64]; //Buffer para recivir datos del puerto serie
int count=0;    
int Alarma=10;
//VARIABLES PWM
const int pinPWMA = 3;
const int pinAIN2 = 4;
const int pinAIN1 = 5;  
const int pinPWMB = 6;
const int pinBIN2 = 2;
const int pinBIN1 = 11;  
const int pinSTBY = 12;
 
const int waitTime = 2000;   //tiempo de espera
const int speed = 200;      //velocidad de giro
const int pinMotorA[3] = { pinPWMA, pinAIN2, pinAIN1 };
const int pinMotorB[3] = { pinPWMB, pinBIN1, pinBIN2 };
//NUMERO CELULAR
#define phonenumber "+573505166745"

enum moveDirection {
   forward,
   backward
};
 
enum turnDirection {
   clockwise,
   counterClockwise
};
 
void setup()
{  
  pinMode(Alarma,OUTPUT);
  InicializarSIM808();
  pinMode(pinAIN2, OUTPUT);
  pinMode(pinAIN1, OUTPUT);
  pinMode(pinPWMA, OUTPUT);
  pinMode(pinBIN1, OUTPUT);
  pinMode(pinBIN2, OUTPUT);
  pinMode(pinPWMB, OUTPUT);

  SIM808.begin(19200);//Velocidad del puerto serie por Software pines 2 y 3
  Serial.begin(19200);      //Velocidad del puerto serie por Hardware pines 0 y 1
  delay(500);
}

void loop()
{
   if (SIM808.available())              
  {
    while(SIM808.available())          //Leyendo datos del arreglo de caracteres 
    {
      buffer[count++]=SIM808.read();     //Almacenando los datos del arreglo en un buffer
      if(count == 64)break;
  }
    Serial.write(buffer,count);            
    Cmd_Read_Act();                        
    clearBufferArray();              
    count = 0;                       
 
 
  }
  if (Serial.available())            // Verifica si se dispone de datos en el puerto serie por hardware
 SIM808.write(Serial.read());       // y los escribe en el escudo SIM808
 //Envíamos y recibimos datos
}
void InicializarSIM808()
{
 delay (5000);
 SIM808.begin(19200); //Configura velocidad del puerto serie para el SIM900
 Serial.begin(19200); //Configura velocidad del puerto serie del Arduino 
 Serial.println("OK");
 SIM808.println("AT\r");
 delay (100);
 SIM808.println("AT+CPIN=\"1013\""); //Comando AT para introducir el PIN de la tarjeta
 delay(25000); //Tiempo para que encuentre una RED
 Serial.println("PIN OK");
 SIM808.print("AT+CLIP=1\r"); // Activa la identificación de llamada
 delay(100);
 SIM808.print("AT+CMGF=1\r"); // Configura el modo texto para enviar o recibir SMS
 delay(100);
 SIM808.print("AT+CNMI=2,2,0,0,0\r");   // Saca el contenido del SMS por el puerto serie del GPRS
 delay(100);
}
void MoverMotor()
{
  enableMotors();
  move(forward, 180);
  delay(waitTime);
 
  fullStop();
  delay(waitTime);
}
void enableMotors()
{
   digitalWrite(pinSTBY, HIGH);
}
 
void disableMotors()
{
   digitalWrite(pinSTBY, LOW);
}
void move(int direction, int speed)
{
   if (direction == forward)
   {
      moveMotorForward(pinMotorA, speed);
      moveMotorForward(pinMotorB, speed);
   }
   else
   {
      moveMotorBackward(pinMotorA, speed);
      moveMotorBackward(pinMotorB, speed);
   }
}
void moveMotorForward(const int pinMotor[3], int speed)
{
   digitalWrite(pinMotor[1], HIGH);
   digitalWrite(pinMotor[2], LOW);
 
   analogWrite(pinMotor[0], speed);
}
void moveMotorBackward(const int pinMotor[3], int speed)
{
   digitalWrite(pinMotor[1], LOW);
   digitalWrite(pinMotor[2], HIGH);
 
   analogWrite(pinMotor[0], speed);
}
 
void stopMotor(const int pinMotor[3])
{
   digitalWrite(pinMotor[1], LOW);
   digitalWrite(pinMotor[2], LOW);
 
   analogWrite(pinMotor[0], 0);
}
void fullStop()
{
   disableMotors();
   stopMotor(pinMotorA);
} 
void Cmd_Read_Act(void)          
{  
  char buffer2[64];
  char comparetext[25];    
  for (int i=0; i<count;i++)
  { buffer2[i]=char(buffer[i]);}  
  memcpy(comparetext,buffer2,25); 
  if (strstr(comparetext,phonenumber))
  {    
    if (strstr(buffer2,"AlarmaON"))           
    {
      digitalWrite(Alarma, HIGH);      
    }
    if (strstr(buffer2,"AlarmaOFF"))        
    {
      digitalWrite(Alarma, LOW);       
    }
    if (strstr(buffer2,"Freno"))        
    {
      MoverMotor();      
    }
    if (strstr(buffer2,"FrenoOFF"))        
    {
      fullStop();
    }
  }
}
void clearBufferArray()              
{
  for (int i=0; i<count;i++)
    { buffer[i]=NULL;}                  // borrar todos los índices del arreglo
}
