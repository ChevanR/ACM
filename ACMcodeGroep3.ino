#include <WiFi.h>
#include <WiFiClient.h>
 
// Pin configuration
#define IR_SENSOR_PIN_1 15  // GPIO 15
#define IR_SENSOR_PIN_2 16  // GPIO 16
#define IR_SENSOR_PIN_3 12  // GPIO 12
#define REED_SWITCH_PIN 13  // GPIO 13
 
// WiFi credentials
const char *ssid = "MBAB";
const char *password = "12345678";
 
WiFiServer server(80);
 
String lastButtonPressed = "No action";
 
// Motor Driver pin configuration
const int IN1 = 21;
const int IN2 = 22;
const int IN3 = 18;
const int IN4 = 17;
const int ENA = 23;
const int ENB = 19;
 
// IR-sensor pin configuration
const int leftSensorPin = 15;
const int rightSensorPin = 12;
const int frontSensorPin = 16;
const int reedPin = 13;
 
// Function declarations
void handleButtonAction(String action);
void handleClient();
void sendHTML(WiFiClient &client);
void forward();
void right();
void backward();
void left();
void turn180();
void Stop();
void ACM();
 
void setup() {
  // Pin initialization for IR Sensors and Motor Driver
  pinMode(IR_SENSOR_PIN_1, INPUT);
  pinMode(IR_SENSOR_PIN_2, INPUT);
  pinMode(IR_SENSOR_PIN_3, INPUT);
  pinMode(REED_SWITCH_PIN, INPUT);
 
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(leftSensorPin, INPUT);
  pinMode(frontSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);
 
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
 
  if (WiFi.status() != WL_CONNECTED) {
    if (WiFi.softAP(ssid, password)) {
      IPAddress myIP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(myIP);
      server.begin();
 
      Serial.println("Server started");
    } else {
      Serial.println("Error configuring access point. Check credentials and try again.");
      while (1) {
        delay(1000);
      }
    }
  } else {
    Serial.println("Already connected to WiFi. Skipping access point setup.");
  }
}
 
void loop() {
  handleClient();
  ACM();
  // Standaard No action
  if (lastButtonPressed == "No action") {
    lastButtonPressed = "No action";
  }
}
 
void handleClient() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            sendHTML(client);
            break;
          } else {
            if (currentLine.startsWith("GET /control?action=")) {
              String action = currentLine.substring(21, currentLine.indexOf(" ", 21));
              handleButtonAction(action);
            }
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
  }
}
 
void sendHTML(WiFiClient &client) {
  // HTML response headers
  client.print("HTTP/1.1 200 OK\nContent-type:text/html\nConnection: close\nRefresh: 2\n\n");
  client.print("<html>");
  client.print("<head><title>IR Sensor Status</title>");
  client.print("<style>body {font-family: Arial, sans-serif; background-color: #333; color: white; text-align: center;} h1 {color: #fff; font-size: 2em;} table {border-collapse: collapse; width: 50%; margin: 20px auto;} th, td {border: 1px solid #ddd; padding: 12px; text-align: left;} .acm-control {font-size: 2em; text-align: center; margin-top: 30px;} .section-margin {margin-top: 40px;} button {font-size: 1.5em; padding: 10px 20px;}</style>");
  client.print("</head>");
  client.print("<body><h1>Sensor Status</h1>");
  client.print("<table><tr><th>Sensor</th><th>Status</th></tr>");
 
  // Sensor tabel
  client.print("<tr><td>IR Sensor 1</td><td>" + String((digitalRead(IR_SENSOR_PIN_1) == HIGH) ? "Object detected" : "No object detected") + "</td></tr>");
  client.print("<tr><td>IR Sensor 2</td><td>" + String((digitalRead(IR_SENSOR_PIN_2) == HIGH) ? "Object detected" : "No object detected") + "</td></tr>");
  client.print("<tr><td>IR Sensor 3</td><td>" + String((digitalRead(IR_SENSOR_PIN_3) == HIGH) ? "Object detected" : "No object detected") + "</td></tr>");
  client.print("<tr><td>Reed Switch</td><td>" + String((digitalRead(REED_SWITCH_PIN) == HIGH) ? "Magnet detected" : "No magnet detected") + "</td></tr>");
 
  client.print("</table>");
 
  // Toggle button
  client.print("<div class='section-margin'></div><h2 class='acm-control'>ACM Control</h2><button style='background-color: green; color: white;' onclick='startACM()'>Start ACM</button><button style='background-color: red; color: white;' onclick='stopACM()'>Stop ACM</button>");
 
  // Last button pressed html
  client.print("<div class='section-margin'></div><h3>Last Button Pressed</h3><table><tr><th>Last Button Pressed</th></tr><tr><td>");
  client.print(lastButtonPressed);
  client.print("</td></tr></table>");
 
  // Javascript voor 'action'
  client.print("<script>function startACM() {sendRequest('SStartACM');} function stopACM() {sendRequest('SStopACM');} function sendRequest(action) {var xhr = new XMLHttpRequest(); xhr.open('GET', '/control?action=' + action, true); xhr.onload = function() {if (xhr.status == 200) {var message = (action === 'SStartACM') ? 'ACM Is Turned On' : 'ACM Is Turned Off'; document.getElementById('lastButtonState').innerHTML = 'Last Button Pressed: ' + message;}}; xhr.send();}</script>");
 
  client.print("</body>");
  client.print("</html>");
}
 
void handleButtonAction(String action) {
  lastButtonPressed = action;
  if (action == "SStartACM") {
    ACM();  // Example: Start moving forward
 
  }else if (action == "SStopACM") {
     Stop();  // Example: Stop the motors
  }
}
 
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
 
void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
 
void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
 
void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
 
void turn180() {
  right();
  delay(700);
  Stop();
  delay(500);
}
 
void Stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
 
void ACM() {
  int value = 0;
  unsigned long currentMillis = millis();
  Stop();
 
  // Check if not in case 3 before reading IR sensor values
    int leftSensorValue = digitalRead(leftSensorPin);
    int rightSensorValue = digitalRead(rightSensorPin);
 
    if (leftSensorValue == HIGH) {
      // Turn left
      value = 1;
 
      if (rightSensorValue == LOW) {
        // Turn right
        value = 2;
      }
      if (rightSensorValue == HIGH) {
        // Backward and turn right
        value = 3;
      }
    } else if (leftSensorValue == LOW) {
      // Left
      value = 4;
    }
 
 
  switch (value) {
    case 1:
      left();
      break;
    case 2:
      right();
      break;
    case 3:
      Stop();
      backward();
     
 
 
      break;
    case 4:
      left();
      break;
  }
}