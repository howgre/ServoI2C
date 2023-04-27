
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <elapsedMillis.h>
#include <FastLED.h>
#include <FastLED_GFX.h>
#include <LEDMatrix.h>
#include <LEDText.h>

#include <FontMatrise.h>
// Change the next defines to match your matrix type and size
#define LED_PIN            16


#define COLOR_ORDER         GRB
#define CHIPSET             WS2812B

#define MATRIX_WIDTH   96
#define MATRIX_HEIGHT  8  // Changed from + to negative
#define MATRIX_TYPE   VERTICAL_MATRIX

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;
cLEDText ScrollingMsg;

const unsigned char TxtDemo[] = { EFFECT_SCROLL_LEFT "            LEFT SCROLL "
                                  EFFECT_SCROLL_RIGHT "            LLORCS THGIR"
                                  EFFECT_SCROLL_DOWN "            SCROLL DOWN             SCROLL DOWN            " EFFECT_FRAME_RATE "\x04" " SCROLL DOWN            " EFFECT_FRAME_RATE "\x00" " "
                                  EFFECT_SCROLL_UP "             SCROLL UP               SCROLL UP             " EFFECT_FRAME_RATE "\x04" "  SCROLL UP             " EFFECT_FRAME_RATE "\x00" " "
                                  EFFECT_CHAR_UP EFFECT_SCROLL_LEFT "            UP"
                                  EFFECT_CHAR_RIGHT "  RIGHT"
                                  EFFECT_CHAR_DOWN "  DOWN"
                                  EFFECT_CHAR_LEFT "  LEFT"
                                  EFFECT_HSV_CV "\x00\xff\xff\x40\xff\xff" EFFECT_CHAR_UP "           HSV_CV 00-40"
                                  EFFECT_HSV_CH "\x00\xff\xff\x40\xff\xff" "    HSV_CH 00-40"
                                  EFFECT_HSV_AV "\x00\xff\xff\x40\xff\xff" "    HSV_AV 00-40"
                                  EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff" "    HSV_AH 00-FF"
                                  "           " EFFECT_HSV "\x00\xff\xff" "R" EFFECT_HSV "\x20\xff\xff" "A" EFFECT_HSV "\x40\xff\xff" "I" EFFECT_HSV "\x60\xff\xff" "N" EFFECT_HSV "\xe0\xff\xff" "B" EFFECT_HSV "\xc0\xff\xff" "O"
                                  EFFECT_HSV "\xa0\xff\xff" "W" EFFECT_HSV "\x80\xff\xff" "S " EFFECT_DELAY_FRAMES "\x00\x96" EFFECT_RGB "\xff\xff\xff" };

uint8_t angle = 0;

//Create object to represent PCA9685 at default I2C address
Adafruit_PWMServoDriver pwms[] {
  Adafruit_PWMServoDriver(0x40),
  Adafruit_PWMServoDriver(0x41) 
};
const size_t pwms_count = sizeof(pwms)/sizeof(Adafruit_PWMServoDriver);
int arrayVal = 0;



#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// Define maximum and minimum number of "ticks" for the servo motors
// Range from 0 to 4095
// This determines the pulse width

#define SERVOMIN 90 // Minimum value
#define SERVOMAX 500 // Maximum value


//restricts the limits of travel
int maxPanAngle[] = {150, 120, 30, 150, 125, 90, 120};
//int maxPanAngle[] = {180, 120, 30, 150, 175, 90, 120}; This is the original
const size_t maxPanAngle_count = sizeof(maxPanAngle)/sizeof(maxPanAngle[0]);

int minPanAngle[] = {30, 60, 15, 30, 25, 30, 60};
//int minPanAngle[] = {0, 60, 0, 30, 5, 0, 60}; This is the original
const size_t minPanAngle_count = sizeof(minPanAngle)/sizeof(minPanAngle[0]);

// Following code is mine to arrange arrays
int arrayPeriod[] =   {100, 10, 0, 50, 5, 8, 50 } ; // This sets the speed, with 0 being fastest.
int arrayPosition[] = {30, 60,  90,  150, 30, 30, 90};


void servo_fn_pan(int& i, int& delta) {
  
  if(i+delta>maxPanAngle[arrayVal] || i+delta<minPanAngle[arrayVal]) {
    delta=-delta;
  }
  i+=delta;
}


