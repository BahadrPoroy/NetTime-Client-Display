/**
 * Displays the current temperature and humidity on the OLED screen.
 * Centered horizontally for a clean look.
 */
void displayWeatherPage() {
  int16_t x1, y1;
  uint16_t w, h;
  char buffer[20];
  display.setTextSize(2);

  int currentTemp = DHT.temperature;
  //int currentHum = DHT.humidity;

  // Row 1: Temperature (Örn: SICAKLIK: 25°C)
  sprintf(buffer, "SICAKLIK:");
  display.getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 10);
  display.print(buffer);

  // Row 2: Humidity (Örn: NEM: %45)
  sprintf(buffer, "%02d%cC", currentTemp, 247);
  display.getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 35);
  display.print(buffer);
}