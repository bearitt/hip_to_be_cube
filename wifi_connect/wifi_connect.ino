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

    // test: send post request to wled controller
    delay(2000);
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);

    // Specify content-type header
    // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    // String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
    // Send HTTP POST request
    // int httpResponseCode = http.POST(httpRequestData);
    
    // If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"on\": true}");
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
      
    // Free resources
    http.end();
}

void loop(){}