#include <Arduino.h>

#define DEFAULT_SSID "3DPractical"
#define DEFAULT_PASS "embeddedelectronics"
#define DEFAULT_UID  "abcdefg"

#include "Config.h"

TaskHandle_t upload_handle;
TaskHandle_t blink_handle;
TaskHandle_t sleep_handle;

void wakeup_reason()
{
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch(wakeup_reason)
    {
        case ESP_SLEEP_WAKEUP_EXT0 :
            Serial.println("Wakeup caused by external signal using RTC_IO");
            break;
        default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
    }
}

void upload(void *param)
{
  for(;;){
    while (counter <= 20)
    {
      int dataMillis = millis();
      if(uid!=""){
        String path = "/test/" + uid + "/";
        if(counter < 10){
          path+="00" + String(counter);
        } else if(counter < 100){
          path+="0" + String(counter);
        } else{
          path+=String(counter);
        }
        
        bool sent_status = Firebase.setInt(fb, path, dataMillis);
        do{
          Serial.println(fb.errorReason().c_str());
          sent_status = Firebase.setInt(fb, path, dataMillis);
        } while(sent_status == false);
        
        Serial.print("[" + String(millis())+"] ");
        Serial.print("[" + String(counter)+"] ");
        
        Serial.println("Data sent");
        counter++;
        // Serial.printf("Set int... %s\n", Firebase.setInt(fb, path, dataMillis) ? "ok" : fb.errorReason().c_str());
      } else{
        Serial.printf("Set int... %s\n", Firebase.setInt(fb, "/test/int", dataMillis) ? "ok" : fb.errorReason().c_str());
      }

      lastUpload = millis();

      if(counter == 20){
        Serial.print("[" + String(millis())+"] ");
        Serial.println("Pause upload task");
      }
      vTaskDelay(700);
    }
    // vTaskSuspend(NULL);
    Serial.println("[" + String(millis())+"] ");
    vTaskDelay(1000);
  }
}

void sleep (void *param)
{  
  while(1){
    if(digitalRead(27) == LOW){
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      if(digitalRead(27) == LOW){
        rtc_gpio_pullup_en(GPIO_NUM_27);
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 0);
        Serial.println("Enter Sleeping Mode...");
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Sleeping...", 80, 40, 4);
        vTaskSuspend(upload_handle);
        vTaskSuspend(blink_handle);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        esp_deep_sleep_start();
      } else{
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Button", 80, 25, 4);
        tft.drawString("Pressed", 80, 55, 4);
        vTaskSuspend(upload_handle);
        vTaskSuspend(blink_handle);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        vTaskResume(upload_handle);
        vTaskResume(blink_handle);
      }
    }
  }
}

void blink(void *param)
{
  unsigned long lastUpdate = 0;
  tft.fillScreen(TFT_BLACK);
  while(1){
    if(millis() - lastUpload > 30000){
      // vTaskResume(upload_handle);
      counter = 1;
      Serial.print("[" + String(millis())+"] ");
      Serial.println("Resume upload task");
    }
    
    timeClient.update();
    pox.update();
    
    if(millis() - lastUpdate > 500){
      Serial.print("[" + String(millis())+"] ");
      Serial.println("[T] "+timeClient.getFormattedDate());
      
      tft.drawString(timeClient.getFormattedTime(), 80, 25, 4);
      
      lastUpdate = millis();
      int bpm = pox.getHeartRate();
      int spo2 = pox.getSpO2();
      float glucose = pox.getHeartRate() * 140 / 120;
      
      Serial.print("[" + String(millis())+"] ");
      Serial.printf("BPM = %d || SPO2 = %d% || Glucose = %f.2\n", bpm, spo2, glucose);
      
      String printValue = "SPO2 "+String(spo2)+"% | Glu "+String(glucose, 2);
      
      tft.drawString(printValue, 80, 55, 2);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  // pinMode(23, OUTPUT);
  Serial.begin(115200);
  pinMode(27, INPUT_PULLUP);
 
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(CC_DATUM);
  
  boot_init();

  tft.fillScreen(TFT_BLACK);
  tft.drawString("Initialize", 80, 25, 4);
  tft.drawString("Device", 80, 55, 4);
  
  delay(2000);

  wakeup_reason();
  
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Connecting", 80, 25, 4);
  tft.drawString("Network", 80, 55, 4);
  Serial.print("[" + String(millis())+"] ");
  Serial.println("Connecting to saved connection...");
  wifi_CONNECT();
  // reconnect();
  
  delay(1000);

  if(pox.begin()){
    Serial.print("[" + String(millis())+"] ");
    Serial.println("Sensor Detected");
  }
  
  delay(1000);

  tft.fillScreen(TFT_BLACK);
  tft.drawString("Initialize", 80, 25, 4);
  tft.drawString("Completed!", 80, 55, 4);
  Serial.print("[" + String(millis())+"] ");
  Serial.println("Setup Completed");

  xTaskCreate(upload, "Upload Task", 10000, NULL, 2, &upload_handle);
  xTaskCreate(blink, "Blink Task", 5000, NULL, 1, &blink_handle);
  xTaskCreate(sleep, "Sleep Task", 5000, NULL, 1, &sleep_handle);
}

void loop() {
  // put your main code here, to run repeatedly:
}
