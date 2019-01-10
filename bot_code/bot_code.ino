/*
 *  Authors: Narcís Bustins & Ferran Veciana
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <SPI.h>  
#include "RF24.h" 

//Definicio dels moviments segons protocol moviments
#define goEndevant 1
#define goEndarrere 2
#define goDreta 3
#define goEsquerra 4
#define goStop 0

//Definicio de les velocitats segons protocol velocitats
#define vLent 0
#define vNormal 1  
#define vRapid 2


//Pins Motor Driver
const int enA = 6;
const int in1 = 5;
const int in2 = 4;
const int enB = 11;
const int in3 = 12;
const int in4 = 13;

//Pins sensor ultrasons
const int trigPin = 24;
const int echoPin = 22;

//Valors velocitats
const int valor_vLent = 50;
const int valor_vNormal = 155;
const int valor_vRapid = 255;

//Variables ultrasons
long duracio;
int distancia;

//variables generals
int rec_vel;
int rec_mov;


RF24 myRadio (7, 8); 
struct package
{
  int mov=0;
  int vel=0;
};

byte addresses[][6] = {"0"}; 

typedef struct package Package;
Package data;

void setup() 
{
  Serial.begin(115200);
  delay(1000);

  myRadio.begin(); 
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ; 
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();

  //Pins Motor Driver
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  //Pins ultrasons
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

}

// ================================================================
// ===                    FUNCIONS MOVIMENT                     ===
// ================================================================

//Realitzar moviment endavant
void endavant(int vel){
  
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  //velocitat
  analogWrite(enA, vel);
  // turn on motor B
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  //velocitat 
  analogWrite(enB, vel);
  
}

//Realitzar moviment endarrere
void endarrere(int vel){
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  //velocitat
  analogWrite(enA, vel);
  // turn on motor B
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  //velocitat
  analogWrite(enB, vel);
 
  
}

//Realitzar moviment de gir cap a la esquerra
void esquerra(int vel){
  
  //Roda endevnat
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, vel);

  //Roda endarrere
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, vel);

  
}

//Realitzar moviment de gir cap a la dreta
void dreta(int vel){
  
  //Roda endavant
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, vel);

  //Roda endarrere
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, vel);
  
}

//Parar el moviment dels dos motors
void stopMov(){
  //Parar els dos motors 
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW); 
}


//Protocol
int triarVelocitat(int vel){
    
  int v = 0;
  if (vel == vLent){
    v = valor_vLent;
  }
  if (vel == vNormal){
    v = valor_vNormal;
  }
  else if (vel == vRapid){
    v = valor_vRapid;
      
  }
  else 
       v = 0;
  
  return v;  
     
}

//Calcul de la distancia segons el sensor d'ultrasons
int llegirUltrasons(){
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duracio = pulseIn(echoPin, HIGH);
  distancia= duracio*0.034/2;

  return distancia;
  
}


void loop()  
{

  //Rebre senyals
  if ( myRadio.available()) 
  {
    while (myRadio.available())
    {
      //llegir paquet 
      myRadio.read( &data, sizeof(data) );
    }

    rec_vel = data.vel;
    rec_mov = data.mov;

    //Establir velocitat
    int vel = triarVelocitat(rec_vel);
    
    
    //Moure motors
    stopMov(); //Primer aturem el moviment previ
    if(rec_mov == goEndevant && llegirUltrasons() <= 17){
      endavant(vel);
    }
    else if(rec_mov == goEndarrere){
      endarrere(vel);
    }
    else if(rec_mov == goDreta){
      dreta(vel);
    }
    else if(rec_mov == goEsquerra){
      esquerra(vel);
    }
    else{
      stopMov();
    }
  }
 
  else{
     stopMov();
  }
    
 delay(10);
 
   
}
