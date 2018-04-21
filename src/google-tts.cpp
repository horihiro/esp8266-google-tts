#include "google-tts.h"

#define HTTP_TCP_BUFFER_SIZE (1460)

String GoogleTTS::urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
    
}

char* GoogleTTS::lltoa(long long val, int base){

    static char buf[64] = {0};

    int i = 62;
    int sign = (val < 0);
    if(sign) val = -val;

    if(val == 0) return "0";

    for(; val && i ; --i, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }

    if(sign) {
        buf[i--] = '-';
    }
    return &buf[i+1];

}

long long int GoogleTTS::XL (long long int a, char* b) {
  int len = strlen(b);
  for (int c = 0; c < len - 2; c += 3) {
    int  d = (long long int)b[c + 2];
    d = d >= 97 ? d - 87 : d - 48;
    d = (b[c + 1] == '+' ? a >> d : a << d);
    a = b[c] == '+' ? a + d & 4294967295 : a ^ d;
  }
  return a;
}

String GoogleTTS::createToken(const char* text, const char* key) {
  String keyStr = String(key);
  int periodPos = keyStr.indexOf('.');
  String key1 = keyStr.substring(0,periodPos);
  String key2 = keyStr.substring(periodPos + 1);
  long long int a, b;
  a = b = strtoll(key1.c_str(), NULL, 10);

  int f, g;

  int len = strlen(text);
  for (f = 0; f < len; f++) {
    a += text[f];
    a = XL(a, "+-a^+6");
  }
  a = XL(a, "+-3^+b+-f");
  a = a ^ (strtoll(key2.c_str(), NULL, 10));
  if (0 > a) {
    a = (a & 2147483647) + 2147483648;
  }
  a = a % 1000000;
  return String(lltoa(a, 10)) + '.' + lltoa(a ^ b, 10);
}

String GoogleTTS::getTKK() {
  Serial.print("TTS is :");
  Serial.println(m_tkk);
  unsigned long current = millis();
  if (m_tkk.length() != 0
  //  && millis() > m_lastTimestamp
  //  && (
  //    (m_lastTimestamp < m_lastTimestamp + TTK_EXPIRE && current < m_lastTimestamp + TTK_EXPIRE)
  //     || (current - TTK_EXPIRE < current && current - TTK_EXPIRE < m_lastTimestamp)
  ) {
    return m_tkk;
  }

  WiFiClientSecure client;
  if (!client.connect(HOST_GTRANS, 443)) {
    Serial.println("connection failed");
    return "_ERROR";
  }

  // if (client.verify(FINGERPRINT_GTRANS, HOST_GTRANS)) {
  //   Serial.println("certificate matches");
  // } else {
  //   Serial.println("certificate doesn't match");
  // }
  client.print(
    String("GET / HTTP/1.0\r\n") +
    "Host: " + HOST_GTRANS + "\r\n" +
    "User-Agent: " + LIB_NAME + "/" + LIB_VERSION + "\r\n" +
    "Accept-Encoding: identity\r\n" +
    "Accept: text/html\r\n\r\n");
  int timeout = millis() + 5000;
  client.flush();

  while (client.available() == 0) {
    if (timeout < millis()) {
      client.stop();
      return "_TIMEOUT";
    }
  }
  String line = "";
  boolean isHeader = true;
  do {
    line = client.readStringUntil('\r');
    line.trim();
    if (line.length() == 0) {
      isHeader = false;
    }
    if (isHeader) continue;

    String tkkFunc = "";
    char ch;
    do {
      tkkFunc = "";
      client.readBytes(&ch, 1);
      if (ch != 'T') continue;
      tkkFunc += String(ch);
      client.readBytes(&ch, 1);
      if (ch != 'K') continue;
      tkkFunc += String(ch);
      client.readBytes(&ch, 1);
      if (ch != 'K') continue;
      tkkFunc += String(ch);
    } while(tkkFunc.length() < 3);

    tkkFunc +=  client.readStringUntil('V');
    client.stop();

    int head = tkkFunc.indexOf("3d") + 2;
    int tail = tkkFunc.indexOf(";", head);
    char* buf;
    unsigned long a = strtoul(tkkFunc.substring(head, tail).c_str(), &buf, 10);
    head = tkkFunc.indexOf("3d", tail) + 2;
    tail = tkkFunc.indexOf(";", head);
    unsigned long b = strtoul(tkkFunc.substring(head, tail).c_str(), &buf, 10);
    head = tkkFunc.indexOf("return ", tail) + 7;
    tail = tkkFunc.indexOf("+", head);
    String c = tkkFunc.substring(head, tail);
    m_tkk = (c + "." + String(a + b));
    m_lastTimestamp = millis();
    return m_tkk;
    delay(0);
  } while(line.length() > 0);
  client.stop();
  return "_ERROR";
}

String GoogleTTS::getSpeechUrl(String text, String lang) {
  String tkk = this->getTKK();
  if (tkk.indexOf("_") == 0) {
    return tkk;
  }
  String token = this->createToken(text.c_str(), tkk.c_str());
  int i,j;
  const char* t = text.c_str();
  for(i=0,j=0;i<strlen(t);i++) {
    if (t[i] < 0x80 || t[i] > 0xbf) {
      j++;
    }
  }

  return String("https://") + HOST_GTRANS + PATH_GTRANS + "?ie=UTF-8&q=" + this->urlencode(text) + "&tl=" + lang + "&textlen=" + String(j) + "&tk=" + token + "&total=1&idx=0&client=t&prev=input&ttsspeed=1";
}


