/* DuinoLightProp.ino
   MIT License (c) Faure Systems <dev at faure dot systems>

   Set SECRET_SSID and SECRET_PASS in arduino_secrets.h

   Requirements:
   - install ArduinoProps.zip library and dependencies (https://github.com/fauresystems/ArduinoProps)
   - help: https://github.com/xcape-io/ArduinoProps/blob/master/help/ArduinoProps_sketch.md
*/
#include "ArduinoProps.h"
#include "arduino_secrets.h"

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA)

WifiProp prop(u8"Arduino Lights", // as MQTT client id, should be unique per client for given broker
              u8"Room/My room/Props/Arduino Lights/inbox",
              u8"Room/My room/Props/Arduino Lights/outbox",
              "192.168.1.53", // your MQTT server IP address
              1883); // your MQTT server port;

// Relays pinout
#define LIGHT1 0
#define LIGHT2 1
#define LIGHT3 2
#define LIGHT4 3
#define LIGHT5 4
#define LIGHT6 5

PropDataLogical lighting(u8"lighting", u8"yes", u8"no", false);
PropDataLogical light1(u8"light1");
PropDataLogical light2(u8"light2");
PropDataLogical light3(u8"light3");
PropDataLogical light4(u8"light4");
PropDataLogical light5(u8"light5");
PropDataLogical light6(u8"light6");
PropDataText rssi(u8"rssi");

bool wifiBegun(false);

void setup()
{
  Serial.begin(9600);

  prop.addData(&lighting);
  prop.addData(&light1);
  prop.addData(&light2);
  prop.addData(&light3);
  prop.addData(&light4);
  prop.addData(&light5);
  prop.addData(&light6);
  prop.addData(&rssi);

  prop.begin(InboxMessage::run);

  pinMode(LIGHT1, OUTPUT);
  pinMode(LIGHT2, OUTPUT);
  pinMode(LIGHT3, OUTPUT);
  pinMode(LIGHT4, OUTPUT);
  pinMode(LIGHT5, OUTPUT);
  pinMode(LIGHT6, OUTPUT);

  // At this point, the broker is not connected yet
}

void loop()
{
  if (!wifiBegun) {
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    Serial.println(WiFi.firmwareVersion());
    delay(250); // acceptable freeze for this prop (otherwise use PropAction for async-like behavior)
    // do static IP configuration disabling the dhcp client, must be called after every WiFi.begin()
    String fv = WiFi.firmwareVersion();
    if (fv.startsWith("1.0")) {
      Serial.println("Please upgrade the firmware for static IP");
      // see https://github.com/fauresystems/ArduinoProps/blob/master/WifiNinaFirmware.md
    }
    else {
      //WiFi.config(IPAddress(192, 168, 1, 21), // local_ip
      //	IPAddress(192, 168, 1, 1),  // dns_server
      //	IPAddress(192, 168, 1, 1),  // gateway
      //	IPAddress(255, 255, 255, 0)); // subnet
    }
    if (WiFi.status() == WL_CONNECTED) {
      wifiBegun = true;
      Serial.println(WiFi.localIP());
      Serial.println(WiFi.subnetMask());
      Serial.println(WiFi.gatewayIP());
    } else {
      WiFi.end();
    }
  } else if (wifiBegun && WiFi.status() != WL_CONNECTED) {
    WiFi.end();
    wifiBegun = false;
  }

  prop.loop();

  rssi.setValue(WiFi.RSSI() + String(" dBm")); // https://www.metageek.com/training/resources/understanding-rssi.html

  light1.setValue(digitalRead(LIGHT1)); // read I/O
  light2.setValue(digitalRead(LIGHT2)); 
  light3.setValue(digitalRead(LIGHT3)); 
  light4.setValue(digitalRead(LIGHT4)); 
  light5.setValue(digitalRead(LIGHT5)); 
  light6.setValue(digitalRead(LIGHT6)); 
}

void InboxMessage::run(String a) {

  if (a == u8"app:startup")
  {
    prop.sendAllData();
    prop.sendDone(a);
  }
  else if (a == u8"reset-mcu")
  {
    prop.resetMcu();
  }
  else if (a == "lighting:1")
  {


    prop.sendAllData(); // all data change, we don't have to be selctive then
    prop.sendDone(a); // acknowledge prop command action
  }
  else if (a == "lighting:0")
  {


    prop.sendAllData(); // all data change, we don't have to be selctive then
    prop.sendDone(a); // acknowledge prop command action
  }
  else
  {
    // acknowledge omition of the prop command
    prop.sendOmit(a);
  }
}
