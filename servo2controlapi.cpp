#include <WiFi.h>
#include <WebServer.h>
// Thư viện Servo dành riêng cho ESP32
#include "ESP32Servo.h" 

// --- Cấu hình WiFi (Giữ nguyên) ---
const char* ssid     = "PhuCuong";
const char* password = "0984617703";

WebServer server(80);

// --- Cấu hình Servo ---
Servo servo1; // Khai báo đối tượng Servo 1
Servo servo2; // Khai báo đối tượng Servo 2

const int servoPin1 = 18; // Chân GPIO điều khiển Servo 1
const int servoPin2 = 19; // Chân GPIO điều khiển Servo 2

// Giới hạn góc an toàn (theo yêu cầu)
const int MIN_ANGLE = 10;
const int MAX_ANGLE = 160;

// --- Hàm Xử lý API Servo ---
void handleSetServoAngle() {
    // 1. Kiểm tra tham số cần thiết
    if (!server.hasArg("id") || !server.hasArg("angle")) {
        server.send(400, "text/plain", "Tham số 'id' và 'angle' là bắt buộc.");
        return;
    }

    // 2. Lấy và kiểm tra ID Servo
    int servoId = server.arg("id").toInt();
    if (servoId < 1 || servoId > 2) {
        server.send(400, "text/plain", "ID Servo không hợp lệ (chỉ chấp nhận 1 hoặc 2).");
        return;
    }

    // 3. Lấy và kiểm tra Góc
    int targetAngle = server.arg("angle").toInt();
    
    // Giới hạn góc an toàn (10-160 độ)
    if (targetAngle < MIN_ANGLE || targetAngle > MAX_ANGLE) {
        String msg = "Góc không an toàn. Chỉ chấp nhận góc từ " + String(MIN_ANGLE) + " đến " + String(MAX_ANGLE) + " độ.";
        server.send(400, "text/plain", msg);
        return;
    }
    
    // 4. Điều khiển Servo
    if (servoId == 1) {
        servo1.write(targetAngle);
    } else { // servoId == 2
        servo2.write(targetAngle);
    }

    // 5. Phản hồi thành công
    String response = "{";
    response += "\"status\":\"success\"";
    response += ",\"servo_id\":" + String(servoId);
    response += ",\"angle\":" + String(targetAngle);
    response += "}";
    
    server.send(200, "application/json", response);
}

// --- Hàm Setup (Đã cập nhật) ---
void setup() {
    Serial.begin(115200);

    // Bắt đầu kết nối WiFi (Giữ nguyên)
    WiFi.begin(ssid, password);
    Serial.print("Đang kết nối WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi đã kết nối!");
    Serial.print("Truy cập: http://");
    Serial.println(WiFi.localIP());

    // Cấu hình Servo
    // Gắn đối tượng Servo với chân GPIO
    servo1.attach(servoPin1); 
    servo2.attach(servoPin2); 
    
    // Thiết lập vị trí ban đầu an toàn (ví dụ: 90 độ)
    servo1.write(90);
    servo2.write(90);

    // Định tuyến (Routing)
    server.on("/", [](){server.send(200, "text/plain", "ESP32 Servo API is running.");}); // Thay thế handleRoot cũ
    server.on("/api/servo", HTTP_GET, handleSetServoAngle); // API mới cho Servo
    
    server.begin();
}

// --- Hàm Loop (Giữ nguyên) ---
void loop() {
    server.handleClient();
}

// platformio.ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    ESP32WebServer 
    madhephaestus/ESP32Servo
monitor_speed = 115200