void servo_fn_tilt(int& i, int& delta) {
  
  
  if(i+delta<minPanAngle[arrayVal] || i+delta>maxPanAngle[arrayVal]) {
    delta=-delta;
  }
  i+=delta;
  
}

void servo_fn_tilt2(int& i, int& delta) {
  
  
  if(i+delta<minPanAngle[arrayVal] || i+delta>maxPanAngle[arrayVal]) {
    delta=-delta;
  }
  for (int j = 16; j <= 23; j++)
  if (j > 18) i = i + 10;
  i+=delta;

}

typedef void(*servo_fn)(int& i,int& delta);

servo_fn arrayFn[] = {
  servo_fn_pan,
  servo_fn_tilt,
  servo_fn_tilt2
};

struct servo_entry {
  // the id of the servo, 0-31
  int id;
  
  // the function used to update i/position
  servo_fn fn;
  
  // the period in ms between updating the position
  // This varies the speed of the motor
  int period;
  
  // internal timestamp
  uint32_t ts;
  
  // the position indicator
  int i;
  
  // the delta/step for each position movement can be pos or neg
  // This should usually be either -1 or 1, 0 will produce a stop
  int delta;
};



servo_entry servos[] {
  {0,servo_fn_pan, arrayPeriod[arrayVal],0,90,1},
  {1,servo_fn_pan,arrayPeriod[arrayVal],0,90,1},
  {2,servo_fn_pan,arrayPeriod[arrayVal],0,90,1},
  {3,servo_fn_pan,arrayPeriod[arrayVal],0,90,1},
  {4,servo_fn_pan,arrayPeriod[arrayVal],0,90,1},
  {5,servo_fn_pan, arrayPeriod[arrayVal],0,90,1},
  {6,servo_fn_pan, arrayPeriod[arrayVal],0,90,1},
  {7,servo_fn_pan, arrayPeriod[arrayVal],0,90,1},
  
  {8,servo_fn_pan, arrayPeriod[1],0,135,1},
  {9,servo_fn_pan, arrayPeriod[1],0,135,1},
  {10,servo_fn_pan, arrayPeriod[1],0,135,1},
  {11,servo_fn_pan, arrayPeriod[1],0,135,1},

  {12,servo_fn_pan, arrayPeriod[3],0,135,1},
  {13,servo_fn_pan, arrayPeriod[3],0,135,1},
  {14,servo_fn_pan, arrayPeriod[3],0,135,1},
  {15,servo_fn_pan, arrayPeriod[3],0,135,1},
  
  {16, servo_fn_tilt, arrayPeriod[arrayVal],0,90,1},
  {17,servo_fn_tilt, arrayPeriod[arrayVal],0,90,1},
  {18,servo_fn_pan, arrayPeriod[arrayVal],0,90,1},
  {19,servo_fn_pan, arrayPeriod[arrayVal],0,90,1},
  
  {20,servo_fn_tilt, arrayPeriod[arrayVal],0,90,1},
  {21,servo_fn_tilt, arrayPeriod[arrayVal],0,90,1},
  {22,servo_fn_tilt, arrayPeriod[arrayVal],0,90,1},
  {23,servo_fn_tilt, arrayPeriod[arrayVal],0,90,1}
  
 
};



const size_t servos_count = sizeof(servos)/sizeof(servo_entry);


uint32_t ts = 0;  //time stamp for changing motions
int state = 0;

void testFunction(){
   Serial.println("testFunction");
      // first change
      arrayVal = 0;

      for(int i = 0; i<16; ++i) {
        servos[i].period = arrayPeriod[arrayVal];
      }
      
      for(int i = 16; i<24; ++i) {
        servos[i].period = arrayPeriod[1];
      }
      

      for(int i = 0; i<16 ; ++i) {
        servos[i].delta = 0;
      }
      for(int i = 16; i<servos_count; ++i) {
        servos[i].delta = 0;
      }

      for (int j = 0; j<8 ; j++){
      servos[j].i =  1;
      } 
      
      /*for (int j = 16; j<servos_count ; j++){
      servos[j].i =  30;
      } */

      servos[0].fn = servo_fn_pan;
      for (int j = 16; j<=23 ; j++){
      servos[j].i = 90;
      }
}
void trafficLights(){
   Serial.println("Traffic Lights");
      
      arrayVal = 0;
      FastLED.showColor(CRGB::Red);
        FastLED.show();
      for(int i = 0; i<servos_count; ++i) {
        servos[i].period = arrayPeriod[arrayVal];
      }
      
      for(int i = 0; i<servos_count; ++i) {
        servos[i].delta = 0;
      }

      for (int j = 0; j<8 ; j++){
      //servos[j].i =  1;
      servos[j].i =  0;
      } 
      
      servos[0].fn = servo_fn_pan;
      for (int j = 16; j<=23 ; j++){
      servos[j].i = 90;
      
      }
        
        
        
       
}

