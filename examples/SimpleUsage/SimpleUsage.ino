#include <google-tts.h>
#include <ESP8266WiFi.h>

const char* ssid     = "<REPLASE_YOUR_WIFI_SSID>";
const char* password = "<REPLASE_YOUR_WIFI_PASSWORD>";

String getMacAddress() {
  byte mac[6];

  WiFi.macAddress(mac);
  String cMac = "";
  for (int i = 0; i < 6; ++i) {
    if (mac[i] < 0x10) {
      cMac += "0";
    }
    cMac += String(mac[i], HEX);
    if (i < 5)
      cMac += ":"; // put : or - if you want byte delimiters
  }
  cMac.toUpperCase();
  return cMac;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("");
//  Serial.println(String(b>>7));
  WiFi.mode(WIFI_STA);  WiFi.begin(ssid, password);
  Serial.print("MAC: ");
  Serial.println(getMacAddress());
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  TTS tts;
  Serial.println(tts.getSpeechUrl("こんにちは、世界！", "ja"));
  Serial.println(tts.getSpeechUrl("Hello, World!"));

}

void loop() {
  // put your main code here, to run repeatedly:

}
