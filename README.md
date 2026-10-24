# 🌡️ ESP32 Environment Monitor (Giám Sát Thông Số Môi Trường)

Hệ thống giám sát nhiệt độ, độ ẩm, ánh sáng và chất lượng không khí thời gian thực dựa trên ESP32. Dữ liệu được hiển thị tại chỗ qua màn hình OLED SH1106, cảnh báo qua hệ thống LED/Còi (Buzzer) và đồng bộ trực tiếp lên ứng dụng Blynk IoT.

## 📌 Tính Năng Chính

1. Đọc dữ liệu cảm biến thời gian thực:
- Nhiệt độ & Độ ẩm (DHT11)
- Cường độ ánh sáng (Cảm biến Quang trở LDR)
- Chất lượng không khí (MQ-135)
2. Hiển thị tại chỗ: Màn hình OLED SH1106 (128x64) kết nối qua chuẩn I2C.
3. Cảnh báo cục bộ: Hệ thống LED chỉ thị từng trạng thái ngưỡng (Quá nhiệt, Quá ẩm, Sáng/Tối, Không khí kém) và Còi báo động (Buzzer).
4. Kết nối Cloud (Blynk IoT):
- Đẩy thông số đo lên Dashboard ứng dụng Blynk.
- Bật Push Notification (Blynk Events) khi thông số vượt ngưỡng cài đặt.

### 🛠️ Linh Kiện Phần Cứng (Hardware)

1. Board điều khiển: ESP32 DevKit
2. Cảm biến:
- Cảm biến Nhiệt độ & Độ ẩm DHT11
- Cảm biến chất lượng không khí MQ-135
- Cảm biến ánh sáng LDR (có đầu ra Analog A0 và Digital D0)
3. Hiển thị & Cảnh báo:
- Màn hình OLED SH1106 1.3 inch (128x64)
- Còi báo (Buzzer 5V)
- 8x Đèn LED (LED nhiệt độ, độ ẩm, ánh sáng, không khí, trạng thái WiFi)

#### 🔌 Sơ Đồ Cấu Hình Chân (Pinout Configuration)

## 🔌 Sơ Đồ Cấu Hình Chân (Pinout Configuration)

| Linh kiện / Chức năng | Chân trên ESP32 | Kiểu dữ liệu / Vai trò |
| :--- | :---: | :--- |
| **DHT11 Data** | `GPIO 23` | Digital Input |
| **MQ-135 Analog** | `GPIO 32` | Analog Input (ADC) |
| **LDR Analog (A0)** | `GPIO 34` | Analog Input (ADC) |
| **LDR Digital (D0)** | `GPIO 27` | Digital Input |
| **OLED SDA** | `GPIO 21` | I2C Data Line |
| **OLED SCL** | `GPIO 22` | I2C Clock Line |
| **Buzzer** | `GPIO 25` | Output |
| **LED WiFi** | `GPIO 14` | Output (Trạng thái kết nối) |
| **LED Air Bad** | `GPIO 13` | Output (Báo không khí kém) |
| **LED Temp Low / High** | `GPIO 33` / `GPIO 19` | Output (Báo nhiệt độ) |
| **LED Hum Low / High** | `GPIO 18` / `GPIO 4` | Output (Báo độ ẩm) |
| **LED Light Low / High** | `GPIO 26` / `GPIO 12` | Output (Báo ánh sáng) |

---

###### 📚 Thư Viện Cần Thiết (Libraries)

Trước khi biên dịch code, vui lòng cài đặt các thư viện sau qua Library Manager trên Arduino IDE:

* `Blynk` by Volodymyr Shymanskyy
* `DHT sensor library` by Adafruit
* `Adafruit GFX Library` by Adafruit
* `Adafruit SH110X` by Adafruit

---

####### 🚀 Hướng Dẫn Cài Đặt & Nạp Code

1. Clone dự án về máy tính:
   git clone [https://github.com/your-username/your-repo-name.git](https://github.com/your-username/your-repo-name.git)
2. Mở file .ino bằng Arduino IDE.
3. Cập nhật thông tin Blynk Template ID, Device Name và Auth Token ở đầu file:
   #define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
   #define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
   #define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"
4. Điền tên WiFi và Mật khẩu của bạn:
   char ssid[] = "YOUR_WIFI_NAME";
   char pass[] = "YOUR_WIFI_PASSWORD";
5. Chọn đúng Board (ESP32 Dev Module) và Port tương ứng, sau đó tiến hành Compile và Upload.
