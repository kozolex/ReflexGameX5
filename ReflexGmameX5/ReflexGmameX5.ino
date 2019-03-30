#include <LiquidCrystal_I2C.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h> //https://github.com/junwha0511/LiquidCrystal_I2C_Hangul
LiquidCrystal_I2C lcd(0x27, 20, 4);

int Lampka = 0, LampkaPoprzednia = 20;
float koniecCzas;
int sredniCzas;
int szybkiCzas = 10000, wolnyCzas;
int liczbaRund = 50;
unsigned long  koniecCzasGry, poczatekCzasGry, poczatekCzasPunktu, koniecCzasPunktu, rekordCzas =5000 ;


bool decyzja = false;
short int i, point = 0;
short int Pozwolenie = 0;

void point_count() { // Funkcja dodaj�ca punkty

  decyzja = true;
}
void powtarzanie() {  // Funkcja zapobiegaj�ca powtarzaniu si� tych samych diod
  while (LampkaPoprzednia == Lampka) {
    if (Lampka == 4) {
      Lampka = 0;
    }
    else {
      Lampka++;
    }
  }
}

void setup() {    //   Inicjacja.
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print( "- - WITAJ GRACZU - -");
  randomSeed(analogRead(A0));
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), point_count, RISING);
}

void start_routine() { // Funkcja startuj�ca program. Wykonuj� si� tu �wiecenia 3 razy wszystkie diody.
  int szybkosc = 100;
  for (i = 3; i > 0; i--)
  {
    for (int j = 3; j < 8; j++) {
      digitalWrite(j, HIGH);
      delay(szybkosc);
    }
    for (int j = 3; j < 8; j++) {
      digitalWrite(j, LOW);
      delay(szybkosc);
    }
    lcd.setCursor(0, 1);
    lcd.print("- - - - - - - - - -");
    lcd.setCursor(9, 1);
    lcd.print(i);
    delay(500);
    for (int j = 3; j < 8; j++) {
      digitalWrite(j, LOW);
    }
    delay(500);
  }
  for (int j = 3; j < 8; j++) {
    digitalWrite(j, HIGH);
  }
  delay(500);
  for (int j = 3; j < 8; j++) {
    digitalWrite(j, LOW);
  }
  delay(500);
  Serial.println(" - - - - - GO - - - - - - ");
}

void loop() {   // Funkcja g��wna
  decyzja = false;
  String linia1;
  lcd.init();
  point = 0;
  start_routine();       //Funkcja start
  lcd.setCursor(0, 1);
  lcd.print( "                    ");
  lcd.setCursor(0, 0);
  lcd.print( "- - - - GO - - - - -");
  poczatekCzasGry = millis();           //***************Poczatek gry
  for (i = 0; i < liczbaRund; i++) {     //P�tla w kt�rej nast�puje losowanie przycisk�w
    Lampka = random(5);
    powtarzanie();
    Pozwolenie = point;
    poczatekCzasPunktu = millis();     //***************poczatek czasu zliczania refleksu
    digitalWrite(Lampka + 3, HIGH);
    lcd.setCursor(0, 2);
    if (liczbaRund - point <10) 
    linia1 = "     ->  " + String(liczbaRund - point) + " <-       ";
    else 
    linia1 = "     -> " + String(liczbaRund - point) + " <-       ";
    
    lcd.print(linia1);
    while (decyzja == false) {   // P�tla kt�ra stopuje program. Uczestnik musi wcisn�� przycisk, aby kolejna z dioda zapali�� si�.
      delay(0);
    }
    digitalWrite(Lampka + 3, LOW);
    decyzja = false;
    koniecCzasPunktu = millis();
    point = point + 1;
    delay(50);
    // Warunki sprawdzaj�ce czas reakcji. Sprawdzaj� one czas najszybszy i najwolniejszy.
    if ((koniecCzasPunktu - poczatekCzasPunktu) > wolnyCzas) { //Najwolniejszy czas
      wolnyCzas = koniecCzasPunktu - poczatekCzasPunktu;
    }
    if ((koniecCzasPunktu - poczatekCzasPunktu) < szybkiCzas) { // Najszybszy czas
      szybkiCzas = koniecCzasPunktu - poczatekCzasPunktu;
    }
    LampkaPoprzednia = Lampka;
  }
  koniecCzasGry = millis();
  koniecCzas = koniecCzasGry - poczatekCzasGry;
  sredniCzas = koniecCzas / liczbaRund;

  // Poni�ej wypisanie do portu szeregowego wynik�w.
  Serial.print(" Wynik = ");  Serial.println(point);  Serial.print( " Czas = "); Serial.print(koniecCzas / 1000) ; Serial.println( "sekund.");
  Serial.print(" Sredni czas reakcji = ");  Serial.println(sredniCzas);
  Serial.print(" Najszybsza reakcja = ");  Serial.println(szybkiCzas);
  Serial.print(" Najwolniejsza reakcja = ");  Serial.println(wolnyCzas);

  int k ;
  // Poni�ej wy�wietlenie wynik�w na LCD.
  int zwloka = 1000;
  
  lcd.init();
  //lcd.setCursor(0, 0);
  //lcd.print("Wynik:");
  //lcd.setCursor(6, 0);
  //lcd.print(point);
  delay(zwloka);
  lcd.setCursor(0, 0);
  linia1 = "  Czas: " + String(koniecCzas / 1000) + " s";
  lcd.print(linia1);
  //lcd.setCursor(15, 0);
  //lcd.print(koniecCzas / 1000);
  delay(zwloka);
  lcd.setCursor(0, 1);
  if(sredniCzas <1000){
    linia1 = "  Reflex : " + String(sredniCzas) + " ms";
  }
  else{
    linia1 = "  Reflex: " + String(sredniCzas) + " ms";
  }
  lcd.print(linia1);
  //lcd.setCursor(8, 1);
  //lcd.print(sredniCzas);
  delay(zwloka);
  if (rekordCzas > sredniCzas) rekordCzas=sredniCzas;
    lcd.setCursor(0, 2);
  if(sredniCzas <1000){
    linia1 = "  Rekord : " + String(rekordCzas) + " ms";
  }
  else{
    linia1 = "  Rekord: " + String(rekordCzas) + " ms";
  }
  lcd.print(linia1);
  /*lcd.setCursor(0, 2);
    lcd.print("Refleks max:   ");
    lcd.setCursor(12, 2);
    lcd.print(szybkiCzas);
    lcd.setCursor(15,2);
    lcd.print("ms");
    delay(zwloka);
  */

  delay(zwloka);
  while (decyzja == false) {   // P�tla kt�ra stopuje program. Uczestnik musi wcisn�� przycisk, aby kolejna z dioda zapali�� si�.
    digitalWrite(5, HIGH);
    lcd.setCursor(0, 3);
    lcd.print("NACISNIJ    PRZYCISK");
    delay(100);
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    digitalWrite(5, LOW);
    delay(100);
  }

}
