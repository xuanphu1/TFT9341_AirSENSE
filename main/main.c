#include <stdio.h>
#include "../component/TFT_DISPLAY/tft.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "../component/lvgl/src/lv_draw/lv_draw_line.h"
#include "../component/lvgl/src/lv_widgets/lv_img.h"
#include "../component/lvgl/src/lv_core/lv_obj.h"
#include "datamanager.h"

// struct tm Time = {
//         .tm_year = 122, // (2022 - 1900)
//         .tm_mon  = 11,  // 0-based
//         .tm_mday = 15,
//         .tm_hour = 19,
//         .tm_min  = 59,
//         .tm_sec  = 55
// };

SemaphoreHandle_t tft_semaphore = NULL;
struct dataSensor_st dataSensorReceiveFromQueue = {0} ;
struct tm Time = {0};

struct dataOfDay dataDay = {0} ;
struct data_hourly_day0 dataDay0 = {0};

LV_IMG_DECLARE(img_cogwheel_argb);// Ngày mưa
LV_IMG_DECLARE(img_cogwheel_rgb); // Ngày nắng
LV_IMG_DECLARE(img_hand); // Ngày nhiều mây
void imageShowWeatherSatusDay0(void)
{

    lv_obj_t * weatherStatus = lv_img_create(lv_scr_act(), NULL);
    if (dataDay.temperature_day0 <= 25 && dataDay.temperature_day0 >= 10 && dataDay.humidity_day0 >= 70) 
    {
        lv_img_set_src(weatherStatus, &img_cogwheel_argb);
    } else if (dataDay.temperature_day0 <= 35 && dataDay.temperature_day0 >= 20 && dataDay.humidity_day0 <= 60)
    {
        lv_img_set_src(weatherStatus, &img_cogwheel_rgb);
    } else if(dataDay.temperature_day0 <= 25 && dataDay.temperature_day0 >= 15 && dataDay.humidity_day0 > 60 && dataDay.humidity_day0 < 70) 
    {
        lv_img_set_src(weatherStatus, &img_hand);
    }
    
    lv_obj_align(weatherStatus, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

}

LV_IMG_DECLARE(img_cogwheel_indexed16); // giọt nước

void imageDroplets(void)
{
    lv_obj_t * giot_nuoc = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(giot_nuoc, &img_cogwheel_indexed16);
    lv_obj_align(giot_nuoc, NULL, LV_ALIGN_IN_TOP_LEFT, 155, 47);

}



static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        printf("Option: %s\n", buf);
    }
}

