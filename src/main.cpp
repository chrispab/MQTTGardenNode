//#define DEBUG
#define RELEASE

#include "Arduino.h"
#include "version.h"
#include <NewRemoteTransmitter.h>
#include <RF24.h>
#include <WiFi.h>
#include <stdlib.h> // for dtostrf(FLOAT,WIDTH,PRECSISION,BUFFER);
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "MyOTA.h"
#include "Display.h"
#include "ZoneController.h"
#include "secret.h"
#include "LedFader.h"
#include "TempSensor.h"
#include "sendemail.h"
#include "pins.h"

//time stuff
#include <NTPClient.h>
#include <WiFiUdp.h>
#define NTP_OFFSET 60 * 60     // In seconds, 0 for GMT, 60*60 for BST
#define NTP_INTERVAL 60 * 1000 // In miliseconds
#define NTP_ADDRESS "europe.pool.ntp.org"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
//NTPClient timeClient(ntpUDP);

#include "MQTTLib.h"
extern char subscribeTopic[];   // = "433Bridge/cmnd/#";
extern char publishTempTopic[]; // = "433Bridge/Temperature";
extern char publishHumiTopic[]; // = "433Bridge/Humidity";
extern bool MQTTNewData;


// forward decs
void checkConnections(void);
//void updateDisplayData(void);
void resetWatchdog(void);
//boolean processTouchPads(void);

void IRAM_ATTR resetModule();

// DHT22 stuff
///TempSensor DHT22Sensor;

//MQTT stuff
#include <PubSubClient.h>
IPAddress mqttBroker(192, 168, 0, 200);
WiFiClient WiFiEClient;
PubSubClient MQTTclient(mqttBroker, 1883, MQTTRxcallback, WiFiEClient);

// 433Mhz settings
// 282830 addr of 16ch remote
// param 3 is pulse width, last param is num times control message  is txed
//#include "My433Transmitter.h"
//NewRemoteTransmitter transmitter(282830, TX433PIN, 260, 4);
//My433Transmitter transmitter(282830, TX433PIN, 260, 4);

//#include "RF24Lib.h" //// Set up nRF24L01 rf24Radio on SPI bus plus pins 7 & 8
//RF24 rf24Radio(RF24_CE_PIN, RF24_CS_PIN);

//#include "LightSensor.h"
//LightSensor myLightSensor(LDR_PIN);

// Global vars
unsigned long currentMillis = 0;
unsigned long previousConnCheckMillis = 0;
unsigned long intervalConnCheckMillis = 63000;

unsigned long intervalTempDisplayMillis = 60000;
unsigned long previousTempDisplayMillis =
    millis() - intervalTempDisplayMillis; // trigger on start

// create the display object
Display myDisplay(U8G2_R0, /* reset=*/U8X8_PIN_NONE, OLED_CLOCK_PIN, OLED_DATA_PIN);
//ZoneController ZCs[3] = {ZoneController(0, 14, "GRG", "GGG"),
                        // ZoneController(1, 4, "CNV", "CCC"),
                        // ZoneController(2, 15, "SHD", "SSS")};

WiFiServer server(80);

// create object
//SendEmail e("smtp.gmail.com", 465, EMAIL_ADDRESS, APP_PASSWORD,
//2000, true);
// set parameters. pin 13, go from 0 to 255 every n milliseconds
LedFader heartBeatLED(ESP32_ONBOARD_BLUE_LED_PIN, 1, 0, 70, 1000, true);
LedFader warnLED(RED_LED_PIN, 2, 0, 255, 451, true);

#include <WebSerial.h>
WebSerial myWebSerial;

//#include "WebSocketLib.h"

//WebSocketsServer webSocket = WebSocketsServer(81);

//#define EMAIL_SUBJECT "ESP32 Bridge - REBOOTED"

//#include "WebSocketLib.h"

//extern void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

#include "WiFiLib.h"
//#include "WebPageLib.h"

#include "SupportLib.h"
extern boolean processTouchPads(void);
extern char *getElapsedTimeStr();
extern void updateDisplayData();
extern void checkConnections();
extern displayModes displayMode;
extern boolean touchedFlag; // = false;

//#include "TouchPad.h"
//TouchPad touchPad1 = TouchPad(TOUCH_SENSOR_1);
//TouchPad touchPad2 = TouchPad(TOUCH_SENSOR_2);

//! WATCHDOG STUFF
hw_timer_t *timer = NULL;

// ! big issue - does not work when no internet connection - resolve

#define myWEBHOOk "https://maker.ifttt.com/trigger/ESP32BridgeBoot/with/key/dF1NEy_aQ5diUyluM3EKcd"
#include <IFTTTWebhook.h>
IFTTTWebhook myWebhook(IFTTT_API_KEY, IFTTT_EVENT_NAME);


#include "MoistureSensor.h"
MoistureSensor myMoistureSensor(CSMS_PIN);


