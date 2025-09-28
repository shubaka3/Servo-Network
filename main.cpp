#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid     = "PhuCuong";
const char* password = "0984617703";

WebServer server(80);
Servo myServo;

// --- Trang HTML điều khiển thủ công ---
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="utf-8">
      <title>ESP32 Servo Control</title>
    </head>
    <body>
      <h2>Điều khiển Servo (10–160°)</h2>
      <form action="/set" method="GET">
        Góc (10–160): <input type="number" name="angle" min="10" max="160">
        <input type="submit" value="Gửi">
      </form>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// --- Xử lý từ form HTML ---
void handleSet() {
  if (server.hasArg("angle")) {
    int angle = constrain(server.arg("angle").toInt(), 10, 160);  // Giới hạn 10–160
    myServo.write(angle);
    Serial.printf("[HTML] Servo tới %d°\n", angle);
    server.send(200, "text/html",
                "Servo đang tới góc: " + String(angle) + "°<br><a href='/'>Quay lại</a>");
  } else {
    server.send(400, "text/plain", "Thiếu tham số góc");
  }
}

// --- REST API: /api/servo?angle=90 ---
void handleApi() {
  if (server.hasArg("angle")) {
    int angle = constrain(server.arg("angle").toInt(), 10, 160);  // Giới hạn 10–160
    myServo.write(angle);
    Serial.printf("[API] Servo tới %d°\n", angle);
    server.send(200, "application/json",
                "{\"status\":\"ok\",\"angle\":" + String(angle) + "}");
  } else {
    server.send(400, "application/json",
                "{\"status\":\"error\",\"message\":\"missing angle\"}");
  }
}

void setup() {
  Serial.begin(115200);

  // Kết nối Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi đã kết nối!");
  Serial.print("Truy cập: http://");
  Serial.println(WiFi.localIP());

  // Gắn servo (chân 27) và đưa về 10° để nằm trong vùng an toàn
  myServo.attach(27);
  myServo.write(10);

  // Định nghĩa các route
  server.on("/", handleRoot);        // Giao diện HTML
  server.on("/set", handleSet);      // Xử lý form
  server.on("/api/servo", handleApi);// API REST

  server.begin();
}

void loop() {
  server.handleClient();
}
