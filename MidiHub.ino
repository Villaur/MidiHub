#include <EEPROM.h>

#define nameLength 20
#define maxProfile 32
#define maxLayer 8
#define maxPad 8
#define maxPadNote 43


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
  char name[nameLength] = "Empty Profile";
  Layer layers[maxLayer];
  PadLayer padLayer;
};

uint8_t currentProfileNumber;
Profile currentProfile;

uint8_t screenNum;
uint8_t tabNum;
uint8_t linwNum;

void setup() {
  const HardwareSerial * serials[4] = {&Serial, &Serial1, &Serial2, &Serial3};
  for (uint8_t serialNum = 0; serialNum < 4; serialNum++)
  {
    serials[serialNum]->begin(31250);
  }

  if (EEPROM.read(0))
  {
    currentProfileNumber = 1;
    EEPROM.write(1, currentProfileNumber);

    for (int i = 2; i < maxProfile; i++)
    {
      EEPROM.put(2 + (i-1) * sizeof(Profile), currentProfile);
    }

    strcpy(currentProfile.name, "Default Through");
    currentProfile.layers[0].isActive = 1;
    EEPROM.put(2, currentProfile);

    EEPROM.write(0, 0);

    Serial.println("BF");
  } 
  else
  {
      currentProfileNumber = EEPROM.read(1);
      currentProfile = EEPROM.get(2 + (currentProfileNumber - 1) * sizeof(Profile), currentProfile);

      Serial.println("LF");
  }

  screenNum = 0;
  tabNum = 0;
  lineNum = 0;
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
          uint8_t padNum = bytes[i] - 36;
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
