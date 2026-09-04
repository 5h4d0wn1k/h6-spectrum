// H6 — Spectrum Analyzer (ESP32-C6)
// WiFi spectrum visualization, channel energy detection, frequency scanning

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

// ── Configuration ──────────────────────────────────────────────
#define SCAN_INTERVAL_MS    200
#define WIFI_CHANNEL_MIN    1
#define WIFI_CHANNEL_MAX    13
#define NUM_WIFI_CHANNELS   14
#define HISTORY_DEPTH       64

// ── Energy history per WiFi channel ────────────────────────────
int8_t  channelEnergy[NUM_WIFI_CHANNELS][HISTORY_DEPTH];
uint8_t historyIdx[NUM_WIFI_CHANNELS];
float   energyAvg[NUM_WIFI_CHANNELS];

// ── Forward declarations ──────────────────────────────────────
void initChannelHistory();
void printSpectrum();
void printBar(int level, int maxLen);

// ── WiFi raw callback for energy detection ────────────────────
static volatile int rssiSum = 0;
static volatile int rssiCount = 0;

static void IRAM_ATTR wifiPromiscuousCb(void* buf, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_MGMT) return;
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  if (pkt->rx_ctrl.rssi != 0) {
    rssiSum += pkt->rx_ctrl.rssi;
    rssiCount++;
  }
}

// ── Setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println(F("\n=== H6 — Spectrum Analyzer (ESP32-C6) ==="));

  initChannelHistory();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(wifiPromiscuousCb);
  esp_wifi_set_channel(WIFI_CHANNEL_MIN, WIFI_SECOND_CHAN_NONE);

  Serial.println(F("WiFi channel scanning active"));
  Serial.println(F("Commands: b=busiest  r=reset  p=print\n"));
}

// ── Main loop ─────────────────────────────────────────────────
void loop() {
  static uint32_t lastScan = 0;
  static uint8_t  currentChannel = WIFI_CHANNEL_MIN;

  // Handle serial commands
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'p' || c == 'P') printSpectrum();
    if (c == 'r' || c == 'R') initChannelHistory();
    if (c == 'b' || c == 'B') {
      int busiest = WIFI_CHANNEL_MIN;
      float maxE = -100;
      for (int ch = WIFI_CHANNEL_MIN; ch <= WIFI_CHANNEL_MAX; ch++) {
        if (energyAvg[ch] > maxE) { maxE = energyAvg[ch]; busiest = ch; }
      }
      Serial.printf("Busiest: Ch%d (%.1f dBm)\n", busiest, maxE);
    }
  }

  if (millis() - lastScan >= SCAN_INTERVAL_MS) {
    lastScan = millis();

    // Measure energy on current channel
    rssiSum = 0;
    rssiCount = 0;
    esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
    delay(10);
    uint32_t t0 = millis();
    while (millis() - t0 < 50) { /* accumulate packets */ }

    float energy = (rssiCount > 0) ? (float)rssiSum / rssiCount : -100.0;
    if (energy < -100) energy = -100;
    channelEnergy[currentChannel][historyIdx[currentChannel]] = (int8_t)energy;
    historyIdx[currentChannel] = (historyIdx[currentChannel] + 1) % HISTORY_DEPTH;

    // Running average
    float sum = 0;
    for (int i = 0; i < HISTORY_DEPTH; i++) sum += channelEnergy[currentChannel][i];
    energyAvg[currentChannel] = sum / HISTORY_DEPTH;

    currentChannel++;
    if (currentChannel > WIFI_CHANNEL_MAX) {
      currentChannel = WIFI_CHANNEL_MIN;
      printSpectrum();
    }
  }
}

// ── Init channel history ──────────────────────────────────────
void initChannelHistory() {
  for (int ch = 0; ch < NUM_WIFI_CHANNELS; ch++) {
    historyIdx[ch] = 0;
    energyAvg[ch] = -100;
    for (int i = 0; i < HISTORY_DEPTH; i++) channelEnergy[ch][i] = -100;
  }
}

// ── Print spectrum ────────────────────────────────────────────
void printSpectrum() {
  Serial.println(F("\n=== WiFi Channel Energy (dBm) ==="));
  for (int ch = WIFI_CHANNEL_MIN; ch <= WIFI_CHANNEL_MAX; ch++) {
    float avg = energyAvg[ch];
    int barLen = map((int)avg, -100, -20, 0, 30);
    if (barLen < 0) barLen = 0;
    if (barLen > 30) barLen = 30;

    Serial.printf("Ch%2d [%6.1f] ", ch, avg);
    printBar(barLen, 30);
    Serial.println();
  }
  Serial.println(F("================================="));

  int busiest = WIFI_CHANNEL_MIN;
  float maxE = -100;
  for (int ch = WIFI_CHANNEL_MIN; ch <= WIFI_CHANNEL_MAX; ch++) {
    if (energyAvg[ch] > maxE) { maxE = energyAvg[ch]; busiest = ch; }
  }
  Serial.printf("Busiest: Ch%d (%.1f dBm)\n", busiest, maxE);
}

// ── Draw bar ──────────────────────────────────────────────────
void printBar(int level, int maxLen) {
  for (int i = 0; i < maxLen; i++) {
    Serial.write(i < level ? '#' : ' ');
  }
}
