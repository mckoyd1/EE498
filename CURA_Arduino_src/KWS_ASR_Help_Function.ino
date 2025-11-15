/*
Code function: Speak keyword to LLM module on M5 core2 device 
and device displays help message, sends signal to blynk https/app 
alerting help. 

 */
#include <Arduino.h>
//#include <M5Unified.h>
#include <M5Core2.h>
#include <M5ModuleLLM.h>
#include <WiFi.h> // For ESP32, use <ESP8266WiFi.h> for ESP8266
#include <HTTPClient.h>


#define BLYNK_TEMPLATE_ID "TMPL2ieuJcx5l"
#define BLYNK_TEMPLATE_NAME "CURA Health Watch"
#define BLYNK_AUTH_TOKEN    "RRHlA3GZ703ig6CQt6NgP4Bi9ejpF_Uz"

//const char* blynkAuthToken = "RRHlA3GZ703ig6CQt6NgP4Bi9ejpF_Uz";
const char* blynkServer = "blynk.cloud"; // Or your custom Blynk server if self-hosting


// Your WiFi credentials
char ssid[] = "DMCKOY2.4";
char pass[] = "degan020304"; 

int V0_value;
int zero = 0; // used to update blynk alert value
int one = 1; // used to update blynk alert value

#include <BlynkSimpleEsp32.h> // For ESP32, use <BlynkSimpleEsp8266.h> for ESP8266

M5ModuleLLM module_llm;

/* Must be capitalized */
String wake_up_keyword = "CALL HELP";
// String wake_up_keyword = "你好你好";
String kws_work_id;
String asr_work_id;
String language;



/* ================================================================
=========================== Function prototypes ===================
=================================================================== */ 

void Cura_SettingBtnC();
void sendAlert();  //send alert to blynk cloud/app





void setup()
{   
    M5.begin();
    Blynk.run();
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
}

void loop()
{
    /* Update ModuleLLM */
    module_llm.update();

    /* Handle module response messages */
    for (auto& msg : module_llm.msg.responseMsgList) {
        /* If KWS module message */
        if (msg.work_id == kws_work_id) {
            M5.Lcd.setTextColor(TFT_GREENYELLOW);
           M5.Lcd.printf(">> !!CALLING HELP!!\n");

            sendAlert();

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
