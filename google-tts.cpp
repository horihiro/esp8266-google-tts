#include "google-tts.h"

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

  WiFiClientSecure client;
  if (!client.connect(HOST_GTRANS, 443)) {
    Serial.println("connection failed");
    return "ERROR";
  }

  // if (client.verify(FINGERPRINT_GTRANS, HOST_GTRANS)) {
  //   Serial.println("certificate matches");
  // } else {
  //   Serial.println("certificate doesn't match");
  // }
  client.print(
    String("GET / HTTP/1.1\r\n") +
    "Host: " + HOST_GTRANS + "\r\n" +
    "User-Agent: " + LIB_NAME + "/" + LIB_VERSION + "\r\n" +
    "Accept: */*\r\n\r\n");
  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return "_TIMEOUT";
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\r');
    int tkkpos = line.indexOf("TKK=eval");
    if (tkkpos >= 0) {
      int termpos = line.indexOf(");", tkkpos) + 2;
      client.stop();
      line = line.substring(tkkpos, termpos);
      int head = line.indexOf("3d") + 2;
      int tail = line.indexOf(";", head);
      char* buf;
      unsigned long a = strtoul(line.substring(head, tail).c_str(), &buf, 10);
      head = line.indexOf("3d", tail) + 2;
      tail = line.indexOf(";", head);
      unsigned long b = strtoul(line.substring(head, tail).c_str(), &buf, 10);
      head = line.indexOf("return ", tail) + 7;
      tail = line.indexOf("+", head);
      String c = line.substring(head, tail);
      return (c + "." + String(a + b));
    }
  }
  client.stop();
  return "_ERROR";
}

String GoogleTTS::getSpeechUrl(String text, String lang) {
  String tkk = this->getTKK();
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


