#include <WiFiS3.h>

// --- WiFi Credentials ---
const char ssid[] = "panda";       // Replace with your WiFi Name
const char pass[] = "12345678";    // Replace with your WiFi Password

// --- Motor Pin Definitions ---
// Motor 1 (Left Motor)
const int M1A = 5;
const int M1B = 6;
// Motor 2 (Right Motor)
const int M2A = 9;
const int M2B = 10;

WiFiServer server(80);

// Track current state to avoid spamming the monitor
String lastCommand = "S";

void setup() {
  Serial.begin(115200);
  
  // Set motor pins as outputs
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);
  
  // Ensure motors are stopped initially
  stopMotors();

  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected!");
  Serial.print("Access the controller at: http://");
  Serial.println(WiFi.localIP());
  Serial.println("----------------------------------------");
  Serial.println("Waiting for commands...");
  Serial.println("----------------------------------------");

  // Start the web server
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        // If we reach the end of the HTTP request headers
        if (c == '\n') {
          if (currentLine.length() == 0) {
            sendHTML(client); // Serve the web page
            break;
          } else {
            processCommand(currentLine); // Check for movement commands
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}

// Read the incoming HTTP GET requests and trigger motors
void processCommand(String req) {
  // Only log actual command requests (not every header line)
  if (req.indexOf("GET /") >= 0) {
    Serial.print("[REQUEST] ");
    Serial.println(req);
  }

  if (req.indexOf("GET /F") >= 0) {
    if (lastCommand != "F") {
      Serial.println(">> COMMAND: FORWARD  (Motor1: FWD | Motor2: FWD)");
      lastCommand = "F";
    }
    forward();
  } else if (req.indexOf("GET /B") >= 0) {
    if (lastCommand != "B") {
      Serial.println(">> COMMAND: BACKWARD (Motor1: REV | Motor2: REV)");
      lastCommand = "B";
    }
    backward();
  } else if (req.indexOf("GET /L") >= 0) {
    if (lastCommand != "L") {
      Serial.println(">> COMMAND: TURN LEFT (Motor1: REV | Motor2: FWD)");
      lastCommand = "L";
    }
    turnLeft();
  } else if (req.indexOf("GET /R") >= 0) {
    if (lastCommand != "R") {
      Serial.println(">> COMMAND: TURN RIGHT (Motor1: FWD | Motor2: REV)");
      lastCommand = "R";
    }
    turnRight();
  } else if (req.indexOf("GET /S") >= 0) {
    if (lastCommand != "S") {
      Serial.println(">> COMMAND: STOP (All motors OFF)");
      lastCommand = "S";
    }
    stopMotors();
  }
}

// Serve the HTML Frontend Web Page
void sendHTML(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  
  // Raw literal string for neat, easy-to-read HTML and CSS
  client.print(R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
  <title>Arduino R4 Controller</title>
  <style>
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; margin-top: 50px; background-color: #222; color: white; }
    h2 { margin-bottom: 30px; font-weight: 300; letter-spacing: 2px; }
    .btn { padding: 25px 35px; font-size: 22px; margin: 10px; cursor: pointer; border-radius: 12px; border: none; background-color: #007bff; color: white; box-shadow: 0 4px #0056b3; transition: 0.1s; user-select: none; -webkit-tap-highlight-color: transparent; }
    .btn:active { background-color: #0056b3; box-shadow: 0 0px #0056b3; transform: translateY(4px); }
    .btn-stop { background-color: #dc3545; box-shadow: 0 4px #a71d2a; }
    .btn-stop:active { background-color: #c82333; box-shadow: 0 0px #a71d2a; }
    .row { display: flex; justify-content: center; align-items: center; }
  </style>
</head>
<body>
  <h2>ROBOT CONTROLLER</h2>
  <div>
    <button class="btn" onmousedown="sendCmd('F')" onmouseup="sendCmd('S')" ontouchstart="sendCmd('F')" ontouchend="sendCmd('S')">&#9650;<br>Forward</button>
    <div class="row">
      <button class="btn" onmousedown="sendCmd('L')" onmouseup="sendCmd('S')" ontouchstart="sendCmd('L')" ontouchend="sendCmd('S')">&#9664; Left</button>
      <button class="btn btn-stop" onclick="sendCmd('S')">STOP</button>
      <button class="btn" onmousedown="sendCmd('R')" onmouseup="sendCmd('S')" ontouchstart="sendCmd('R')" ontouchend="sendCmd('S')">Right &#9654;</button>
    </div>
    <button class="btn" onmousedown="sendCmd('B')" onmouseup="sendCmd('S')" ontouchstart="sendCmd('B')" ontouchend="sendCmd('S')">Reverse<br>&#9660;</button>
  </div>
  <script>
    // Sends fetch requests quietly in the background without reloading the page
    function sendCmd(c) { fetch('/' + c); }
  </script>
</body>
</html>
  )rawliteral");
  client.println();
}

// --- Motor Control Logic ---
// Note: If a motor spins backwards, simply flip the HIGH/LOW states in its function
// or swap its physical wires (M1A with M1B, or M2A with M2B).

void forward() {
  digitalWrite(M1A, HIGH); digitalWrite(M1B, LOW);
  digitalWrite(M2A, HIGH); digitalWrite(M2B, LOW);
}

void backward() {
  digitalWrite(M1A, LOW); digitalWrite(M1B, HIGH);
  digitalWrite(M2A, LOW); digitalWrite(M2B, HIGH);
}

void turnLeft() {
  digitalWrite(M1A, LOW); digitalWrite(M1B, HIGH);
  digitalWrite(M2A, HIGH); digitalWrite(M2B, LOW);
}

void turnRight() {
  digitalWrite(M1A, HIGH); digitalWrite(M1B, LOW);
  digitalWrite(M2A, LOW); digitalWrite(M2B, HIGH);
}

void stopMotors() {
  digitalWrite(M1A, LOW); digitalWrite(M1B, LOW);
  digitalWrite(M2A, LOW); digitalWrite(M2B, LOW);
}