void crossEyed(){
   Serial.println("Cross Eyed");
      // first change
      FastLED.showColor(CRGB::Red);
        FastLED.show();
      arrayVal = 0;

      for(int i = 0;i<servos_count;++i) {
        servos[i].period = 5; //arrayPeriod[arrayVal];
      }
      for(int k = 16; k<servos_count; ++k) {
        //servos[k].i = 0;
        servos[k].i = 30;
      }

      for(int i = 16;i<servos_count;++i) { //Stop the tilt motors from moving
        servos[i].delta = 0;
      }
      for(int i=0; i<8; i++){
      servos[i].fn = servo_fn_tilt;
      if ((i % 2) == 0) { servos[i].delta = -1;} // This reverses the direction of the stroke
      else                                        // of alternate motors
        servos[i].delta = 1;
        
      }


      servos[0].fn = servo_fn_pan;
      for (int j = 0; j<8 ; j++){
      servos[j].i = 90; //sets the pan motors to 90 degrees.
      }
      for (int j = 16; j<servos_count ; j++){
      //servos[j].i = 5; // sets the tilt motors to 5 degrees. Original
      servos[j].i = 35; // sets the tilt motors to 5 degrees.
      }
}

void staggered(){
   Serial.println("Void Staggered");
   arrayVal = 2;
      for(int i = 0;i<servos_count;++i) {
        servos[i].period = arrayPeriod[arrayVal];
      }
      servos[0].fn = servo_fn_pan;
      
      
      for(int i=16; i<24; i++){
      servos[i].fn = servo_fn_tilt;
      if ((i % 2) == 0) { servos[i].delta = -1;} // This reverses the direction of the stroke
      else                                        // of alternate motors
        servos[i].delta = 1;
        
      }

      //servos[0].i = 90;
     
      for(int i = 0;i < 8;++i) { // sets pan servos to static
        servos[i].delta = 0;
      }


     for (int j = 0; j<8 ; j++){
      servos[j].i =  90;
      
      } 
     for (int j = 16; j<=23 ; j++){
      //servos[j].i =  10; Original
      servos[j].i =  30;
      
      }
    servos[22].i = 20;
}

void shortBounce(){
  Serial.println("Short Bounce");
      // second change
      
      arrayVal = 2;
            
      for(int i = 0;i<servos_count;++i) {
        servos[i].period = arrayPeriod[arrayVal];
      }
      servos[0].fn = servo_fn_pan;
      
      for(int i=16; i<24; i++){
        servos[i].fn = servo_fn_tilt;
        if ((i % 2) == 0) { servos[i].delta = -1;} // This reverses the direction of the stroke
        else                                        // of alternate motors
        servos[i].delta = 1;
      }
      
      for(int i = 0;i < 8;++i) {
        servos[i].delta = 1;
      }
      
      
      for (int j = 0; j<=15 ; j++){
      servos[j].i = 90;
      }
     for (int j = 23; j>=16 ; j--){
      //servos[j].i = 0; Original
      servos[j].i = 30;
      }
           
}

void centreBounce(){
  Serial.println("Centre Bounce");
      // second change
      
      arrayVal = 6;
            
      for(int i = 0;i<servos_count;++i) {
        servos[i].period = arrayPeriod[arrayVal];
      }
      servos[0].fn = servo_fn_pan;
      
      for(int i=16; i<24; i++){
       // servos[i].fn = servo_fn_tilt;
       // if ((i % 2) == 0) { servos[i].delta = -1;} // This reverses the direction of the stroke
        // else                                        // of alternate motors
        servos[i].delta = 1;
      }
      
      for(int i = 0;i < 8;++i) {
        servos[i].delta = 0;
      }
      
      
      for (int j = 0; j<=15 ; j++){
      servos[j].i = 90;
      }
     for (int j = 23; j>=16 ; j--){
      servos[j].i = 90;
      }
           
}

