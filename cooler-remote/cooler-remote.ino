#include "WiFiS3.h"

const char ssid[] = "CoolerBot";
const char pass[] = "12345678";   // min 8 chars
WiFiServer server(80);

const int M1A = 5, M1B = 6;   // left motor
const int M2A = 9, M2B = 10;  // right motor
int spd = 200;
unsigned long lastCmd = 0;
bool moving = false;

void drive(int l, int r) {
  analogWrite(M1A, l > 0 ? l : 0);
  analogWrite(M1B, l < 0 ? -l : 0);
  analogWrite(M2A, r > 0 ? r : 0);
  analogWrite(M2B, r < 0 ? -r : 0);
  moving = (l != 0 || r != 0);
}

const char page[] =
"<!DOCTYPE html><html><head><meta name=viewport content='width=device-width,initial-scale=1'>"
"<style>body{font-family:sans-serif;text-align:center;background:#111;color:#fff;touch-action:none;user-select:none}"
"button{width:90px;height:90px;margin:6px;font-size:32px;border-radius:16px;border:0;background:#2a7;color:#fff}"
"#s{background:#c33}input{width:80%}</style></head><body><h2>Cooler Remote</h2>"
"<div><button onpointerdown=\"hold('F')\" onpointerup=rel() onpointerleave=rel()>&#9650;</button></div>"
"<div><button onpointerdown=\"hold('L')\" onpointerup=rel() onpointerleave=rel()>&#9664;</button>"
"<button id=s onclick=\"go('S')\">&#9632;</button>"
"<button onpointerdown=\"hold('R')\" onpointerup=rel() onpointerleave=rel()>&#9654;</button></div>"
"<div><button onpointerdown=\"hold('B')\" onpointerup=rel() onpointerleave=rel()>&#9660;</button></div>"
"<p>Speed</p><input type=range min=80 max=255 value=200 onchange=\"go('V'+this.value)\">"
"<script>let t;function go(c){fetch('/'+c).catch(()=>{})}"
"function hold(c){clearInterval(t);go(c);t=setInterval(()=>go(c),200)}"
"function rel(){clearInterval(t);go('S')}</script></body></html>";

void setup() {
  pinMode(M1A, OUTPUT); pinMode(M1B, OUTPUT);
  pinMode(M2A, OUTPUT); pinMode(M2B, OUTPUT);
  drive(0, 0);
  WiFi.beginAP(ssid, pass);
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String req = client.readStringUntil('\r');
    while (client.available()) client.read();

    if (req.startsWith("GET / ")) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Connection: close");
      client.println();
      client.print(page);
    } else {
      if (req.startsWith("GET /F")) drive(spd, spd);
      else if (req.startsWith("GET /B")) drive(-spd, -spd);
      else if (req.startsWith("GET /L")) drive(-spd, spd);
      else if (req.startsWith("GET /R")) drive(spd, -spd);
      else if (req.startsWith("GET /S")) drive(0, 0);
      else if (req.startsWith("GET /V")) spd = constrain(req.substring(6).toInt(), 80, 255);
      lastCmd = millis();
      client.println("HTTP/1.1 204 No Content");
      client.println("Connection: close");
      client.println();
    }
    client.stop();
  }
  // failsafe: stop if the phone stops sending commands
  if (moving && millis() - lastCmd > 600) drive(0, 0);
}