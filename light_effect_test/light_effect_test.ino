#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "hip_to_be_cube";
const char* password = "hueyWouldBeProud69";
// url for wled controller
const char* serverName = "http://192.168.4.2/json";
// unsigned long lastTime = 0;
// unsigned long timerDelay = 5000;

void setup(){
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}

void loop(){
    // test: send post request to wled controller
    delay(2000);
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);
    
    // Turn strip on
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"on\": true}");
    Serial.print("Strip on\nHTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();

    delay(5000);

    // Turn on effect
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");
    httpResponseCode = http.POST("{\"seg\":{\"fx\":30}}");
    Serial.print("Rainbox effect\nHTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();

    delay(5000);

    // Set solid colour
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");
    httpResponseCode = http.POST("{\"seg\":{\"fx\":0}}");
    Serial.print("Solid colour\nHTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();

    delay(5000);

    // Turn strip off
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");
    httpResponseCode = http.POST("{\"on\": false}");
    Serial.print("Strip off\nHTTP Response code: ");
    Serial.println(httpResponseCode);
    // Free resources
    http.end();
}