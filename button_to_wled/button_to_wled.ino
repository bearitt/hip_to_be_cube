#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <stdlib.h>
#include <time.h>

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;  // the number of the pushbutton pin
const int ledPin = 16;    // the number of the LED pin
const char* ssid = "hip_to_be_cube";
const char* password = "hueyWouldBeProud69";
const char* cubeZeroUrl = "http://192.168.4.2/json/";
const char* cubeOneUrl = "http://192.168.4.11/json/";
const char* cubeTwoUrl = "http://192.168.4.12/json/";
const int numEffects = 15;
const char* lightEffects[numEffects] = { "30", "74", "112", "66", "42", "64", "143", "129", "63", "43", "79", "94", "14", "106", "184" };
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

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status
int isLightOn = 0;

bool isCubeZeroInitialized = 0;
bool isCubeZeroConnected = 0;
bool isCubeOneInitialized = 0;
bool isCubeOneConnected = 0;
bool isCubeTwoInitialized = 0;
bool isCubeTwoConnected = 0;

int lastButtonState = 0;

void setup() {
  // initialize serial connection
  Serial.begin(115200);
  delay(1000);
  // seed rng
  srand(time(NULL));

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

  // check status of all cubes
  isCubeZeroConnected = isCubeConnected(0);
  isCubeOneConnected = isCubeConnected(1);
  isCubeTwoConnected = isCubeConnected(2);

  if (isCubeZeroConnected) {
    int turnOffStatus = httpPostRequest(cubeZeroUrl, "{\"on\": false}");
    if (turnOffStatus != 200) {
      Serial.println("You fucked up");
    }
  }
  if (isCubeOneConnected) {
    int turnOffStatus = httpPostRequest(cubeOneUrl, "{\"on\": false}");
    if (turnOffStatus != 200) {
      Serial.println("You fucked up");
    }
  }
  if (isCubeTwoConnected) {
    int turnOffStatus = httpPostRequest(cubeTwoUrl, "{\"on\": false}");
    if (turnOffStatus != 200) {
      Serial.println("You fucked up");
    }
  }

  buttonOn();
  delay(1000);
}

void loop() {
  // delay(2000);
  WiFiClient client;

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  if (!isCubeZeroInitialized && isCubeZeroConnected) {
    initializeCube(0);
  }
  if (!isCubeOneInitialized && isCubeOneConnected) {
    initializeCube(1);
  }
  if (!isCubeTwoInitialized && isCubeTwoConnected) {
    initializeCube(2);
  }

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH && buttonState != lastButtonState) {
    Serial.println("Button smashed");
    buttonOff();
    if (isCubeZeroConnected) {
      changeCubeEffect(0);
    }
    if (isCubeOneConnected) {
      changeCubeEffect(1);
    }
    if (isCubeTwoConnected) {
      changeCubeEffect(2);
    }
    // check for connectivity of other cubes
    // allows for hot swap of cubes
    isCubeZeroConnected = isCubeConnected(0);
    isCubeOneConnected = isCubeConnected(1);
    isCubeTwoConnected = isCubeConnected(2);

    delay(20000);
    buttonOn();
  }
  lastButtonState = buttonState;
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);

  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

int httpPostRequest(const char* serverName, const char* payload) {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, serverName);
  // Turn strip on
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);

  // Free resources
  http.end();
  return httpResponseCode;
}

bool isCubeConnected(int cubeNumber) {
  WiFiClient client;
  HTTPClient http;
  char serverName[300] = {};
  switch (cubeNumber) {
    case 0:
      strcpy(serverName, cubeZeroUrl);
      break;
    case 1:
      strcpy(serverName, cubeOneUrl);
      break;
    case 2:
      strcpy(serverName, cubeTwoUrl);
      break;
  }

  http.begin(client, serverName);

  int httpResponseCode = http.GET();

  // Free resources
  http.end();
  if (httpResponseCode == 200) {
    Serial.printf("Cube %i is connected\n", cubeNumber);
    return true;
  }
  Serial.printf("Cube %i is NOT connected\n", cubeNumber);
  return false;
}

void initializeCube(int cubeNumber) {
  char serverName[300] = {};
  switch (cubeNumber) {
    case 0:
      strcpy(serverName, cubeZeroUrl);
      break;
    case 1:
      strcpy(serverName, cubeOneUrl);
      break;
    case 2:
      strcpy(serverName, cubeTwoUrl);
      break;
  }
  String sensorReadings;
  sensorReadings = httpGETRequest(serverName);
  JSONVar cubeResponse = JSON.parse(sensorReadings);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(cubeResponse) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
  }

  bool isCubeOn = cubeResponse["state"]["on"];
  int cubeFx = cubeResponse["state"]["seg"]["fx"];
  if (!isCubeOn) {
    int turnOnStatus = httpPostRequest(serverName, "{\"on\": true}");
    if (turnOnStatus != 200) {
      Serial.println("Error turning on cube");
      return;
    }
  }
  if (cubeFx != 105) {
    int changeToDefaultEffect = httpPostRequest(serverName, "{\"seg\":{\"fx\":105}}");
    if (changeToDefaultEffect != 200) {
      Serial.println("Error setting default pattern");
      return;
    }
  }
  switch (cubeNumber) {
    case 0:
      isCubeZeroInitialized = true;
      break;
    case 1:
      isCubeOneInitialized = true;
      break;
    case 2:
      isCubeTwoInitialized = true;
      break;
  }
}

void buttonOff() {
  if (isLightOn == 1) {
    digitalWrite(ledPin, LOW);
    isLightOn = 0;
    Serial.println("Button turned off");
  }
}

void buttonOn() {
  if (isLightOn == 0) {
    digitalWrite(ledPin, HIGH);
    isLightOn = 1;
    Serial.println("Button turned on");
  }
}

void changeCubeEffect(int cubeNumber) {
  int effectIndex = rand() % numEffects;
  const char* cubeEffect = lightEffects[effectIndex];
  Serial.printf("Cube index: %i\nCube effect chosen: %s\n", cubeNumber, cubeEffect);
  char cubePayload[200] = {};
  sprintf(cubePayload, "{\"seg\":{\"fx\":%s}}", cubeEffect);

  // apply effect to cube
  Serial.println("Send to cube");
  if (cubeNumber == 0) {
    int cubeResponse = httpPostRequest(cubeZeroUrl, cubePayload);
    Serial.println(cubeResponse);
    isCubeZeroInitialized = false;
  } else if (cubeNumber == 1) {
    int cubeResponse = httpPostRequest(cubeOneUrl, cubePayload);
    Serial.println(cubeResponse);
    isCubeOneInitialized = false;
  } else if (cubeNumber == 2) {
    int cubeResponse = httpPostRequest(cubeTwoUrl, cubePayload);
    Serial.println(cubeResponse);
    isCubeTwoInitialized = false;
  }
}
