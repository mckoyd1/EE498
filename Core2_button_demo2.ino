#include <M5Core2.h>
#include <Wire.h>     // I2C communication library
#include "MAX30100_PulseOximeter.h"

 ButtonColors on_clrs = { BLACK, WHITE, WHITE }; // Text color, outline color, fill color when pressed
 ButtonColors off_clrs = {ORANGE, WHITE, WHITE }; // Text color, outline color, fill color when not pressed
 ButtonColors off = { BLACK, BLACK, BLACK }; // Text color, outline color, fill color when not pressed
 PulseOximeter HR_SP;    //This creates a HR/SPo2 object

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

}

void loop() {
  M5.update(); // Update button and touch states

   
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

  delay(100); // Small delay for stability



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

  if (M5.BtnA.wasPressed()) {
    
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
