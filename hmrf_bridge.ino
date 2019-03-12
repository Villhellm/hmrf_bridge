#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <RCSwitch.h>

// Update these with values suitable for your network.
byte mac[] = {  0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
IPAddress bridge_ip(0, 0, 0, 0);
IPAddress mqtt_server(0, 0 , 0, 0);
IPAddress dnss(8, 8, 8, 8);
IPAddress gateway(0, 0, 0, 0);
char mqtt_user[] = "user";
char mqtt_password[] = "password";
char mqtt_state_topic[] = "HMRF_Bridge/state";

int interrupt_pin = 0; //The interrupt index i.e. pin 2 = interrupt 0 on Uno.
int mqtt_connection_timeout = 10; //in seconds
int multiple_code_timeout = 1000; //in milliseconds
const int number_of_codes = 10;
long codes[number_of_codes];

EthernetClient ethClient;
PubSubClient client(mqtt_server, 1883, ethClient);
RCSwitch mySwitch = RCSwitch();

void setup()
{
  Serial.begin(9600);
  reset_codes();
  Ethernet.begin(mac, bridge_ip, dnss, gateway);
  mySwitch.enableReceive(interrupt_pin);
  if (mqtt_connect()) {
    Serial.println("Connection successful.");
  }
}

void loop()
{
  if (mySwitch.available()) {
    long current = mySwitch.getReceivedValue();
    long then = millis();
    long now = millis();
    codes[0] = current;
    int index = 1;
    mySwitch.resetAvailable();
    publish_mqtt_state(current);
    while (then + multiple_code_timeout >= now) {
      now = millis();
      if (mySwitch.available()) {
        current = mySwitch.getReceivedValue();
        if (!codes_contains_value(current)) {
          codes[index] = current;
          index++;
          if (index < number_of_codes) {
            then = millis();
          }
          else {
            index = 0;
          }
        }
        mySwitch.resetAvailable();
      }
    }

    for (int i = 1; i < number_of_codes; i++) {
      if(codes[i] != 0){
          publish_mqtt_state(codes[i]);
      }
    }
    reset_codes();
  }
}

bool codes_contains_value(long value) {
  for (int i = 0; i < number_of_codes; i++) {
    if (codes[i] == value) {
      return true;
    }
  }
  return false;
}

void reset_codes() {
  for (int i = 0; i < number_of_codes; i++) {
    codes[i] = 0;
  }
}

void publish_mqtt_state(long code) {
  char vessel[50];
  String filler = String(code, HEX);
  filler.toUpperCase();
  filler.toCharArray(vessel, 50);
  Serial.print("Message:'" + filler + "' received attempting to send...");
  if (mqtt_connect()) {
    client.publish(mqtt_state_topic, vessel);
    Serial.println("message sent");
  }
}

bool mqtt_connect() {
  long then = millis();
  long now = millis();
  while (!client.connect("HMRF_Bridge", mqtt_user, mqtt_password) || !ethClient.connected()) {
    if (then + mqtt_connection_timeout * 1000 <= now) {
      Serial.println("Connection timed out...");
      return false;
    }
    now = millis();
  }
  return true;
}