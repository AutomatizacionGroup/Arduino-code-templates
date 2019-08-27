#include<WiFi.h>

//detalles de la red
const char* ssid = "avana";
const char* password = "avana123";

//cliente de wifi 
WiFiClient cliente;

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
}

void loop() {
  // put your main code here, to run repeatedly:

}
