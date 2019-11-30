#include <Arduino.h>
//#include<stdlib.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include "HX711.h"
//#include "max6675.h"

#define RX 5
#define TX 6
#define DOUT 3
#define CLK 2
#define KTCDO 8
#define KTCCS 9
#define KTCCLK 10
#define LED 15
#define PYRO 16
#define BUZZER 17

///////////////////   --------------   SETTING !!!   -----------------    ///////////////////////////

#define CALIB_FACTOR -48870 // <----------- tu musisz wpisac wspolczynnik kalibracji

#define TIME_FOR_RUN 5000 // czas na ucieczke liczony w [ms]

#define FIRE 15000 // czas przez ktorey bedzie sie palic grzala w [ms]

//////////////////------------------------------------------------------------/////////////////////////////

SoftwareSerial mySerial(RX, TX); //RX, TX
MAX6675 ktc(KTCCLK, KTCCS, KTCDO);
HX711 scale(DOUT, CLK);
File plik;

int idx = 0;
int temp_time = 0;
float thrust = 0;
int temperature = 0;
int time = 0;
bool acces = false;
String input = "";

void logger(float, int, int);

void setup()
{

    pinMode(LED, OUTPUT);   // definicja leda
    digitalWrite(LED, LOW); // led wylaczony

    pinMode(PYRO, OUTPUT);   //definicja zapalnika
    digitalWrite(PYRO, LOW); //zapalnik wylaczony

    pinMode(BUZZER, OUTPUT); //definicja buzzera

    scale.set_scale(CALIB_FACTOR); // ustawienie wspolczynnika kalibracji
    scale.tare();                  //taruj wage

    Serial.begin(9600);   //uruchom UART o prędkości 9600 baud
    mySerial.begin(9600); //uruchom UART dla HC-12

    if (!SD.begin(4)) //sprawdź czy nie ma karty na pinie ChipSelect 4
    {
        Serial.println("Nie wykryto karty(ERR)"); //błąd wykrycia karty
        mySerial.println("Nie wykryto karty(ERR)");
        while (1)
            ; // zatrzymaj program
    }
    Serial.println("Karta Wykryta (3/3))"); //Karta wykryta

    mySerial.println("Karta Wykryta  (3/3))");

    digitalWrite(LED, LOW); //Zaświeć ledem
    tone(17, 2000, 500);    // wlacz buzzer na 0.5s

    if (SD.exists("readouts.txt")) //sprawdź czy istnieje plik o nazwie readouts.txt
    {
        plik = SD.open("readouts.txt", O_CREAT | O_WRITE);
        Serial.println("Plik o podanej nazwie istnieje !");
        mySerial.println("Plik o podanej nazwie istnieje !");
    }

    else //jeżeli nie istnieje to
    {
        plik = SD.open("readouts.txt", O_CREAT | O_WRITE); //utwórz plik
        Serial.println("Utworzono plik o nazwie readouts.txt");
        mySerial.println("Utworzono plik o nazwie readouts.txt");
    }
    //delay(3000);
    Serial.println("Jestem gotowy");
    mySerial.println("melduje gotowosc");
    delay(25);
    while (!acces)
    {
        if (mySerial.available() > 0) //Odczytaj z HC-12
        {
            input = mySerial.readString();
            if (input[0] == 52 && input[1] == 50 && input[2] == 48)
            {
                acces = true;
                mySerial.println("ODPALAMY");
                Serial.println("ODPALAMY");
            }
            else
            {
                mySerial.println("odmowa dostepu");
                Serial.println("odmowa dostepu");
            }
        }
    }
    delay(1000);
    tone(17, 2000, 500);
    delay(1000);
    tone(17, 2000, 500);
    delay(1000);
    tone(17, 2000, 500);
    delay(1000);

    temp_time = millis();     //zapisz aktualny czas
    digitalWrite(PYRO, HIGH); //wlacz grzalke
}

void loop()
{

    if ((millis() - temp_time) > FIRE) //
    {
        digitalWrite(PYRO, LOW); //
    }

    thrust = scale.get_units();
    // temperature = ktc.readCelsius();
    time = millis() - temp_time;

    //  Serial.print("thrust: ");
    //  Serial.print(thrust);
    //  Serial.println(" N");
    //  Serial.println();
    //  Serial.print("temp: ");
    //  Serial.print(temperature);
    //  Serial.println(" C");
    //  Serial.println();
    //  Serial.print("time: ");
    //  Serial.print(time);
    //  Serial.println(" ms");
    //  Serial.println();
    //  Serial.println();

    logger(thrust, temperature, time); //zapisz na karte

    if (mySerial.available() > 0) //Odczytaj z HC-12
    {
        input = mySerial.readString();
        if (input[0] == 52 && input[1] == 50 && input[2] == 48)
        {
            plik.close();
        }
    }

    idx++
}

void logger(float thrust, int temperature, int time)
{
    char data[64];
    char buffer[16];
    dtostrf(thrust, 5, 2, buffer);

    //  { \"thrust\" : %s, \"temp\" : %d, \"time\" : %d },

    sprintf(dataSD, ",{ \"thrust\" : %s, \"temp\" : %d, \"time\" : %d }", buffer, temperature, time);

    sprintf(data, "{ \"thrust\" : %s, \"temp\" : %d, \"time\" : %d }", buffer, temperature, time);
    //otwórz plik readouts.txt
    plik.println(dataSD); //zapisz dane
    //plik.close();                                            //zamknij/zapisz plik
    if (idx == 10)
    {
        mySerial.println("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); //wyslij dane HC-12
    }
    else
    {
    }

    delay(50);
    //Serial.println("Zapisano !");                            //poinformuj o zapisaniu pliku
}