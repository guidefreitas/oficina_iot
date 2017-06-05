
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <SSD1306Wire.h>

#define WIFI_SSID "<NOME REDE>"
#define WIFI_PASSWORD "<SENHA REDE>"

#define I2C_DISPLAY_ADDRESS 0x3c
#define SDA_PIN D2
#define SCK_PIN D1
#define PIN_LED D8
#define DHTTYPE DHT22
#define DHTPIN  D6

String ADAFRUIT_USERNAME = "<USUARIO ADAFRUIT>";
String ADAFRUIT_AIO = "<AIO ADAFRUIT>";
String FEED_KEY_TEMPERATURA = "temperatura";
String FEED_KEY_UMIDADE = "umidade";
String FEED_KEY_LED = "led";
String ADAFRUIT_HOST = "http://io.adafruit.com/api/v2";
String URL_TEMPERATURA = ADAFRUIT_HOST + "/" + ADAFRUIT_USERNAME + "/feeds/" + FEED_KEY_TEMPERATURA + "/data/";
String URL_UMIDADE = ADAFRUIT_HOST + "/" + ADAFRUIT_USERNAME + "/feeds/" + FEED_KEY_UMIDADE + "/data/";
String URL_LED = ADAFRUIT_HOST + "/" + ADAFRUIT_USERNAME + "/feeds/" + FEED_KEY_LED + "/data/last/";

long tpLed = 0;
long tpTemperatura = 0;
long tpUmidade = 0;

long tempoEnvioTemperatura = 20000;
long tempoEnvioUmidade = 20000;
long tempoConsultaLED = 2000;

HTTPClient http;
DHT dht(DHTPIN, DHTTYPE, 11);
SSD1306Wire  display(I2C_DISPLAY_ADDRESS, SDA_PIN, SCK_PIN);

const char activeSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B01000010,
    B01000010,
    B00100100,
    B00011000
};

const char inactiveSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000
};

void setupPins(){
  pinMode(PIN_LED, OUTPUT);
  dht.begin();
  delay(2000);
}

void setupDisplay(){
  //Serial.println("Configurando display");
  display.init();
  display.clear();
  display.display();
  //display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setContrast(255);
  display.drawString(0, 0, "Hello world");
  //Serial.println("Display configurado");
}

void setupWifi(){
  Serial.println("Configurando Wifi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando");
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    display.clear();
    String msg = String("Conectando em ") + String(WIFI_SSID);
    display.drawString(64, 10, msg);
    display.drawXbm(46, 30, 8, 8, counter % 3 == 0 ? activeSymbol : inactiveSymbol);
    display.drawXbm(60, 30, 8, 8, counter % 3 == 1 ? activeSymbol : inactiveSymbol);
    display.drawXbm(74, 30, 8, 8, counter % 3 == 2 ? activeSymbol : inactiveSymbol);
    display.display();
    counter++;
  }
  delay(3000);
  display.clear();
  display.drawString(64, 10, "Conectado!!!");
  display.display();
  delay(3000);
  display.clear();
  display.display();
  Serial.println();
  Serial.print("Conectado: ");
  Serial.println(WiFi.localIP());
}

void enviaPost(String Url, String dados){
  if((WiFi.status() == WL_CONNECTED)) {
      //Serial.println("Enviando dados...");
      //Serial.print("URL: ");
      //Serial.println(Url);
      //Serial.println("Conectando");
      http.begin(Url);
      http.addHeader("X-AIO-Key", ADAFRUIT_AIO, true);
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST(dados);
      if(httpCode == HTTP_CODE_OK){
        //Serial.println("Dados enviados para o servidor");
      }else{
        Serial.println("Erro ao enviar os dados");
        Serial.print("Code:");
        Serial.println(httpCode);
      }
      http.end();
  }
}

String enviaGet(String Url){
  if((WiFi.status() == WL_CONNECTED)) {
      //Serial.print("URL: ");
      //Serial.println(Url);
      http.begin(Url);
      http.addHeader("X-AIO-Key", ADAFRUIT_AIO, true);
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.GET();
      if(httpCode == HTTP_CODE_OK){
         //Serial.println("Dados enviados para o servidor");
         return http.getString();
      }else{
        Serial.println("Erro ao enviar os dados");
        Serial.print("Code:");
        Serial.println(httpCode);
      }
      http.end();
  }
  return String();
}

void mostraDadosDisplay(float umidade, float temperatura){
  //Limpa e mostra a umidade no display
  display.clear();
  String msgUmidade = "Umidade: " + String(umidade);
  display.drawString(64, 10, msgUmidade);
  String msgTemperatura = "Temperatura: " + String(temperatura);
  display.drawString(64, 30, msgTemperatura);
  display.display();
}

void buscaDadosLED(){
  String resultado = enviaGet(URL_LED);
  if(resultado.length() != 0){
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(resultado);
    String valorLed = root["value"];
    if(valorLed == "0"){
      Serial.println("Desligando a luz");
      digitalWrite(PIN_LED, 0);
    }else if(valorLed == "1"){
      Serial.println("Ligando a luz");
      digitalWrite(PIN_LED, 255);
    }
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  setupPins();
  setupDisplay();
  setupWifi();
}

void loop() {
  long tempo = millis();
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature(false);
  mostraDadosDisplay(umidade, temperatura);

  if((tempo - tpTemperatura) > tempoEnvioTemperatura){
    tpTemperatura = tempo;
    String dados = "{ \"value\" : \"" + String(temperatura) + "\" }";
    enviaPost(URL_TEMPERATURA, dados);
  }

  if((tempo - tpUmidade) > tempoEnvioUmidade){
    tpUmidade = tempo;
    String dados = "{ \"value\" : \"" + String(umidade) + "\" }";
    enviaPost(URL_UMIDADE, dados);
  }

  if((tempo - tpLed) > tempoConsultaLED){
    tpLed = tempo;
    buscaDadosLED();
  }
}

