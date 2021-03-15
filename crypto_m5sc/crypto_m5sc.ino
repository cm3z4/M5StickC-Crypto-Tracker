#include <M5StickC.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Insert your network's (Wi-Fi) ssid and password below.
const char *ssid = "";
const char *password = "";

void connectToWiFi()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to " + String(ssid) + ".");

  uint8_t i = 0;

  while (WiFi.status() != WL_CONNECTED)
  {

    delay(1000); // Pause for 1 second.

    if ((++i % 16) == 0)
    {
      Serial.println("\nCAN'T MAKE A CONNECTION, CHECK YOUR SSID/PASSWORD!");
      Serial.print("Connecting to " + String(ssid) + ".");
      i = 0;
    }
    else
    {
      Serial.print(".");
    }
  }

  Serial.print("\nCONNECTED! ");
  Serial.print(WiFi.localIP());
  Serial.println(" is your IP address.");
}

float previousPrice;
uint8_t decimalPoint = 5; // Edit the decimal point here.

void trackCrypto()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    // Visit https://www.cryptonator.com/api/currencies for a list of supported currencies.
    // Edit the path below for the crypto you want to track. Bitcoin is the default.

    String serverPath = "https://api.cryptonator.com/api/ticker/btc-usd";

    http.begin(serverPath.c_str());
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200)
    {

      Serial.println(previousPrice, decimalPoint);
      Serial.print("HTTP response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);

      String price = payload.substring(48, 56); // Extracts the "price" string from JSON.
      float currentPrice = price.toFloat();
      Serial.println(currentPrice, decimalPoint);

      M5.Lcd.fillScreen(TFT_BLACK);          // Clears the screen before redrawing/updating.
      M5.Lcd.setTextColor(TFT_BLUE);         // Edit the crypto name color here.
      M5.Lcd.drawString("BITCOIN", 2, 4, 4); // Edit the crypto name here.

      // Changes the price color according to the -/+ change.
      if (currentPrice <= previousPrice)
      {
        M5.Lcd.setTextColor(TFT_RED);
      }
      else
      {
        M5.Lcd.setTextColor(TFT_GREEN);
      }

      M5.Lcd.drawString(price, 2, 40, 4);
      previousPrice = currentPrice;
    }
    else
    {
      Serial.print("HTTP error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}

void setup(void)
{
  M5.begin();
  M5.Lcd.setRotation(3); // Set the rotation of the display.
  connectToWiFi();
  trackCrypto();
}

unsigned long lastTime = 0;
unsigned long timerDelay = 60000; // Update every 60 seconds.

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {
    trackCrypto();
    lastTime = millis();
  }
}
