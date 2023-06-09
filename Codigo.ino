// Adicionando Library
#include "EmonLib.h"                 
#include <LiquidCrystal_I2C.h>
#include <avr/eeprom.h>
#include <IRremote.h> int IR_Recv = 11; 

 

//Declarando variavel a porta do Arduino
int ledVermelho = 7;
int ledAmarelo = 6;
int ledVerde = 5;
int ledVermelho2 = 4;
IRrecv irrecv(IR_Recv);
decode_results results;
#define I2C_ADDR   0x3F
#define BACKLIGHT_PIN   3
#define En 2
#define Rw 1
#define Rs 0
#define D4 4
#define D5 5
#define D6 6
#define D7 7 LiquidCrystal_I2C  lcd(I2C_ADDR,En,Rw,Rs,D4,D5,D6,D7);
#define eeprom_read_to(dst_p, eeprom_field, dst_size) eeprom_read_block(dst_p, (void *)offsetof(__eeprom_data, eeprom_field), MIN(dst_size, sizeof((__eeprom_data*)0)->eeprom_field))
#define eeprom_read(dst, eeprom_field) eeprom_read_to(&dst, eeprom_field, sizeof(dst))
#define eeprom_write_from(src_p, eeprom_field, src_size) eeprom_write_block(src_p, (void *)offsetof(__eeprom_data, eeprom_field), MIN(src_size, sizeof((__eeprom_data*)0)->eeprom_field))
#define eeprom_write(src, eeprom_field) { typeof(src) x = src; eeprom_write_from(&x, eeprom_field, sizeof(x)); }
#define MIN(x,y) ( x > y ? y : x ) 
struct __eeprom_data
{
    double flash_kwhtotal;
};
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2 

 

//Criando variaveis Globais      
EnergyMonitor emon1;      
double kwhTotal;
double vlreais;
unsigned long ltmillis, tmillis, timems, previousMillis;
char charBuf[30];

 

//Painel do Display
void setup()
{
    Serial.begin(9600); lcd.begin(16, 2);
    lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
    lcd.setBacklight(HIGH); lcd.setCursor(0, 0);
    lcd.print("Ap=");
    lcd.setCursor(0, 1);
    lcd.print("R$=");

    emon1.current(1, 1.0); 
    eeprom_read(kwhTotal, flash_kwhtotal);
    previousMillis = millis(); irrecv.enableIRIn();

 

   
    //Declarando pino digital como saída do IR
    pinMode(ledVermelho, OUTPUT); 
    pinMode(ledAmarelo, OUTPUT); 
    pinMode(ledVerde, OUTPUT); 
    pinMode(ledVermelho2, OUTPUT);
}
void loop()
{
    //Calcula o tempo desde a última medição de potência real.
    ltmillis = tmillis;
    tmillis = millis();
    timems = tmillis - ltmillis;
    double Irms = emon1.calcIrms(1480);  

    //Realiza o calculo do KwhTOtal e mostra no display LCD
    kwhTotal = kwhTotal + (((Irms * 127.0) / 1000.0) * 1.0 / 3600.0 * (timems / 1000.0));
    Serial.print("Watts: ");
    Serial.println(Irms * 127.0);      
    Serial.print("Current: ");
    Serial.println(Irms);           
    Serial.print("kwhTotal: ");
    printFloat(kwhTotal, 10);
    Serial.println("");
    //grava na memoria EEprom a cada 1 minuto
    if ((millis() - previousMillis) > 4000)
    {
        Serial.println("Gravando na EEprom");
        eeprom_write(kwhTotal, flash_kwhtotal);
        previousMillis = millis();
    }
    //convert double em string
    dtostrf(kwhTotal, 8, 7, charBuf);
    //Multiplica pelo valor kilowatt hora R$ 0.35 Reais
    vlreais = kwhTotal * 0.35;
    lcd.setCursor(4, 0);
    lcd.print(Irms);
    lcd.setCursor(4, 1);
    dtostrf(vlreais, 8, 7, charBuf);
    lcd.print(charBuf);
    if (irrecv.decode(&results))
    {
        long int decCode = results.value;
        Serial.println(results.value);
        //SWITCH CASE para usar o botão pressionado do controle remoto referente as bandeiras tarifaricas 
        switch (results.value)
        {
            case 16724175: 
                digitalWrite(ledVermelho, HIGH);
                break;
            case 16718055: 
                digitalWrite(ledVermelho, LOW);
                break;
            case 16743045: 
                digitalWrite(ledAmarelo, HIGH);
                break;
            case 16716015: 
                digitalWrite(ledAmarelo, LOW);
                break;
            case 16726215: 
                digitalWrite(ledVerde, HIGH);
                break;
            case 16734885: 
                digitalWrite(ledVerde, LOW);
                break;
            case 16728765:
                digitalWrite(ledVermelho2, HIGH);
                break;
            case 16730805:
                digitalWrite(ledVermelho2, LOW);
                break;
        }
        irrecv.resume();
    }

}
void printFloat(float value, int places)
{
    // Calibração 
    int digit;
    float tens = 0.1;
    int tenscount = 0;
    int i;
    float tempfloat = value;     

    float d = 0.5;
    if (value < 0)
        d *= -1.0;

    for (i = 0; i < places; i++)
        d /= 10.0;

    tempfloat += d;   

    tempfloat *= -1.0;
    while ((tens * 10.0) <= tempfloat)
    {
        tens *= 10.0;
        tenscount += 1;
    }

    if (value < 0)
        Serial.print('-'); if (tenscount == 0)
        Serial.print(0, DEC); for (i = 0; i < tenscount; i++)
    {
        digit = (int)(tempfloat / tens);
        Serial.print(digit, DEC);
        tempfloat = tempfloat - ((float)digit * tens);
        tens /= 10.0;
    }  
    if (places <= 0)
        return;   /
    Serial.print('.');    
    for (i = 0; i < places; i++)
    {
        a
tempfloat *= 10.0;
        digit = (int)tempfloat;
        Serial.print(digit, DEC);

        tempfloat = tempfloat - (float)digit;
    }
}