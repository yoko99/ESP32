#include <M5Stack.h>
#include <esp_now.h>
#include <WiFi.h>

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)
uint8_t ad5;
float incomingroll;
float incomingpitch;
float incomingyaw;
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    float x,y,z;
    
} struct_message;

float data [15];

File file;
const char* fname= "/data.csv";

// Create a struct_message called myData
struct_message BNO055Readings;
struct_message myData;


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  memcpy(&myData, incomingData, sizeof(myData));
  
    if(mac[5]==28)// change the mac from hexadecimal to decimal (from the mac of M5atom took the first 2 digit)
  {
    data[0] = mac[5];
    data[1] = myData.x;
    data[2] = myData.y;
    data[3] = myData.z; 
  }
  else if(mac[5]==164)
  {
    data[5] = mac[5];
    data[6] = myData.x;
    data[7] = myData.y;
    data[8] = myData.z; 
  }
 else if(mac[5]==148)
  {
    data[10] = mac[5];
    data[11] = myData.x;
    data[12] = myData.y;
    data[13] = myData.z; 
  }
 
// Write Mac adress, Quaternion to SD in M5Stack
  File datafile = SD.open(fname, FILE_APPEND);
  if (datafile) {
    datafile.print(mac[5]);
    datafile.print(",");
    datafile.print(myData.x, 6);
    datafile.print(",");
    datafile.print(myData.y, 6);
    datafile.print(",");
    datafile.print(myData.z, 6);
    datafile.println("");
    datafile.close();
  }
  
}
 
void setup() {
    // M5Stack LCD
  M5.begin();
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
 
void loop() 
{
//Data for python
  int i;
  for(i=0; i<15; i++){
    Serial.print(data[i], 6);
    Serial.print("\t");
  }
  Serial.println(' ');

//M5 LCD -----------------------------------------
  M5.Lcd.setCursor(0, 10); 
  M5.Lcd.setTextSize(2);
  M5.Lcd.print(data[0],0);
  M5.Lcd.print(':');
  for(i=1; i<5; i++){
    M5.Lcd.print(data[i],2);
    M5.Lcd.print(',');
  }
  M5.Lcd.println(' ');
  M5.Lcd.print(data[4],0);
  M5.Lcd.print(':');
  for(i=5; i<10; i++){
    M5.Lcd.print(data[i],2);
    M5.Lcd.print(',');
  }  
  M5.Lcd.println(' ');
  M5.Lcd.print(data[8],0);
  M5.Lcd.print(':');
  for(i=10; i<15; i++){
    M5.Lcd.print(data[i],2);
    M5.Lcd.print(',');
  } 
  //delay(1000);
}
