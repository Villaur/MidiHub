#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define nameLength 11 //10 + end of the line symbol;
#define maxProfile 32
#define maxLayer 8
#define maxPad 8
#define maxPadNote 43 //35+maxPad;

#define rewriteMemory 0 //set it to 1 if you want to force the program to rewrite all profiles in EEPROM to default on startup;
#define softwareVersion 0.4 //increase the integer part each time the Profile structure changes;

#define MYDELAY 3000

Adafruit_SH1106 display(-1);

// 'PENUP_20240215_212607', 88x64px
const unsigned char logo_bitmap [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x3f, 0xe0, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 
	0x3f, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 
	0x3f, 0xc0, 0x00, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x03, 0x80, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 
	0x03, 0xfe, 0x00, 0x07, 0xc0, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x00, 0x1f, 0xc0, 
	0x3f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x3f, 0xc0, 0xff, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x3f, 0xf0, 0x00, 0xff, 0x83, 0xfe, 0x00, 0x06, 0x00, 0x00, 0x00, 0x7f, 0xe0, 0x01, 0xff, 
	0x1f, 0xf0, 0x00, 0x0f, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x03, 0xff, 0x7f, 0xc0, 0x00, 0x1f, 0x00, 
	0x00, 0x03, 0xf7, 0xc0, 0x07, 0xff, 0xff, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x07, 0xef, 0x80, 0x0f, 
	0xff, 0xfc, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x0f, 0xcf, 0x00, 0x1f, 0xff, 0xf0, 0x00, 0x00, 0xfc, 
	0x00, 0x00, 0x3f, 0x1e, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x7e, 0x3c, 0x00, 
	0x7f, 0xff, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x01, 0xfc, 0x78, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x1f, 
	0xe0, 0x00, 0x07, 0xf0, 0xf8, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x3f, 0xe1, 0xf0, 
	0x00, 0x7f, 0x80, 0x00, 0x00, 0xff, 0x00, 0x00, 0x7f, 0x83, 0xe0, 0x00, 0x3e, 0x00, 0x00, 0x01, 
	0xfe, 0x00, 0x00, 0xfe, 0x03, 0xc0, 0x00, 0x7c, 0x00, 0x00, 0x07, 0xfc, 0x00, 0x00, 0x78, 0x07, 
	0x80, 0x00, 0xf8, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xf0, 0x00, 0x00, 
	0x3f, 0xf0, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x07, 0xe0, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x1e, 0x00, 0x0f, 0xc0, 0x00, 0x03, 0xff, 0x80, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x1f, 0x80, 0x00, 
	0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x3e, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 
	0x00, 0x78, 0x00, 0x7c, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x01, 0xf8, 0x00, 
	0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x03, 0xf0, 0x00, 0x00, 0xe7, 0xe0, 0x00, 0x00, 
	0x00, 0x01, 0xe0, 0x0f, 0xe0, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x1f, 0x80, 
	0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0x81, 0xfe, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x03, 0x83, 0xf8, 
	0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x8f, 0xf0, 0x00, 0x00, 0x1f, 0xc0, 0x00, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 
	0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x03, 0xf8, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x07, 0xf0, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 
	0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 
	0x00, 0x00, 0x00, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xfe, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf8, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfe, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

struct Layer
{
  uint8_t isActive = 0;
  uint16_t chIn = 0b1111111111111111; //i-th bit indicates the i-th channel;
  uint16_t chOut = 16; //0-15 to assign a channel, 16 to keep the original hannel;
  uint8_t noteLow  = 0;
  uint8_t noteHigh = 127;
  uint8_t midiIn = 0b00001111; //i-th bit indicates the i-th input;
  uint8_t midiOut = 0b00001111; //i-th bit indicates the i-th output;
};

struct PadLayer
{
  uint8_t isActive = 0;
  uint16_t chIn = 0b0000001000000000; //i-th bit indicates the i-th channel;
  uint16_t chOut = 16; //0-15 to assign a channel, 16 to keep the original hannel;
  uint8_t midiIn = 0b00001111; //i-th bit indicates the i-th input;
  uint8_t midiOut = 0b00001111; //i-th bit indicates the i-th output;
  uint8_t specificNotes[maxPad][2]; //i-th (starting from 0) pad should send note 36+i on channel 10. [i][0] is a base not (0=off). [i][1] is the chord:
                                    //Both 4 bits of [i][0] are the interval in semitones (1-15) from the base. 0 is off;
};

struct Profile
{
  char nameline1[nameLength] = "Empty";
  char nameline2[nameLength] = "Profile";
  Layer layers[maxLayer];
  PadLayer padLayer;
};

class Button
{
  private: 
    uint8_t pin;
    uint8_t prevstate;
  public:
    Button(uint8_t pin)
    {
      this->pin = pin;
      pinMode(pin, INPUT);
      prevstate = digitalRead(this->pin);
    }

    uint8_t Pressed()
    {
      if (digitalRead(pin) == LOW)
      {
        if (prevstate == HIGH)
          prevstate = LOW;
      }
      else // current state == HIGH;
      {
        if (prevstate == LOW)
        {
          prevstate = HIGH;
          return 1;
        }
      }
      return 0;
    } 
};

uint8_t currentProfileNumber;
Profile currentProfile;

uint8_t screenNum;
uint8_t tabNum;
uint8_t lineNum;
uint8_t lineMax;
uint8_t tabMax;

uint8_t profileChanged;

Button buttonUp = Button(31);
Button buttonDown = Button(25);
Button buttonLeft = Button(33);
Button buttonRight = Button(27);
Button buttonCenter = Button(29);
Button buttonBack = Button(23);

/* Buttons:
  U
L C R
  D    B
*/


void loadProfile(uint8_t profileNum)
{
  currentProfileNumber = profileNum;
  currentProfile = EEPROM.get(2 + (currentProfileNumber - 1) * sizeof(Profile), currentProfile);
  Serial.println("Profile Loaded: "); Serial.println(profileNum);
  profileChanged = 0;
}

void saveProfile(uint8_t profileNum)
{
  EEPROM.put(2 + (profileNum-1) * sizeof(Profile), currentProfile);
  Serial.print("Profile Saved: "); Serial.println(profileNum);
  profileChanged = 0;
}

void setDefaultProfile(uint8_t profileNum)
{
  EEPROM.update(1, profileNum);
}

void drawMainScreen()
{
  display.setTextSize(1);
  display.setCursor(3,1);

  if (profileChanged)
  {
    display.setTextColor(BLACK, WHITE);
    display.print(currentProfileNumber); display.println("*");
  }
  else
  {
    display.setTextColor(WHITE);
    display.println(currentProfileNumber);
  }

  display.setTextColor(WHITE);

  display.setTextSize(2);
  display.setCursor(3 + (10 - strlen(currentProfile.nameline1)) * 6, 16);
  display.print(currentProfile.nameline1);

  display.setCursor(3 + (10 - strlen(currentProfile.nameline2)) * 6, 42);
  display.print(currentProfile.nameline2);
}

void drawFrame()
{
  display.drawLine(2, 27, 121, 27, WHITE);
  display.drawPixel(1, 28, 1);
  display.drawPixel(122, 28, 1);
  display.drawLine(0, 29, 0, 44, WHITE);
  display.drawLine(123, 29, 123, 44, WHITE);
  display.drawPixel(1, 45, 1);
  display.drawPixel(122, 45, 1);
  //display.drawLine(1, 44, 123, 44, WHITE);
  display.drawLine(2, 46, 122, 46, WHITE);
}

void drawHeader()
{
  display.setTextSize(1);
  display.setCursor(1,1);

  if (profileChanged)
  {
    display.setTextColor(BLACK, WHITE);
    if(currentProfileNumber < 10) {display.print(" ");}
    display.print(currentProfileNumber); display.print("*");
  }
  else
  {
    display.setTextColor(WHITE);
    if(currentProfileNumber < 10) {display.print(" ");}
    display.print(currentProfileNumber); display.print(" ");
  }

  display.setTextColor(WHITE);
  display.print(currentProfile.nameline1);

  display.print(" ");

  char trimmedname[11];
  uint8_t pointer = 0;
  for(pointer = 0; pointer < 17 - strlen(currentProfile.nameline1) && pointer < strlen(currentProfile.nameline2); pointer++)
    trimmedname[pointer] = currentProfile.nameline2[pointer];
  if (pointer < 11)
    trimmedname[pointer] = '\0';
  else
    trimmedname[10] = '\0';
  
  display.print(trimmedname);
}

void drawScrollbar()
{
  for (uint8_t y = 11; y < 64; y+=2)
  {
    display.drawPixel(126, y, 1);
  }

  uint8_t len = 53 / lineMax + (lineNum < 53 % lineMax ? 1 : 0);
  uint8_t pos = 11 + lineNum * (53 / lineMax) + (lineNum < 53 % lineMax ? lineNum : 53 % lineMax);

  display.drawRect(125, pos, 3, len, 1);
}

void drawTabs(const char * t0, const char * t1, const char * t2, const char * t3)
{
  display.setTextSize(1);
  //tab 0;
  if (tabNum != 0)
  {
    display.drawLine(2, 0, 31, 0, 1);
    display.drawLine(1, 0, 1, 8, 1);
  
    display.setCursor(2,1);
    display.setTextColor(BLACK, WHITE);
  }
  else
  {
    display.setCursor(2,1);
    display.setTextColor(WHITE);
  }
  for (uint8_t i = 0; i < (5 - strlen(t0)) / 2; i++)
    display.print(" ");

  display.print(t0);

  for (uint8_t i = 0; i < (6 - strlen(t0)) / 2; i++)
    display.print(" ");

  //tab 1;
  if (tabNum != 1)
  {
    display.drawLine(34, 0, 63, 0, 1);
    display.drawLine(33, 0, 33, 8, 1);
  
    display.setCursor(34,1);
    display.setTextColor(BLACK, WHITE);
  }
  else
  {
    display.setCursor(34,1);
    display.setTextColor(WHITE);
  }
  for (uint8_t i = 0; i < (5 - strlen(t1)) / 2; i++)
    display.print(" ");

  display.print(t1);

  for (uint8_t i = 0; i < (6 - strlen(t1)) / 2; i++)
    display.print(" ");

  //tab 2;
  if (tabNum != 2)
  {
    display.drawLine(66, 0, 95, 0, 1);
    display.drawLine(65, 0, 65, 8, 1);
  
    display.setCursor(66,1);
    display.setTextColor(BLACK, WHITE);
  }
  else
  {
    display.setCursor(66,1);
    display.setTextColor(WHITE);
  }
  for (uint8_t i = 0; i < (5 - strlen(t2)) / 2; i++)
    display.print(" ");

  display.print(t2);

  for (uint8_t i = 0; i < (6 - strlen(t2)) / 2; i++)
    display.print(" ");

  //tab 3;
  if (tabNum != 3)
  {
    display.drawLine(98, 0, 127, 0, 1);
    display.drawLine(97, 0, 97, 8, 1);
  
    display.setCursor(98,1);
    display.setTextColor(BLACK, WHITE);
  }
  else
  {
    display.setCursor(98,1);
    display.setTextColor(WHITE);
  }
  for (uint8_t i = 0; i < (5 - strlen(t3)) / 2; i++)
    display.print(" ");

  display.print(t3);

  for (uint8_t i = 0; i < (6 - strlen(t3)) / 2; i++)
    display.print(" ");
}

void handleButton(char btn)
{
  //home screen;
  if (screenNum == 0)
  {
    if (btn == 'U' || btn == 'R')
    {
      if (currentProfileNumber + 1 > maxProfile)
        loadProfile(currentProfileNumber + 1 - maxProfile);
      else
        loadProfile(currentProfileNumber + 1);
    }
    else if (btn == 'D' || btn == 'L')
    {
      if (currentProfileNumber <= 1)
        loadProfile(currentProfileNumber - 1 + maxProfile);
      else
        loadProfile(currentProfileNumber - 1);
    }
    else if (btn == 'C')
    {
      screenNum = 1;
      tabNum = 0;
      lineNum = 0;
      lineMax = 5;
      tabMax = 1;
    }
  } //Menu;
  else if (screenNum == 1)
  {
    if (btn == 'U')
    {
      if (lineNum <= 0)
        lineNum = lineMax - 1;
      else
        lineNum--;
    }
    else if (btn == 'D')
    {
      if (lineNum >= lineMax - 1)
        lineNum = 0;
      else
        lineNum++;
    }
    else if (btn == 'B')
    {
      screenNum = 0;
      tabNum = 0;
      lineNum = 0;
      lineMax = 1;
      tabMax = 1;
    }
    else if (btn == 'C')
    {
      if (lineNum == 0) //Select
      {
        screenNum = 2;
        tabNum = 0;
        lineNum = 0;
        lineMax = 8;
        tabMax = 4;
      }
      else if (lineNum == 1) //Edit
      {
        screenNum = 4;
        tabNum = 0;
        lineNum = 0;
        lineMax = 16;
        tabMax = 4;
      }
      else if (lineNum == 2) //Save
      {
        if (profileChanged)
          saveProfile(currentProfileNumber);

        screenNum = 0;
        tabNum = 0;
        lineNum = 0;
        lineMax = 1;
        tabMax = 1;
      }
      else if (lineNum == 3) //Save as
      {
        screenNum = 3;
        tabNum = 0;
        lineNum = 0;
        lineMax = 8;
        tabMax = 4;
      }
      else if (lineNum == 4) //Options
      {
        screenNum = 5;
        tabNum = 0;
        lineNum = 0;
        lineMax = 2;
        tabMax = 1;
      }
    }
  } //Select;
  else if (screenNum == 2)
  {
    if (btn == 'U')
    {
      if (lineNum <= 0)
        lineNum = lineMax - 1;
      else
        lineNum--;
    }
    else if (btn == 'D')
    {
      if (lineNum >= lineMax - 1)
        lineNum = 0;
      else
        lineNum++;
    }
    else if (btn == 'L')
    {
      if (tabNum > 0)
        tabNum--;
    }
    else if (btn == 'R')
    {
      if (tabNum < tabMax - 1)
        tabNum++;
    }
    else if (btn == 'B')
    {
      screenNum = 1;
      tabNum = 0;
      lineNum = 0;
      lineMax = 5;
      tabMax = 1;
    }
    else if (btn == 'C')
    {
      loadProfile(lineMax * tabNum + lineNum + 1);

      screenNum = 0;
      tabNum = 0;
      lineNum = 0;
      lineMax = 1;
      tabMax = 1;
    }
  }  //Save as;
  else if (screenNum == 3)
  {
    if (btn == 'U')
    {
      if (lineNum <= 0)
        lineNum = lineMax - 1;
      else
        lineNum--;
    }
    else if (btn == 'D')
    {
      if (lineNum >= lineMax - 1)
        lineNum = 0;
      else
        lineNum++;
    }
    else if (btn == 'L')
    {
      if (tabNum > 0)
        tabNum--;
    }
    else if (btn == 'R')
    {
      if (tabNum < tabMax - 1)
        tabNum++;
    }
    else if (btn == 'B')
    {
      screenNum = 1;
      tabNum = 0;
      lineNum = 0;
      lineMax = 5;
      tabMax = 1;
    }
    else if (btn == 'C')
    {
      screenNum = 31;
      tabNum = 1;
      lineNum = lineMax * tabNum + lineNum + 1;
      lineMax = 1;
      tabMax = 1;
    }
  } //Save as Confirmation;
  else if (screenNum == 31)
  {
    if (btn == 'L')
    {
      tabNum = 0;
    }
    else if (btn == 'R')
    {
      tabNum = 1;
    }
    else if (btn == 'B' || (btn == 'C' && tabNum == 1))
    {
      screenNum = 3;
      tabNum = 0;
      lineNum = 0;
      lineMax = 8;
      tabMax = 4;
    }
    else if (btn == 'C' && tabNum == 0)
    {
      saveProfile(lineNum);
      loadProfile(lineNum);

      screenNum = 0;
      tabNum = 0;
      lineNum = 0;
      lineMax = 1;
      tabMax = 1;
    }
  }
  else if (screenNum == 5)//Options
  {
    if (btn == 'U')
    {
      if (lineNum <= 0)
        lineNum = lineMax;
      else
        lineNum--;
    }
    else if (btn == 'D')
    {
      if (lineNum >= lineMax - 1)
        lineNum = 0;
      else
        lineNum++;
    }
    else if (btn == 'L')
    {
      if (tabNum > 0)
        tabNum--;
    }
    else if (btn == 'R')
    {
      if (tabNum < tabMax - 1)
        tabNum++;
    }
    else if (btn == 'B')
    {
      screenNum = 1;
      tabNum = 0;
      lineNum = 0;
      lineMax = 5;
      tabMax = 1;
    }
    else if (btn == 'C' && lineNum == 1) // Panic;
    {
      const HardwareSerial * serials[4] = {&Serial, &Serial1, &Serial2, &Serial3};
      for (uint8_t ser = 0; ser < 4; ser++)
      {
        for(uint8_t ch = 0; ch < 16; ch++)
        {
          while(serials[ser]->available())
            serials[ser]->read();

          serials[ser]->write(0xB0 + ch);
          serials[ser]->write(123);
          serials[ser]->write(0);
          serials[ser]->write(0xB0 + ch);
          serials[ser]->write(120);
          serials[ser]->write(0);
        }
      }
    }
    else if (btn == 'C' && lineNum == 0) // Starting preset;
    {
      screenNum = 51;
      tabNum = (EEPROM.read(1) - 1) / 8;
      lineNum = (EEPROM.read(1) - 1) % 8;
      lineMax = 8;
      tabMax = 4;
    }
  } //Starting preset;
  else if (screenNum == 51)
  {
    if (btn == 'U')
    {
      if (lineNum <= 0)
        lineNum = lineMax - 1;
      else
        lineNum--;
    }
    else if (btn == 'D')
    {
      if (lineNum >= lineMax - 1)
        lineNum = 0;
      else
        lineNum++;
    }
    else if (btn == 'L')
    {
      if (tabNum > 0)
        tabNum--;
    }
    else if (btn == 'R')
    {
      if (tabNum < tabMax - 1)
        tabNum++;
    }
    else if (btn == 'B')
    {
      screenNum = 5;
      tabNum = 0;
      lineNum = 0;
      lineMax = 2;
      tabMax = 1;
    }
    else if (btn == 'C')
    {
      setDefaultProfile(lineMax * tabNum + lineNum + 1);
      screenNum = 5;
      tabNum = 0;
      lineNum = 0;
      lineMax = 2;
      tabMax = 1;
    }
  }

  redrawDisplay();
}

void redrawDisplay()
{
  display.clearDisplay();

  //home screen;
  if (screenNum == 0)
  {   
    drawMainScreen();
  } //Menu;
  else if (screenNum == 1)
  {
    char items[5][11] = {"Select", "Edit", "Save", "Save as", "Options"};

    display.setTextColor(WHITE);
    display.setTextSize(2);

    display.setCursor(3,11);
    if (lineNum > 0)
    {
      display.print(items[lineNum - 1]);
    }

    display.setCursor(3,29);
    display.print(items[lineNum]);

    display.setCursor(3,48);
    if (lineNum < lineMax - 1)
    {
      display.print(items[lineNum + 1]);
    }

    drawHeader();
    drawFrame();
    drawScrollbar();
  } //Select, Save as, Default Prest;
  else if (screenNum == 2 || screenNum == 51 || screenNum == 3)
  {
    uint8_t names[3];
    names[1] = tabNum * lineMax + lineNum + 1;
    names[0] = names[1] - 1;
    names[2] = names[1] + 1;

    for(uint8_t i = 0; i < 3; i++)
    {
      if (names[i] > tabNum * lineMax && names[i] <= (tabNum + 1) * lineMax)
      {
        Profile pr;
        EEPROM.get(2 + (names[i] - 1) * sizeof(Profile), pr);

        uint8_t y;
        if (i == 0)
          y = 11;
        else if (i == 1)
          y = 29;
        else
          y = 48;

        display.setTextSize(2);

        display.setCursor(3, y);

        if (names[i] == currentProfileNumber && profileChanged)
        {
          display.setTextColor(BLACK, WHITE);
          if (names[i] < 10) {display.print(" ");}
          display.print(names[i]); display.print("*");
        }
        else
        {
          display.setTextColor(WHITE);
          if (names[i] < 10) {display.print(" ");}
          display.print(names[i]); display.print(".");
        }

        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.println(pr.nameline1);
        display.setCursor(39, y + 8);
        display.println(pr.nameline2);
      }
    }

    drawTabs("1-8","9-16","17-24","25-32");
    drawScrollbar();
    drawFrame();
  }  //Save as Confirmation;
  else if (screenNum == 31)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(1, 0);
    display.print("Save: ");

    if (profileChanged)
    {
      display.setTextColor(BLACK, WHITE);
      if (currentProfileNumber < 10) {display.print(" ");}
      display.print(currentProfileNumber); display.print("* ");
    }
    else
    {
      display.setTextColor(WHITE);
      if (currentProfileNumber < 10) {display.print(" ");}
      display.print(currentProfileNumber); display.print(". ");
    }
    display.setTextColor(WHITE);
    display.println(currentProfile.nameline1);

    display.setCursor(61, 8);
    display.println(currentProfile.nameline2);

    display.setCursor(1, 22);
    display.print("into: ");

    Profile pr;
    EEPROM.get(2 + (lineNum - 1) * sizeof(Profile), pr);

    if (lineNum < 10) {display.print(" ");}
    display.print(lineNum); display.print(". ");
    display.println(pr.nameline1);

    display.setCursor(61, 30);
    display.println(pr.nameline2);

    display.setCursor(23, 48);
    display.print("Yes");

    display.setCursor(90, 48);
    display.print("No");

    if (tabNum == 0)
    {
      display.drawRect(16, 46, 32, 12, 1);
    }
    else if (tabNum == 1)
    {
      display.drawRect(80, 46, 32, 12, 1);
    }
  } //Options;
  else if (screenNum == 5)
  {
    char items[2][11] = {"Def.Pst ", "Panic"};
    
    display.setTextColor(WHITE);
    display.setTextSize(2);

    display.setCursor(3,11);
    if (lineNum > 0)
    {
      display.print(items[lineNum - 1]);
      //reading value for Default Preset;
      if (lineNum == 1)
      {
        if (EEPROM.read(1) < 10)
          display.print(" ");
        display.print(EEPROM.read(1));
      }
    }

    display.setCursor(3,29);
    display.print(items[lineNum]);
    //reading value for Default Preset;
    if (lineNum == 0)
    {
      display.setTextColor(BLACK, WHITE);
      if (EEPROM.read(1) < 10)
          display.print(" ");
      display.print(EEPROM.read(1));
      display.setTextColor(WHITE);
    }

    display.setCursor(3,48);
    if (lineNum < lineMax - 1)
    {
      display.print(items[lineNum + 1]);
    }

    drawHeader();
    drawFrame();
    drawScrollbar();
  }

  display.display();
}

