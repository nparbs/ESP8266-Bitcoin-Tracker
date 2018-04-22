#include <Pushover.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

const char* ssid = "Parbs";
const char* password = "";
const int httpPort = 80;
const char* host = "api.coindesk.com";

void setup() {

  delay(100);

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  Wire.begin(0, 2);  // Initialize I2C and OLED Display

  delay(100);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(200);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(INVERSE);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
  Serial.println("Started!");
  delay(200);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connecting to ");
  display.print(ssid);
  display.display();


  // Connect to API
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/v1/bpi/currentprice.json";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(100);


  String answer;
  while (client.available()) {
    String line = client.readStringUntil('\r');
    answer += line;
  }

  client.stop();
  Serial.println();
  Serial.println("closing connection");
  Serial.println();
  Serial.println("Answer: ");
  Serial.println(answer);

  // Convert to JSON
  String jsonAnswer;
  int jsonIndex;

  for (int i = 0; i < answer.length(); i++) {
    if (answer[i] == '{') {
      jsonIndex = i;
      break;
    }
  }

  // Get JSON data
  jsonAnswer = answer.substring(jsonIndex);
  Serial.println();
  Serial.println("JSON answer: ");
  Serial.println(jsonAnswer);
  jsonAnswer.trim();

  // Get rate as float
  int rateIndex = jsonAnswer.indexOf("rate_float");
  String priceString = jsonAnswer.substring(rateIndex + 12, rateIndex + 18);
  priceString.trim();
  float price = priceString.toFloat();

  // Print price
  Serial.println();
  Serial.println("Bitcoin price: ");
  Serial.println(price);

  // Display on OLED

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("BTC / USD");
  display.setCursor(0, 16);
  display.print("$" + priceString);
  display.display();

  // Wait 5 seconds
  delay(3000);
  
  ESP.deepSleep(0, WAKE_NO_RFCAL);
}

void loop() {
}


