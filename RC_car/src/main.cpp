#define BLINKER_WIFI
//#define BLINKER_ESP_SMARTCONFIG

#include <Blinker.h>
#include "ESP32_CAM_SERVER.h"

#include "sbus.h"
#include "car_use.h"
#include "blinker_camera.h"

char auth[] = "b68e5c2d9340";
char ssid[] = "biubiubiu";
char pswd[] = "12345678";

//上 128    0
//左 0      128 
//右 255    128
//下 128    255
//中 128    128

#define JOY_1 "JOYKey"

BlinkerJoystick JOY1(JOY_1);

// 新建组件对象
BlinkerButton Button1("btn-mode");

BlinkerNumber Number1("num-mode");
BlinkerNumber Number2("num-rrr");

BlinkerSlider Slider1("ran-rate");


bool setup_camera = false;


BLINKER_CAMERA blinker_camera;
SPEED_CONTROL speed_con;
SBUS sbus;
CAR car;

void joystick1_callback(uint8_t xAxis, uint8_t yAxis)
{

    blinker_camera.joy_x = xAxis;
    blinker_camera.joy_y = yAxis;
    
}
void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

    Blinker.vibrate();
    
    uint32_t BlinkerTime = millis();
    
    Blinker.print("millis", BlinkerTime);
    Blinker.print("IP:",WiFi.localIP().toString());
    Number2.print(blinker_camera.speed_slinder);
}
// 按下按键即会执行该函数
void button1_callback(const String & state)
{
    BLINKER_LOG("get button state: ", state);
    if(blinker_camera.mode)
  {
    blinker_camera.mode = 0;
  }else
  {
    blinker_camera.mode = 1;
  }
    Number1.print(blinker_camera.mode);
    Number2.print(speed_con.left_speed);
}

    
void slider1_callback(int32_t value)
{
  blinker_camera.speed_slinder = value;
  
}


void setup()
{
    car.CAR_init();
    sbus.sbus_init();


    BLINKER_DEBUG.stream(Serial);
    BLINKER_DEBUG.debugAll();
    Blinker.begin(auth, ssid, pswd);

    Blinker.attachData(dataRead);
    Button1.attach(button1_callback);
    Slider1.attach(slider1_callback);
    JOY1.attach(joystick1_callback);
}

void loop()
{
  Blinker.run();
  sbus.sbus_handle();
  try
  {
    if(Blinker.connected() && !setup_camera)
    {
        setupCamera();
        setup_camera = true;
        Blinker.printObject("video", "{\"str\":\"mjpg\",\"url\":\"http://"+ WiFi.localIP().toString() + "\"}");
    }
    if(car.car_work_handle(blinker_camera,sbus.read_channel()))
    {

    }
  }
  catch(const std::exception& e)
  {
    car.car_error_handle();
  }
  car.debug();
  //sbus.sbus_debug();
  delay(20);
}





