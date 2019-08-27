#include <PubSubClient.h>
#include<WiFi.h>
#include <DHT.h>

//pin de led
const int led = 23;

//detalles de la red y de servidor mqtt
const char* ssid = "avana";
const char* password = "avana123";
const char* mqtt_server = "192.168.0.122";
//const char* username = "esp32";
//const char* pass = "password";

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

  Serial.println("medicion de temperatura exitosa");
  //convierte temp a string
  String str = String(temp);

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
  Serial.println();

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
  Serial.begin(115200);
  
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

  long now = millis();
  if (now - lastMsg > 5000){
    lastMsg = now;

    client.publish("esp32/temperature", temperatura().c_str());
    
  }
  
}
