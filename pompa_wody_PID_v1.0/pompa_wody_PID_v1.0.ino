/*
 * Athor: ***
 * Date: 04.02.2021
 * 
 * Układ czujnikow:
 * |                                                (6p)|
 * |*czujnik1                                           |
 * |                                                (5p)|
 * |*czujnik2                                           |
 * |                                                (4p)|
 * |*czujnik3                                           |
 * |                                                    |
 * |~Zadany poziom wody~~~~~~~~~~~~~~~~~~~~~~~~~~~~~(3p)|
 * |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
 * |*czujnik4~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
 * |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~(2p)|
 * |*czujnik5~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
 * |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~(1p)|
 * |*czujnik6~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
 * |________________________________________________(0p)|
 * 
 * 
 */
 
 //
 // NUMERY PINOW ARDUINO DO KTORYCH PODLACZONE SA KOLEJNE CZUJNIKI
 //
 #define CZUJNIK_1_PIN 1
 #define CZUJNIK_2_PIN 2
 #define CZUJNIK_3_PIN 3
 #define CZUJNIK_4_PIN 4
 #define CZUJNIK_5_PIN 5
 #define CZUJNIK_6_PIN 6

// ZADANY POZIOM WODY, WARTOSC OD 0 DO 6
 #define ZADANYPOZIOM 3

// CZAS W MILISEKUNDACH CO JAKI MA BYC LICZONA WARTOSC REGULATORA PID
 #define CZASPOWTARZANIAPID 100

 

 int AktualnyPoziomWody = 0;

//
// ZMIENNE ZDEFINIOWANE DO OBSLUGI CZASU
//
 unsigned long AktualnyCzas = 0;
 unsigned long ZapamietanyCzasPID = 0;

void setup() {
  //
  //DEFINICJA PINOW CZUJNIKOW POZIOMU CIECZY - PINY PODCIAGNIETE WEWNETRZNIE DO VCC - ZALACZENIE CZUJNIKA ZWIERA GO DO GND
  //CZUJNIK POLOZONY W TEN SPOSOB, ZE GDY JEST W WODZIE PLYWAK PODNOSI SIE DO GORY - ROZWARCIE - GDY POZIOM OPADA, PLYWAK OPADA - KONTRAKTON ZWIERA DO MASY
  //
  pinMode(CZUJNIK_1_PIN, INPUT_PULLUP);
  pinMode(CZUJNIK_2_PIN, INPUT_PULLUP);
  pinMode(CZUJNIK_3_PIN, INPUT_PULLUP);
  pinMode(CZUJNIK_4_PIN, INPUT_PULLUP);
  pinMode(CZUJNIK_5_PIN, INPUT_PULLUP);
  pinMode(CZUJNIK_6_PIN, INPUT_PULLUP);
  

}

void loop() {
  
  AktualnyCzas = millis();
  if(AktualnyCzas - ZapamietanyCzasPID >= CZASPOWTARZANIAPID)
  {
    LiczPID();
  }
  
}

int LiczPID()
{
  
}

int LiczAktualnyPoziomWody()
{
  //6poziom
  if(digitalRead(CZUJNIK_1_PIN) == digitalRead(CZUJNIK_2_PIN) == digitalRead(CZUJNIK_3_PIN) == digitalRead(CZUJNIK_4_PIN) == digitalRead(CZUJNIK_5_PIN) == digitalRead(CZUJNIK_6_PIN) == HIGH) // 6p
  {
    AktualnyPoziomWody = 6;
  }

  //5poziom
  else if((digitalRead(CZUJNIK_1_PIN) == LOW) and (digitalRead(CZUJNIK_2_PIN) == digitalRead(CZUJNIK_3_PIN) == digitalRead(CZUJNIK_4_PIN) == digitalRead(CZUJNIK_5_PIN) == digitalRead(CZUJNIK_6_PIN) == HIGH)) //5p
  {
    AktualnyPoziomWody = 5;
  }

  //4poziom
  else if((digitalRead(CZUJNIK_1_PIN) == digitalRead(CZUJNIK_2_PIN) ==  LOW) and (digitalRead(CZUJNIK_3_PIN) == digitalRead(CZUJNIK_4_PIN) == digitalRead(CZUJNIK_5_PIN) == digitalRead(CZUJNIK_6_PIN) == HIGH)) //4p
  {
    AktualnyPoziomWody = 4;
  }

  //3poziom
  else if((digitalRead(CZUJNIK_1_PIN) == digitalRead(CZUJNIK_2_PIN) == digitalRead(CZUJNIK_3_PIN) == LOW) and (digitalRead(CZUJNIK_4_PIN) == digitalRead(CZUJNIK_5_PIN) == digitalRead(CZUJNIK_6_PIN) == HIGH)) //3p
  {
    AktualnyPoziomWody = 3;
  }

  //2poziom
  else if((digitalRead(CZUJNIK_1_PIN) == digitalRead(CZUJNIK_2_PIN) == digitalRead(CZUJNIK_3_PIN) == digitalRead(CZUJNIK_4_PIN) == LOW) and (digitalRead(CZUJNIK_5_PIN) == digitalRead(CZUJNIK_6_PIN) == HIGH)) //2p
  {
    AktualnyPoziomWody = 2;
  }

  //1poziom
  else if((digitalRead(CZUJNIK_1_PIN) == digitalRead(CZUJNIK_2_PIN) == digitalRead(CZUJNIK_3_PIN) == digitalRead(CZUJNIK_4_PIN) == digitalRead(CZUJNIK_5_PIN) == LOW) and (digitalRead(CZUJNIK_6_PIN) == HIGH)) //1p
  {
    AktualnyPoziomWody = 1;
  }

  //0poziom
  else if(digitalRead(CZUJNIK_1_PIN) == digitalRead(CZUJNIK_2_PIN) == digitalRead(CZUJNIK_3_PIN) == digitalRead(CZUJNIK_4_PIN) == digitalRead(CZUJNIK_5_PIN) == digitalRead(CZUJNIK_6_PIN) == LOW) //0p
  {
    AktualnyPoziomWody = 0;
  }

  else
  {
    BladUszkodzeniaCzujnika(); // NIESPELNIENIE ZADNEGO Z POWYZSZYCH WARUNKOW OZNACZA USZKODZENIE KTOREGOS Z CZUJNIKOW
  }
  
}

void BladUszkodzeniaCzujnika()
{
  
}
