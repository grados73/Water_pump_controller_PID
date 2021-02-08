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
 #define CZUJNIK_1_PIN 2
 #define CZUJNIK_2_PIN 3
 #define CZUJNIK_3_PIN 4
 #define CZUJNIK_4_PIN 5
 #define CZUJNIK_5_PIN 7
 #define CZUJNIK_6_PIN 8

 //
 // NUMERY PINOW ARDUINO DO OBSLUGI L293D
 //
  #define INPUT1PIN 11   // kierunek kanalu pierwszego
  #define INPUT2PIN 13   // kierunek kanalu pierwszego
  #define ENABLE1PIN 6  // PWM kanalu pierwszego, wybieramy pin 6, bo Fpwm dla pin 5 i 6 = 980Hz, dla reszty 490Hz

  #define PRZEPLYWOMIERZPIN 12 // PIN DO KTOREGO PODLACZONY JEST PRZEPLYWOMIERZ

// ZADANY POZIOM WODY, WARTOSC OD 0 DO 6
 #define ZADANYPOZIOM 3

// CZAS W MILISEKUNDACH CO JAKI MA BYC LICZONA WARTOSC REGULATORA PID I PRZEPLYWU
 #define CZASPOWTARZANIAPID 1000      // 1000ms = 1s
 #define CZASPOWTARZANIAPRZEP 2000    // 2000ms = 2s
 #define CZASPOWTARZANIAUARTADOTV 100 // 100ms = 0,1s => 10Hz
 #define CZASPOWTARZANIAZMIANYPARAMETROWPID 500 // 500ms = 0,5s

// ZMIENNE PRZECHOWUJACE NASTAWY REGULATORA PID
  float Kp = 0.7; //A5
  float Ki = 1;   //A4
  float Kd = 0.2; //A3

// ZMIENNE PRZECHOWUJACE WYNIKI OBLICZEN KONKRETNYCH NASTAW REGULATORA
  float Pout, Iout, Dout; 

// WARTOSCI BLEDU
  float Error = 0;          // WARTOSC BLEDU W AKTUALNEJ CHWILI CZASU
  float LastError = 0;      // WARTOSC BLEDU Z OSTATNIEGO POMIARU
  float ErrorSum = 0;       // WARTOSC BLEDU DLA CZESCI CALKUJACEJ
  float Derror = 0;         // WARTOSC WYKORZYSTYWANA DO REALIZACJI CZESCI ROZNICZKUJACEJ

// ZMIENNA PRZECHOWUJACA CZESTOTTLIWOSC SYGNALU Z PRZEPLYWOMIERZA
  float InputFrequency = 0;    // CZESTOTLIWOSC SYGNALU Z PRZEPLYWOMIERZA
  unsigned long HighTime = 0;  // CZAS TRWANIA IMPULSU WYSOKIEGO
  unsigned long LowTime = 0;   // CZAS TRWANIA IMPULSU NISKIEGO
  unsigned long ImpulseTimeDuration = 0 ;  // CZAS TRWANIA CALEGO IMPULSU Z PRZEPLYWOMIERZA
  unsigned int WartoscPrzeplywomierza = 0; // ILOSC ML/MIN
 

 int AktualnyPoziomWody = 0; // informacja z czujnikow
 int WartoscWypelnienia = 0; // WARTOSC PRZESYLANA DO MOSTKA H - PWM (0-255)

