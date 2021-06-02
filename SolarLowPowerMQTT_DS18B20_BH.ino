
//ESP8266 Solar Powered Temp Sensor/MQTT
//by Bill Poulsen iot-techshop.com
//Join our FB Group at: https://www.facebook.com/groups/3188341227932417/

//Sends Data to MQTT Server over wifi
//Auto-Adjust Intervals based on Battery Voltage to conserve power
//Uses NTP Server to obtain time date stamps
//Device ID is auto filled using ESP unique MAC address
//MQTT Data String Format:    {"ID":"DC4F2211E835","time":"20200806_110320","devTemp":"81.72","vBat:","3.45","rInt:","60"}
//DS18B20 Temp; myString = "{\"ID\":\"" + devID + "\",\"time\":\"" + myDateTime + "\",\"" + "devTemp" + "\":\"" + myTemp + "\",\"" + "vBat" + "\":\"" + vBat + "\",\"" + "rInt" + "\":\"" + rInt +"\"}";
//DHT11bTemp/Humid; myString = "{\"ID\":\"" + devID + "\",\"tim\":\"" + myDateTime + "\",\"" + "Tmp" + "\":\"" + myTemp + "\",\"" + "Hum" + "\":\"" + humid + "\",\"" + "soil" + "\":\"" + moist + "\",\"" + "Bat" + "\":\"" + vBat + "\",\"" + "Int" + "\":\"" + rInt + "\"}";
//rInt is in seconds
//Hardware Config
//D2 Temp Pin
//To enable wakeup from sleep jumper RST to the D0(GPIO16) pin on your ESP8266.  The RTC generates a reset pulse on that
//pin which is used to restart the device.  The simple concept is that you can use a F/W command to put the device into 
//into sleepmode, but need a hardware pulse to wake it up again. Wakeup acts like pressing your reset button. 

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define RETRY_COUNT       10
String devID = "";
String myMac = "";
String myString = "";
String myTemp;
String vBat = "";
float volts = 0.0;
float vCal = .044; // Voltage Check Calibration Factor dependent on Analog In series resistor( 150K Resistor)
//NTP Stuff
String dateTime;
int xCnt = 0;
long lastMsg = 0;
long lastRefresh = 0;
char msg[100];
int value = 0;
int rInt =20;
#define LED D1
const int oneWireBus = D2; // GPIO where the DS18B20 is connected to
OneWire oneWire(oneWireBus); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature sensor
WiFiUDP ntpUDP;
// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "us.pool.ntp.org", 3600, 60000);
// Update these with values suitable for your network.
const char* ssid = "xxxxxx";   //Use Your Wifi SSID  Note: ESP's only work on 2,4GHz wifi, 
//be sure to have one active on your Router
const char* password = "xxxxxxx"; //Use your WiFi Password
const char* mqtt_server = "xxxxxx";  //You can use any mqt server URL
const char* mqtt_topic = "bill/Solar/temp"; //Set MQTT Topic
//Serial Messages/debug
#define serial_out
WiFiClient espClient;
PubSubClient client(espClient);
void setup() {  
  pinMode(LED, OUTPUT);     
#ifdef serial_out
  Serial.begin(115200);
  Serial.println();
  Serial.println();
#endif
  sensors.begin();
  volts = analogRead(A0);
  myMac = WiFi.macAddress();
  //Creates device ID from ESP01 MAC address
  devID = myMac.substring(0, 2) +  myMac.substring(3, 5) +  myMac.substring(6, 8) +  myMac.substring(9, 11) +  myMac.substring(12, 14) +  myMac.substring(15, 17);
#ifdef serial_out
  Serial.print("MAC: ");
  Serial.println(myMac);
  Serial.println(devID);
#endif
  // using getTemp 2X to get a clean tmp sensor output Stable after that
  getTemp("F");
  delay(100);
  getTemp("F");
  delay(100);  
  setup_wifi();
  timeRefresh();//Gets clean NTP Time
  delay(500);
  timeRefresh();//Gets clean NTP Time
  client.setServer(mqtt_server, 1883);
}
void setup_wifi() {
  delay(10);
#ifdef serial_out
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
#endif
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
#ifdef serial_out
    Serial.print(".");
#endif
  }
#ifdef serial_out
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}
void callback(char* topic, byte* payload, unsigned int length) {
#ifdef serial_out
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
#endif
  for (int i = 0; i < length; i++) {
#ifdef serial_out
    Serial.print((char)payload[i]);
#endif
  }
#ifdef serial_out
  Serial.println();
#endif 
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
#ifdef serial_out
    Serial.print("Attempting MQTT connection...");
#endif
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
#ifdef serial_out
      Serial.println("connected");
#endif
    } else {
#ifdef serial_out
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
#endif
      // Wait 3 seconds before retrying
      delay(3000);
    }
  }
}
void loop() {
volts = (analogRead(A0) * vCal * .205);
if(volts <2.0){
  digitalWrite(LED, HIGH);
  rInt = 300;
}
if(volts >= 2.0){ 
  rInt = 60;
}
  String dataString = "";
  String myDateTime = "";
  dateTime = timeClient.getFormattedDate();
  myDateTime = dateTime.substring(0, 4) + dateTime.substring(5, 7) + dateTime.substring(8, 10) + "_" + dateTime.substring(11, 13) + dateTime.substring(14, 16) + dateTime.substring(17, 19);
  myTemp = String(getTemp("F"));  
  vBat = String(volts);  
  myString = "";  
  myString = "{\"ID\":\"" + devID + "\",\"time\":\"" + myDateTime + "\",\"" + "devTemp" + "\":\"" + myTemp + "\",\"" + "vSol" + "\":\"" + vBat + "\",\"" + "rInt" + "\":\"" + rInt + "\"}";
  myString.toCharArray(msg, myString.length() + 1);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
#ifdef serial_out
  Serial.print("Publish message: ");
  Serial.println(msg);
#endif
  client.publish(mqtt_topic, msg);
  espClient.stop();
  /* Put device into deep sleep.
     Although without hardware mod it will never wake up from this! */
digitalWrite(LED, LOW);
#ifdef serial_out
  Serial.println("Time to sleep!");
#endif
  ESP.deepSleep((rInt - 10) * 1000000);//Note that we subtract 10 from the sleep time as a calibration factor
  // to get a more consistant report interval. Not needed if report intervals are not critical(meaning off by seconds is ok)
  // WiFi connect times can vary to effect the actual reporting intervals as well  
}
// tType "F" = farenheight, "C" = Celcius
float getTemp(String tType) {
  float temperature = 0.0;
  sensors.requestTemperatures();
  if (tType == "C") {
    temperature = sensors.getTempCByIndex(0);
     }
  if (tType == "F") {
    temperature = sensors.getTempFByIndex(0);
  }
  return temperature;
}
void timeRefresh() {
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 100 );    
#ifdef serial_out
    Serial.print ( "." );
#endif
  }
  timeClient.setTimeOffset(-14400);
  timeClient.begin();
  timeClient.update();
  dateTime = timeClient.getFormattedDate();  
#ifdef serial_out
  Serial.print("Time Refresh: DateTime=");
  Serial.println(dateTime);
  Serial.println();
#endif
}
