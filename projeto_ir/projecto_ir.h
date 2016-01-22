#define SENSOR1 A0
#define SENSOR2 A1
#define LED1 8
#define LED2 9
#define BUTTON_ON 0xF7C03F
#define BUTTON_OFF 0xF740BF
#define BUTTON_BLUE 0xF7609F
#define BUTTON_WHITE 0xF7E01F
#define BUTTON_GREEN 0xF7A05F


enum stateIds {
  BEGIN=0, IN1, IN1IN2, OUT1IN2, OUT2, IN2, IN2IN1, OUT2IN1, OUT1, NAS
};

stateIds getActualState();
