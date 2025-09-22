#include <M5Core2.h>
#include <Wire.h>     // I2C communication library
#include "MAX30100_PulseOximeter.h"
//#include <M5Unified.h> // Include the M5Unified library
#include <SD.h>       // Include the SD card library
//#include <M5Display.h>
//#include <M5GFX.h>

 ButtonColors on_clrs = { BLACK, WHITE, WHITE }; // Text color, outline color, fill color when pressed
 ButtonColors off_clrs = {ORANGE, WHITE, WHITE }; // Text color, outline color, fill color when not pressed
 ButtonColors off = { BLACK, BLACK, BLACK }; // Text color, outline color, fill color when not pressed
 PulseOximeter HR_SP;    //This creates a HR/SPo2 object

RTC_TimeTypeDef RTC_TimeStruct; // Structure to hold time data
RTC_DateTypeDef RTC_DateStruct; // Structure to hold date data


//Function prototypes//
void Cura_SettingBtnA();
void Heartbtn_funct();
void tempbtn_funct();
void SPO2btn_funct();




  // Variables for storing heart rate and SpO2
uint32_t tsLastReport = 0;
const int REPORTING_PERIOD_MS = 1000; // Report every 1 second

void onBeatDetected() {
    M5.Lcd.println("♥ Beat!");
}


void setup() {
  M5.begin(); // Initialize M5Stack Core2
  M5.Lcd.setTextFont(2); // Set a suitable font for display
  M5.Lcd.setTextSize(2); // Set text size
  M5.Lcd.setTextColor(WHITE, BLACK); // Set text color to white on black background


}



void loop() {

  setup();
  M5.update(); // Update button and touch states

  M5.Rtc.GetTime(&RTC_TimeStruct); // Get current time from RTC
  M5.Rtc.GetDate(&RTC_DateStruct); // Get current date from RTC

  //M5.Lcd.clear();
  M5.Lcd.setCursor(0, 0); // Set cursor to the top-left corner
  M5.Lcd.printf("Date: %04d-%02d-%02d\n", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
  M5.Lcd.printf("Time: %02d:%02d:%02d\n", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);

/////////////////////button A settings menu and functions/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
if (M5.BtnA.wasPressed()) {
    
   Cura_SettingBtnA();

}





    else if (M5.BtnB.wasPressed()) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("Button B Pressed!");

  } else if (M5.BtnC.wasPressed()) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("Button C Pressed!");
  }

  delay(10); // Small delay for stability



}






//Function definitions///////////////////////////////////////////

void Cura_SettingBtnA(){
    M5.Lcd.fillScreen(WHITE); // Clear screen
    M5.Lcd.setCursor(0, 0);

    Button HeartButton(5, 4, 311, 72, false, "Heart Rate", off_clrs, on_clrs, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)

    Button SpoButton(5, 80, 311, 72, false, "SPo2", off_clrs, on_clrs, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)

    Button TempButton(5, 158, 311, 72, false, "Temperature", off_clrs, on_clrs, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)








    for(;;) {
      M5.update(); // Update button and touch states


    ////////////////Calls for Heart Rate function if HeartButton touch button pressed//////////////////////////////////////////////////////////////////////////////////////////////////////
    if (HeartButton.wasPressed()) {
      Heartbtn_funct();

    break;  
    }


  ////////////////Calls for SP02 function if SP02 touch button pressed/////////////////////////////////
   if (SpoButton.wasPressed()) { 
      SPO2btn_funct();

    break;  
    }


  //////////////////////// calls for temperature function if temperature touch button pressed ////////////////////////
  if (TempButton.wasPressed()) {
      tempbtn_funct();

  break;
  }

  if (M5.BtnB.wasPressed()) {  //if button B pressed then return to home screen. 
    
  return;
  }

  } 


return;
}



void Heartbtn_funct(){
     M5.Lcd.fillScreen(BLACK); // Clear screen


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
    M5.Lcd.setCursor(80, 100);
    M5.Lcd.print("Heart Rate: " ); 
    M5.Lcd.print(HR_SP.getHeartRate());
    M5.Lcd.println("    bpm");

    tsLastReport = millis();
    }

  if (M5.BtnA.wasPressed()) {       //return back to sensor selection menu
    
    break;
    }

    //////////////////////////////////////////////////////////////////////////////////    
        if (M5.BtnB.wasPressed()) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(80, 100);
    M5.Lcd.println("Exiting loop!"); 
    return;
    }

        //break;

    }
return;

}

void tempbtn_funct(){

 M5.Lcd.fillScreen(BLACK); // Clear screen


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

    Button TempButton(5, 4, 311, 72, false, "Temperature", off, off, MC_DATUM );
    // (x, y, width, height, isToggle, label, off_colors, on_colors, datum)
    
    //////////read and print heart rate values to LCD here in this section/////////////
     // Report heart rate and SpO2 at regular intervals
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

    M5.Lcd.clear();
    M5.Lcd.setCursor(80, 100);
    M5.Lcd.print("Temp: " ); 
    M5.Lcd.print(HR_SP.getSpO2());
    M5.Lcd.println("    *");

    tsLastReport = millis();
    }


    if (M5.BtnA.wasPressed()) {     //retun back to sensor selection menu
    
    break;
    }

    //////////////////////////////////////////////////////////////////////////////////    
        if (M5.BtnB.wasPressed()) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(80, 100);
    M5.Lcd.println("Exiting loop!"); 
    break;
    }
    
        //break;

    }
    
return;
}


void SPO2btn_funct(){

 M5.Lcd.fillScreen(BLACK); // Clear screen


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
    M5.Lcd.setCursor(80, 100);
    M5.Lcd.print("SP02: " ); 
    M5.Lcd.print(HR_SP.getSpO2());
    M5.Lcd.println("    %");

    tsLastReport = millis();
    }

    if (M5.BtnA.wasPressed()) {     //retun back to sensor selection menu
    
    break;
    }


    //////////////////////////////////////////////////////////////////////////////////    
        if (M5.BtnB.wasPressed()) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(80, 100);
    M5.Lcd.println("Exiting!"); 
    break;
    }
    
        //break;

    }
    
return;
}