void longBounce(){
  Serial.println("Long Bounce");
      
      
      arrayVal = 4;
            
      for(int i = 0;i<servos_count;++i) {
        servos[i].period = arrayPeriod[arrayVal];
      }
      servos[0].fn = servo_fn_tilt;
      
      for(int i=16; i<24; i++){
        servos[i].fn = servo_fn_tilt;
        if ((i % 2) == 0) { servos[i].delta = -1;} // This reverses the direction of the stroke
        else                                        // of alternate motors
        servos[i].delta = 1;
        
      }
      
      for(int i = 0;i < 8;++i) {
        servos[i].delta = 0;
      }
      
      
      for (int j = 0; j<=15 ; j++){
      //servos[j].i = 0; //Original
      servos[j].i = 90;
      }
     for (int j = 23; j>=16 ; j--){
      servos[j].i = 90;
      }
           
}
  void testCase2(){
      Serial.println("testCase2");
      // Third change
      arrayVal = 5;
       //Set the start angle for all servos to 30 degrees.
     for(int i = 0;i<servos_count;++i) {
        servos[i].period = arrayPeriod[arrayVal];
      }
      

      for(int i = 0; i < 8; ++i) {
        if ((i % 2) == 0) { servos[i].delta = -1;}
        else
        servos[i].delta = 1;
      }
      
      
      for(int i = 16;i <=23;  ++i) {
        servos[i].delta = 1;
      } 
      
      servos[0].fn = servo_fn_pan;
      for (int j = 0 ; j < servos_count; j++)
      {
        servos[j].i = 90;
        
      }
      if (ScrollingMsg.UpdateText() == -1)
    ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
  else
    FastLED.show();
  delay(10);
         
  }

void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.setBrightness(127);
  FastLED.clear(true);
  
  FastLED.showColor(CRGB::Red);
  delay(100);
  FastLED.showColor(CRGB::Lime);
  delay(100);
  FastLED.showColor(CRGB::Blue);
  delay(100);
  FastLED.showColor(CRGB::White);
  delay(100);
  FastLED.show();
  ScrollingMsg.SetFont(MatriseFontData);
  ScrollingMsg.Init(&leds, leds.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);
  ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
  ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);



  Serial.begin(115200);
  
  // Print to monitor
  Serial.println("PCA9685 Servo Test");

  // Initialise PCA9685s
  for(int i = 0;i<pwms_count;++i) {
    pwms[i].begin();
    pwms[i].setPWMFreq(SERVO_FREQ);
  }
  
}

void loop() {
  if(millis()>ts + 10*1000) {
    ts = millis();
    


    switch(state) {
      
      
      case 0:
      crossEyed();
      //staggered();
      //testFunction();

      ++state;
      break;
      
      
      case 1:
      shortBounce();
      
      ++state;
      break;
      

    case 2:
      testCase2();
      ++state;
      break;

      
      case 3:
      //staggered();
      //testFunction();
      
      trafficLights();  
      ++state;
      break;

      case 4:
      staggered();
      //testFunction();  
      ++state;
      break;


      case 5:
      longBounce();

      ++state;
      break;
      
      case 6:
      centreBounce();
      ++state;
      break;
      
      
      default:
        Serial.println("Reset state");
        state = 0;
        break; 

        
    }
  }
  for(int i = 0; i<servos_count; ++i) {
    servo_entry& e = servos[i];
    if(millis()>=e.ts+e.period) {
      e.ts = millis();
      int duty = map(e.i, 0, 180, SERVOMIN, SERVOMAX);
      pwms[e.id/16].setPWM(e.id%16,0,duty);
      //Serial.printf("id: %d, i: %d\n",e.id,e.i);
      //Serial.println( arrayVal);
      e.fn(e.i, e.delta);
      
    }
  }
  /*
  //Added by Maribel
  if(++state >1)
    {
      state = 0;
    }
    */
  //delay(1000);
}