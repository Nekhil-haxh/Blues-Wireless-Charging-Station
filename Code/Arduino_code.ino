#include <WiFi.h>
#include <WebServer.h>
#include <Notecard.h>
#include "webpage.h"
#include <ZMPT101B.h>

const int ACPin = A1;         //set arduino signal read pin
#define ACTectionRange 20    //set Non-invasive AC Current Sensor tection range (5A,10A,20A)
#define VREF 3.3
#define SENSITIVITY 500.0f
int relayPin = D6;

// ZMPT101B sensor output connected to analog pin A0
// and the voltage source frequency is 50 Hz.
ZMPT101B voltageSensor(A0, 50.0);

float totalEnergy = 0.0;
unsigned long startTime, elapsedTime;
bool authenticated = false;
bool charging = false;
float credit_used = 0.0;
String username;

unsigned long energyCalcInterval = 1000; // Energy calculation interval in milliseconds
unsigned long lastEnergyCalcTime = 0;

const char* ssid = "ESP"; // SSID of the Access Point
const char* password = "8848191317"; // Password for the Access Point
WebServer server(80); // Create a web server listening on port 80

Notecard notecard;

#define PRODUCT_UID "com.gmail.nekiary07:charging_station"

struct UserData 
{
    String password;
    float credit;
};


// Including the required Arduino libraries
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Uncomment according to your hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

// Defining size, and output pins
#define MAX_DEVICES 3
#define CS_PIN D7

// Create a new instance of the MD_Parola class with hardware SPI connection
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


void handleRoot() {
  Serial.println("New client connected.");
  String header = String(style);
  String body = String(home);
  server.send(200, "text/html", header+body);
}

UserData getData(String uname){
    UserData userData;
    String name = "user/"+uname+".json";

    J *get = NoteNewRequest("web.get");
    JAddStringToObject(get, "route", "UserInfo");
    JAddStringToObject(get, "name", name.c_str());

    J *rsp = notecard.requestAndResponse(get);
    if (rsp != NULL)
    {  
        J* body = JGetObject(rsp, "body");
        J* credit = JGetObject(body,"credit");
        userData.password = JGetString(body, "password");
        userData.credit = JGetNumber(credit, "creditvalue");
        notecard.deleteResponse(rsp);
        String message = uname + ":" + userData.password;
        Serial.println(message);
        return userData;
    }
}

void handleFormSubmit() {
  if (server.method() == HTTP_POST) {
    username = server.arg("username");
    String password = server.arg("password");
    Serial.print("Entered Username: ");
    Serial.println(username);
    Serial.print("Entered Password: ");
    Serial.println(password);
    UserData fb_info = getData(username);
    if (fb_info.password != ""){
      if (fb_info.password == password){
        String header = String(style);
        String body = String(start);
        body.replace("{{username}}", username);
        body.replace("{{credit}}", String(fb_info.credit));
        server.send(200, "text/html", header+body);
        authenticated = true;
      }
      else{
        server.send(200, "text/plain", "Incorrect Credentials");
      }
    }
    else{
      server.send(200, "text/plain", "User Not Found!");
    }
  } 
}

float readACCurrentValue()
{
  float ACCurrtntValue = 0;
  float peakVoltage = 0;  
  float voltageVirtualValue = 0;  //Vrms
  for (int i = 0; i < 20; i++)
  {
    peakVoltage += analogRead(ACPin);   //read peak voltage
    delay(1);
  }
  peakVoltage = peakVoltage / 20;   
  voltageVirtualValue = peakVoltage * 0.707;    //change the peak voltage to the Virtual Value of voltage

  /*The circuit is amplified by 2 times, so it is divided by 2.*/
  voltageVirtualValue = (voltageVirtualValue / 4096 * VREF ) / 2;  

  ACCurrtntValue = voltageVirtualValue * ACTectionRange;

  return ACCurrtntValue;
}

void calculateEnergy() 
{
  if (charging){
    unsigned long currentTime = millis();
    // Check if it's time to calculate energy consumption
    if (currentTime - lastEnergyCalcTime >= energyCalcInterval) {
      lastEnergyCalcTime = currentTime;
      float ACCurrentValue = readACCurrentValue();
      float voltage = voltageSensor.getRmsVoltage();
      float power = voltage * ACCurrentValue;
      float energyConsumed = power * (energyCalcInterval / 3600000.0); // Convert milliseconds to hours
      totalEnergy += energyConsumed;
  
      credit_used = totalEnergy*1.0;
  
      // Print or handle energy consumption data
      Serial.printf("Current: %f\n", ACCurrentValue);
      Serial.printf("Voltage: %f\n", voltage);
      Serial.print("Energy Consumed: ");
      Serial.print(totalEnergy);
      Serial.println(" Wh");
      myDisplay.setTextAlignment(PA_CENTER);
      myDisplay.print(String(totalEnergy));
      delay(1);
    }
  }
}


void handleStart() {
  if (!authenticated){
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain","");
    return;
  }
  String header = String(style);
  String body = String(progress);
  server.send(200, "text/html", header+body);

  if (!charging){
    charging = !charging;
    digitalWrite(relayPin, LOW);
    Serial.println("Turning on relay");
  }
}

void putData(String username, float credit){
  String name = "user/"+username+"/credit.json";
  J *put =  notecard.newRequest("web.put");
  JAddStringToObject(put, "route", "UserInfo");
  JAddStringToObject(put, "name", name.c_str());

  J *body = JCreateObject();
  JAddNumberToObject(body, "creditvalue", credit);
  JAddItemToObject(put, "body", body);
  notecard.sendRequest(put);
  delay(100);
}

void handleStop(){
  if (!authenticated){
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain","");
    return;
  }
  
  if (charging)
  {
    charging = !charging;
    digitalWrite(relayPin, HIGH);
    Serial.println("Turning off relay");
  }
  totalEnergy = 0.0;
  UserData fb = getData(username);
  float credit_left = fb.credit - credit_used;
  putData(username, credit_left);
  String header = String(style);
  String body = String(stop);
  body.replace("{{credit}}", String(credit_left));
  server.send(200, "text/html", header+body);
  myDisplay.displayClear();
}

void handleLogout()
{
  authenticated = !authenticated;
  server.send(200, "text/plain", "");
}

void setup() 
{
  Serial.begin(115200);
  delay(1000);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  voltageSensor.setSensitivity(SENSITIVITY);
  notecard.setDebugOutputStream(Serial);
  notecard.begin();

  J *restore = notecard.newRequest("card.restore");
  JAddBoolToObject(restore, "delete", true);
  notecard.sendRequest(restore);
  delay(5000);

  J *req = notecard.newRequest("hub.set");
  JAddStringToObject(req, "product", PRODUCT_UID);
  JAddStringToObject(req, "mode", "continuous");
  JAddBoolToObject(req, "sync", true);
  JAddNumberToObject(req, "inbound", 1);
  JAddNumberToObject(req, "outbound", 1);
  notecard.sendRequestWithRetry(req, 5);
  delay(2000);

  // Set up Access Point
  WiFi.softAP(ssid, password);
  delay(100);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/submit", HTTP_POST, handleFormSubmit);
  server.on("/start", HTTP_POST, handleStart);
  server.on("/stop", HTTP_POST, handleStop);
  server.on("/logout", HTTP_POST, handleLogout);

  server.begin(); // Start the web server
   // Intialize the object
  myDisplay.begin();
  // Set the intensity (brightness) of the display (0-15)
  myDisplay.setIntensity(0);
  // Clear the display
  myDisplay.displayClear();

}

void loop() 
{
  server.handleClient();
  calculateEnergy();
}
