#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

char ssid[] = "ist_members";
char pass[] = "8gAp3nY!s2Gm";

const char* ntp_server = "ntp.nict.jp";
const char* host = "iot.hongo.wide.ad.jp";
const int port = 40006;
unsigned long last_sync_time = 0;


#define OLED_RESET 2
Adafruit_SSD1306 display(OLED_RESET);

void setup() {

  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Connecting to ");
  display.println(ssid);
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  delay(15000);
  if (WiFi.status() != WL_CONNECTED) {
    display.println("WiFi error");
    display.display();
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi connected");
  display.print("IP address ");
  display.println(WiFi.localIP());
  display.display();

  delay(3000);

  pinMode(16, INPUT);
  pinMode(A0, INPUT);

  boolean time = syncNTPTime(ntp_server);

  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("Sync to ");
  display.println(ntp_server);
  display.display();

  unsigned long t = now();

  if (time == false) {
    display.println("time error");
    display.display();
  } else {
    char str_time[30];
    sprintf(str_time, "%04d-%02d-%02dT%02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));
    display.println(str_time);
    display.display();
    delay(3000);
  }


}

char send_buf[100];
String response;

void loop() {

  WiFiClient client;

  if (!client.connect(host, port)) {
    display.println("...ERR\r\n");
    display.display();
    delay(3000);
    return;
  }

  while (true) {
    unsigned long t = now();
    char str_time[30];
    sprintf(str_time, "%04d-%02d-%02dT%02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));
    int id = getDIPSWStatus();
    int illuminance = getIlluminance();
    int MDStatus = getMDStatus();

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("ID:");
    display.println(id);
    display.print("time:");
    display.println(str_time);
    display.print("Illuminance:");
    display.println(illuminance);
    display.print("MDStatus");
    display.println(MDStatus);
    display.display();

    last_sync_time = t;

    sprintf(send_buf, "%d,%s,%d,%d", id, str_time, illuminance, MDStatus);

    client.print(send_buf);
    display.println("send to the server!");
    display.display();
    if (!client.connected()) {
      client.stop();
      display.println("break");
      display.display();
      break;
    }

    response = client.readStringUntil('\n');
    display.println(response);
    display.display();
    delay(5000);
  }
}

int getDIPSWStatus() {
  int setNum = 0;
  int bit1 = digitalRead(12);
  int bit0 = digitalRead(13);
  if (bit0 == LOW) {
    setNum |= 0x01;
  }
  if (bit1 == LOW) {
    setNum |= 0x02;
  }
  return setNum;
}

int getIlluminance() {
  float score = analogRead(A0);
  float voltage = score / 1024.0;
  float currency = (voltage * 3.2 / 1000.0) + (voltage / 100000.0);
  float illumi = currency / 0.000003;
  return (int)illumi;
}

void setBZ(boolean on) {
  if (on == true) {
    digitalWrite(14, HIGH);
  } else {
    digitalWrite(14, LOW);
  }
}

boolean getPushSWStatus() {
  int state = digitalRead(2);
  if (state == HIGH) {
    return false;
  } else {
    return true;
  }
}

boolean detectPushSWON() {
  static int prev_state = LOW;
  int state = digitalRead(2);
  if (prev_state == HIGH && state == LOW) {
    prev_state = state;
    return true;
  } else {
    prev_state = state;
    return false;
  }
}

boolean getMDStatus() {
  int status = digitalRead(16);
  if (status == HIGH) {
    return true;
  } else {
    return false;
  }
}

unsigned long getNTPTime(const char* ntp_server) {
  WiFiUDP udp;
  udp.begin(8888);
  unsigned long unix_time = 0UL;
  byte packet[48];
  memset(packet, 0, 48);
  packet[0] = 0b11100011;
  packet[1] = 0;
  packet[2] = 6;
  packet[3] = 0xEC;
  packet[12] = 49;
  packet[13] = 0x4E;
  packet[14] = 49;
  packet[15] = 52;

  udp.beginPacket(ntp_server, 123);
  udp.write(packet, 48);
  udp.endPacket();

  for (int i = 0; i < 10; i++) {
    delay(500);
    if (udp.parsePacket()) {
      udp.read(packet, 48);
      unsigned long highWord = word(packet[40], packet[41]);
      unsigned long lowWord = word(packet[42], packet[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      unix_time = secsSince1900 - seventyYears + 32400UL; // 32400 = 9 hours (JST)
      break;
    }
  }
  udp.stop();
  return unix_time;
}

boolean syncNTPTime(const char* ntp_server) {
  unsigned long unix_time = getNTPTime(ntp_server);
  if (unix_time > 0) {
    setTime(unix_time);
    return true;
  }
  return false;
}
