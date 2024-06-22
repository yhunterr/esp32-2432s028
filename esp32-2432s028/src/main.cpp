#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <TFT_Touch.h>

/*Don't forget to set Sketchbook location in File/Preferencesto the path of your UI project (the parent foder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */


#define DOUT 39  /* Data out pin (T_DO) of touch screen */
#define DIN  32  /* Data in pin (T_DIN) of touch screen */
#define DCS  33  /* Chip select pin (T_CS) of touch screen */
#define DCLK 25  /* Clock pin (T_CLK) of touch screen */
TFT_Touch touch = TFT_Touch(DCS, DCLK, DIN, DOUT);


#define SENSOR_CDS_PIN 34
#define LED_RED_PIN    4
#define LED_GREEN_PIN  16
#define LED_BLUE_PIN   17


#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
    uint16_t touchX = 0, touchY = 0;

    bool touched = touch.Pressed();

    if( !touched )
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        touchX = touch.X();
        touchY = touch.Y();
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        //Serial.print( "Data x " );
        //Serial.println( touchX );

        //Serial.print( "Data y " );
        //Serial.println( touchY );
    }
}

void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    tft.setRotation( 3 ); /* Landscape orientation, flipped */
    tft.invertDisplay(true);
    touch.setCal(526, 3443, 750, 3377, 320, 240, 3);
    touch.setRotation(3);

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );


    ui_init();
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_BLUE_PIN, OUTPUT);
    digitalWrite(LED_RED_PIN,HIGH);
    digitalWrite(LED_GREEN_PIN,HIGH);
    digitalWrite(LED_BLUE_PIN,HIGH);
    Serial.println( "Setup done" );
 
}

int val=0;

unsigned long timer1 = millis();
unsigned long stopWatch_time;
char start_status = -1;
char reset_status = -1;
///////////////////// FUNCTIONS ////////////////////
void ui_event_Button1(lv_event_t * e) //START
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        start_status = 1;
    }

}
void ui_event_Button2(lv_event_t * e) //STOP
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        start_status = 0;
    }
}
void ui_event_Button3(lv_event_t * e) //RESET
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        start_status = 0;
        reset_status = 1;
    }
}

char* time_format(long l) {
    int hours = l / 3600;
    int minutes = (l % 3600) / 60;
    int seconds = l % 60;
    static char formatted_time[9];
    sprintf(formatted_time, "%02d:%02d:%02d", hours, minutes, seconds);
    return formatted_time;
}

void loop()
{
    if(millis() > timer1+1000)
    {
      timer1 = millis();
      if(reset_status == 1)
      {
        Serial.println("RESET");
        stopWatch_time = 0;
        lv_label_set_text_fmt(ui_Label6,"00:00:00");
        reset_status = -1;
      }
      if(start_status == 1)
      {
        char* result = time_format(stopWatch_time);
        Serial.println("START");
        stopWatch_time++;
        lv_label_set_text_fmt(ui_Label6,"%s",result);
      }
      else if(start_status == 0)
      {
        Serial.println("STOP");
        start_status = -1;
      }
      Serial.println(stopWatch_time);
    }
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
}
