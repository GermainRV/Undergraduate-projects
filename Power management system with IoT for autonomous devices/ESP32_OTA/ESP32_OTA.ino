#include <Wire.h>
//===============Importing MAX17043 sensor===============
#include <LiFuelGauge.h>
void lowPower();
LiFuelGauge gauge(MAX17043, 0, lowPower);
volatile boolean alert = false; // A flag to indicate a generated alert interrupt
//===============Importing DHT sensor===============
#include <DHTesp.h>
const int pinDHTdata = 13;
const int pinDHTsrc = 12;
DHTesp dhtSensor;
//===============Importing TEMT6000 sensor===============
#define TEMT6000 A0
const int pinTEMTsrc = 14;
//===============Importing PIR sensor===============
const int pinPIRdata = 27; // GPIO14
const int pinPIRsrc = 26;
//===============Importing LCD I2C===============
#include "lcd128_32_io.h"
lcd lcd(19, 18); //Create lCD128*32 pin，sda, scl

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>

#include "data.h"
const int pinLed = 2; // Pin number connected to the built-in LED
const int pinLoadsrc = 25; // Pin number connected to the load
WebServer server(80);
// Variables to store sensor readings
String MAXoutput;
String DHToutput;
String TEMToutput;
String PIRoutput;
float oldSOC = 0;
float newSOC;

struct OperationModes {
  bool mode1;
  bool mode2;
  bool mode3;
  bool mode4;
  bool mode5;
};
// Function prototypes
OperationModes operationModes(float socValue, float difSOC);

void setup() {
  pinMode(pinLed, OUTPUT); // initialize digital pin pinLed as an output.
  digitalWrite(pinLed, LOW);
  pinMode(pinLoadsrc, OUTPUT);  // initialize digital pin pinLoadsrc as an output
  digitalWrite(pinLoadsrc, HIGH);  // Encender el relé
  Wire.setClock(400000); // Set the desired I2C clock frequency here
  Serial.begin(115200);
  //===============Setting DHT11 sensor===============
  pinMode(pinDHTsrc, OUTPUT);
  digitalWrite(pinDHTsrc, HIGH);
  dhtSensor.setup(pinDHTdata, DHTesp::DHT11);
  //===============Setting TEMT6000 sensor===============
  pinMode(pinTEMTsrc, OUTPUT);
  digitalWrite(pinTEMTsrc, HIGH);
  pinMode(TEMT6000, INPUT);
  //===============Setting HC-SR501 sensor===============
  pinMode(pinPIRsrc, OUTPUT);
  digitalWrite(pinPIRsrc, HIGH);
  pinMode(pinPIRdata, INPUT);
  //===============Setting MAX17043 sensor===============
  Wire.begin();
  MAX17043_code(30);
  //===============Connecting to WiFi network===============
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("\nConnected to: "+ String(ssid));
  Serial.print("IP address: ");
  IPAddress ip = WiFi.localIP();
  String ipAddressString = ip.toString();
  Serial.println(ipAddressString);
  char charIPadress[ipAddressString.length() + 1];
  ipAddressString.toCharArray(charIPadress, sizeof(charIPadress));
  //===============Setting LCD I2C===============
  lcd.Init(); //initialize
  lcd.Clear();  //clear
  //===============Setting Arduino OTA===============
  // ArduinoOTA.setPort(3232); // Port defaults to 3232
  // ArduinoOTA.setHostname("myesp32"); // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setPassword("admin"); // No authentication by default
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
  ArduinoOTA.begin();

  //===============Setting WebServer===============
  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  server.on("/", HTTP_GET, []() {
    if (server.hasArg("DHTstate")){
      if (server.arg("DHTstate") == "on"){
      digitalWrite(pinDHTsrc, HIGH);
      digitalWrite(pinLed, HIGH); delay(100); digitalWrite(pinLed, LOW);
      }
      else if (server.arg("DHTstate") == "off"){
        digitalWrite(pinDHTsrc, LOW);
        digitalWrite(pinLed, HIGH); delay(100); digitalWrite(pinLed, LOW);
      }
    }
    if (server.hasArg("PIRstate")){
      if (server.arg("PIRstate") == "on"){
      digitalWrite(pinPIRsrc, HIGH);
      digitalWrite(pinLed, HIGH); delay(100); digitalWrite(pinLed, LOW);
      } 
      else if (server.arg("PIRstate") == "off"){
        digitalWrite(pinPIRsrc, LOW);
        digitalWrite(pinLed, HIGH); delay(100); digitalWrite(pinLed, LOW);
      }
    }
    if (server.hasArg("TEMTstate")){
      if (server.arg("TEMTstate") == "on"){
      digitalWrite(pinTEMTsrc, HIGH);
      digitalWrite(pinLed, HIGH); delay(100); digitalWrite(pinLed, LOW);
      } 
      else if (server.arg("TEMTstate") == "off"){
        digitalWrite(pinTEMTsrc, LOW);
        digitalWrite(pinLed, HIGH); delay(100); digitalWrite(pinLed, LOW);
      }
    }
    if (server.hasArg("pinLoad")){
      if (server.arg("pinLoad") == "on"){
        digitalWrite(pinDHTsrc, HIGH);
        digitalWrite(pinPIRsrc, HIGH);
        digitalWrite(pinTEMTsrc, HIGH);
        digitalWrite(pinLed, HIGH); delay(100); digitalWrite(pinLed, LOW);
      } 
      else if (server.arg("pinLoad") == "off"){
        digitalWrite(pinDHTsrc, LOW);
        digitalWrite(pinPIRsrc, LOW);
        digitalWrite(pinTEMTsrc, LOW);
        digitalWrite(pinLed, HIGH); delay(500); digitalWrite(pinLed, LOW);
      }
    }
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", ControlPageCode);
  });
  server.on("/LoginPage", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", LoginPageCode);
  });
  server.on("/UpdatePage", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", UpdatePageCode);
  });
  server.on("/SensorsPage", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", SensorsPageCode);
  });
  // server.on("/SensorsPage", handleSensorsPage);
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    Serial.println("Received firmware update request");
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}

