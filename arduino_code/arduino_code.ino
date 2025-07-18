#include <Servo.h>
#include "AFMotor.h"
#define Echo A0
#define Trig A4
#define motor 10
#define Speed 170
#define spoint 103
char mode;
int distance;
int distGauche {0};
int distDroite {0};
char toggle{'0'};
Servo servo;
AF_DCMotor M2(2);
AF_DCMotor M4(4);

void setup() {
  Serial.begin(9600);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  servo.attach(motor);
  M2.setSpeed(Speed);
  M4.setSpeed(Speed);

  
}

void loop() 
{
 
  //initialisation connexion Bluetooth
  if (Serial.available() > 0) {
      mode = Serial.read();
      Serial.println(mode);
    } 

    //stockage de la valeur du mode
    if(mode=='X' || mode == 'Y' || mode== 'Z' || mode=='Q')
    {
        toggle = mode;
    }

    //sélection du bon mode
    if(toggle == 'Y') //bluetooth
    {
      Bluetoothcontrol();
    }
    if(toggle=='Z') //autonome
    {
      bluetoothObstacle();
    }

    if(toggle=='X') //conduite assistée
    {
        Obstacle();
    }
    if(toggle == "Q") 
    {
        Stop();
    }

    
}


//Fonction gérant le mode auto
void Obstacle() 
{
    //lecture de la distance 
   int distance=ultrasonic(); 
   delay(20);

   //si l'obstacle est à plus de 12cm
   if(distance>12)
   {
      forward();
   }
   else  //si l'obstacle est à moins de 12cm
   {
      //la voiture s'arrete et recule légèrement
      Stop();
      backward();
      delay(200);
      Stop();

      //regarder à gauche et stocker la valeur de la distance de l'éventuelle obstacle
      distGauche = checkLeft();
      servo.write(spoint);
      delay(800);

      //regarder à droite et stocker la valeur de la distance de l'éventuelle obstacle
      distDroite = checkRight();
      servo.write(spoint);

      //si l'obstacle droit est plus loin que l'obstacle gauche => aller à droite
      if (distGauche < distDroite) {
        right();
        delay(500);
        Stop();
        delay(200);
       }  
       else { //sinon aller à gauche
        left();
        delay(500);
        Stop();
        delay(200);
      }
    } 
}

     
//Fonction gérant le mode pilotage par Bluetooth
void Bluetoothcontrol() 
{
   if (mode == 'F') //avancer
   {
      forward();
      delay(300);
      mode='S';
      
    } 
    else if (mode == 'B') //reculer
    {
      backward();
      delay(300);
      mode='S';
    } 
    else if (mode == 'L') //aller à gauche
    {
      left();
      delay(300);
      mode='S'; 
    } 
    else if (mode == 'R')  //aller à droite
    {
      right();
      delay(300);
      mode='S';
    } 
    else if (mode == 'S')  //s'arreter
    {
      Stop();
    }
}

//Fonction gérant le mode conduite assistée
void bluetoothObstacle()
{

  if (mode == 'F') //avancer
  {
      //lecture de la distance au prochain obstacle
      int distance=ultrasonic(); 
      delay(20);

      //si l'obstacle est à plus de 12cm => avancer
      if(distance >=12)
      {
        forward();
        delay(300);
        mode='S';
      }
      else //sinon la voiture recule légèrement et s'arrete 
      {
        backward();
        delay(300);
        mode = 'S';
      }
    
 } 
  else if (mode == 'B') //reculer
  { 
    backward();
    delay(300);
    mode='S';
  } 
  else if (mode == 'L') //aller à gauche
  {
    
    distGauche = checkLeft();   //distance à l'obstacle à gauche
    servo.write(spoint);
    mode=='S';
    delay(800);

    if(distGauche>=12) //si l'obstacle est à + de 12cm
    {
      left(); //aller à gauche
      delay(300);
      mode='S';
    }
}
  else if (mode == 'R') //aller à droite
  {
     distDroite = checkRight(); //distance à l'obstacle à droite
     servo.write(spoint);
     mode=='S';
     delay(800);
 
    if(distDroite >=12) //si l'obstacle est à + de 12cm
    {
      right(); //aller à droite
      delay(300);
      mode='S';
    }
  } 
  else if (mode == 'S')  //s'arreter
  {
    Stop();
  }
}
 

//Fonction pour la lecture de la distance
int ultrasonic() 
{
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  long t = pulseIn(Echo, HIGH);
  long cm = t / 29 / 2; // convertir le temps en distance
  return cm;
}

//Fonction pour tourner l'hélice du servo à gauche et lire la valeur de la distance
int checkLeft() {
  servo.write(180);
  delay(800);
  distGauche = ultrasonic();
  return distGauche;
}

//Fonction pour tourner l'hélice du servo à droite et lire la valeur de la distance
int checkRight() {
  servo.write(20);
  delay(800);
  distDroite = ultrasonic();
  servo.write(20);;
  return distDroite;
}

//aller tout droit
void forward() {
  M2.run(FORWARD);
  M4.run(FORWARD);
}
//reculer
void backward() {
  M2.run(BACKWARD);
  M4.run(BACKWARD);
}
//aller à gauche
void left() {
    M4.run(RELEASE);
    M2.run(FORWARD);
    mode='S';
}
//aller à droite
void right() {
  M2.run(RELEASE);
  M4.run(FORWARD);
  mode='S';
}
//s'arreter
void Stop() {
  M2.run(RELEASE);
  M4.run(RELEASE);
}
