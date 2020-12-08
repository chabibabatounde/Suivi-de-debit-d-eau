#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial gprsSerial(7,8);

int numbersArraySize = 5;
//String numeros[]={"+33782662557","+33605775142"};
String numeros[]={"+33624248860","33621340531","33619355335","33629460505","33605775142"};

String identifiant = "conteneur 1";
int limiteSuperieur = 40000;
int limiteInferieur = 2000;
//int limiteInferieur = 100;
int green = 13;
int red = 12;
int blue = 11;
int defaut = 1;
int sent =  0;
int debit = 0;

volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
unsigned char flowsensor = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;

void flow () // Interrupt function
{
   flow_frequency++;
}

void alertMessage(int debit){
  //Passer un appel
  //gprsSerial.println("ATD+33605775142;");
  //
  for (int i =0; i< numbersArraySize; i++){
    String numero = numeros[i];
    
    gprsSerial.print("AT+CMGF=1\r");
    delay(100);
    gprsSerial.println("AT+CMGS = \""+numero+"\"");
    delay(100);
    gprsSerial.println("Alerte "+identifiant+". Debit = "+String(debit)+" L/h");
    delay(100);
    gprsSerial.print((char)26);

    digitalWrite(blue, LOW);
   
    Serial.println("Alerte envoyee a "+numero);
    //gprsSerial.flush ();
    delay(5000);
  }
}


void okMessage(int debit){
  for (int i =0; i< numbersArraySize; i++){
    String numero = numeros[i];
    
    gprsSerial.print("AT+CMGF=1\r");
    delay(100);
    gprsSerial.println("AT+CMGS = \""+numero+"\"");
    delay(100);
    gprsSerial.println("Retour a la normale "+identifiant+". Debit = "+String(debit)+" L/h");
    delay(100);
    gprsSerial.print((char)26);

    digitalWrite(blue, LOW);
   
    Serial.println("Retour normal a "+numero);
    //gprsSerial.flush ();
    delay(5000);
  }
}

void setup()
{
  pinMode(green, OUTPUT);
  digitalWrite(green, LOW);
  pinMode(red, OUTPUT);
  digitalWrite(red, HIGH);
  pinMode(blue, OUTPUT);
  digitalWrite(blue, LOW);
  
  gprsSerial.begin(19200); // GPRS shield baud rate
  Serial.begin(19200);  

  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
  attachInterrupt(0, flow, RISING); // Setup Interrupt
  sei(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;
  defaut = 0;
  delay(500);  
}

void loop()
{
  //Mesure du débit
  debit = 0;
  //================================================================================================================================
  cloopTime = currentTime; // Updates cloopTime
  // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
  debit = (flow_frequency *60 / 0.2); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
  flow_frequency = 0; // Reset Counter
  Serial.println("Débit : "+String(debit));
  //================================================================================================================================
 
  //On vérifie s'il y a situation d'urgence
  //S'il y a situation d'urgence
  if(debit < limiteInferieur){
    //On verifie si le message a été déja envoyé.
    //Si oui, on ne fait rien, si non on l'envoi
    if(sent==0){
      alertMessage(debit);
      sent = 1;
    }
  }
  //S'il n'y a pas situation d'urgence
  else{
    //On verifie si c'est un retour à la normale
    if(sent==1){
      okMessage(debit);
      sent = 0;
    }
  }
  delay(1000);
}
