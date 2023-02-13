//#include <HttpClient.h>

//#include <ArduinoHttpClient.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//#include <WiFi.h>
//#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <ThingSpeak.h>

WiFiClient client;

unsigned long Channel_ID = 1679904; // Your Channel ID
const char * myWriteAPIKey = "AXYGDC5S28XYAV25"; //Your write API key

char ssid[] = "OPPO A3s"; //SSID here
char pass[] = "12345679"; // Passowrd here

String openWeatherMapApiKey = "53d78067682172ffceee4456fca604fa";
String city = "Mumbai";
String countryCode = "IN";

unsigned long lastTime = 0;
unsigned long timerDelay = 1000;
String jsonBuffer;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid,pass);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);
  Serial.println("Timer set to 10 sec");

}

void loop() {
  // put your main code here, to run repeatedly:
  //Send htp get request
  if ((millis() - lastTime) > timerDelay) {
    //Check wifi connection status
    if(WiFi.status() == WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      Serial.println(serverPath);
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myobject = JSON.parse(jsonBuffer);
      if (JSON.typeof(myobject) == "undefined"){
        Serial.println("Parsing input failed!");
        return;
      }
      
    JSONVar temperature=myobject["main"]["temp"];
    JSONVar pressure=myobject["main"]["pressure"];
    JSONVar humidity=myobject["main"]["humidity"];
    JSONVar windspeed=myobject["wind"]["speed"];

    int tempThings = temperature;
    tempThings = tempThings-273;
    int pressureThings = pressure;
    int humidityThings = humidity;
    int windspeedThings = windspeed;

            ThingSpeak.setField(1, tempThings);
            ThingSpeak.setField(2, pressureThings);
            ThingSpeak.setField(3, humidityThings);
            ThingSpeak.setField(4, windspeedThings);
            
         int x = ThingSpeak.writeFields(Channel_ID, myWriteAPIKey);
         if(x == 200){
          Serial.println("Channel update successful"); 
         }
         else{
          Serial.println("Problem");
         }
       Serial.print("JSON object = ");
       Serial.print(myobject);
       Serial.print("Temperature: ");
       Serial.print(temperature);
       Serial.print("Pressure ");
       Serial.print(myobject["main"]["pressure"]);
       Serial.print("Humidity ");
       Serial.print(myobject["main"]["humidity"]);
       Serial.print("Wind Speed ");
       Serial.print(myobject["wind"]["speed"]);
    }
    else {
      Serial.println("WiFi disconnected");
    }
    lastTime = millis();
  }
}


String httpGETRequest(const char* serverName){
  WiFiClient client;
  HTTPClient http;
  //ESP8266HTTPClient http;
  http.begin(client,serverName);
  int httpResponseCode = http.GET();
  String payload = "{}";
  if (httpResponseCode>0){
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("error code ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return payload;
}
