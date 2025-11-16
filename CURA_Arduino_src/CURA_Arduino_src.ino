#include <M5Core2.h>
#include <Wire.h>     // I2C communication library
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>
#include <SD.h>       // Include the SD card library
#include <WiFi.h> // For ESP32, use <ESP8266WiFi.h> for ESP8266
#include <HTTPClient.h>
#include "CuraImage.h"  //Says include local header file
//#include <M5Stack.h>
//#include "LLM_Function.h" //Says include local LLM function header file
#include <Arduino.h>
//#include <M5Unified.h>
#include <M5ModuleLLM.h>

#define BLYNK_TEMPLATE_ID "TMPL2ieuJcx5l"
#define BLYNK_TEMPLATE_NAME "CURA Health Watch"
#define BLYNK_AUTH_TOKEN    "RRHlA3GZ703ig6CQt6NgP4Bi9ejpF_Uz"
//#define PAHUB2_ADDRESS 0x71 

//const char* blynkAuthToken = "RRHlA3GZ703ig6CQt6NgP4Bi9ejpF_Uz";
const char* blynkServer = "blynk.cloud"; // Or your custom Blynk server if self-hosting


//WiFi credentials
 char ssid[] = "DMCKOY2.4";
char pass[] = "degan020304";  

/* char ssid[] = "UNLV-PSK";
char pass[] = "XTq7c94pcu"; 
 */


int V0_value;
int zero = 0; // used to update blynk alert value
int one = 1; // used to update blynk alert value
M5ModuleLLM module_llm;


String wake_up_keyword = "CALL HELP";
String kws_work_id;
String asr_work_id;
String language;

 ButtonColors on_clrs = { BLACK, WHITE, WHITE }; // Text color, outline color, fill color when pressed
 ButtonColors Alert_clrs = { RED, WHITE, WHITE };
 ButtonColors off_clrs = {ORANGE, WHITE, WHITE }; // Text color, outline color, fill color when not pressed
 ButtonColors off = { BLACK, BLACK, BLACK }; // Text color, outline color, fill color when not pressed
 ButtonColors off_mode = { BLACK, WHITE, WHITE }; // Text color, outline color, fill color when not pressed
 PulseOximeter HR_SP;    //This creates a HR/SPo2 object
 ButtonColors off_clrs2 = {RED, WHITE, WHITE }; // Text color, outline color, fill color when not pressed


#include <BlynkSimpleEsp32.h> // For ESP32, use <BlynkSimpleEsp8266.h> for ESP8266

RTC_TimeTypeDef RTC_TimeStruct; // Structure to hold time data
RTC_DateTypeDef RTC_DateStruct; // Structure to hold date data

Adafruit_MLX90614 mlx;

/* M5ModuleLLM module_llm;
M5ModuleLLM_VoiceAssistant voice_assistant(&module_llm); */



/* ================================================================
=========================== Function prototypes ===================
=================================================================== */ 

void Cura_SettingBtnA();
void Cura_SettingBtnB();
void Cura_SettingBtnC();
void Cura_SettingBtnC_LLM();
void Heartbtn_funct();
void tempbtn_funct();
void SPO2btn_funct();
void Font_Setup();
void Detection_Off();
void Detection_On();


/* ================================================================
============================== Variables ==========================
=================================================================== */

  // Variables for storing heart rate, temp and SpO2
uint32_t tsLastReport = 0;
const int REPORTING_PERIOD_MS = 1000; // Report every 1 second
uint32_t TempF;    //VARIABLE FOR STORING TEMPERATURE
//int Current_Mode;




/* ================================================================
======================== LLM Module functions =====================
=================================================================== */

//place LLM module functions here




/* ===============================================================
======================= System setup =============================
================================================================== */

