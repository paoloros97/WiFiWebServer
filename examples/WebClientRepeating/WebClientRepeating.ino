/****************************************************************************************************************************
    WebClientRepeating.ino - Simple Arduino web server sample for SAMD21 running WiFiNINA shield
    For any WiFi shields, such as WiFiNINA W101, W102, W13x, or custom, such as ESP8266/ESP32-AT, Ethernet, etc
    
    WiFiWebServer is a library for the ESP32-based WiFi shields to run WebServer
    Based on and modified from ESP8266 https://github.com/esp8266/Arduino/releases
    Based on  and modified from Arduino WiFiNINA library https://www.arduino.cc/en/Reference/WiFiNINA
    Built by Khoi Hoang https://github.com/khoih-prog/WiFiWebServer
    Licensed under MIT license
    
    A simple web server that shows the value of the analog input pins via a web page using an ESP8266 module.
    This sketch will start an access point and print the IP address of your ESP8266 module to the Serial monitor.
    From there, you can open that address in a web browser to display the web page.
    The web page will be automatically refreshed each 20 seconds.
    
    For more details see: http://yaab-arduino.blogspot.com/p/wifiesp.html
    
    Version: 1.0.6
    
    Version Modified By   Date      Comments
    ------- -----------  ---------- -----------
    1.0.0   K Hoang      12/02/2020 Initial coding for SAMD21, Nano 33 IoT, etc running WiFiNINA
    1.0.1   K Hoang      28/03/2020 Change to use new WiFiNINA_Generic library to support many more boards running WiFiNINA
    1.0.2   K Hoang      28/03/2020 Add support to SAMD51 and SAM DUE boards
    1.0.3   K Hoang      22/04/2020 Add support to nRF52 boards, such as AdaFruit Feather nRF52832, nRF52840 Express, BlueFruit Sense, 
                                    Itsy-Bitsy nRF52840 Express, Metro nRF52840 Express, NINA_B30_ublox, etc. 
    1.0.4   K Hoang      23/04/2020 Add support to MKR1000 boards using WiFi101 and custom WiFi libraries.
    1.0.5   K Hoang      21/07/2020 Fix bug not closing client and releasing socket.    
    1.0.6   K Hoang      24/07/2020 Add support to all STM32F/L/H/G/WB/MP1 and Seeeduino SAMD21/SAMD51 boards. Restructure examples 
 ***************************************************************************************************************************************/
#include "defines.h"

int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "arduino.cc";

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10000L; // delay between updates, in milliseconds

// Initialize the Web client object
WiFiClient client;

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  // you're connected now, so print out the data
  Serial.print(F("You're connected to the network, IP = "));
  Serial.println(WiFi.localIP());

  Serial.print(F("SSID: "));
  Serial.print(WiFi.SSID());

  // print the received signal strength:
  int32_t rssi = WiFi.RSSI();
  Serial.print(F(", Signal strength (RSSI):"));
  Serial.print(rssi);
  Serial.println(F(" dBm"));
}

// this method makes a HTTP connection to the server
void httpRequest()
{
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 80))
  {
    Serial.println(F("Connecting..."));

    // send the HTTP PUT request
    client.println(F("GET /asciilogo.txt HTTP/1.1"));
    client.println(F("Host: arduino.cc"));
    client.println(F("Connection: close"));
    client.println();

    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else
  {
    // if you couldn't make a connection
    Serial.println(F("Connection failed"));
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nStarting WebClientRepeating on " + String(BOARD_NAME));

  // check for the presence of the shield
#if USE_WIFI_NINA
  if (WiFi.status() == WL_NO_MODULE)
#else
  if (WiFi.status() == WL_NO_SHIELD)
#endif
  {
    Serial.println(F("WiFi shield not present"));
    // don't continue
    while (true);
  }

#if USE_WIFI_NINA
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware");
  }
#endif

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED)
  {
    Serial.print(F("Connecting to SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  printWifiStatus();
}

void loop()
{
  // if there's incoming data from the net connection send it out the serial port
  // this is for debugging purposes only
  while (client.available())
  {
    char c = client.read();
    Serial.write(c);
  }

  // if 10 seconds have passed since your last connection,
  // then connect again and send data
  if (millis() - lastConnectionTime > postingInterval)
  {
    httpRequest();
  }
}
