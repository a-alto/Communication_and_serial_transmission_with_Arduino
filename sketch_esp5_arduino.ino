/*
 * ANDREA ALTOMARE 5^A SISTEMI E RETI - ESPERIENZA DI LABORATORIO 5
 * Obiettivo: Trasmettere una stringa fra due PC utilizzando una linea con trasmissione seriale fra due Arduino (i quali saranno collegati ai rispettivi PC).
 * Il sistema è stato realizzato con un architettura a quattro dispositivi: i due pc connessi ai rispettivi Arduino (tramite cavo USB) e proprio i due Arduino, connessi insieme
 * con tre fili. In ogni Arduino, ci sarà un filo messo nel pin predisposto all'invio dei dati all'altro Arduino, il quale li riceverà sul proprio pin predisposto alla ricezione dei dati,
 * grazie allo stesso filo, il quale però per questo sarà considerato il filo dei dati in ricezione; ci sarà inoltre un filo comune ad entrambe le schede Arduino collegato ai pin per
 * realizzare la Ground (GND): la messa a terra.
 * Per inviare una frase, l'utente dovrà aprire il monitor seriale del proprio Arduino, scrivere la frase e premere invio: a quel punto, tramite il cavo USB (seriale), la frase verrà ricevuta
 * dalla scheda Arduino, la quale, "captanto" del testo arrivato, provvederà ad inviarlo all'altra scheda Arduino alla quale è collegata. Specularmente, la scheda Arduino che riceve dei dati
 * dall'altro Arduino, dovrà provvedere a reinviarli sulla seriale USB per mostrarli al proprio utente sul rispettivo monitor seriale.
 * In questo modo la comunicazione fra i due utenti utilizzatori delle schede Arduino è garantita.
*/

#include <Serial.h> // inclusione della libreria per la comunicazione seriale
#include <SoftwareSerial.h> // inclusione della libreria per la comunicazione seriale (ovvero fra le due schede Arduino)
#define rxPin 2 // pin predisposto alla ricezione dei dati (Rx)
#define txPin 3 // pin predisposto alla trasmissione dei dati (Tx)
#define ms 300 // definizione dei millisecondi per i delay

int i; // indice
String message=""; // messaggio scambiato fra gli utenti tramite Arduino
char character; // variabile d'appoggio per un carattere
SoftwareSerial swSerial=SoftwareSerial(rxPin,txPin); // istanziazione e inizializzazione della seriale software comunicando i pin da utilizzare in ricezione ed invio dei dati

void setup() {
  // put your setup code here, to run once:
  pinMode(rxPin,INPUT); // Rx è in input
  pinMode(txPin,OUTPUT); // Tx è in output
  Serial.begin(9600); // il baud rate per la comunicazione seriale è di 9600 (baud)
  swSerial.begin(9600); // il baud rate per la comunicazione seriale software è di 9600 (baud)
}

void loop() {
  // put your main code here, to run repeatedly:
  serialReceive(); // prima compie le operazioni relative alla ricezione e l'invio dei messaggi da parte del proprio utente (ovvero dal PC al quale è connesso)
  swSerialReceive(); // poi compie le operazioni nel caso in cui siano stati ricevuti dati da parte dell'altro Arduino al quale è collegato
}

// funzione per determinare se è arrivato qualcosa sulla seriale (dunque se l'utente sta cercando di trasmettere dei dati)
void serialReceive()
{
  // se c'è qualcosa nel buffer della seriale, vuol dire che è stato ricevuto qualcosa da parte del PC al quale si è connessi
  if(Serial.available()>0)
  {
    delay(ms); // inserisco un delay prima di iniziare a comporre il messaggio con i caratteri ricevuti poiché bisogna dare il tempo alla scheda di aver ricevuto tutti i caratteri
    character=0; // pulisco la variabile per il carattere

    // ciclo per la lettura dei caratteri dal buffer della seriale e la formazione del messaggio
    do
    {
      character=Serial.read(); // leggo carattere per carattere ciò che ho ricevuto sulla seriale e lo metto sulla variabile char apposita (d'appoggio)
      message=String(message+character); // formo la stringa "message" grazie alla concatenazione di ciò che già è presente in "message" con il carattere appena letto dal buffer della seriale
    }while(Serial.available()>0); // ripeto le operazioni di lettura dal buffer finché ci sono dati da leggere
    Serial.flush(); // pulisco il buffer della seriale una volta terminata la lettura
    
    swSerialTransmit(); // dato che il messaggio è stato composto, bisogna successivamente trasmetterlo sulla seriale software per mandarlo all'altro Arduino
  }
}

// funzione per determinare se è arrivato qualcosa sull'rxPin (pin di ricezione della SoftwareSerial), dunque se si deve procedere a mostrare un messaggio ricevuto da un altro Arduino
void swSerialReceive()
{
  // se c'è qualcosa nel buffer della seriale software, vuol dire che è stato ricevuto qualcosa da parte dell'altro Arduino
  if(swSerial.available()>0)
  {
    delay(ms); // inserisco un delay prima di iniziare a comporre il messaggio con i caratteri ricevuti poiché bisogna dare il tempo alla scheda di aver ricevuto tutti i caratteri
    character=0; // pulisco la variabile per il carattere

    // ciclo per la lettura dei caratteri dal buffer della seriale software e la formazione del messaggio
    do
    {
      character=swSerial.read(); // leggo carattere per carattere ciò che ho ricevuto sulla seriale software e lo metto sulla variabile char apposita (d'appoggio)
      message=String(message+character); // formo la stringa "message" grazie alla concatenazione di ciò che già è presente in "message" con il carattere appena letto dal buffer della seriale software
    }while(swSerial.available()>0); // ripeto le operazioni di lettura dal buffer finché ci sono dati da leggere
    swSerial.flush(); // pulisco il buffer della SoftwareSerial una volta terminata la lettura
    
    serialTransmit(); // dato che il messaggio è stato composto, bisogna successivamente trasmetterlo sulla seriale per mostrarlo sul monitor seriale del pc al quale si è connessi
  }
}

// funzioine richiamata se si è ricevuto un messaggio da un altro Arduino (ricezione su SoftwareSerial) e dunque bisogna inviarlo al computer per mostrarlo a video
void serialTransmit()
{
  Serial.println("RICEVUTO: " + message); // stampo il messaggio che ho ricevuto sul monitor seriale
  message=""; // pulisco la stringa del messaggio dato che l'ho trasmesso
}

// fuinzione richiamata se si è ricevuto un messaggio dalla seriale, dunque si procede alla trasmissione del messaggio (con SoftwareSerial) ad un altro Arduino
void swSerialTransmit()
{
  // eseguo un ciclo per inviare un carattere alla volta all'altro Arduino, tramite la seriale software
  for(i=0;i<message.length();i++)
  {
    swSerial.print(message.charAt(i)); // "stampa" (invio) della lettera i-esima dell messaggio
  }
  //swSerial.println(); // invio un carattere per andare a "linea nuova"
  
  Serial.println("INVIATO: " + message); // stampo una stringa di "riuscita operazione", col messaggio appena inviato, sul mio monitor seriale in modo da essere sicuro che le operazioni effettuate siano andate a buon fine
  message=""; // pulisco la stringa del messaggio dato che l'ho trasmesso
}
