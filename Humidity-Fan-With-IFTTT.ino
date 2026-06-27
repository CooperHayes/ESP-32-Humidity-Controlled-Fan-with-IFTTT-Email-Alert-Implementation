
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>


// ---------- OLED INITIALIZATION ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- DHT22 INITIALIZATION ----------
#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// ---------- FAN INITIALIZATION ----------
#define FAN_PIN 18


// ---------- WIFI INITIALIZATION ----------
const int potPin = 32;

// ---------- WIFI INITIALIZATION ----------
const char* ssid = "..."; // use personal WiFi name
const char* password = "..."; // use personal WiFi password


// ---------- EDGE-DETECTION INITIALIZATION ----------
bool alertSent = false;

void setup() {
	Serial.begin(115200);

	// I2C communication for OLED
	Wire.begin(OLED_SDA, OLED_SCL);

	// OLED start
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
		Serial.println("OLED failed");
		while (true);
	}

	display.clearDisplay();
	display.setTextColor(SSD1306_WHITE);

	// DHT start
	dht.begin();

	// Fan setup
	pinMode(FAN_PIN, OUTPUT);
	digitalWrite(FAN_PIN, LOW); // fan OFF at boot


	// Connect to WiFi
	WiFi.begin( ssid , password );
  
  Serial.print("Connecting to WiFi");

  while(WiFi.status() != WL_CONNECTED){
    Serial.println("Waiting...");
    delay(200);
  }
  Serial.println("Connected.");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
	

} // END SETUP

void loop() {
	// Data collection
	float temp = dht.readTemperature();
	float hum = dht.readHumidity();

	// Controllable humidity threshold via potentiometer
	float potValue = analogRead(potPin);

	int HUMIDITY_THRESHOLD = map(potValue , 0 , 4095 , 60 , 90);

	if (isnan(temp) || isnan(hum)) {
		Serial.println("DHT read failed");
		delay(2000);
		return;
	}

	// ---------- FAN CONTROL ----------
	if (hum > HUMIDITY_THRESHOLD) {

		digitalWrite(FAN_PIN, HIGH); // ON

		if(alertSent == false){ // Send email alert
			HTTPClient http;
			http.begin("url"); // use webhook url provided by IFTTT
			int httpResponseCode = http.GET();

  		Serial.print("HTTP Response Code: ");
  		Serial.println(httpResponseCode);

			alertSent = true;
  		http.end();
		}
	} 
	else {
		digitalWrite(FAN_PIN, LOW);  // OFF
		alertSent = false;
	}

	// ---------- SERIAL DISPLAY ----------
	Serial.print("Temp: ");
	Serial.print(temp);
	Serial.print(" C  Hum: ");
	Serial.println(hum);

	if (WiFi.status() == WL_CONNECTED) {
  	Serial.println("WiFi OK");
	}
	else {
  	Serial.println("WiFi Lost");
	}

	// ---------- OLED DISPLAY ----------
	display.clearDisplay();

	display.setTextSize(1);
	display.setCursor(0, 0);
	display.print("Temp:");
	display.print(temp, 1);

	display.setCursor(0, 15);
	display.print("Hum:");
	display.print(hum, 1);
	display.print("%");

	display.setCursor(0, 30);
	display.print("Threshold:");
	display.print(HUMIDITY_THRESHOLD);
	display.print("%");

	display.setCursor(0,45);
	if (hum > HUMIDITY_THRESHOLD) {
		display.print("Fan: ON");
	} else {
		display.print("Fan: OFF");
	}

	display.display();
	delay(2000);

} // END LOOP



