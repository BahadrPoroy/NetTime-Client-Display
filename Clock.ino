// --- SAAT ÇİZİMİ ---
void displayTimePage() {
  display.setFont(&FreeSansBold24pt7b);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(displayTime, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128 - w) / 2, 35);
  display.print(displayTime);

  // --- ARA ÇİZGİ ---
  // SH110X kütüphanesinde renk ismi genelde SH110X_WHITE olarak geçer
  display.setFont();
  display.fillRect(0, 42, SCREEN_WIDTH, 3, SH110X_WHITE);

  // --- TARİH ÇİZİMİ ---
  display.setTextSize(2);
  String showDate = displayDate;
  display.getTextBounds(showDate, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128 - w) / 2, 48);
  display.print(showDate);
}
