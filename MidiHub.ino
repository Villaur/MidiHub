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
uint8_t lineTotal;

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
  profileChanged = 0;
}

void saveProfile(uint8_t profileNum)
{
  EEPROM.put(2 + (profileNum-1) * sizeof(Profile), currentProfile);
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
  display.drawLine(0, 29, 0, 43, WHITE);
  display.drawLine(123, 29, 123, 43, WHITE);
  display.drawPixel(1, 43, 1);
  display.drawPixel(122, 43, 1);
  display.drawLine(1, 44, 123, 44, WHITE);
  display.drawLine(2, 45, 122, 45, WHITE);
}

void drawScrollbar()
{
  for (uint8_t y = 9; y < 64; y+=2)
  {
    display.drawPixel(126, y, 1);
  }

  uint8_t len = 55 / lineTotal + (lineNum < 55 % lineTotal ? 1 : 0);
  uint8_t pos = 9 + lineNum * (55 / lineTotal) + (lineNum < 55 % lineTotal ? lineNum : 55 % lineTotal);

  Serial.println(pos);
  Serial.println(len);

  display.drawRect(125, pos, 3, len, 1);
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
    if (btn == 'D' || btn == 'L')
    {
      if (currentProfileNumber <= 1)
        loadProfile(currentProfileNumber - 1 + maxProfile);
      else
        loadProfile(currentProfileNumber - 1);
    }
    if (btn == 'C')
    {
      screenNum = 1;
      tabNum = 0;
      lineNum = 0;
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
      // saveProfile(i);
    }

    strcpy(currentProfile.nameline1, "Default");
    strcpy(currentProfile.nameline2, "Through");
    currentProfile.layers[0].isActive = 1;

    currentProfileNumber = 1;
    // setDefaultProfile(currentProfileNumber);

    // saveProfile(1);

    // EEPROM.update(0 , profileversion);

    Serial.println("Rewriting the profiles to EEPROM");
  } 
  else
  {
      // loadProfile(EEPROM.read(1));

      Serial.println("Profiles loaded");
  }

  strcpy(currentProfile.nameline1, "Tornado of");
  strcpy(currentProfile.nameline2, "Souls");
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.println(currentProfile.nameline1);
  Serial.println(currentProfile.nameline2);
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
  lineTotal = 1;

  redrawDisplay();

  delay(3000);

  display.clearDisplay();
  drawFrame();
  lineTotal = 1;
  lineNum = 0;
  drawScrollbar();
  display.display();
  delay(3000);

  display.clearDisplay();
  drawFrame();
  lineTotal = 5;
  lineNum = 1;
  drawScrollbar();
  display.display();
  delay(3000);

  display.clearDisplay();
  drawFrame();
  lineTotal = 16;
  lineNum = 15;
  drawScrollbar();
  display.display();
  delay(3000);


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
