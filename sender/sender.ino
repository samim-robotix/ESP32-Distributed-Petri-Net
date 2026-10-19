#include <dummy.h>


#include <esp_now.h>
#include <WiFi.h>

// Replace with receiver's MAC address (format: {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX})
uint8_t receiverAddress[] = {0xa0, 0xb7, 0x65, 0x48, 0xb7, 0xac}; // Example

String msgToSend;
bool awaitingEcho = false;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  String echoedMsg = "";
  for (int i = 0; i < len; i++) echoedMsg += (char)incomingData[i];
  Serial.print("Echoed Message: ");
  Serial.println(echoedMsg);
  awaitingEcho = false;
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

  // Register receiver as peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  static unsigned int msgCount = 1;
  if (!awaitingEcho) {
    msgToSend = String(msgCount);
    esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)msgToSend.c_str(), msgToSend.length());
    if (result == ESP_OK) {
      Serial.print("Sent: ");
      Serial.println(msgToSend);
      awaitingEcho = true;
    } else {
      Serial.println("Error sending data");
    }
    msgCount++;
    delay(100); // Small delay for demonstration
    if (msgCount>=1000){
      msgCount = 1;
    }
  }
}
