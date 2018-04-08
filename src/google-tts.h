#ifndef GoogleTTS_h
#define GoogleTTS_h

#include <WiFiClientSecure.h>

#define LIB_NAME "GoogleTTS for ESP8266"
#define LIB_VERSION "1.0.2"

#define HOST_GTRANS "translate.google.com"
#define PATH_GTRANS "/translate_tts"
#define FINGERPRINT_GTRANS "51:95:BF:29:CC:79:4B:FE:E2:0B:0F:EA:B5:95:1B:5A:0A:91:AB:39"

typedef class GoogleTTS {

private:
  unsigned long m_lastTimestamp = 0;
  String m_tkk;

  String getTKK();
  String createToken(const char* text, const char* key);
  String urlencode(String str);
  char* lltoa(long long val, int base);
  long long int  XL (long long int a, char* b);
  
public:
  String getSpeechUrl(String text, String lang);
  String getSpeechUrl(String text) {
    return getSpeechUrl(text, "en");
  }
} TTS;

#endif
