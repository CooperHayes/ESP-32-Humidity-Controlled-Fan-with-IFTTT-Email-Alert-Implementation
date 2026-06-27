# ESP-32-Humidity-Controlled-Fan-with-IFTTT-Email-Alert-Implementation
A DHT22 is used to take humidity readings that are then displayed on a .96 OLED display and compared to threshold to turn a 5V fan on or off. By utilizing the ESP-32's ability to connect to WiFi and set web requests to websites, the ESP32 send a webhook request to an IFTTT URL, which then sends an email to a Gmail inbox