void setup() {


  /* ===============================================================
  ===================== CURA Setup =================================
  ================================================================== */

  M5.begin(); // Initialize M5Stack Core2
  M5.Rtc.begin(); // Initialize the RTC module
  Blynk.run();
  M5.Lcd.fillScreen(WHITE); // Clear screen
  M5.Lcd.setTextColor(BLACK);

  Wire.begin(); // Initialize I2C communication
 
 // Wire.beginTransmission(PAHUB2_ADDRESS);
  //Wire.write(1 << 0);
  //Wire.endTransmission();


/*================================================================
 ====================== Voice Assistant Setup =====================
 ================================================================== */


  M5.Lcd.setTextSize(2);
    //M5.Lcd.setTextScroll(true);
    // M5.Display.setFont(&fonts::efontCN_12);  // Support Chinese display

    language = "en_US";

    /* Init module serial port */
    #define RXD2_PIN 13
    #define TXD2_PIN 14
    //int rxd = M5.getPin(m5::pin_name_t::port_c_rxd);
    //int txd = M5.getPin(m5::pin_name_t::port_c_txd);
    Serial2.begin(115200, SERIAL_8N1, RXD2_PIN, TXD2_PIN);

    /* Init module */
    module_llm.begin(&Serial2);

    /* Make sure module is connected */
    M5.Lcd.printf(">> Check ModuleLLM connection..\n");
    while (1) {
        if (module_llm.checkConnection()) {
            break;
        }
    }

    /* Reset ModuleLLM */
    M5.Lcd.printf(">> Reset ModuleLLM..\n");
    module_llm.sys.reset();

    /* Setup Audio module */
    M5.Lcd.printf(">> Setup audio..\n");
    module_llm.audio.setup();

    /* Setup KWS module and save returned work id */
    M5.Lcd.printf(">> Setup kws..\n");
    m5_module_llm::ApiKwsSetupConfig_t kws_config;
    kws_config.kws = wake_up_keyword;
    kws_work_id    = module_llm.kws.setup(kws_config, "kws_setup", language);

    /* Setup ASR module and save returned work id */
    M5.Lcd.printf(">> Setup asr..\n");
    m5_module_llm::ApiAsrSetupConfig_t asr_config;
    asr_config.input = {"sys.pcm", kws_work_id};
    asr_work_id      = module_llm.asr.setup(asr_config, "asr_setup", language);

    M5.Lcd.printf(">> Setup ok\n>> Say \"%s\" to wakeup\n", wake_up_keyword.c_str()); 

    M5.Lcd.clear();
    M5.Lcd.fillScreen(WHITE);
    
}






/* ================================================================
========================== Main code ==============================
=================================================================== */

