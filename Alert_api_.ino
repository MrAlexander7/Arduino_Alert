//Libraries (Бібліотека)
#include <Ethernet.h> 
#include <SPI.h>

//API data (API дані)
#define HOST "tcp.alerts.com.ua"
#define PORT 1024
#define API_KEY "a459001eaa441af2b4e91b8a7b3a3dfaf1bed21f,10"

String Oldsignal  = "s:10=0";
const byte relayPin = 9;
const byte relayPin2 = 8;
const byte smokeSensorPin = A0;
byte mac[] = {0xDE,0xAD,0xBE,0xEF,0xFE,0xEC};
IPAddress ip(192,168,1,102);
EthernetClient client;

//Date processing from API (обробка даних з API)
void processPacket(String data) {
    Serial.print("Got packet: ");
    Serial.println(data);
    
    if (data == "a:wrong_api_key") {
      Serial.println("Please specify your API key!");
      delay(5000);
    } else if (data == "s:10=1") {
      Oldsignal = "s:10=1";
      digitalWrite(relayPin2, LOW);
      delay(120000); // 2 minute delay (2 хв затримки)
      digitalWrite(relayPin2, HIGH);
      } else if (data == "s:10=0" && Oldsignal != data) {
        Oldsignal = "s:10=0";
        digitalWrite(relayPin2, LOW);
        delay(60000); // 1 minute delay (1 хв затримки)
        digitalWrite(relayPin2, HIGH);
        }
}
//Date processing from API (обробка даних з API)
void handlerData(String data){
  static String buffer;
    buffer += data;
    while (1) {
        int border = buffer.indexOf("\n"); //Search for the number of the given argument (Пошук номера заданого аргумента)
        if (border == -1) {
            break;
        }
        processPacket(buffer.substring(0, border)); //sends a string to functions (відправляє рядок до функцій)
        buffer = buffer.substring(border + 1);
    }
  }
//Setup settings (Початкові налаштування)
void setup() {
  Ethernet.begin(mac, ip);
  pinMode(relayPin, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  digitalWrite(relayPin, HIGH);
  digitalWrite(relayPin2, HIGH);
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  if (!client.connected()) {
    Serial.print("Connecting to Air Raid Alert API...");
    while (!client.connect(HOST, PORT)) //connect to API (Підключення до API)
    {
        Serial.println(" Failed.");
        delay(1000);
    }
    Serial.println(" Connected!");
    client.write(API_KEY);
  }
  String data;
  while (client.available() > 0)
  {
      data += (char)client.read(); //read API data (зчитування даних з API)
  }
  if (data.length()) {
    handlerData(data);
  }
  delay(500);
  int sensorValue = analogRead(smokeSensorPin);
  Serial.println(smokeSensorPin);
   if (sensorValue > 500) {
    digitalWrite(relayPin, LOW);  
    Serial.println("Smoke detected!");
    Serial.println(smokeSensorPin);
  } else {
    digitalWrite(relayPin, HIGH);
  }
  delay(1000);
}
