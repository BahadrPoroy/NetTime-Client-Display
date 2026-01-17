void handleHybridSync() {
  // Uyarı yanıp sönmesini kontrol eden değişken artık sadece fiziksel WiFi'a bağlı
  isConnected = (WiFi.status() == WL_CONNECTED);

  if (!isConnected) return; // WiFi yoksa işlem yapma

  int packetSize = udp.parsePacket();
  if (packetSize) {
    char packetBuffer[32];
    int len = udp.read(packetBuffer, 31);
    packetBuffer[len] = 0;
    
    String payload = String(packetBuffer);
    int sep = payload.indexOf('|');
    if (sep != -1) {
      displayTime = payload.substring(0, sep);
      displayDate = payload.substring(sep + 1);
      failCounter = 0;
      isLocalMode = true;
      return; 
    }
  }

  // Sunucu yoksa Fallback
  static unsigned long lastAttempt = 0;
  if (millis() - lastAttempt >= 2000) {
    lastAttempt = millis();
    if (isLocalMode) {
      failCounter++;
      if (failCounter >= MAX_RETRIES) isLocalMode = false;
    }
  }

  // NTP Modu
  if (!isLocalMode) {
    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);
    
    if (p_tm->tm_year > 70) { // NTP'den geçerli veri geldi mi?
      char t_buf[9], d_buf[11];
      sprintf(t_buf, "%02d:%02d", p_tm->tm_hour, p_tm->tm_min);
      sprintf(d_buf, "%02d/%02d/%04d", p_tm->tm_mday, p_tm->tm_mon + 1, p_tm->tm_year + 1900);
      displayTime = String(t_buf);
      displayDate = String(d_buf);
    } else {
      // WiFi var ama henüz ne sunucu ne NTP veri gönderdi
      displayTime = "--:--";
      displayDate = "--/--/--";
    }
  }
}