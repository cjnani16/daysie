#include <SoftwareSerial.h>
#include <Wire.h>

////adafruit
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_LEDBackpack.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
////

//////OLED Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 
int imageWidth = 128;
int imageHeight = 32;
#include "daysielogo.h"

SoftwareSerial BTSerial(11, 10); //RX|TX

int x = 0;
int minX = 0;
int month = 1;
String msg = "";
String goal = "";
int boot[][7] = { {0, 0, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 0, 0, 0},
  {2, 0, 0, 0, 0, 0, 2},
  {0, 3, 3, 3, 3, 3, 0}
};

///
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
///

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //9.6k baud rate
  BTSerial.begin(9600);
  
  while (!Serial);
  Serial.println("AT commands: okay ");

  /// boot matrix, draw smiley
  matrix.begin(0x70);  // pass in the address
  matrix.clear();
  matrix.setRotation(1);
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 7; j++) {
      switch(boot[i][j]) {
        case 0: break;
        case 1: matrix.drawPixel(j,i, LED_GREEN); break;
        case 2: matrix.drawPixel(j,i, LED_RED); break;
        case 3: matrix.drawPixel(j,i, LED_YELLOW); break;
        default: Serial.println("unknown led val"); break;
      }
    }
  }
  matrix.writeDisplay();
  ///

  ///
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  goal = "Ich liebe dich, Miv <3";
  display.setRotation(2);
  display.clearDisplay();
  display.drawBitmap(0,0,bitmap, imageWidth, imageHeight, 1);
  display.display();
  delay(4269);
  display.clearDisplay();
  display.setTextSize(2); // Draw X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  display.setCursor(display.width()+x, 10);
  display.println(goal);
  display.display();      // Show goal text
  minX = -12 * goal.length();
  ///
}

void loop() {
  // put your main code here, to run repeatedly:
  bool refresh = false;
  
  if (BTSerial.available()) {

    if (BTSerial.read() == 'T') {
      msg = BTSerial.readString();
      refresh=true;
      x=0;

      int cutoff = msg.indexOf('G');
      goal = msg.substring(cutoff+1);
      msg = msg.substring(0,cutoff-1);
      minX = -12 * goal.length();

      Serial.print("Recv'd track: ");
      Serial.println(msg);
      Serial.print("Recv'd goal: ");
      Serial.println(goal);
    }
  }

  //write goal text
  if (goal=="") {
    display.setRotation(2);
    display.clearDisplay();
    display.drawBitmap(0,0,bitmap, imageWidth, imageHeight, 1);
    display.display();
  } else {
    display.clearDisplay();
    display.setTextSize(2); // Draw X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);
    display.setCursor(display.width()+x, 10);
    display.println(goal);
    display.display();      // Show goal text
    if(--x < minX-display.width()) x = 0;
  }

  //if refresh scheduled
  if (refresh==true) {
    matrix.clear();
    matrix.setRotation(1);
    
    for (int i=0; i<28; i++) {
      if (i >= msg.length()) {
        break;
      }

      switch (msg.charAt(i)) {
        case '0': break;
        case '1': matrix.drawPixel(i%7, int(floor(i/7)), LED_GREEN); break;
        case '2': matrix.drawPixel(i%7, int(floor(i/7)), LED_RED); break;
        case '3': matrix.drawPixel(i%7, int(floor(i/7)), LED_YELLOW); break;
        default: Serial.println("unknown led val"); break;
      }
      
    }

    matrix.writeDisplay();
  }

  //if (Serial.available())
  //  BTSerial.write(Serial.read());
}
