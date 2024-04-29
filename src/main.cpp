// Include necessary libraries here
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Define global variables here
#define lcdAddress 0x27 // link to the address number of LCD
#define LED_R 32
#define LED_G 25
#define LED_Y 33
#define BUTTON 26

const char* ssid = "IoTLaB";
const char* password = "iot303abab";
const char* mqtt_server = "mqtt.iotserver.uz";
const int mqtt_port = 1883;
const char* mqtt_username = "userTTPU";
const char* mqtt_password = "mqttpass";
const char* myID = "OtabekDavronbek";
const char* pubTopic = "ttpu/iot/OtabekDavronbek/btn";
const char* subTopic = "ttpu/iot/global/led";

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 10000;

LiquidCrystal_I2C lcd(lcdAddress, 16, 2); // initializing LCD with the address, columns, rows
WiFiClient espClient; 
PubSubClient client(espClient);

// Define functions here
    // Function to set up WiFi connection
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
    //Function to set up MQTT connection 
void setup_mqtt(){
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}
    //Function to reconnect to MQTT
void reconnect(){
  if(client.connected()){
    return;
  }

  if(millis() - lastReconnectAttempt > reconnectInterval) {
    lastReconnectAttempt = millis();
    Serial.print("Attempting MQTT connection...");
    if(client.connect(myID, mqtt_username, mqtt_password)){
      Serial.println("connected");
 //     client.publish("ttpu/");
 //     client.subscribe("");
    }
    else{
      Serial.print("failure, rc = ");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Convert payload to a string
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Toggle LEDs and display message on LCD
  if (message.indexOf("toggle_led_r") != -1) {
    digitalWrite(LED_R, message.charAt(message.indexOf("toggle_led_r") + 13) == '1' ? HIGH : LOW);
  }
  if (message.indexOf("toggle_led_g") != -1) {
    digitalWrite(LED_G, message.charAt(message.indexOf("toggle_led_g") + 13) == '1' ? HIGH : LOW);
  }
  if (message.indexOf("toggle_led_y") != -1) {
    digitalWrite(LED_Y, message.charAt(message.indexOf("toggle_led_y") + 13) == '1' ? HIGH : LOW);
  }
  if (message.indexOf("lcd_message") != -1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message.substring(message.indexOf("lcd_message") + 13));
  }
}

/**********************************/
// SETUP //
void setup(void) 
{
  Serial.begin(115200);

  // Initialize peripherals
  Wire.begin();
  lcd.begin(16, 2);
  lcd.backlight(); // turn backlight on
  lcd.setCursor(3, 0); 
  lcd.print("WAY AHEAD!");

  pinMode(BUTTON, INPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);

  // Connect to WiFi
  setup_wifi();

  // Setup MQTT
  setup_mqtt();
}

/**********************************/
// LOOP //
void loop(void) 
{
  // Keep WiFi and MQTT connection alive
  if(WiFi.status() != WL_CONNECTED){
    setup_wifi();
  }
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  //Publish message when the button is pressed
  if(digitalRead(BUTTON) == 0){
    client.publish(pubTopic, "PRE55ED");
    Serial.println("BUTTON PRESSED");
  }
}