/* Christmas Gift Box - opens when game is successfully played
   Unlocks christmas lockbox after winning level 1

   References:
   Based on Volos breakout game
   https://www.youtube.com/watch?v=N6V7ZJkhSbc&list=PLRCQZ78SXVWjVWOfMjREtLX2lKyB0Eh7J&index=5

   board:  ESP32 Dev Module | TTGO T-Display ESP32 with built in TFT Display
   Getting started with TFT_eSPI library on a TTGO T-Display board:  https://www.youtube.com/watch?v=UE1mtlsxfKM&list=PLRCQZ78SXVWjVWOfMjREtLX2lKyB0Eh7J&index=6
   Library Location:  https://github.com/Bodmer/TFT_eSPI or https://www.arduino.cc/reference/en/libraries/tft_espi/
*/

//TTGO TFT Setup:  135x250 pixel TFT
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

// main splashscreen - uncomment only one.  These are 135x240 png files converted to C format using http://rinkydinkelectronics.com/t_imageconverter565.php
#include "alex.h"  //15,60  Servo lock and unlock positions - obtain by experiment
//#include "avery.h" //5, 55
//#include "hana.h"  //25,75
//#include "james.h"  //7, 55
//#include "kev.h"
//#include "lark.h"
//#include "linz.h"  //5,48
//#include "mel.h"  //0, 55
//#include "syd.h"  //16,60

// splashscreen to display when level 1 is achieved and box opens
#include "winner.h"  // winner splashscreen

// Servo
#include <ESP32Servo.h>
Servo myservo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32
int LOCK = 0;    // variable to store the servo position
int UNLOCK = 55;
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
int servoPin = 12;

// TFT Colors
#define TFT_GREY 0x5AEB
#define lightblue 0x2D18
#define orange 0xFB60
#define purple 0xFB9B

// Variable used in the game
float ys = 1;
float x = random(30, 100); //coordinates of ball
float y = 70;
int ny = y; //coordinates of previous position
int nx = x;
float px = 45; //67 is the center position of the player
int   pxn = px;
int vrije[2] = {1, -1};
int enx[16] = {8, 33, 58, 83, 108, 8, 33, 58, 83, 108, 22, 47, 72, 97, 47, 72};
int eny[16] = {37, 37, 37, 37, 37, 45, 45, 45, 45, 45, 53, 53, 53, 53, 61, 61};
int enc[16] = {TFT_RED, TFT_RED, TFT_RED, TFT_RED, TFT_RED, TFT_GREEN, TFT_GREEN, TFT_GREEN, TFT_GREEN, TFT_GREEN, orange, orange, orange, orange, lightblue, lightblue};
int score = 0;
int level = 1;
float amount[4] = {0.25, 0.50, 0.75, 1};
float xs = amount[random(4)] * vrije[random(2)];
int fase = 0;
int pom = 0;
int gameSpeed = 13000;  // higher number for slower ball

// for debugging
#define DEBUG 0

/**********************************************************
  SETUP
***********************************************************/
void setup(void) {
  pinMode(0, INPUT);
  pinMode(35, INPUT);
  tft.init();
  tft.setRotation(0);
  tft.setSwapBytes(true);

  // show splashscreen
  tft.pushImage(0, 0,  135, 240, bootlogo);  // show boot logo
#if DEBUG
  delay(1000);
  tft.pushImage(0, 0,  135, 240, winner);  // test winner logo
  delay(1000);
#endif

  // setup servo
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
  // using default min/max of 1000us and 2000us
  // different servos may require different min/max settings
  // for an accurate 0 to 180 sweep
#if DEBUG
  myservo.write(UNLOCK);  // test unlock
  delay(1000);
#endif
  myservo.write(LOCK); // put servo on lock position

}

