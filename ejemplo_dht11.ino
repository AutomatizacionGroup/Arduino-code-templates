#include "DHT.h"

//se define pin de donde se va a leer y cual es el sensor que se esta usando
#define pin 26
#define type DHT11

//inicializa senson con pin y modelo de sensor
DHT dht(pin,type);

void setup() {
  // put your setup code here, to run once:

  //inicializa monitor serial
  Serial.begin(9600);
  Serial.print("Este codigo es para probar el sensor dht11 de temperatura y humedad");
  
  //prende el sensor
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  //Se espera algunos segundos entre lecturas porque el sensor es lento
  delay(2000);

  //para medir temperatura (en celsius) e imprimirla en monitor serial
  float temp = dht.readTemperature();
  Serial.print("La temperatura actual es: ");
  Serial.println(temp);

  //para medir humedad (relativa) e imprimirla en monitor serial
  float hum = dht.readHumidity();
  Serial.print("la humedad relativa actual es: ");
  Serial.println(hum);

  //reinicia loop si lectura falla
  if(isnan(temp) || isnan(hum)){
    Serial.println("lectura fallida!");
    return;
  }

  
}
