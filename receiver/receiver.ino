#include <esp_now.h>
#include <WiFi.h>

// Store sender MAC address on first reception
uint8_t senderAddress[6];
bool senderKnown = false;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Echo Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  String msg = "";
  for (int i = 0; i < len; i++) msg += (char)incomingData[i];
  Serial.print("Received Message: ");
  Serial.println(msg);

  if (!senderKnown) {
    memcpy(senderAddress, mac_addr, 6);
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, senderAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add sender as peer");
    } else {
      senderKnown = true;
    }
  }
  // Echo message back to sender
  esp_err_t result = esp_now_send(senderAddress, incomingData, len);
  if (result == ESP_OK) {
    Serial.print("Echoed: ");
    Serial.println(msg);
  } else {
    Serial.println("Error echoing message");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Nothing needed in loop
}
