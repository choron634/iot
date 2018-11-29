//人感センサー読み取りプログラム
void setup(){
    pinMode(16,INPUT);
    Serial.begin(9600);
}

void loop(){
    int bit = digitalRead(16);
    if(bit==HIGH){
        Serial.print("detected");
    }
    else if(bit==LOW){
        Serial.print("not detected");
    }
}