void setup() {
  const HardwareSerial * serials[4] = {&Serial, &Serial1, &Serial2, &Serial3};
  for (uint8_t serialNum = 0; serialNum < 4; serialNum++)
  {
    serials[serialNum]->begin(31250);
  }

  uint8_t profileversion = softwareVersion;

  if (EEPROM.read(0) != profileversion || rewriteMemory)
  {
    

    for (int i = 2; i < maxProfile; i++)
    {
      saveProfile(i);
    }

    strcpy(currentProfile.nameline1, "Default");
    strcpy(currentProfile.nameline2, "Through");
    currentProfile.layers[0].isActive = 1;

    currentProfileNumber = 1;
    setDefaultProfile(currentProfileNumber);

    saveProfile(1);

    EEPROM.update(0 , profileversion);
  } 
  else
  {
    loadProfile(EEPROM.read(1));
  }

  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  display.drawBitmap(20, 0,  logo_bitmap, 88, 64, 1);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(66, 52);
  display.print("ver "); display.println(softwareVersion);
  display.display();
  delay(3000);

  screenNum = 0;
  tabNum = 0;
  lineNum = 0;
  lineMax = 1;
  tabMax = 1;

  redrawDisplay();

  delay(3000);

  handleButton('C');
  delay(MYDELAY);
  handleButton('U');
  delay(MYDELAY);
  handleButton('C');
  delay(MYDELAY);
  handleButton('D');
  delay(MYDELAY);
  // handleButton('C');
  // delay(MYDELAY);
  // handleButton('U');
  // delay(MYDELAY);
  // handleButton('C');
  // delay(MYDELAY);
  // handleButton('C');
  // delay(MYDELAY);
  // handleButton('R');
  // delay(MYDELAY);
  // handleButton('L');
  // delay(MYDELAY);
  // handleButton('L');
  // delay(MYDELAY);
  // handleButton('R');
  // delay(MYDELAY);
  // handleButton('R');
  // delay(MYDELAY);
  // handleButton('C');
  // delay(MYDELAY);


}