void loop() {
  Serial.print("IP address: ");
  IPAddress ip = WiFi.localIP();
  String ipAddressString = ip.toString();
  Serial.println(ip);
  //===============Reading MAX17043 sensor===============
  // Read SOC and V values from the fuel gauge
  // delay(1000);
  float socValue = gauge.getSOC();
  float voltageValue = gauge.getVoltage();
  newSOC = socValue;
  float difSOC = newSOC - oldSOC;
  oldSOC = socValue;
  // Generate the battery information string
  MAXoutput = String("SOC: ") + String(socValue) + "%\t V: " + String(voltageValue) + "V";
  String MAXdisplay = String(socValue) + "%, " + String(voltageValue) + "V";
  if (alert){
    Serial.println("Beware, Low Power!");
    Serial.println("Finalizing operations...");
    gauge.clearAlertInterrupt();  // Resets the ALRT pin
    alert = false;
    Serial.println("Storing data...");
    Serial.println("Sending notification...");
    Serial.println("System operations are halted...");
    gauge.sleep();  // Forces the MAX17043 into sleep mode
    while ( true ) ;
  }
  //===============Reading DHT and TEMT sensor===============
  TempAndHumidity  dhtValues = dhtSensor.getTempAndHumidity();
  DHToutput = "Temp: " + String(dhtValues.temperature, 1) + "\u00B0C" + "\tHumidity: " + String(dhtValues.humidity, 0) + "%";
  String DHTdisplay = String(dhtValues.temperature, 1) + "°C, " + String(dhtValues.humidity, 0) + "%";

  float luxValue = TEMT6000_code();
  TEMToutput = "Illuminance: "+ String(luxValue) + " lx";
  String TEMTdisplay = String(luxValue) + " lx";
  //===============Reading PIR sensor===============
  int pirState = digitalRead(pinPIRdata);
  PIRoutput;
  PIRoutput = "---";
  // Verifica si se detectó movimiento
  if (pirState == HIGH) {
    PIRoutput = "Se detectó movimiento";
    // digitalWrite(pinLed, HIGH);   // turn the LED on (HIGH is the voltage level)
    // delay(100);
    // digitalWrite(pinLed, LOW); // turn the LED off by making the voltage LOW
  } else {
    PIRoutput = "Se detectó movimiento";
  Serial.println(PIRoutput);
        // delay(1000);                       // wait for a second
  }
  String output = MAXoutput + "\t" + DHToutput + "\t" + TEMToutput;
  Serial.println(output);

  OperationModes modes = operationModes(socValue, difSOC);

  /// Create the redirect URL with the mode values as query parameters
  // String redirectURL = "/SensorsPage";
  // redirectURL += "mode1=" + String(modes.mode1) + "&";
  // redirectURL += "mode2=" + String(modes.mode2) + "&";
  // redirectURL += "mode3=" + String(modes.mode3) + "&";
  // redirectURL += "mode4=" + String(modes.mode4) + "&";
  // redirectURL += "mode5=" + String(modes.mode5);

  // Send the redirect URL as a response to the client
  // server.sendHeader("Location", redirectURL, true);
  // server.send(200, "text/plain", "Redirecting to SensorsPage...");
  //===============Displaying outputs===============
  // lcd.Init(); //initialize
  // lcd.Clear();  //clear
  lcd.Cursor(0, 0); //Set display position
  char charIPadress[ipAddressString.length() + 1];
  ipAddressString.toCharArray(charIPadress, sizeof(charIPadress));
  lcd.Display(charIPadress);
  lcd.Cursor(1, 0);
  char charMAX[MAXdisplay.length() + 1]; // +1 for the null-terminator
  MAXdisplay.toCharArray(charMAX, sizeof(charMAX));
  lcd.Display(charMAX);
  lcd.Cursor(2, 0);
  char charDHT[DHTdisplay.length() + 1]; // +1 for the null-terminator
  DHTdisplay.toCharArray(charDHT, sizeof(charDHT));
  lcd.Display(charDHT);
  // lcd.WriteFont(96); // ASCII code 176 represents the degrees symbol
  // lcd.Display("C");
  lcd.Cursor(3, 0);
  char charTEMT[TEMTdisplay.length() + 1]; // +1 for the null-terminator
  TEMTdisplay.toCharArray(charTEMT, sizeof(charTEMT));
  lcd.Display(charTEMT);
  
  //===============Uploading webpage code===============
  ArduinoOTA.handle();
  // Replace the battery information placeholder in the serverIndex string
  String serverResponse = String(SensorsPageCode);
  serverResponse.replace("%MAX17043_VALUE%", MAXoutput);
  serverResponse.replace("%DHT11_VALUE%", DHToutput);
  serverResponse.replace("%TEMT6000_VALUE%", TEMToutput);
  serverResponse.replace("%PIR_VALUE%", PIRoutput);
  // char charNewPage[serverResponse.length() + 1];
  // serverResponse.toCharArray(charNewPage, sizeof(charNewPage));
  // Handle client requests
  
  // server.sendHeader("Content-Type", "text/html; charset=utf-8");
  // server.send(200, "text/html", serverResponse);
  // server.on("/SensorsPage", handleSensorsPage);
  // server.on("/SensorsPage", HTTP_GET, []() {
  //   server.sendHeader("Content-Type", "text/html; charset=utf-8");
  //   server.send(200, "text/html", serverResponse);
  // });
  server.handleClient();
  digitalWrite(pinLed, HIGH); delay(100); digitalWrite(pinLed, LOW);
  delay(1000);                       // wait for a second
}