void loop() {

   /* Update ModuleLLM */
    module_llm.update();

    /* Handle module response messages */
    for (auto& msg : module_llm.msg.responseMsgList) {
        /* If KWS module message */
        if (msg.work_id == kws_work_id) {
            M5.Lcd.setTextColor(RED);
            M5.Lcd.setCursor(6, 207);
            M5.Lcd.printf(">> !!CALLING HELP!!\n");

            Cura_SettingBtnC_LLM();
            //sendAlert();

            //delay(5000);
        }

        /* If ASR module message */
        if (msg.work_id == asr_work_id) {
            /* Check message object type */
            if (msg.object == "asr.utf-8.stream") {
                /* Parse message json and get ASR result */
                JsonDocument doc;
                deserializeJson(doc, msg.raw_msg);
                String asr_result = doc["data"]["delta"].as<String>();

                M5.Lcd.setTextColor(TFT_YELLOW);
                M5.Lcd.printf(">> %s\n", asr_result.c_str());
                
            }
        }
    }

    /* Clear handled messages */
    module_llm.msg.responseMsgList.clear();

  Font_Setup(2, 2, BLACK, WHITE);
  M5.update(); // Update button and touch states


  M5.Lcd.pushImage(50, 60, HEART2_WIDTH, HEART2_HEIGHT, (uint8_t*)Heart2);    //displays home screen image
  

  M5.Rtc.GetTime(&RTC_TimeStruct); // Get current time from RTC
  M5.Rtc.GetDate(&RTC_DateStruct); // Get current date from RTC

  
  M5.Lcd.setCursor(0, 0); // Set cursor to the top-left corner
  M5.Lcd.printf("Date: %04d-%02d-%02d\n", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
  M5.Lcd.printf("Time: %02d:%02d:%02d\n", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);


//button A settings menu and functions
if (M5.BtnA.wasPressed()) {
   
   M5.Axp.SetVibration(200);
   delay(100);
   M5.Axp.SetVibration(0);

   Cura_SettingBtnA();

}

//button B settings menu and functions
if (M5.BtnB.wasPressed()) {
    
   M5.Axp.SetVibration(200);
   delay(100);
   M5.Axp.SetVibration(0);

   Cura_SettingBtnB();

   M5.Lcd.fillScreen(WHITE); // Clear screen
   

}

if (M5.BtnC.wasPressed()) {
    
   M5.Axp.SetVibration(200);
   delay(100);
   M5.Axp.SetVibration(0);

   Cura_SettingBtnC();

}
  delay(10); // Small delay for stability
}






/* ===============================================================
========================== FUNCTION DEFINITIONS ==================
================================================================== */

void Cura_SettingBtnA(){

    M5.Lcd.fillScreen(WHITE); // Clear screen
    M5.Lcd.setCursor(0, 0);

    Button HeartButton(5, 4, 311, 72, false, "Heart Rate", off_clrs2, on_clrs, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)

    Button SpoButton(5, 80, 311, 72, false, "SPo2", off_clrs2, on_clrs, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)

    Button TempButton(5, 158, 311, 72, false, "Temperature", off_clrs2, on_clrs, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)








    for(;;) {
      M5.update(); // Update button and touch states


    ////////////////Calls for Heart Rate function if HeartButton touch button pressed//////////////////////////////////////////////////////////////////////////////////////////////////////
    if (HeartButton.wasPressed()) {

         M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

      Heartbtn_funct();

    break;  
    }


  ////////////////Calls for SP02 function if SP02 touch button pressed/////////////////////////////////
   if (SpoButton.wasPressed()) { 

         M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        SPO2btn_funct();

    break;  
    }


  //////////////////////// calls for temperature function if temperature touch button pressed ////////////////////////
  if (TempButton.wasPressed()) {

     M5.Axp.SetVibration(200);
    delay(100);
    M5.Axp.SetVibration(0);

      tempbtn_funct();

  break;
  }

  if (M5.BtnA.wasPressed()) {     //retun back to sensor selection menu

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        break;
        }

    //////////////////////////////////////////////////////////////////////////////////    
    if (M5.BtnB.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }

    if (M5.BtnC.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }
  } 


return;
}

void Cura_SettingBtnB(){

    M5.Lcd.fillScreen(WHITE); // Clear screen
    M5.Lcd.setCursor(0, 0);
    //M5.Lcd.setTextSize(3);

    Button ModeOn_Button(5, 4, 311, 72, false, "Detect On", off_clrs, on_clrs, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)

    Button ModeOff_Button(5, 80, 311, 72, false,"Detection Off", off_clrs, on_clrs, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)

    Button Med_Remind(5, 158, 311, 72, false, "Set Med Remind", off_clrs, on_clrs, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)








    for(;;) {
      M5.update(); // Update button and touch states


    ////////////////Calls for function to turn on fall detection//////////////////
    if (ModeOn_Button.wasPressed()) {

         M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen

        //Turn on sensor functions here
        Detection_On();
    break;  
    }


  ////////////////Calls for function to turn off fall detection/////////////////////////////////
   if (ModeOff_Button.wasPressed()) { 

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen

        //Turn off sensor functions here
        Detection_Off();

    break;  
    }


  //////////////////////// Can use this to set up additional functions when button pressed ////////////////////////
   if (Med_Remind.wasPressed()) {

    M5.update();

     M5.Axp.SetVibration(200);
    delay(100);
    M5.Axp.SetVibration(0);


      

  break;
  } 

  if (M5.BtnA.wasPressed()) {     //retun back to sensor selection menu

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        break;
        }

    //////////////////////////////////////////////////////////////////////////////////    
    if (M5.BtnB.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }

    if (M5.BtnC.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }
  } 


return;
}
 

//////////////Press button to send alert to blynk cloud ////////////////////
void Cura_SettingBtnC(){

    //Blynk.run();

    M5.Lcd.fillScreen(WHITE); // Clear screen
    M5.Lcd.setCursor(0, 0);

    Button HelpButton(5, 4, 311, 230, false, "Press for Help", off_clrs, on_clrs, MC_DATUM );
    
    //Blynk.run();

 
    for(;;) {
      M5.update(); // Update button and touch states


    ////////////////Calls for Heart Rate function if HeartButton touch button pressed//////////////////////////////////////////////////////////////////////////////////////////////////////
    if (HelpButton.wasPressed()) {


      M5.Lcd.clear();
      M5.Lcd.fillScreen(WHITE); // Clear screen
      sendAlert();  //send alert to blynk cloud/app




      for(;;){
          M5.update(); // Update button and touch states



    //Alerted_HelpButton.draw();
        Button Alerted_HelpButton(5, 4, 311, 230, false, "Alerted Help!!!", Alert_clrs, on_clrs, MC_DATUM );

        M5.Axp.SetVibration(255);
        delay(100);
        M5.Axp.SetVibration(0);

      if (M5.BtnA.isPressed()) {     //retun back to sensor selection menu

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        break;
        }

    //////////////////////////////////////////////////////////////////////////////////    
    if (M5.BtnB.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }

    if (M5.BtnC.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }

    delay(100);
    }
    }


  if (M5.BtnA.wasPressed()) {     //retun back to sensor selection menu

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        break;
        }

    //////////////////////////////////////////////////////////////////////////////////    
    if (M5.BtnB.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }

    if (M5.BtnC.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }
  } 


return;
}

void Cura_SettingBtnC_LLM(){

     sendAlert();

    //Blynk.run();
    M5.Lcd.clear();
    M5.Lcd.fillScreen(WHITE); // Clear screen
    M5.Lcd.setCursor(0, 0);



    ////////////////Calls for Heart Rate function if HeartButton touch button pressed////////////////////////////////////////////////////////////////////////////////////////////////////
      




      for(;;){
          M5.update(); // Update button and touch states



    //Alerted_HelpButton.draw();
        Button Alerted_HelpButton(5, 4, 311, 230, false, "Alerted Help!!!", Alert_clrs, on_clrs, MC_DATUM );

        M5.Axp.SetVibration(255);
        delay(100);
        M5.Axp.SetVibration(0);

      if (M5.BtnA.isPressed()) {     //retun back to sensor selection menu

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        break;
        }

    //////////////////////////////////////////////////////////////////////////////////    
    if (M5.BtnB.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }

    if (M5.BtnC.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }

    delay(100);
    }
    


  

return;
}

void Heartbtn_funct(){ 
     M5.Lcd.fillScreen(BLACK); // Clear screen
    M5.Lcd.setTextColor(WHITE, BLACK); // Set text color to white on black background

    Button HeartButton(5, 4, 311, 72, false, "Heart Rate", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)
    
    Button SpoButton(5, 80, 311, 72, false, "SPo2", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)

    Button TempButton(5, 158, 311, 72, false, "Temperature", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)


     // Initialize the MAX30100 sensor
    if (!HR_SP.begin()) {
        M5.Lcd.println("FAILED: MAX30100 not found or I2C error.");
        while (1); // Halt if sensor initialization fails

    } 
    
    else {
        M5.Lcd.println("MAX30100 initialized successfully.");
    }

        for(;;) {
         M5.update(); // Update button and touch states
         HR_SP.update(); //update sensor data for continuous reading

    Button HeartButton(5, 4, 311, 72, false, "Heart Rate", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)
    
    //////////read and print heart rate values to LCD here in this section/////////////
     // Report heart rate and SpO2 at regular intervals
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    
    M5.Lcd.clear();
    M5.Lcd.pushImage(10, 95, HEART_WIDTH, HEART_HEIGHT, (uint8_t*)heart);    //displays home screen image
    M5.Lcd.setCursor(70, 130);
    M5.Lcd.setTextSize(2); // Set text size
    M5.Lcd.print("HR: " ); 
    M5.Lcd.print(HR_SP.getHeartRate());
    //M5.Lcd.println(" bpm");

    tsLastReport = millis();
    }



   if (M5.BtnA.wasPressed()) {     //retun back to sensor selection menu

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        HR_SP.shutdown();
        break;
        }

    //////////////////////////////////////////////////////////////////////////////////    
    if (M5.BtnB.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        HR_SP.shutdown();
        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }

    if (M5.BtnC.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        HR_SP.shutdown();
        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }

    

    }
return;

}