void loop() {
  const HardwareSerial * serials[4] = {&Serial, &Serial1, &Serial2, &Serial3};

  for (uint8_t serialNum = 0; serialNum < 4; serialNum++)
  {
    if (serials[serialNum]->available())
    {
      Serial.print("Port: ");
      Serial.println(serialNum);  
      uint8_t bytes[3];
      serials[serialNum]->readBytes(bytes, 3);
      Serial.println(bytes[0]);
      Serial.println(bytes[1]);
      Serial.println(bytes[2]);
      uint8_t code = bytes[0] && 0b11110000;
      uint8_t channel = bytes[0] && 0b00001111;
      for (uint8_t layerNum = 0; layerNum < maxLayer; layerNum++ )
      {
         if (currentProfile.layers[layerNum].isActive && (currentProfile.layers[layerNum].midiIn & (1 << serialNum)) && (currentProfile.layers[layerNum].chIn & (1 << channel)))
         {
          if (code != 0b10000000 && code != 0b10010000 || bytes[1] >= currentProfile.layers[layerNum].noteLow && bytes[1] <= currentProfile.layers[layerNum].noteHigh)
          {
            uint8_t newcommand = bytes[0];
            if (currentProfile.layers[layerNum].chOut < 16)
            {
              newcommand = code | currentProfile.layers[layerNum].chOut;
            }

            for (uint8_t midiOutNum = 0; midiOutNum < 4; midiOutNum++)
            {
              if (currentProfile.layers[layerNum].midiOut & (1 << midiOutNum))
              {
                //serials[midiOutNum]->write(newcommand);
                //serials[midiOutNum]->write(bytes[1]);
                //serials[midiOutNum]->write(bytes[2]);
                if (midiOutNum == 0)
                 {Serial.println(newcommand);
                 Serial.println(bytes[1]);
                 Serial.println(bytes[2]);}
              }
            }
          }
        }
      }
      if (currentProfile.padLayer.isActive && (currentProfile.padLayer.midiIn & (1 << serialNum)) && (currentProfile.padLayer.chIn & (1 << channel)))
      {
        if ((code == 0b10000000 || code == 0b10010000) && 36 <= bytes[1] && bytes[1] <= maxPadNote)
        {
          uint8_t padNum = bytes[1] - 36;
          if (currentProfile.padLayer.specificNotes[padNum][0] > 0)
          {
            uint8_t newcommand = bytes[0];
            if (currentProfile.padLayer.chOut < 16)
            {
              newcommand = code | currentProfile.padLayer.chOut;
            }
            for (uint8_t midiOutNum = 0; midiOutNum < 4; midiOutNum++)
            {
              if (currentProfile.padLayer.midiOut & (1 << midiOutNum))
              {
                //serials[midiOutNum]->write(newcommand);
                //serials[midiOutNum]->write(bytes[1]);
                //serials[midiOutNum]->write(bytes[2]);
                if (midiOutNum == 0)
                 {Serial.println(newcommand);
                 Serial.println(currentProfile.padLayer.specificNotes[padNum][0]);
                 Serial.println(bytes[2]);}
              }
            }

            if (currentProfile.padLayer.specificNotes[padNum][1] & 0b11110000> 0)
            {
              uint8_t newnote = currentProfile.padLayer.specificNotes[padNum][0] + currentProfile.padLayer.specificNotes[padNum][1] >> 4;
              for (uint8_t midiOutNum = 0; midiOutNum < 4; midiOutNum++)
              {
                if (currentProfile.padLayer.midiOut & (1 << midiOutNum))
                {
                  //serials[midiOutNum]->write(newcommand);
                  //serials[midiOutNum]->write(newnote);
                  //serials[midiOutNum]->write(bytes[2]);
                  if (midiOutNum == 0)
                  {Serial.println(newcommand);
                  Serial.println(newnote);
                  Serial.println(bytes[2]);}
                }
              }
            }

            if (currentProfile.padLayer.specificNotes[padNum][1] & 0b00001111> 0)
            {
              uint8_t newnote = currentProfile.padLayer.specificNotes[padNum][0] + currentProfile.padLayer.specificNotes[padNum][1];
              for (uint8_t midiOutNum = 0; midiOutNum < 4; midiOutNum++)
              {
                if (currentProfile.padLayer.midiOut & (1 << midiOutNum))
                {
                  //serials[midiOutNum]->write(newcommand);
                  //serials[midiOutNum]->write(newnote);
                  //serials[midiOutNum]->write(bytes[2]);
                  if (midiOutNum == 0)
                  {Serial.println(newcommand);
                  Serial.println(newnote);
                  Serial.println(bytes[2]);}
                }
              }
            }
          }
        }
      }
    }
  }
  //Buttons here;
}
