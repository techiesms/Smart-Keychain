/*
  WiFiTelnetToSerial - Example Transparent UART to Telnet Server for esp8266
*/
#include <ESP8266WiFi.h>

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 1
const char* ssid = "Your SSID";
const char* password = "Your Password";
bool togg = 0;

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to "); Serial.println(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if (i == 21) {
    Serial.print("Could not connect to"); Serial.println(ssid);

  }
  //start UART and the server
  Serial.begin(115200);
  server.begin();
  server.setNoDelay(true);

  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect");
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
}

void loop()
{

  uint8_t i;
  //check if there are any new clients
  if (server.hasClient())
  {
    for (i = 0; i < MAX_SRV_CLIENTS; i++)
    {
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected())
      {
        if (serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        Serial.print("New client: "); Serial.print(i);
        continue;
      }
    }
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }


  //check clients for data
  for (i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      if (serverClients[i].available())
      {
        //get data from the telnet client and push it to the UART
        while (serverClients[i].available())
          Serial.write(serverClients[i].read());
        uint8_t sig = serverClients[i].read();
        Serial.print("THIS: ");
        Serial.println(sig);
        if (sig == 255)
          digitalWrite(BUILTIN_LED, togg);
        togg = !togg;

      }
    }
  }
}
