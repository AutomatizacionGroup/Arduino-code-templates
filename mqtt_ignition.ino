#include <PubSubClient.h>
#include<WiFi.h>
#include <DHT.h>


//detalles de la red y de servidor mqtt
const char* ssid = "avana";
const char* password = "avana123";
const char* mqtt_server = "192.168.0.122";
const char* username = "esp32";
const char* pass = "password";

//cliente de wifi y de mqtt
WiFiClient cliente;
PubSubClient client(cliente);

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

//funcion que reconecta en caso de que se caiga conexion
void reconnect(){

  //recorrer hasta que se conecte
  while(!client.connected()){
    //id de cliente
    const char* clientId = "esp32_client";
    //intento de conectar
    if(client.connect(clientId,username,pass)){
      //una vez conectado, publicar (c_str() convierte el string de temperatura a const char*)
      client.publish("temp", temperatura().c_str());
    }
    else{
      //esperar 5 segundos antes de reintentar
      delay(5000);
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
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()){
    reconnect();
  }
  client.loop();
}
