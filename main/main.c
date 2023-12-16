#include <stdio.h>
#include "../component/TFT_DISPLAY/tft.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "../component/lvgl/src/lv_widgets/lv_img.h"
#include "../component/lvgl/src/lv_core/lv_obj.h"
#include "datamanager.h"
#include "xpt2046.h"

lv_indev_data_t data_test;
lv_indev_drv_t driver_touch;

SemaphoreHandle_t tft_semaphore = NULL;
struct dataSensor_st dataSensorReceiveFromQueue = {0} ;
struct tm Time = {0};

struct dataOfDay dataDay = {0} ;
struct data_hourly_day0 dataDay0 = {0};



LV_IMG_DECLARE(img_cogwheel_indexed16); // nền bầu trời

void imageSky(void)
{
    lv_obj_t * bautroi = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(bautroi, &img_cogwheel_indexed16);
    // lv_obj_align(giot_nuoc, NULL, LV_ALIGN_IN_TOP_LEFT, 155, 47);
    lv_obj_align(bautroi, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

}



// static void event_handler(lv_obj_t * obj, lv_event_t event)
// {
//     if(event == LV_EVENT_PRESSING || event == LV_EVENT_VALUE_CHANGED || event == LV_EVENT_SHORT_CLICKED ||
//        event == LV_EVENT_PRESSED || event == LV_EVENT_PRESS_LOST|| event ==LV_EVENT_LONG_PRESSED || event ==LV_EVENT_LONG_PRESSED_REPEAT || event == LV_EVENT_CLICKED|| event == LV_EVENT_RELEASED|| event == LV_EVENT_DRAG_BEGIN|| event ==LV_EVENT_DRAG_END || event ==LV_EVENT_DRAG_THROW_BEGIN ||
//        event == LV_EVENT_GESTURE|| event == LV_EVENT_KEY || event ==LV_EVENT_FOCUSED || event ==LV_EVENT_DEFOCUSED || event ==LV_EVENT_LEAVE || event == LV_EVENT_VALUE_CHANGED|| event == LV_EVENT_INSERT || event ==LV_EVENT_REFRESH || event == LV_EVENT_APPLY||
//        event == LV_EVENT_CANCEL || event == LV_EVENT_DELETE || event == _LV_EVENT_LAST
//                                                                                     ) {
//         char buf[32];
//         lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
//         printf("Option: %s\n", buf);
//         ESP_LOGI(__func__,"successfull!");
//     }
// }

// void select_location(void)
// {

//     /*Create a normal drop down list*/
//     lv_obj_t * ddlist = lv_dropdown_create(lv_scr_act(), NULL);
//     lv_dropdown_set_options (ddlist,    "Nghe An\n"
//                                         "Ha Noi\n"
//                                         "Ho Chi Minh\n"
//                             );

//     lv_obj_align(ddlist, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
//     // lv_dropdown_open(ddlist);
//     lv_obj_set_event_cb(ddlist, event_handler);
// }




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

int processingDataUsingDrawChart(float x){
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
    lv_style_set_text_color(&style_text, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    // Information time present
    convertWDay(dataDay.day0.tm_wday,wDay);
    lv_obj_t * textShowInformationTemperaturePresent = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(textShowInformationTemperaturePresent, LV_LABEL_LONG_EXPAND);     
    lv_label_set_text_fmt(textShowInformationTemperaturePresent, "Time: %s %d/%d\nTemperature: %0.2f°C\nHumidity: %0.2f%%rH", wDay,
                                                                                                                            dataDay.day0.tm_mday,
                                                                                                                            dataDay.day0.tm_mon,
                                                                                                                            dataDay.temperature_day0,
                                                                                                                            dataDay.humidity_day0);                                                                            
    lv_obj_add_style(textShowInformationTemperaturePresent, LV_OBJ_PART_MAIN, &style_text);
    lv_obj_align(textShowInformationTemperaturePresent, NULL, LV_ALIGN_IN_TOP_LEFT, 0,10);

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

    lv_obj_t *textLocation = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(textLocation, LV_LABEL_LONG_EXPAND);
    lv_label_set_text(textLocation,LV_SYMBOL_HOME"Ha Noi");
    lv_obj_add_style(textLocation, LV_OBJ_PART_MAIN, &style_text);
    lv_obj_align(textLocation, NULL, LV_ALIGN_IN_TOP_RIGHT, -10,0);
    
}



void chartTemperature(void){
    /*Create a chart*/
    char wday[5];
    static lv_style_t style_text;
    lv_style_init(&style_text);
    lv_style_set_text_color(&style_text, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&style_text,LV_STATE_DEFAULT, &lv_font_unscii_8);

    static lv_style_t style_textNoteTemperature;
    lv_style_init(&style_textNoteTemperature);
    lv_style_set_text_color(&style_textNoteTemperature, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_style_set_text_font(&style_textNoteTemperature,LV_STATE_DEFAULT, &lv_font_unscii_8);

    static lv_style_t style_textNoteHumidity;
    lv_style_init(&style_textNoteHumidity);
    lv_style_set_text_color(&style_textNoteHumidity, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_style_set_text_font(&style_textNoteHumidity,LV_STATE_DEFAULT, &lv_font_unscii_8);


    float array_temperature[] = {20,25,30,35,40};
    lv_obj_t * chart;
    chart = lv_chart_create(lv_scr_act(), NULL);
    lv_obj_set_size(chart, 320, 75);
    lv_chart_set_point_count(chart,4);
    lv_obj_align(chart, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    
    /*Add two data series*/
    lv_chart_series_t * series_data = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_series_t * series2_data = lv_chart_add_series(chart, LV_COLOR_BLUE);

    lv_chart_series_t * pointNoteTemp = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_series_t * pointNoteHumid = lv_chart_add_series(chart, LV_COLOR_BLUE);
    
    /*Set the next points on char*/
    lv_chart_set_next(chart, series_data, -25+processingDataUsingDrawChart(dataDay.temperature_day1));
    lv_chart_set_next(chart, series_data, -25+processingDataUsingDrawChart(dataDay.temperature_day2));
    lv_chart_set_next(chart, series_data, -25+processingDataUsingDrawChart(dataDay.temperature_day3));
    lv_chart_set_next(chart, series_data, -25+processingDataUsingDrawChart(dataDay.temperature_day4));

    lv_chart_set_next(chart, series2_data, -25+processingDataUsingDrawChart(dataDay.humidity_day1));
    lv_chart_set_next(chart, series2_data, -25+processingDataUsingDrawChart(dataDay.humidity_day2));
    lv_chart_set_next(chart, series2_data, -25+processingDataUsingDrawChart(dataDay.humidity_day3));
    lv_chart_set_next(chart, series2_data, -25+processingDataUsingDrawChart(dataDay.humidity_day4));

    // Set point note 
    
    // lv_chart_set_next(chart, pointNoteTemp, 100);
    pointNoteHumid->points[0] = 100 ;
    
    pointNoteTemp->points[0] = 87 ;
    // pointNoteTemp->points[1] = 100 ;

    lv_obj_t * noteTemperature = lv_label_create(lv_scr_act(),NULL);
    lv_label_set_long_mode(noteTemperature, LV_LABEL_LONG_EXPAND);
    lv_obj_add_style(noteTemperature, LV_OBJ_PART_MAIN, &style_textNoteTemperature);
    lv_label_set_text(noteTemperature,"Temperature");
    lv_obj_align(noteTemperature, NULL, LV_ALIGN_IN_BOTTOM_MID, -103,-53);

    lv_obj_t * noteHumidity = lv_label_create(lv_scr_act(),NULL);
    lv_label_set_long_mode(noteHumidity, LV_LABEL_LONG_EXPAND);
    lv_obj_add_style(noteHumidity, LV_OBJ_PART_MAIN, &style_textNoteHumidity);
    lv_label_set_text(noteHumidity,"Humidity");
    lv_obj_align(noteHumidity, NULL, LV_ALIGN_IN_BOTTOM_MID, -115,-62);


    lv_obj_t * status_day1 = lv_label_create(lv_scr_act(),NULL);
    lv_label_set_long_mode(status_day1, LV_LABEL_LONG_EXPAND);
    convertWDay(dataDay.day0.tm_wday+1,&wday); // the next one day 
    lv_obj_add_style(status_day1, LV_OBJ_PART_MAIN, &style_text);
    lv_label_set_text_fmt(status_day1,"%s\n%d/%d\n%0.1f°C\n%0.1f%%rH",wday,dataDay.day1.tm_mday,dataDay.day1.tm_mon,dataDay.temperature_day1,dataDay.humidity_day1);
    lv_obj_align(status_day1, NULL, LV_ALIGN_IN_BOTTOM_MID, -130,-80);

    lv_obj_t * status_day2 = lv_label_create(lv_scr_act(),NULL);
    lv_label_set_long_mode(status_day2, LV_LABEL_LONG_EXPAND);
    convertWDay(dataDay.day0.tm_wday+2,&wday); // the next two day 
    lv_obj_add_style(status_day2, LV_OBJ_PART_MAIN, &style_text);
    lv_label_set_text_fmt(status_day2,"%s\n%d/%d\n%0.1f°C\n%0.1f%%rH",wday,dataDay.day2.tm_mday,dataDay.day2.tm_mon,dataDay.temperature_day2,dataDay.humidity_day2);
    lv_obj_align(status_day2, NULL, LV_ALIGN_IN_BOTTOM_MID, -45,-80);

    lv_obj_t * status_day3 = lv_label_create(lv_scr_act(),NULL);
    lv_label_set_long_mode(status_day3, LV_LABEL_LONG_EXPAND);
    convertWDay(dataDay.day0.tm_wday+3,&wday); // the next three day
    lv_obj_add_style(status_day3, LV_OBJ_PART_MAIN, &style_text);
    lv_label_set_text_fmt(status_day3,"%s\n%d/%d\n%0.1f°C\n%0.1f%%rH",wday,dataDay.day3.tm_mday,dataDay.day3.tm_mon,dataDay.temperature_day3,dataDay.humidity_day3);
    lv_obj_align(status_day3, NULL, LV_ALIGN_IN_BOTTOM_MID, 50,-80);

    lv_obj_t * status_day4 = lv_label_create(lv_scr_act(),NULL);
    lv_label_set_long_mode(status_day4, LV_LABEL_LONG_EXPAND);
    convertWDay(dataDay.day0.tm_wday+4,&wday); // the next four day
    lv_obj_add_style(status_day4, LV_OBJ_PART_MAIN, &style_text);
    lv_label_set_text_fmt(status_day4,"%s\n%d/%d\n%0.1f°C\n%0.1f%%rH",wday,dataDay.day4.tm_mday,dataDay.day4.tm_mon,dataDay.temperature_day4,dataDay.humidity_day4);
    lv_obj_align(status_day4, NULL, LV_ALIGN_IN_BOTTOM_MID, 140,-80);


    lv_chart_refresh(chart); /*Required after direct set*/
}

void tft_task(void* parameters){
    tft_semaphore = xSemaphoreCreateMutex();
    // lv_ex_line_1();
    imageSky();
    textShowOnScreen();
    chartTemperature();
    while (1){
        
        xpt2046_read(&driver_touch,&data_test);
        
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

    dataDay.humidity_day1 = 80.2 ;
    dataDay.humidity_day2 = 70.6 ;
    dataDay.humidity_day3 = 75.2 ;
    dataDay.humidity_day4 = 80.4 ;

    dataDay0.temperature_03h_day0 = 23;
    dataDay.temperature_day0 = 25;
    dataDay.humidity_day0 = 75;
    printf("%d\n",processingDataUsingDrawChart(dataDay.temperature_day1));
    printf("%d\n",processingDataUsingDrawChart(dataDay.temperature_day2));
    printf("%d\n",processingDataUsingDrawChart(dataDay.temperature_day3));
    printf("%d",processingDataUsingDrawChart(dataDay.temperature_day4));

    tft_initialize();
    xTaskCreate(tft_task,"test", 1024*8,NULL,25,NULL);
   
}