//
// ZMIENNE ZDEFINIOWANE DO OBSLUGI CZASU
//
 unsigned long AktualnyCzas = 0;
 unsigned long ZapamietanyCzasPID = 0;
 unsigned long ZapamietanyCzasPrzep = 0;
 unsigned long ZapamietanyCzasTV = 0;
 unsigned long ZapamietanyCzasZmianaPID = 0;

 //
 //TABLICA DO TELEMETRYVIEW
 //
  char text[24]; 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  //
  //DEFINICJA PINOW CZUJNIKOW POZIOMU CIECZY - PINY PODCIAGNIETE WEWNETRZNIE DO VCC - ZALACZENIE CZUJNIKA ZWIERA GO DO GND
  //CZUJNIK POLOZONY W TEN SPOSOB, ZE GDY JEST W WODZIE PLYWAK PODNOSI SIE DO GORY - ROZWARCIE- WEW. PULLUP - GDY POZIOM OPADA, PLYWAK OPADA - KONTRAKTON ZWIERA SYGNAL DO MASY
  //
  pinMode(CZUJNIK_1_PIN, INPUT_PULLUP);
  pinMode(CZUJNIK_2_PIN, INPUT_PULLUP);
  pinMode(CZUJNIK_3_PIN, INPUT_PULLUP);
  pinMode(CZUJNIK_4_PIN, INPUT_PULLUP);
  pinMode(CZUJNIK_5_PIN, INPUT_PULLUP);
  pinMode(CZUJNIK_6_PIN, INPUT_PULLUP);

  //
  // DEFINICJA PINOW DO OBSLUGI L293D
  //
  pinMode(INPUT1PIN, OUTPUT);
  pinMode(INPUT2PIN, OUTPUT);
  pinMode(ENABLE1PIN, OUTPUT);
  digitalWrite(INPUT1PIN, HIGH);
  digitalWrite(INPUT2PIN, LOW);
  analogWrite(ENABLE1PIN, WartoscWypelnienia);

  //
  // URUCHOMIENIE UARTA DO WYSYLANIA SYGNALOW BLEDOW
  //
  Serial.begin(9600);
  
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//PETLA GLOWNA PROGRAMU//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  
  AktualnyCzas = millis();
  if(AktualnyCzas - ZapamietanyCzasPID >= CZASPOWTARZANIAPID)
  {
    LiczPID();
    ZapamietanyCzasPID = AktualnyCzas;
  }
  
  if(AktualnyCzas - ZapamietanyCzasPrzep >= CZASPOWTARZANIAPRZEP)
  {
    WartoscPrzeplywomierza = LiczPrzeplyw();
    ZapamietanyCzasPrzep = AktualnyCzas;
  }
  
    if(AktualnyCzas - ZapamietanyCzasZmianaPID >= CZASPOWTARZANIAZMIANYPARAMETROWPID)
  {
    ZmienWspolczynnikiPID();
    ZapamietanyCzasZmianaPID = AktualnyCzas;
  }
  
    if(AktualnyCzas - ZapamietanyCzasTV >= CZASPOWTARZANIAUARTADOTV)
  {
    sprintf(text, "%d,%d,%d", AktualnyPoziomWody, WartoscPrzeplywomierza, WartoscWypelnienia);
    Serial.println(text);
    ZapamietanyCzasTV = AktualnyCzas;
  }

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FUNKCJE////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void LiczPID()
{
  Error = ZADANYPOZIOM - LiczAktualnyPoziomWody();                                                    // CZESC P
  ErrorSum = ErrorSum + (int)((float)(Error + LastError)*((float)CZASPOWTARZANIAPID/1000)*0.5);       // CZESC I
  Derror = ((float)(Error - LastError))/((float)CZASPOWTARZANIAPID/1000);                             // CZESC D
  Pout = Kp * Error;
  Iout = Ki * ErrorSum;
  Dout = Kd * Derror;

  if (Iout > 255) Iout = 255; // w celu zapobiegniecia zjawisku wind-up calkowania
  else if (Iout < 0) Iout = 0;
  
  WartoscWypelnienia = Pout + Iout + Dout;

  if(WartoscWypelnienia > 255) WartoscWypelnienia = 255; // wypelnienie jest wartoscia 8bit, z zakresu 0-255
  else if(WartoscWypelnienia < 0) WartoscWypelnienia = 0;

  LastError = Error;

  // ZMIANA WYPELNIENIA STERUJACEGO DZIALANIEM POMPY
  analogWrite(ENABLE1PIN, WartoscWypelnienia);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int LiczPrzeplyw()
{
  unsigned int Przeplyw = 0;
  HighTime = pulseIn(PRZEPLYWOMIERZPIN, HIGH, 100000); // CZAS TRWANIA STANU WYSOKIEGO W MIKROSEKUNDACH
  LowTime = pulseIn(PRZEPLYWOMIERZPIN, LOW, 100000);   // CZAS TRWANIA STANU NISKIEGO W MIKROSEKUNDACH
  if(HighTime == 0 or LowTime == 0)
  {
    BladUszkodzonegoPrzeplywomierza();
    Przeplyw = 0;
  }
  else
  {
    ImpulseTimeDuration = HighTime + LowTime;
    InputFrequency = (1.0/((float)ImpulseTimeDuration/1000000));
    Przeplyw = (unsigned int)(60* InputFrequency * 14); // Ilosc ml/min, w czujniku YF-S402 jeden impuls to okolo 14ml
  }
  return Przeplyw;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    AktualnyPoziomWody = BladUszkodzeniaCzujnika(); // NIESPELNIENIE ZADNEGO Z POWYZSZYCH WARUNKOW OZNACZA USZKODZENIE KTOREGOS Z CZUJNIKOW
  }

  return AktualnyPoziomWody;
  
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZmienWspolczynnikiPID()
{
    float Kp = analogRead(A5)/100.0;
    float Ki = analogRead(A4)/100.0;
    float Kd = analogRead(A3)/100.0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int BladUszkodzeniaCzujnika()
{
  return ZADANYPOZIOM; // WERSJA- GDY KTORYS Z CZUJNIKOW SIE POPSUL -> WYLACZAM POMPKE
  //return 6; // WERSJA- GDY KTORYS Z CZUJNIKOW SIE POPSUL -> POMPUJE CALY CZAS WODE Z MAKSYMALNA PREDKOSCIA
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BladUszkodzonegoPrzeplywomierza()
{
  
}