void select_location(void)
{

    /*Create a normal drop down list*/
    lv_obj_t * ddlist = lv_dropdown_create(lv_scr_act(), NULL);
    lv_dropdown_set_options(ddlist, "Nghe An\n"
            "Ha Noi\n"
            "Ho Chi Minh\n"
            "Melon\n"
            "Grape\n"
            "Raspberry");

    lv_obj_align(ddlist, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_obj_set_event_cb(ddlist, event_handler);
}



void convertWDay(int valuaWDay,char *wday){
    switch (valuaWDay)
    {
        case 0 :
            sprintf(wday,"Mon");
            break;
        case 1 :
            sprintf(wday,"Tue");
            break;
        case 2:
            sprintf(wday,"Wed");
            break;
        case 3 :
            sprintf(wday,"Thu");
            break;
        case 4 : 
            sprintf(wday,"Fri");
            break;
        case 5 :
            sprintf(wday,"Sat");
            break;
        case 6 :
            sprintf(wday,"Sun"); 
            break;
        
    default:
        break;
    }
}

int processingDataTemperatureUsingDrawChart(float x){
    float valuaIntermediate;
    valuaIntermediate =((x-(int)x)*10);
    if (valuaIntermediate == 0) valuaIntermediate = 10;

    return (int)(x+valuaIntermediate);
}



void lv_ex_line_1(void)
{
    /*Create an array for the points of the line*/
    static lv_point_t line_points1[] = {{0, 65}, {359, 65}};
    static lv_point_t line_points2[] = {{0, 85}, {359, 85}};

    /*Create style*/
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 1);
    lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_line_rounded(&style_line, LV_STATE_DEFAULT, true);

    /*Create a line and apply the new style*/
    lv_obj_t * line1;
    line1 = lv_line_create(lv_scr_act(), NULL);
    lv_line_set_points(line1, line_points1, 2);     /*Set the points*/
    lv_obj_add_style(line1, LV_LINE_PART_MAIN, &style_line);     /*Set the points*/
    lv_obj_align(line1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    lv_obj_t * line2;
    line2 = lv_line_create(lv_scr_act(),NULL);
    lv_line_set_points(line2, line_points2, 2);     /*Set the points*/
    lv_obj_add_style(line2, LV_LINE_PART_MAIN, &style_line);     /*Set the points*/
    lv_obj_align(line2, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

}

void textShowOnScreen(void){

    char wDay[5];

    static lv_style_t style_text;
    lv_style_init(&style_text);
    lv_style_set_text_color(&style_text, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    


    // Information time present
    convertWDay(dataDay.day0.tm_wday,wDay);

    lv_obj_t * textShowInformationTemperaturePresent = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(textShowInformationTemperaturePresent, LV_LABEL_LONG_EXPAND);     
    lv_label_set_text_fmt(textShowInformationTemperaturePresent, "%s %d/%d\n%0.2f°C\n%0.2f%%",wDay,
                                                                                    dataDay.day0.tm_mday,
                                                                                    dataDay.day0.tm_mon,
                                                                                    dataDay.temperature_day0,
                                                                                    dataDay.humidity_day0);                                                                            
    lv_obj_add_style(textShowInformationTemperaturePresent, LV_OBJ_PART_MAIN, &style_text);
    lv_obj_align(textShowInformationTemperaturePresent, NULL, LV_ALIGN_IN_TOP_LEFT, 100,10);

    // Information time present in every hour
    lv_obj_t * textShowInformationPresent_hour = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(textShowInformationPresent_hour, LV_LABEL_LONG_SROLL_CIRC);     
    lv_obj_set_width(textShowInformationPresent_hour, 350); 
    lv_label_set_text_fmt(textShowInformationPresent_hour,  "00h: %.2f°C | 01h: %.2f°C | 02h: %.2f°C | 03h: %.2f°C | 04h: %.2f°C | 05h: %.2f°C | 06h: %.2f°C | 07h: %.2f°C | "
                                                            "08h: %.2f°C | 09h: %.2f°C | 10h: %.2f°C | 11h: %.2f°C | 12h: %.2f°C | 13h: %.2f°C | 14h: %.2f°C | 15h: %.2f°C | "
                                                            "16h: %.2f°C | 17h: %.2f°C | 18h: %.2f°C | 19h: %.2f°C | 20h: %.2f°C | 21h: %.2f°C | 22h: %.2f°C | 23h: %.2f°C",
                                                            dataDay0.temperature_00h_day0, dataDay0.temperature_01h_day0, dataDay0.temperature_02h_day0, dataDay0.temperature_03h_day0,
                                                            dataDay0.temperature_04h_day0, dataDay0.temperature_05h_day0, dataDay0.temperature_06h_day0, dataDay0.temperature_07h_day0,
                                                            dataDay0.temperature_08h_day0, dataDay0.temperature_09h_day0, dataDay0.temperature_10h_day0, dataDay0.temperature_11h_day0,             
                                                            dataDay0.temperature_12h_day0, dataDay0.temperature_13h_day0, dataDay0.temperature_14h_day0, dataDay0.temperature_15h_day0,
                                                            dataDay0.temperature_16h_day0, dataDay0.temperature_17h_day0, dataDay0.temperature_18h_day0, dataDay0.temperature_19h_day0,
                                                            dataDay0.temperature_20h_day0, dataDay0.temperature_21h_day0, dataDay0.temperature_22h_day0, dataDay0.temperature_23h_day0                                      
    );

   
    lv_obj_add_style(textShowInformationPresent_hour, LV_OBJ_PART_MAIN, &style_text);
    lv_obj_align(textShowInformationPresent_hour, NULL, LV_ALIGN_IN_TOP_LEFT, 0,65);
}



void chartTemperature(void){
    /*Create a chart*/
    char wday[5];

    
    float array_temperature[] = {20,25,30,35,40};
    lv_obj_t * chart;
    chart = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart, 300, 150);
    lv_chart_set_point_count(chart,4);
    // lv_chart_set_x_tick_texts(chart,array_temperature,5,LV_CHART_AXIS_DRAW_LAST_TICK);
    lv_obj_align(chart, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    /*Add two data series*/
    lv_chart_series_t * series_data = lv_chart_add_series(chart, LV_COLOR_RED);

    /*Set the next points on 'ser1'*/
    lv_chart_set_next(chart, series_data, -10+processingDataTemperatureUsingDrawChart(dataDay.temperature_day1));
    lv_chart_set_next(chart, series_data, -10+processingDataTemperatureUsingDrawChart(dataDay.temperature_day2));
    lv_chart_set_next(chart, series_data, -10+processingDataTemperatureUsingDrawChart(dataDay.temperature_day3));
    lv_chart_set_next(chart, series_data, -10+processingDataTemperatureUsingDrawChart(dataDay.temperature_day4));
   
    lv_obj_t * status_day1 = lv_label_create(lv_scr_act(),NULL);
    lv_label_set_long_mode(status_day1, LV_LABEL_LONG_EXPAND);
    convertWDay(dataDay.day1.tm_wday,&wday);
    lv_label_set_text_fmt(status_day1,"%s\n%d/%d\n%0.1f°C",wday,dataDay.day1.tm_mday,dataDay.day1.tm_mon,dataDay.temperature_day1);
    lv_obj_align(status_day1, NULL, LV_ALIGN_IN_BOTTOM_MID, -125,-40-processingDataTemperatureUsingDrawChart(dataDay.temperature_day1));

    lv_obj_t * status_day2 = lv_label_create(lv_scr_act(),NULL);
    lv_label_set_long_mode(status_day2, LV_LABEL_LONG_EXPAND);
    convertWDay(dataDay.day2.tm_wday,&wday);
    lv_label_set_text_fmt(status_day2,"%s\n%d/%d\n%0.1f°C",wday,dataDay.day2.tm_mday,dataDay.day2.tm_mon,dataDay.temperature_day2);
    lv_obj_align(status_day2, NULL, LV_ALIGN_IN_BOTTOM_MID, -45,-40-processingDataTemperatureUsingDrawChart(dataDay.temperature_day2));

    lv_obj_t * status_day3 = lv_label_create(lv_scr_act(),NULL);
    lv_label_set_long_mode(status_day3, LV_LABEL_LONG_EXPAND);
    convertWDay(dataDay.day3.tm_wday,&wday);
    lv_label_set_text_fmt(status_day3,"%s\n%d/%d\n%0.1f°C",wday,dataDay.day3.tm_mday,dataDay.day3.tm_mon,dataDay.temperature_day3);
    lv_obj_align(status_day3, NULL, LV_ALIGN_IN_BOTTOM_MID, 50,-40-processingDataTemperatureUsingDrawChart(dataDay.temperature_day3));

    lv_obj_t * status_day4 = lv_label_create(lv_scr_act(),NULL);
    lv_label_set_long_mode(status_day4, LV_LABEL_LONG_EXPAND);
    convertWDay(dataDay.day4.tm_wday,&wday);
    lv_label_set_text_fmt(status_day4,"%s\n%d/%d\n%0.1f°C",wday,dataDay.day4.tm_mday,dataDay.day4.tm_mon,dataDay.temperature_day4);
    lv_obj_align(status_day4, NULL, LV_ALIGN_IN_BOTTOM_MID, 135,-40-processingDataTemperatureUsingDrawChart(dataDay.temperature_day4));


    lv_chart_refresh(chart); /*Required after direct set*/
}

void tft_task(void* parameters){
    tft_semaphore = xSemaphoreCreateMutex();
    imageShowWeatherSatusDay0();
    
    // tft_initScreen();   
    lv_ex_line_1();
    imageDroplets();
    textShowOnScreen();
    chartTemperature();
    select_location();
    while (1){
        
        
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_task_handler();
    }
    vTaskDelay(1000);
}
void app_main(void)
{
    dataDay.temperature_day1 = 35.4;
    dataDay.temperature_day2 = 30.4;
    dataDay.temperature_day3 = 33;
    dataDay.temperature_day4 = 36;
    dataDay0.temperature_03h_day0 = 23;
    dataDay.temperature_day0 = 25;
    dataDay.humidity_day0 = 75;
    printf("%d\n",processingDataTemperatureUsingDrawChart(dataDay.temperature_day1));
    printf("%d\n",processingDataTemperatureUsingDrawChart(dataDay.temperature_day2));
    printf("%d\n",processingDataTemperatureUsingDrawChart(dataDay.temperature_day3));
    printf("%d",processingDataTemperatureUsingDrawChart(dataDay.temperature_day4));

    tft_initialize();
    
   
    
    
    xTaskCreate(tft_task,"test", 1024*8,NULL,25,NULL);
    // lv_task_handler();

    
    
    // tft_updateScreen()
    // xTaskCreate(task_test,"test", 1024*8,NULL,25,NULL);
    // ESP_LOGI(__func__,"succefully create task");
    //tft_initialize();
}