/**********************************************************
   LOOP
***********************************************************/
void loop() {

  if (fase == 0)
  {

    if (digitalRead(0) == 0 || digitalRead(35) == 0)
    {
      if (pom == 0) {
        tft.fillScreen(TFT_BLACK);
        tft.drawLine(0, 17, 0, 240, TFT_GREY);
        tft.drawLine(0, 17, 135, 17, TFT_GREY);
        tft.drawLine(134, 17, 134, 240, TFT_GREY);

        tft.setCursor(3, 3, 2);

        tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(1);

        tft.setCursor(0, 0, 2);
        tft.println("SCORE " + String(score));

        tft.setCursor(99, 0, 2);
        tft.println("LVL" + String(level));
        fase = fase + 1;
        pom = 1;
      }

    } else {
      pom = 0;
    }
  }

  if (fase == 1) {

    if (y != ny) {
      tft.fillEllipse(nx, ny, 2, 2, TFT_BLACK);   //Erase the ball
      ny = y;
      nx = x;
    }

    if (int(px) != pxn) {
      tft.fillRect(pxn, 234, 24, 4, TFT_BLACK); //Delete Player
      pxn = px;
    }

    //spe=spe+1; // comment out

    if (px >= 2 && px <= 109) {
      if (digitalRead(0) == 0)
        px = px - 1;
      if (digitalRead(35) == 0)
        px = px + 1;
    }

    if (px <= 3)
      px = 4;

    if (px >= 108)
      px = 107;

    if (y > 232 && x > px && x < px + 24) {
      ys = ys * -1;
      xs = amount[random(4)] * vrije[random(2)];
    }

    for (int j = 0; j < 16; j++)
    {
      if (x > enx[j] && x < enx[j] + 20 && y > eny[j] && y < eny[j] + 5)
      {
        tft.fillRect(enx[j], eny[j], 20, 4, TFT_BLACK);
        enx[j] = 400;
        ys = ys * -1;
        xs = amount[random(4)] * vrije[random(2)];
        score = score + 1;
        tft.setCursor(0, 0, 2);
        tft.println("SCORE " + String(score));
      }
    }

    if (y < 21)
      ys = ys * -1.00;

    if (y > 240)
      fase = fase + 1;

    if (x >= 130)
      xs = xs * -1.00;

    if (x <= 4)
      xs = xs * -1.00;

    // update blocks
    for (int i = 0; i < 16; i++) tft.fillRect(enx[i], eny[i], 20, 4, enc[i]);

    // draw ball
    tft.fillEllipse(int(x), y, 2, 2, TFT_WHITE);

    // change coordinates of ball
    y = y + ys;
    x = x + xs;

    tft.fillRect(px, 234, 24, 4, TFT_WHITE);

    if (score == 16 || score == 33 || score == 50 || score == 67 || score == 84 || score == 101 || score == 118 || score == 135 || score == 152 || score == 169)
      newLevel();  //Score is incremented by one

    delayMicroseconds(gameSpeed);
  }

  if (fase == 2)  // game over
  {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(13, 103, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(1);
    tft.println("GAME OVER");
    tft.setCursor(13, 123, 4);
    tft.println("SCORE:" + String(score));
    tft.setCursor(13, 153, 4);
    tft.println("LEVEL:" + String(level));
    tft.setCursor(13, 123, 4);
    tft.println("SCORE:" + String(score));
    delay(3000);
    esp_restart();
  }

}

void newLevel()
{
  if (score == 16) {
    myservo.write(UNLOCK);
    tft.setSwapBytes(true);
    tft.pushImage(0, 0,  135, 240, winner);  // show win logo
    delay(2000);

    for (int n = 10; n >= 0; n--) {
      tft.fillScreen(TFT_BLACK);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(1);
      tft.setCursor(10, 103, 4);
      tft.println("Open Box!");
      tft.setTextColor(TFT_RED, TFT_BLACK);  tft.setTextSize(2);
      tft.setCursor(55, 140, 4);
      tft.println(String(n));
      delay(1000);
    }
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(1);
  }

  score = score + 1;
  delay(3000);
  gameSpeed = gameSpeed - 500;
  level = level + 1;
  tft.setCursor(99, 0, 2);
  tft.println("LVL" + String(level));
  y = 75;
  ys = 1;
  x = random(30, 100);

  int enx2[16] = {8, 33, 58, 83, 108, 8, 33, 58, 83, 108, 22, 47, 72, 97, 47, 72};
  for (int n = 0; n < 16; n++)
  {
    enx[n] = enx2[n];
  }
}
