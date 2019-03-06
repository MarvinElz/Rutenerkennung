// Code zum Testen der Kommunikation zwischen Arduino Mega und Raspberry Pi.
// Der Atmega erwartet einen Befehl in G-Code-Form
// Nach Erhalten des Befehls, wird "Working\n" ausgegeben, eine Sekunde gewartet und dann
// "Done\n" ausgegeben

String inputString = "";
bool stringComplete = true;

void setup()
{
  Serial.begin(115200);    
  //inputString.reserve(200);
}

void loop()
{
  //if (stringComplete) {
    // clear the string:
    //inputString = "";
    //stringComplete = false;
    //Serial.print("Working\n");
    delay(300);
    Serial.print("Done\n");
  //}
  //delay(1);
}

/*
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
*/
