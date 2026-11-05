#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <Stepper.h>

// ---------- Hardware ----------
const int stepsPerRevolution = 2048;

#define IN1 12
#define IN2 14
#define IN3 27
#define IN4 26
const int servoPin = 15;

Stepper motor(stepsPerRevolution, IN1, IN2, IN3, IN4);
Servo tiltServo;

// ---------- Network ----------
// Replace these with your own credentials before flashing.
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Monitor Gimbal</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        text-align: center;
        background-color: #f4f4f9;
        margin: 0;
        padding: 0;
      }
      h1 {
        color: #333;
        margin-top: 20px;
      }
      .container {
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        height: 100vh;
      }
      .button {
        display: inline-block;
        padding: 15px 30px;
        margin: 10px;
        font-size: 18px;
        font-weight: bold;
        color: #fff;
        background-color: #007BFF;
        border: none;
        border-radius: 5px;
        text-decoration: none;
        cursor: pointer;
        transition: background-color 0.3s;
      }
      .button:hover {
        background-color: #0056b3;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>Gimbal Control</h1>
      <a href="/Up" class="button">Up</a>
      <a href="/Down" class="button">Down</a>
      <a href="/Left" class="button">Left</a>
      <a href="/Right" class="button">Right</a>
      <a href="/Stop" class="button">Stop</a>
    </div>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleUp() {
  Serial.println("stepper up");
  motor.step(1000);
  delay(500);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleDown() {
  Serial.println("stepper down");
  motor.step(-333);
  delay(500);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLeft() {
  if (!tiltServo.attached()) tiltServo.attach(servoPin);
  tiltServo.write(120);
  delay(1000);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleRight() {
  if (!tiltServo.attached()) tiltServo.attach(servoPin);
  tiltServo.write(60);
  delay(1000);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleStop() {
  tiltServo.detach();
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  tiltServo.attach(servoPin);
  motor.setSpeed(10);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. Open http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/Up", handleUp);
  server.on("/Down", handleDown);
  server.on("/Left", handleLeft);
  server.on("/Right", handleRight);
  server.on("/Stop", handleStop);

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
