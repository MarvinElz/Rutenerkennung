// das ist evtl. auf anderen System nicht nodwendig, 
// wo NULL oder bool schon bekannt sind
#ifndef NULL
   #include <stddef.h>
#endif
#ifndef bool
   #include <stdio.h>
#endif
 
typedef void(*Verhalten)(void*);
typedef bool(*Bedingung)(void*);
 
typedef struct {
    Verhalten on_enter;
    Verhalten on_stay;
    Verhalten on_exit;
    // Kann auf false gesetzt werden, falls on_enter übersprungen werden soll
    bool erste_Ausfuehrung;
 
    // optionaler Zeiger auf Speicherbereich (struct) der weitere 
    // für den Zustand relevante Daten enthält, damit nicht alles
    // über Globale Variablen laufen muss.
    void* Zustandsdaten;    		           
}Zustand;
 
typedef struct {
    Zustand *from;
    Bedingung check;
    Zustand *to;
}Transition;
 
 
Zustand * run_Automat(Zustand *aktueller_Zustand, const Transition *transitions, int length){       
   // On_Enter mindestens einmal aufrufen
   if( aktueller_Zustand->erste_Ausfuehrung ){
      if( aktueller_Zustand->on_enter != NULL ){
         aktueller_Zustand->on_enter(aktueller_Zustand->Zustandsdaten);
         aktueller_Zustand->erste_Ausfuehrung = false;
         return aktueller_Zustand;     
      }
   }  
 
   for( int i = 0; i < length; i++ ){
      // Transitionen nach dem aktuellen Zustand durchsuchen
      if( transitions[i].from != aktueller_Zustand )
         continue;
 
      // wenn eine Transition schaltet
      if( transitions[i].check(aktueller_Zustand->Zustandsdaten) ){
         // On_Exit ausführen
         if( aktueller_Zustand->on_exit != NULL ){
            aktueller_Zustand->on_exit(aktueller_Zustand->Zustandsdaten);
         }
         // die Variable, die das erstmalige Ausführen des Zustandes
         // enthält, zurücksetzen
         aktueller_Zustand->erste_Ausfuehrung = true;
         // Aktuellen Zustand aktualisieren auf 
         // den nach der Transition folgenden
         aktueller_Zustand = transitions[i].to;            
         return aktueller_Zustand;
      }
   }
   // Falls keine der Transitionen geschaltet hat
   if( aktueller_Zustand->on_stay != NULL )
      aktueller_Zustand->on_stay(aktueller_Zustand->Zustandsdaten);
 
   return aktueller_Zustand;
}