void tempbtn_funct(){
 M5.Lcd.clear();
 M5.Lcd.fillScreen(BLACK); // Clear screen
 M5.Lcd.setTextColor(WHITE, BLACK); // Set text color to white on black background


    Button HeartButton(5, 4, 311, 72, false, "Heart Rate", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)
    
    Button SpoButton(5, 80, 311, 72, false, "SPo2", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)

    Button TempButton(5, 158, 311, 72, false, "Temperature", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)


     // Initialize the MLX90614 sensor
    if (!mlx.begin()) {
        M5.Lcd.println("FAILED: MLX90614 not found or I2C error.");
        while (1); // Halt if sensor initialization fails

    } 
    
    else {
        M5.Lcd.println("MLX90614 initialized successfully.");
    }

        for(;;) {
         M5.update(); // Update button and touch states
        

    Button TempButton(5, 4, 311, 72, false, "Temperature", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)
    
    //////////read and print heart rate values to LCD here in this section/////////////
     // Report heart rate and SpO2 at regular intervals
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

    TempF = mlx.readObjectTempF();


    M5.Lcd.clear();
    M5.Lcd.pushImage(10, 70, THERMOMETER_PNG_WIDTH, THERMOMETER_PNG_HEIGHT, (uint8_t*)Thermometer_PNG);    //displays home screen image
    M5.Lcd.setCursor(100, 130);
    M5.Lcd.print("Temp: " ); 
    M5.Lcd.print(TempF);
    M5.Lcd.println("*");

    tsLastReport = millis();
    }
    

    if (M5.BtnA.wasPressed()) {     //retun back to sensor selection menu

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        break;
        }

    //////////////////////////////////////////////////////////////////////////////////    
    if (M5.BtnB.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }

    if (M5.BtnC.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }
    

    }
    
