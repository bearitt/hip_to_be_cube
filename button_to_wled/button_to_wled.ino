#include <WiFi.h>
#include <HTTPClient.h>

/*
  Button

  Turns on and off a light emitting diode(LED) connected to digital pin 13,
  when pressing a pushbutton attached to pin 2.

  The circuit:
  - LED attached from pin 13 to ground through 220 ohm resistor
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;  // the number of the pushbutton pin
const int ledPin = 16;    // the number of the LED pin
const char* ssid = "hip_to_be_cube";
const char* password = "hueyWouldBeProud69";

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status
int isLightOn = 0;
int lastButtonState = 0;
int isEffectOn = 0;
int isStripOn = 0;
int lightEffects[12] = {};
// 30: chase rainbow
// 74: colortwinkles
// 112: dancing shadows
// 66: fire 2012
// 42: fireworks
// 64: juggle
// 143: noisefire **potential easter egg effect**
// 129: pixelwave
// 63: pride 2015
// 43: rain
// 79: ripple
// 94: sinelon rainbow
// 14: theater rainbow
// 106: twinkleup
// 184: wavesins

void setup() {
  // initialize serial connection
  Serial.begin(115200);
  delay(1000);

  // connect to wifi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

void loop() {
  delay(2000);
  WiFiClient client;
  HTTPClient http;

    // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // if strip is not on, turn it on
  if (isStripOn == 0) {
    http.begin(client, serverName);
    // Turn strip on
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"on\": true}");
    Serial.print("Strip on\nHTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();
    isStripOn = 1;
  }

  // if there's no effect and the button is not pressed, load default effect
  if (isEffectOn == 0) {
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");
    // default effect: phased = 105
    httpResponseCode = http.POST("{\"seg\":{\"fx\":105}}");
    Serial.print("Rainbox effect\nHTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();

  }

  // if the button is pressed: 
  // choose an effect
  // send effect to wled
  // turn off button led
  // wait 20 seconds
  // load default effect
  // turn button led on



  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH && buttonState != lastButtonState) {
    // turn LED on:
    // digitalWrite(ledPin, HIGH);
    if (isLightOn == 0) {
      digitalWrite(ledPin, HIGH);
      isLightOn = 1;
      Serial.println("Lights on");
    } else {
      digitalWrite(ledPin, LOW);
      isLightOn = 0;
      Serial.println("Lights off");
    }
    delay(200);
    // } else {
    // turn LED off:
    // digitalWrite(ledPin, LOW);
  }
  lastButtonState = buttonState;
}
