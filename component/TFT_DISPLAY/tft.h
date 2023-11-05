/**
 * @file tftdisplay.h
 * @author Vu Thanh Trung   (20203623)
 * @brief Library for SPI TFT display module (ILI9341)
 * @note Before use this file and the lvgl_esp32_driver,
 * run idf.py menuconfig, go to Component config then
 * LVGL TFT configuration and LVGL TFT Display configuration
 * to configure lvgl_esp32_drivers.
*/

#ifndef __TFT_H__
#define __TFT_H__

#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "../lvgl/lvgl.h"
#include "../../build/config/sdkconfig.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "../lvgl_esp32_drivers/lvgl_helpers.h"
#include "../lvgl_esp32_drivers/lvgl_spi_conf.h"
#include "../lvgl_esp32_drivers/lvgl_tft/disp_spi.h"
#include "../lvgl_esp32_drivers/lvgl_tft/disp_driver.h"
#include "../datamanager/datamanager.h"
#include <time.h>



#define LV_TICK_PERIOD_MS 1

#define ID_TFT 0x03

#define ESP_ERROR_TFT_INIT_FAILED ((ID_TFT << 12)|0x00)

// struct tm date1 ={0};


struct data_hourly_day0
    {
        float temperature_00h_day0;
        float temperature_01h_day0;
        float temperature_02h_day0;
        float temperature_03h_day0;
        float temperature_04h_day0;
        float temperature_05h_day0;
        float temperature_06h_day0;
        float temperature_07h_day0;
        float temperature_08h_day0;
        float temperature_09h_day0;
        float temperature_10h_day0;
        float temperature_11h_day0;
        float temperature_12h_day0;
        float temperature_13h_day0;
        float temperature_14h_day0;
        float temperature_15h_day0;
        float temperature_16h_day0;
        float temperature_17h_day0;
        float temperature_18h_day0;
        float temperature_19h_day0;
        float temperature_20h_day0;
        float temperature_21h_day0;
        float temperature_22h_day0;
        float temperature_23h_day0;

        float humidity_00h_day0;
        float humidity_01h_day0;
        float humidity_02h_day0;
        float humidity_03h_day0;
        float humidity_04h_day0;
        float humidity_05h_day0;
        float humidity_06h_day0;
        float humidity_07h_day0;
        float humidity_08h_day0;
        float humidity_09h_day0;
        float humidity_10h_day0;
        float humidity_11h_day0;
        float humidity_12h_day0;
        float humidity_13h_day0;
        float humidity_14h_day0;
        float humidity_15h_day0;
        float humidity_16h_day0;
        float humidity_17h_day0;
        float humidity_18h_day0;
        float humidity_19h_day0;
        float humidity_20h_day0;
        float humidity_21h_day0;
        float humidity_22h_day0;
        float humidity_23h_day0;
};

struct dataOfDay{

    struct tm day0 ;
    struct tm day1 ;
    struct tm day2 ;
    struct tm day3 ;
    struct tm day4 ;

    float temperature_day0;
    float temperature_day1;
    float temperature_day2;
    float temperature_day3;
    float temperature_day4;
    float temperature_day5;
    float temperature_day6;
    float temperature_day7;

    float humidity_day0;
    float humidity_day1;
    float humidity_day2;
    float humidity_day3;
    float humidity_day4;
    float humidity_day5;
    float humidity_day6;
    float humidity_day7;


};


struct label_st
{
    lv_obj_t *location;
    lv_obj_t *date1;
    lv_obj_t *date2;
    lv_obj_t *date3;
    lv_obj_t *date4;
    lv_obj_t *date5;
    lv_obj_t *date6;
    lv_obj_t *date7;


    lv_obj_t *dateTime;
    lv_obj_t *groupName;

    lv_obj_t *temp_label;
    lv_obj_t *temp_val;

    lv_obj_t *humi_label;
    lv_obj_t *humi_val;

    lv_obj_t *press_label;
    lv_obj_t *press_val;

    lv_obj_t *co2_label;
    lv_obj_t *co2_val;
    
    lv_obj_t *pm10_label;
    lv_obj_t *pm10_val;
    
    lv_obj_t *pm2_5_label;
    lv_obj_t *pm2_5_val;

    lv_obj_t *pm1_label;
    lv_obj_t *pm1_val;

};

struct label_st label_to_display;

/**
 * @brief Initialize SPI and driver for TFT display
 * @return esp_err_t
*/
esp_err_t tft_initialize();

/**
 * @brief Initialize screen display
*/
void tft_initScreen();

/**
 * @brief Update data and time to screen
 * @param data_sensor
 * @return esp_err_t
*/
// esp_err_t tft_updateScreen(struct dataSensor_st *data_sensor, const char*timestring);





#endif