//照度センサー読み取りプログラム

void setup(){
    pinMode(A0,INPUT);
    Serial.begin(9600);
}

void loop(){
    double v = analogRead(A0)/1024.0;
    double x = (321.0/0.3)*v;
    Serial.println(x);
    delay(1000);
}

/*結果
 * 647.85
649.94
649.94
647.85
646.81
647.85
59.56
37.62
59.56
644.72
641.58
113.90
55.38
54.34
160.92
628.00
57.47
64.79
64.79
58.52
642.63
642.63
649.94
568.44
637.40
130.62
103.45
*/
