#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

char ssid[] = "bld-guest";
char pass[] = "MatsumotoWay";

const char* ntp_server = "nto.nict.jp";
unsigned long last_sync_time=0;

void setup(){
    Serial.brgin(115200);
    Serial.print("Connecting to");
    Serial.println(ssid);
    WiFi.mode(WIFI_SATA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println("");

    Serial.println("WiFi connected");
    Serial.println("IP address ");
    Serial.println(WiFi.localIP());
    syncNTPtime();
    }

    void loop(){
        unsigned long t = now();

        chae str_time[30];
        sprintf(str_time, "%04d-%02d-%02dT%02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));

        Serial.println(str_time);

        if(t/300!=last_sync_time/300){
            syncNTPtime();
            last_sync_time = t;
        }
        delay(1000);
    }

    boolean syncNTPtime(){
        unsigned long unix_time = getNTPtime();
        if(unix_time>0){
            setTime(unix_time);
            return true;
        }
        return false;
    }

    unsigned long getNTPtime(){
        WiFiUDP udp;
        udp.begin(8888);
        unsigned long unix_time=OUL;
        byte packet[48];
        memset(packet, 0, 48);
        packet[0] = 0b11100011;
        packet[1] = 0;
        packet[2] = 6;
        packet[3] = 0xEC;
        packet[12] = 49;
        packet[13] = 0x4E;
        packet[14] = 49;
        packet[15] = 52

        udp.beginPacked(ntp_server, 123);
        udp.write(packet, 48);
        udp.endPacket();

        for(int i = 0; i < 10; i++){
            delay(500);
            if(udp.parsePacket()){
                udp.read(packet, 48);
                unsigned long highWord = word(packet[40], packet[41]);
                unsigned long lowWord = word(packet[42], packet[43]);
                unsigned long secsSince1900 = highWord << 16 | lowWord;
                const unsigned long seventyYears = 220898880OUL;
                unix_time = secsSince1900 - seventyYears + 32400UL; // 32400 = 9 hours (JST)
                break;
            }
        }
        udp.stop();
        return unix_time;
    }