OperationModes operationModes(float socValue, float difSOC) {
  /* 
  mode 1: Security mode
  mode 2: Battery discharge during sunlight
  mode 3: Battery charge during sunlight
  mode 4: Full operation mode
  mode 5: Full battery
  */
  OperationModes modes;
  modes.mode1 = false;
  modes.mode2 = false;
  modes.mode3 = false;
  modes.mode4 = false;
  modes.mode5 = false;

  int pinDHTstate = digitalRead(pinDHTsrc);
  int pinTEMTstate = digitalRead(pinTEMTsrc);
  int pinPIRstate = digitalRead(pinPIRsrc);
  if (socValue <= 30) {
    modes.mode1 = true;
    Serial.println("Mode 1");
    Serial.println("Warning! Battery SOC is less than 30%. Entering safety mode and reducing loads...");
    digitalWrite(pinDHTsrc, LOW);
    digitalWrite(pinPIRsrc, LOW);
    digitalWrite(pinTEMTsrc, LOW);
    digitalWrite(pinLed, HIGH); delay(100); digitalWrite(pinLed, LOW);
  } else if (difSOC < 0) {
    Serial.println("Mode 2");
    Serial.println("Battery is discharging...");
    modes.mode2 = true;
  } else if (difSOC > 0) {
    Serial.println("Mode 3");
    Serial.println("Battery is charging correctly...");
    modes.mode3 = true;
  } else if (pinDHTstate == HIGH && pinTEMTstate == HIGH && pinPIRstate == HIGH) {
    Serial.println("Mode 4");
    Serial.println("All loads are active...");
    modes.mode4 = true;
  } else if (socValue >= 90) {
    Serial.println("Mode 5");
    Serial.println("SOC battery has reached more than 90%...");
    modes.mode5 = true;
  }

  return modes;
}


void handleSensorsPage() {
  // Read sensor data and update the variables
  // Replace the following lines with your actual sensor reading code

  // Generate the HTML response with the updated sensor data
  String serverResponse = String(SensorsPageCode);
  serverResponse.replace("%MAX17043_VALUE%", MAXoutput);
  serverResponse.replace("%DHT11_VALUE%", DHToutput);
  serverResponse.replace("%TEMT6000_VALUE%", TEMToutput);
  serverResponse.replace("%PIR_VALUE%", PIRoutput);

  // Send the updated response to the client
  server.sendHeader("Content-Type", "text/html; charset=utf-8");
  server.send(200, "text/html", serverResponse);
}

void lowPower() { alert = true; }

void MAX17043_code(int AlertThreshold){
  // gauge.reset();  // Resets MAX17043
  delay(200);  // Waits for the initial measurements to be made
  // Sets the Alert Threshold to 10% of full capacity
  gauge.setAlertThreshold(AlertThreshold);
  Serial.println(String("Alert Threshold is set to ") + gauge.getAlertThreshold() + "%");
}

float TEMT6000_code(){
    // Light Reading - TEMT6000
    analogReadResolution(10);
    float volts =  analogRead(TEMT6000) * 5 / 1024.0; // Convert reading to VOLTS
    float VoltPercent = analogRead(TEMT6000) / 1024.0 * 100; //Reading to Percent of Voltage
    //Conversions from reading to LUX
    float amps = volts / 10000.0;  // em 10,000 Ohms
    float microamps = amps * 1000000; // Convert to Microamps
    float lux = microamps * 2.0; // Convert to Lux 
    return lux;
}