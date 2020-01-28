// include the library code:
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>
 
#define SERIAL_PORT_LOG_ENABLE 1
#define Led     13       // 13 pour la led jaune sur la carte
#define BP1     30       // 30 BP1
#define BP2     31       // 31 BP2           
#define BP3     32       // 32 BP3
#define LEDV    33       // 33 led
#define LEDJ    34       // 34 led
#define LEDR    35       // 35 led
#define relay   36       // 36 relay
#define PWM10    10      //11   timer2    

LiquidCrystal lcd(27, 28, 25, 24, 23, 22); // RS=12, Enable=11, D4=5, D5=4, D6= 3, D7=2, BPpoussoir=26
// Configuration des variables

unsigned   int UmoteurF = 0;  // variable to store the value coming from the sensor
unsigned   int Umoteur = 0;
unsigned   int Umoteur2 = 0;
unsigned   int Umoteur3 = 0;
unsigned   int Umoteur4 = 0;

unsigned   int ImoteurF = 0;  
unsigned   int Imoteur = 0;
unsigned   int Imoteur2 = 0;
unsigned   int Imoteur3 = 0;
unsigned   int Imoteur4 = 0;
           byte Rcy=0 ;    //rapport cyclique  8bit
           byte consigne=0 ;
unsigned    int temps=0;
unsigned    int timerepos=0;
unsigned    int timesecurite=0;
           byte flagarret=0;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(Led, OUTPUT);   //led carte arduino
  pinMode(LEDV, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDJ, OUTPUT);
  pinMode (PWM10,OUTPUT);     // broche (10) en sortie  timer2

  
//  digitalWrite(LEDV,LOW);
  Timer1.initialize(10000);         // initialize timer1, and set a 0,1 second period =>  100 000  pour 0.01s  10 000
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt
  lcd.begin(20, 4);  
  Serial1.begin(9600); 

  TCCR2B = (TCCR2B & 0b11111000) | 0x01;         //pin 10  32khz    http://playground.arduino.cc/Main/TimerPWMCheatsheet
                                                  //http://www.pobot.org/Modifier-la-frequence-d-un-PWM.html
  //   analogWriteResolution(bits)      https://www.arduino.cc/en/Reference/AnalogWriteResolution

lcd.setCursor(0,1);
lcd.print("Rcy");

lcd.setCursor(10,1);
lcd.print("Um");

lcd.setCursor(5,1);
lcd.print("Im");


lcd.setCursor(20,1); //afficheur 4lignes*20 caracteres   
lcd.print("BP1+");

lcd.setCursor(25,1);
lcd.print("BP2-");

lcd.setCursor(29,1);
lcd.print("BP3=0");
}

// Interruptions  tous les 0.01s
void callback()  {
temps++;
timerepos++;
//toogle state ledv for check 
  if ( digitalRead(LEDV)== 1 ) {digitalWrite(LEDV,LOW);}
      else {digitalWrite(LEDV,HIGH);}
    
Umoteur=analogRead(A0);
Imoteur=analogRead(A1);

Imoteur2=Imoteur;
Imoteur3=Imoteur2;
Imoteur4=Imoteur3;
ImoteurF=(Imoteur4+Imoteur3+Imoteur2+Imoteur)/4 ;


Umoteur2=Umoteur;
Umoteur3=Umoteur2;
Umoteur4=Umoteur3;
UmoteurF=(Umoteur4+Umoteur3+Umoteur2+Umoteur)/4 ;

if (Imoteur<400)                    // pas de limitation de courant à (2A*10)*20=400
  {if (consigne>Rcy)   {Rcy=Rcy+1;}   // rampe de de la Pwm +1*0.1seconde  integrateur pur
   if (consigne<Rcy &&  Rcy!=0)    {Rcy=Rcy-1;}     //la decrementation est faite seulement pour la poignée d'acceleration ou avec BP2
   if ( Rcy>254)  {Rcy=254;}           //limitation du rapport cyclique
  //  timesecurite=0;
   analogWrite(PWM10,Rcy);   //frequence 32kHz timer2}     
    }
if (Imoteur>400)  { Rcy=Rcy-5;                   //pas de filtrage du courant, pour etre plus rapide  //limitation de courant à (2A*10)*20=400
                    if ( Rcy<6)  {Rcy=5;}       //rcy n'est pas signé, ni la PWM   donc Rcy minimun ne doit pas etre inferieur à 6
                    timesecurite++;                      
                  analogWrite(PWM10,Rcy);   //frequence 32kHz timer2}      
                  }
               
}//fin routine


// Boucle correspondant à la fonction main 
void loop() {  

  // BP + LED
  if ((digitalRead(BP1))==1) {
    digitalWrite(LEDR, LOW);    
    digitalWrite(LEDJ, LOW); 
     if ( consigne<254)  {    consigne++;  }    //incrementation consigne
    delay(10);               //8bits*10ms=2.5S   pente de l'incremetation 25ssecondes
    }  
    
  if ((digitalRead(BP2))==1) {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDJ, HIGH); 
     if ( consigne!=0)  {consigne--;}     //decrementation consigne
     delay(10); 
                          } 

  
  
  if ((digitalRead(BP3))==1) {
    lcd.setCursor(20,0);
    lcd.print("BP3");     //appuie sur le frein
    consigne=0;
    Rcy=0;               //arrent en roue libre
    }

    if (timesecurite>=200 ) {flagarret=1;       // si courant de limitation pour un courant de plus de 2s
                              timerepos=0;
                              consigne=0;
                              Rcy=0;   
                              timesecurite=0;}       //   alors arret moteur pendant un temps d'arret 
                                            
    if (timerepos>=600 &&  flagarret==1) {flagarret=0;
                                          lcd.setCursor(29,0);      // apres un temps de rrepos ici de 3s
                                           lcd.print("       ");   }                                        


if (temps>=40)  {               //afficahge mesure toutes les 0.4s
lcd.setCursor(0,0);
lcd.print("                "); //effacement ligne donnée

lcd.setCursor(0,0);     
lcd.print(Rcy);

lcd.setCursor(5,0);
ImoteurF=(ImoteurF)/20;     //resistance (5/1024)*(10/0.25ohm) si ACS712 66mV/A
                           //pour resistance 1ohm (ImoteurF)/20;   simulation 5/25
lcd.print(ImoteurF);


lcd.setCursor(10,0);
UmoteurF=UmoteurF*10/38 ;                            //10/38   10/30 simula
if (Umoteur>ImoteurF){UmoteurF=UmoteurF-ImoteurF;  }  //U-R*I
lcd.print(UmoteurF);

lcd.setCursor(20,0);      //colonne, ligne
lcd.print("cons=");
lcd.setCursor(25,0);
lcd.print("   ");
lcd.setCursor(25,0);
lcd.print(consigne);

  if (flagarret==1 ) {lcd.setCursor(29,0);         // si courant de limitation pour un courant de plus de 2s
                     lcd.print("securit");  }          //   alors arret moteur pendant un temps d'arret et affichage    
                      



temps=0;
}//fin if temps
   
} // fin loop  
