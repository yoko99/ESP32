#include <M5Stack.h>
#include <WiFi.h>
#include <esp_now.h>

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)
uint8_t ad5;
float incomingqw;
float incomingqx;
float incomingqy;
float incomingqz;
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    float w,x,y,z;
    float qw;
    float qx;
    float qy;
    float qz;
} struct_message;

float ew, ex, ey, ez;
float data[12];

File file;
const char* fname = "/data.csv";

// Create a struct_message called myData
struct_message BNO055Readings;
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  memcpy(&myData, incomingData, sizeof(myData));
  
  if(mac[5]==148){
    data[0] = mac[5];
    data[1] = cal_roll(myData.w, myData.x, myData.y, myData.z);
    data[2] = cal_pitch(myData.w, myData.x, myData.y, myData.z);
    data[3] = cal_yaw(myData.w, myData.x, myData.y, myData.z); 
  }
  else if(mac[5]==232){
    data[4] = mac[5];
    data[5] = cal_roll(myData.w, myData.x, myData.y, myData.z);
    data[6] = cal_pitch(myData.w, myData.x, myData.y, myData.z);
    data[7] = cal_yaw(myData.w, myData.x, myData.y, myData.z); 
  }
  else if(mac[5]==111){
    data[8] = mac[5];
    data[9] = cal_roll(myData.w, myData.x, myData.y, myData.z);
    data[10] = cal_pitch(myData.w, myData.x, myData.y, myData.z);
    data[11] = cal_yaw(myData.w, myData.x, myData.y, myData.z); 
  }
}
 
void setup() {
  // M5Stack LCD
  M5.Lcd.begin();
  delay(1000);
  M5.Lcd.setBrightness(200);
  
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
//Data for python
  int i;
  for(i=0; i<12; i++){
    Serial.print(data[i], 6);
    Serial.print("\t");
  }
  Serial.println(' ');

//M5 LCD -----------------------------------------
  M5.Lcd.setCursor(0, 10); 
  M5.Lcd.setTextSize(2);
  M5.Lcd.print(data[0],0);
  M5.Lcd.print(':');
  for(i=1; i<4; i++){
    M5.Lcd.print(data[i],2);
    M5.Lcd.print(',');
  }
  M5.Lcd.println(' ');
  M5.Lcd.print(data[4],0);
  M5.Lcd.print(':');
  for(i=5; i<8; i++){
    M5.Lcd.print(data[i],2);
    M5.Lcd.print(',');
  }  
  M5.Lcd.println(' ');
  M5.Lcd.print(data[8],0);
  M5.Lcd.print(':');
  for(i=9; i<12; i++){
    M5.Lcd.print(data[i],2);
    M5.Lcd.print(',');
  } 

//Data save to SD --------------------------------------
  file = SD.open(fname, FILE_APPEND);
  for(i=0; i<12; i++){
    file.print(data[i],6);
    file.print(",");
  }
  file.close();
  delay(1000);
}
// Quaternion -> Euler ----------------------------
float cal_roll(float ew, float ex, float ey, float ez){
  float ysqr, t0, t1, roll;
    
  ysqr = ey * ey;
  t0 = +2.0 * (ew * ex + ey * ez);
  t1 = +1.0 - 2.0 * (ex * ex + ysqr);
  roll = std::atan2(t0, t1);
  roll = roll * 57.2957795131;
  return roll;
}

float cal_pitch(float ew, float ex, float ey, float ez){
  float ysqr, t2, t1, pitch;

  ysqr = ey * ey;
  t2 = t2 > 1.0 ? 1.0 : t2;
  t2 = t2 < -1.0 ? -1.0 : t2;
  pitch = std::asin(t2);
  pitch = pitch * 57.2957795131;
  return pitch;
}

float cal_yaw(float ew, float ex, float ey, float ez){
  float ysqr, t3, t4, yaw;

  ysqr = ey * ey;
  t3 = +2.0 * (ew * ez + ex * ey);
  t4 = +1.0 - 2.0 * (ysqr + ez * ez);  
  yaw = std::atan2(t3, t4);
  yaw = yaw * 57.2957795131;
  return yaw;
}
