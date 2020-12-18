#ifndef GoogleTTS_h
#define GoogleTTS_h

#include <WiFiClientSecure.h>
#ifdef ARDUINO_ARCH_ESP8266
#include "esp8266sdk_version.h"
#endif

#define LIB_NAME "GoogleTTS for ESP8266"
#define LIB_VERSION "1.0.10"

#define HOST_GTRANS "translate.google.com"
#define PATH_GTRANS "/translate_tts"
#define FINGERPRINT_GTRANS "F8:1E:31:71:FA:08:5B:C0:4C:83:B6:64:4B:9F:22:9F:0C:BA:8E:57"

typedef class GoogleTTS
{

private:
  String urlencode(String str);

public:
  String getSpeechUrl(String text, String lang);
  String getSpeechUrl(String text)
  {
    return getSpeechUrl(text, "en");
  }
} TTS;

#endif
