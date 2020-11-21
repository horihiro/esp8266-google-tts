#ifndef GoogleTTS_h
#define GoogleTTS_h

#include <WiFiClientSecure.h>
#ifdef ARDUINO_ARCH_ESP8266
#include "esp8266sdk_version.h"
#endif

#define LIB_NAME "GoogleTTS for ESP8266"
#define LIB_VERSION "1.0.7"

#define HOST_GTRANS "translate.google.com"
#define PATH_GTRANS "/translate_tts"
#define FINGERPRINT_GTRANS "F8:1E:31:71:FA:08:5B:C0:4C:83:B6:64:4B:9F:22:9F:0C:BA:8E:57"

typedef class GoogleTTS
{

private:
  WiFiClientSecure *m_pClient = nullptr;
  unsigned long m_lastTimestamp = 0;
  String m_tkk;

  String getTKK(int maxRetry, int currentRetry);
  String createToken(const char *text, const char *key);
  String urlencode(String str);
  char *lltoa(long long val, int base);
  long long int XL(long long int a, char *b);

public:
  void setWiFiClientSecure(WiFiClientSecure *pClient);
  String getSpeechUrl(String text, String lang, int maxRetryCount);
  String getSpeechUrl(String text, String lang)
  {
    return getSpeechUrl(text, lang, 10);
  };
  String getSpeechUrl(String text)
  {
    return getSpeechUrl(text, "en", 10);
  }
} TTS;

#endif
