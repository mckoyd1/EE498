/*
C.U.R.A voice assistant function prototype
 - user can wake voice assistant with command "hello"
 then say command "help" and voice assistant will send HTTP message for help. 
 */

 
#include <Arduino.h>
#include <M5Unified.h>
#include <M5ModuleLLM.h>

M5ModuleLLM module_llm;
M5ModuleLLM_VoiceAssistant voice_assistant(&module_llm);

String Help = "call help";
String Help_data;


/* On ASR data callback */
void on_asr_data_input(String data, bool isFinish, int index)
{
    M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
    // M5.Display.setFont(&fonts::efontCN_12);  // Support Chinese display

    Help_data = data;

    M5.Display.printf(">> %s\n", data.c_str());


    /* If ASR data is finish */
    if (isFinish) {
        M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);


        M5.Display.print(">> ");


    }
};

/* On LLM data callback */
void on_llm_data_input(String data, bool isFinish, int index)
{
    M5.Display.print(data);

     Help_data = data;

     if (Help_data == "call help"){

        M5.Display.print("CALLING HELP");
    } 

    /* If LLM data is finish */
    if (isFinish) {
        M5.Display.print("\n");
    }


};

void setup()
{
    M5.begin();
    M5.Display.setTextSize(2);
    M5.Display.setTextScroll(true);

    /* Init module serial port */
    // int rxd = 16, txd = 17;  // Basic
    // int rxd = 13, txd = 14;  // Core2
    // int rxd = 18, txd = 17;  // CoreS3
    int rxd = M5.getPin(m5::pin_name_t::port_c_rxd);
    int txd = M5.getPin(m5::pin_name_t::port_c_txd);
    Serial2.begin(115200, SERIAL_8N1, rxd, txd);

    /* Init module */
    module_llm.begin(&Serial2);

    /* Make sure module is connected */
    M5.Display.printf(">> Check ModuleLLM connection..\n");
    while (1) {
        if (module_llm.checkConnection()) {
            break;
        }
    }

    /* Begin voice assistant preset */
    M5.Display.printf(">> Begin voice assistant..\n");
    int ret = voice_assistant.begin("HELLO");
    // int ret = voice_assistant.begin("你好你好", "", "zh_CN"); // Chinese kws and asr
    if (ret != MODULE_LLM_OK) {
        while (1) {
            M5.Display.setTextColor(TFT_RED);
            M5.Display.printf(">> Begin voice assistant failed\n");
        }
    }

    /* Register on ASR data callback function */
    voice_assistant.onAsrDataInput(on_asr_data_input);

    /* Register on LLM data callback function */
    voice_assistant.onLlmDataInput(on_llm_data_input);

    M5.Display.printf(">> Voice assistant ready\n");
}

void loop()
{
    /* Keep voice assistant preset update */
    voice_assistant.update();


    
}