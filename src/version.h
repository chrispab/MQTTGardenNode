#ifndef __VERSION_H
#define __VERSION_H

#define SW_VERSION "V4.28 Br:\"dev\""

#define TITLE_LINE1 "     ESP32"
#define TITLE_LINE2 "MQTT 433MhZ Bridge"
#define TITLE_LINE3 "Zone Wireless Dog"
#define TITLE_LINE4 "MQTT Temp Sensor"
#define TITLE_LINE5 SW_VERSION
#define TITLE_LINE6 "powerCycleLED"
// //#define SYS_FONT u8g2_font_8x13_tf
// #define SYS_FONT u8g2_font_6x12_tf       // 7 px high
// #define BIG_TEMP_FONT u8g2_font_fub30_tf //30px hieght
// // 33 too big - #define BIG_TEMP_FONT u8g2_font_inb33_mf

#define CR Serial.println()

#define CMSM_READ_INTERVAL  1000*60

#endif