#include <PubSubClient.h>
#include<WiFi.h>
#include <DHT.h>

//pin de led
const int led = 32;

//detalles de la red y de servidor mqtt
const char* ssid = "bio";
const char* password = "biomic1044";
const char* mqtt_server = "192.168.0.100";
//const char* username = "";
//const char* pass = "";

//cliente de wifi y de mqtt
WiFiClient cliente;
PubSubClient client(cliente);
long lastMsg = 0;
char msg[50];
int value = 0;

//inicializando sensor
#define pin 26
#define type DHT11
DHT dht(pin,type);

//funcion que conecta al wifi como tal
void setup_wifi(){

  delay(10);
  Serial.print("conectandose a: ");
  Serial.println(ssid);

  //inicia proceso de conexion
  WiFi.begin(ssid, password);

  //si no conecta de una vez reintenta conexion cada medio segundo
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado exitosamente");
  Serial.println("Direccion ip: ");
  Serial.println(WiFi.localIP());
  
  
}

//funcion que mide temperatura del dht11 y la convierte a string
String temperatura(){

  //lee temperatura de sensor
  float temp = dht.readTemperature();

  //verifica que lectura sea un valor valido
  while(isnan(temp)){
    delay(2000);
    temp = dht.readTemperature();
  }

  //convierte temp a string
  String str = String(temp);

  return str;
}

//funcion que mide humdedad del dht11 y la convierte a string
String humedad(){

  //lee humedad del sensor
  float hum = dht.readHumidity();

  //verifica que lectura sea un valor valido
  while(isnan(hum)){
    delay(2000);
    hum = dht.readHumidity();
  }

  String str = String(hum);
  return str;

}

//funcion que reconecta en caso de que se caiga conexion
void reconnect(){

  //recorrer hasta que se conecte
  while(!client.connected()){
    Serial.print("intentando conectar a servidor mqtt");
    //id de cliente
    const char* clientId = "esp32_client";
    //intento de conectar
    if(client.connect(clientId)){
      //una vez conectado, suscribirse
      Serial.println("conexion exitosa");
      client.subscribe("esp32/output");
    }
    else{
      Serial.print("conexion fallida, rc= ");
      Serial.print(client.state());
      Serial.println("se reintentara en 5 segundos");
      //esperar 5 segundos antes de reintentar
      delay(5000);
    }
  }
  
}

//funcion que ejecuta accion segun publicacion a la cual el arduino esta subscrito
void callback(char* topic, byte* payload, unsigned int length){

  Serial.print("ha llegado mensaje en el topico: ");
  Serial.print(topic);
  Serial.print(". Mensaje: ");

  String mensajeTemp;

  //construye string a partir de mensaje y lo imprime
  for(int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
    mensajeTemp += (char)payload[i];
  }
  Serial.println(mensajeTemp);

  if(String(topic) == "esp32/output"){
    Serial.print("cambiando estado de LED a: ");
    if(mensajeTemp == "on"){
      Serial.println("encendido");
      digitalWrite(led, HIGH);
    }
    else{
      Serial.println("apagado");
      digitalWrite(led,LOW);
    }
  }

  
}

// put your setup code here, to run once:
void setup() {

  //inicia monitor serial a 115200 baudios
  Serial.begin(9600);
  
  setup_wifi();
  dht.begin();

  //inicializa servidor mqtt
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);

  //inicializa output de led
  pinMode(led, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()){
    reconnect();
  }
  client.loop();

  //publica cada 5 secs
  long now = millis();
  if (now - lastMsg > 5000){
    lastMsg = now;

    client.publish("esp32/temperature", temperatura().c_str());
    client.publish("esp32/humidity", humedad().c_str());
    
  }
  
}
