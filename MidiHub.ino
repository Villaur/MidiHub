#include <EEPROM.h>
#include <U8g2lib.h>

#define nameLength 10
#define maxProfile 32
#define maxLayer 8
#define maxPad 8
#define maxPadNote 43 //35+maxPad;

#define rewriteMemory 0 //set it to 1 if you want to rewrite all profiles in EEPROM to default on startup; 


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
  char name[2][nameLength] = {"Empty", "Profile"};
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

Button buttonUp = Button(31);
Button buttonDown = Button(25);
Button buttonLeft = Button(33);
Button buttonRight = Button(27);
Button buttonCenter = Button(29);
Button buttonBack = Button(23);

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

/* Buttons:
  U
L C R
  D    B
*/
void handleButton(char btn)
{
  //home screen;
  if (screenNum == 0)
  {
    if (btn == 'U' || btn == 'R')
    {
      currentProfileNumber++;
      if (currentProfileNumber > maxProfile)
        currentProfileNumber -= maxProfile;
      currentProfile = EEPROM.get(2 + (currentProfileNumber - 1) * sizeof(Profile), currentProfile);
    }
    if (btn == 'D' || btn == 'L')
    {
      currentProfileNumber--;
      if (currentProfileNumber <= 0)
        currentProfileNumber += maxProfile;
      currentProfile = EEPROM.get(2 + (currentProfileNumber - 1) * sizeof(Profile), currentProfile);
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
  //home screen;
  if (screenNum == 0)
  {

  }
}
void setup() {
  const HardwareSerial * serials[4] = {&Serial, &Serial1, &Serial2, &Serial3};
  for (uint8_t serialNum = 0; serialNum < 4; serialNum++)
  {
    serials[serialNum]->begin(31250);
  }

  if (EEPROM.read(0) || rewriteMemory)
  {
    currentProfileNumber = 1;
    EEPROM.write(1, currentProfileNumber);

    for (int i = 2; i < maxProfile; i++)
    {
      EEPROM.put(2 + (i-1) * sizeof(Profile), currentProfile);
    }

    strcpy(currentProfile.name[0], "Default");
    strcpy(currentProfile.name[1], "Through");
    currentProfile.layers[0].isActive = 1;
    EEPROM.put(2, currentProfile);

    EEPROM.write(0, 0);

    Serial.println("Rewriting the profiles to EEPROM");
  } 
  else
  {
      currentProfileNumber = EEPROM.read(1);
      currentProfile = EEPROM.get(2 + (currentProfileNumber - 1) * sizeof(Profile), currentProfile);

      Serial.println("Profiles loaded");
  }

  screenNum = 0;
  tabNum = 0;
  lineNum = 0;

  redrawDisplay();

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