return;
}


void SPO2btn_funct(){

 M5.Lcd.fillScreen(BLACK); // Clear screen
 M5.Lcd.setTextColor(WHITE, BLACK); // Set text color to white on black background


    Button HeartButton(5, 4, 311, 72, false, "Heart Rate", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)
    
    Button SpoButton(5, 80, 311, 72, false, "SPo2", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)

    Button TempButton(5, 158, 311, 72, false, "Temperature", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)


     // Initialize the MAX30100 sensor
    if (!HR_SP.begin()) {
        M5.Lcd.println("FAILED: MAX30100 not found or I2C error.");
        while (1); // Halt if sensor initialization fails

    } 
    
    else {
        M5.Lcd.println("MAX30100 initialized successfully.");
    }

        for(;;) {
         M5.update(); // Update button and touch states
         HR_SP.update(); //update sensor data for continuous reading

    Button SPoButton(5, 4, 311, 72, false, "SP02", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)
    
    //////////read and print heart rate values to LCD here in this section/////////////
     // Report heart rate and SpO2 at regular intervals
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

        M5.Lcd.clear();
        M5.Lcd.pushImage(10, 70, PULSEOX_PNG_WIDTH, PULSEOX_PNG_HEIGHT, (uint8_t*)PulseOX_PNG);    //displays home screen image
        M5.Lcd.setCursor(100, 130);
        M5.Lcd.print("SP02: " ); 
        M5.Lcd.print(HR_SP.getSpO2());
        M5.Lcd.println("%");

        tsLastReport = millis();
    }

    if (M5.BtnA.wasPressed()) {     //retun back to sensor selection menu

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        HR_SP.shutdown();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        break;
        }

    //////////////////////////////////////////////////////////////////////////////////    
    if (M5.BtnB.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        HR_SP.shutdown();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }

    if (M5.BtnC.wasPressed()) {

        M5.Axp.SetVibration(200);
        delay(100);
        M5.Axp.SetVibration(0);

        M5.Lcd.clear();
        HR_SP.shutdown();
        M5.Lcd.fillScreen(WHITE); // Clear screen
        return;
    }
    
      

    }
    
return;
}


// Function to send an alert to blynk cloud app
void sendAlert() {
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  if (V0_value == one){
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Blynk.virtualWrite(V0, zero); // Send to Virtual Pin V1
  Serial.println("Alert sent to Blynk!");

  V0_value = zero;

  }

  else {
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Blynk.virtualWrite(V0, one); // Send to Virtual Pin V1
  Serial.println("Alert sent to Blynk!");

  V0_value = one;
  
  }


}


//settings for home screen font
void Font_Setup( int font, int size, uint16_t a, uint16_t b){
   M5.Lcd.setTextFont(font); // Set a suitable font for display
   M5.Lcd.setTextSize(size); // Set text size
   M5.Lcd.setTextColor(a, b); // Set text color to black on white background 
}

void Detection_On(){

    M5.update();
    M5.Lcd.fillScreen(BLACK); // Clear screen
    M5.Lcd.setTextColor(WHITE, BLACK); // Set text color to white on black background
    M5.Lcd.clear();
    M5.Lcd.setCursor(10, 120);

    M5.Lcd.println("Fall Detection On!");
    delay(2000);

    M5.Lcd.clear();
    M5.Lcd.fillScreen(WHITE); // Clear screen

    return;
}


void Detection_Off(){

    M5.update();
    M5.Lcd.fillScreen(BLACK); // Clear screen
    M5.Lcd.setTextColor(WHITE, BLACK); // Set text color to white on black background
    M5.Lcd.clear();
    M5.Lcd.setCursor(10, 120);

    M5.Lcd.println("Fall Detection Off!");
    delay(2000);

    M5.Lcd.clear();
    M5.Lcd.fillScreen(WHITE); // Clear screen

    return;
}

void MacAdd(){

    M5.Lcd.fillScreen(BLACK); // Clear screen
    M5.Lcd.setTextColor(WHITE, BLACK); // Set text color to white on black background
    M5.Lcd.clear();
    M5.Lcd.setCursor(10, 120);

    //String deviceName = WiFi.getHostname();

    M5.Lcd.print("Name: ");
    M5.Lcd.println(WiFi.getHostname());

    //M5.Lcd.print("MAC Address: ");
    //M5.Lcd.println(WiFi.macAddress());


    delay(5000);

    M5.Lcd.clear();
    M5.Lcd.fillScreen(WHITE); // Clear screen

    return;
}