void setup()
{ // Initialize serial monitor port to PC and wait for port to
#ifdef DEBUG
    Serial.println(1);
#endif
    Serial.begin(115200);
    //myWebSerial.println("==========running setup==========");
    //MQTTLibSetup();
    heartBeatLED.begin();                        // initialize
    warnLED.begin();                             // initialize
    pinMode(ESP32_ONBOARD_BLUE_LED_PIN, OUTPUT); // set the LED pin mode
    // // setup OLED display
    // displayMode = NORMAL;
    // displayMode = BIG_TEMP;
    // //displayMode = MULTI;
    // myDisplay.begin();
    // myDisplay.setFont(SYS_FONT);
    // myDisplay.wipe();
    // myDisplay.writeLine(1, TITLE_LINE1);
    // myDisplay.writeLine(2, TITLE_LINE2);
    // myDisplay.writeLine(3, TITLE_LINE3);
    // myDisplay.writeLine(4, TITLE_LINE4);
    // myDisplay.writeLine(5, TITLE_LINE5);
    // myDisplay.writeLine(6, TITLE_LINE6);
    // myDisplay.refresh();
    // delay(1000);

    // myDisplay.wipe();
    // myDisplay.writeLine(1, "Connecting to Sensor..");
    // myDisplay.refresh();
    // DHT22Sensor.setup(DHTPIN, DHT22Sensor.AM2302);
    // rf24 stuff
    // myDisplay.writeLine(2, "Connecting to RF24..");
    // myDisplay.refresh();
    // connectRF24();
    // attempt to connect to Wifi network:
    //myDisplay.writeLine(3, "Connecting to WiFi..");
    //myDisplay.refresh();
    connectWiFi();
    // you're connected now, so print out the status:
    printWifiStatus();
    //server.begin();
    //CR;
    //myDisplay.writeLine(4, "Connecting to MQTT..");
    //myDisplay.refresh();
    connectMQTT();
    //myDisplay.writeLine(5, "All Connected");
    //myDisplay.refresh();
    timeClient.begin();
    timeClient.update();
    Serial.println(timeClient.getFormattedTime());
    delay(200);

    //! watchdog setup
    timer = timerBegin(0, 80, true); // timer 0, div 80
    timerAttachInterrupt(timer, &resetModule, true);
    // n0 secs
    timerAlarmWrite(timer, 30000000, false); // set time in us
    timerAlarmEnable(timer);                 // enable interrupt
    //myDisplay.wipe();
    //connectWiFi();
    resetWatchdog();
    // webSocket.begin();
    // webSocket.onEvent(webSocketEvent);
    setupOTA();
    resetWatchdog();

    //MQTTclient.
    //myWebhook.trigger("433Bridge BootReboot");
   // myLightSensor.getLevel();
}

/**
 * @brief 
 * 
 */
void loop()
{
#ifdef DEBUG
    Serial.print("1..");
#endif
    checkMoistureSensor();
    //checkLightSensor();
    //checkPIRSensor();
    checkConnections(); // and reconnect if reqd

    MQTTclient.loop();    // process any MQTT stuff, returned in callback
    //processMQTTMessage(); // check flags set above and act on
    ArduinoOTA.handle();
    resetWatchdog();
    heartBeatLED.update(); // initialize
    //webSocket.loop();

    timeClient.update();

    //broadcastWS();
    //if new readings taken, op to serial etc
    // if (DHT22Sensor.publishReadings(MQTTclient, publishTempTopic, publishHumiTopic))
    // {
    //     myWebSerial.println("New Sensor Readings-MQTT published");
    // }

    // webSocket.loop();
    // broadcastWS();
    MQTTclient.loop(); // process any MQTT stuff, returned in callback
    ArduinoOTA.handle();

    // touchedFlag = touchPad1.getState();
    // (touchPad1.getState()) ? displayMode = MULTI : displayMode = BIG_TEMP;

    // if (touchPad2.getState())
    // {
    //     if (millis() % 2000 == 0)
    //     {
    //         warnLED.fullOn();
    //         delay(10);
    //         warnLED.fullOff();
    //         //!  MQTTclient.publish("433Bridge/Button1", "1");
    //     }
    //     displayMode = MULTI;
    // }
    // else
    // {
    //     displayMode = BIG_TEMP;
    // }
    ArduinoOTA.handle();

    // updateDisplayData();
    ArduinoOTA.handle();

    // webSocket.loop();
    // broadcastWS();
    // processZoneRF24Message(); // process any zone watchdog messages
    // if (ZCs[0].manageRestarts(transmitter) == true)
    // {
    //     myWebhook.trigger("ESP32 Watchdog: Zone 1 power cycled");
    // }
    // broadcastWS();
    // // disbale zone 2 restarts for now
    // ZCs[1].resetZoneDevice();
    // if (ZCs[2].manageRestarts(transmitter) == true)
    // {
    //     myWebhook.trigger("ESP32 Watchdog: Zone 3 power cycled");
    // }
    // broadcastWS();
    // webSocket.loop();

    ArduinoOTA.handle();

    //WiFiLocalWebPageCtrl();
    // checkForPageRequest();
    //webSocket.loop();
    //broadcastWS();
}

/**
 * @brief 
 * 
 */
void IRAM_ATTR resetModule()
{
    ets_printf("ESP32 Rebooted by Internal Watchdog\n");
    //esp_restart_noos();
    esp_restart();
}

/**
 * @brief 
 * 
 */
void resetWatchdog(void)
{
    static unsigned long lastResetWatchdogMillis = millis();
    unsigned long resetWatchdogIntervalMs = 10000;

    if ((millis() - lastResetWatchdogMillis) >= resetWatchdogIntervalMs)
    {
        timerWrite(timer, 0); // reset timer (feed watchdog)
        //myWebSerial.println("Reset Module Watchdog......");
        lastResetWatchdogMillis = millis();
    }
}