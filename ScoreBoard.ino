#include <Wire.h> 
#include "FastLED.h"     // Library for the LED management.
#include <TimeLib.h>        
#include <avr/pgmspace.h>   // Check if needed?
#include <string.h>
#include "DS3232RTC.h"   // DS3232 library that returns time as a time_t
 
#define NUM_LEDS 150     // Number of LED controles (remember I have 3 leds / controler
#define COLOR_ORDER BRG  // Define color order for your strip
#define DATA_PIN 3       // Data pin for led comunication
#define BUTTON1_PIN 4    // pin for mode button
#define BUTTON2_PIN 5    // pin for start/stop button
#define BUTTON3_PIN 6    // pin for reset button
#define BUTTON4_PIN 7    // pin for + 1 button
#define BUTTON5_PIN 8    // pin for - 1 button
#define BUTTON6_PIN 9    // pin for + 2 button
#define BUTTON7_PIN 10   // pin for - 2 button

int ledColor = 0x0000FF;//0xFFFFFF;0x0000FF; // Color used (in hex)
 
CRGB leds[NUM_LEDS]; // Define LEDs strip

byte digits[16][21] = 
{
  {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // Digit 0
  {0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1}, // Digit 1
  {1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0}, // Digit 2
  {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1}, // Digit 3
  {1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1}, // Digit 4
  {1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1}, // Digit 5
  {1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // Digit 6
  {0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1}, // Digit 7
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // Digit 8
  {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1}, // Digit 9
  {0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // Digit G
  {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // Digit O
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1}, // Digit A
  {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0}, // Digit L
  {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0}, // Digit °
  {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0}  // Digit C
}; 

// Structure pour utilisation de different leds.
int structure[9][2] = 
{
  {42,2},  // Double dot
  {0,21},  // Hour Decade
  {21,21}, // Hour Unit
  {44,21}, // Minute Decade
  {65,21}, // Minute Unit
  {86,6},  // Score V Decade
  {92,21}, // Score V Unid
  {113,6}, // Score L Decade
  {119,21} // Score L Unit
}; 
 
bool Dot = true;  //Dot state
bool Started = false; // Start Match
int Mode = 1; // Mode
int Button = 0;
int ScoreL; // Score Local
int ScoreV; // Score Visitor
int NewTime;
int OldTime;
int NewTemp;
int OldTemp;
bool Celcius = true;
 
char buffer[64];
size_t buflen;
 
 
int tSeconds=0, tMinutes=35, hours=0;  //this line, along with another line in void timerFunction(),     
                // is where you can adjust the amount of time that is counted down in
                //the timer function
int centiseconds=0, sSeconds=0, sMinutes=0;
 
int button1Counter = 0;  //this section initializes the button counters,
                // which is how the buttons can have multiple operations within the same function - more info:      
int button1State = 0;        // http://arduino.cc/en/Tutorial/ButtonStateChange
int lastButton1State = 0;
int button2Counter = 0;
int button2State = 0;
int lastButton2State = 0;
 
void setup()
{
  Serial.begin(9600);
  Serial.println("resetting");
  LEDS.addLeds<WS2812B,DATA_PIN,RGB>(leds,NUM_LEDS);
  LEDS.setBrightness(84);
}
 
void printDec2(int value)
{
  Serial.print((char)('0' + (value / 10)));
  Serial.print((char)('0' + (value % 10)));
}
 
void ReadSerial()
{
  if (Serial.available()) 
  {
    // Process serial input for commands from the host.
    int ch = Serial.read();
    if (ch == 0x0A || ch == 0x0D) 
    {
      // End of the current command.  Blank lines are ignored.
      if (buflen > 0) {
        buffer[buflen] = '\0';
        buflen = 0;
        //Serial.print("buffer : "); Serial.println(buffer);
        int Temp=atoi(buffer);
        if (Temp == 1)
        {
          if (Mode == 3)
          {
            Mode=1;
          }
          else 
          {
            Mode += 1;
          };
          ScoreV=0;
          ScoreL=0;
          OldTemp=0;
        }
        else if (Temp == 2){Button=2;}
        else if (Temp == 3){Button=3;}
        else if (Temp == 4){Button=4;}
        else if (Temp == 5){Button=5;}
        else if (Temp == 6){Button=6;}
        else if (Temp == 7){Button=7;}
      }
    } 
    else if (ch == 0x08) 
    {
      // Backspace over the last character.
      if (buflen > 0) --buflen;
    } 
    else if (buflen < (sizeof(buffer) - 1)) 
    {
      // Add the character to the buffer after forcing to upper case.
      if (ch >= 'a' && ch <= 'z')
        buffer[buflen++] = ch - 'a' + 'A';
      else
        buffer[buflen++] = ch;
    }
  }
}  

int GetTimerOld(){
  // Geting time until timer is developed.
  tmElements_t Now;
  RTC.read(Now);
  //time_t Now = RTC.Now();// Getting the current Time and storing it into a DateTime object
  int hour=Now.Hour;
  int minutes=Now.Minute;
  int second =Now.Second;
  //Serial.print("Time is: "); Serial.println(hour*100+minutes);
  return (hour*100+minutes);
}
 
int GetTime()
{
  tmElements_t Now;
  RTC.read(Now);
  //time_t Now = RTC.Now();// Getting the current Time and storing it into a DateTime object
  int hour=Now.Hour;
  int minutes=Now.Minute;
  int second =Now.Second;
  if (second % 2==0) 
  {
    Dot = false;
  }
  else 
  {
    Dot = true;
  };
  //Serial.print("Time is: "); Serial.println(hour*100+minutes);
  return (hour*100+minutes);
}
 
void TimeAdjust()
{
  tmElements_t Now;
  RTC.read(Now);
  if (Button == 4)
  {
    if (Now.Hour== 23){Now.Hour=0;}
    else {Now.Hour += 1;};
  }
    if (Button == 5){
      if (Now.Hour== 0){Now.Hour=23;}
      else {Now.Hour -= 1;};
  }
    if (Button == 6){
      if (Now.Minute== 59){Now.Minute=0;}
      else {Now.Minute += 1;};
  }
    if (Button == 7){
      if (Now.Minute== 0){Now.Minute=59;}
      else {Now.Minute -= 1;};
  }
    RTC.write(Now); 
} 
  
  
int GetTimer() 
{ //the timer function was made with the help of this post: http://pastebin.com/f57045830
  if(Started) 
  { //if "Start/Stop" is pressed, the timer counts down
    static unsigned long lastTick = 0;
    if (tSeconds > -1)
    {
      if (millis() - lastTick >= 1000) 
      {
        lastTick = millis();
        tSeconds--;
      }
    }
    if (tMinutes > 0) 
    {
      if (tSeconds < 0) 
      {
        tMinutes--;
        tSeconds = 59;
      }
    }
    if (hours > 0) 
    {
      if (tMinutes <= 0) 
      {
        hours--;
        tMinutes = 60;
      }
    }
  }
  return (tMinutes*100+tSeconds); 
}

void TimerAdjust()
{
  if (Button == 4)
  {
    if (tMinutes < 99){
      tMinutes += 1;};
    }
    if (Button == 5){
      if (tMinutes > 0)
      {
        tMinutes -= 1;
      };
    }
    if (Button == 6){
      if (tSeconds == 59) 
      {
        tSeconds = 0; tMinutes +=1;
      }
      else 
      {
        tSeconds += 1; 
      };
    }
    if (Button == 7){
      if (tSeconds == 0)
      {
        tSeconds = 59;tMinutes -=1;
      }
      else {
        tSeconds -= 1;
      };
    }   
  }   
  
  
void TimerReset()
{
  if (Button == 3)
  {
    tSeconds=0, tMinutes=35, hours=0;
  }
}

void ShowGoal()
{
  static uint8_t hue = 0;
  Serial.println("GOALLLLLL");
  for(int i = 0; i < 140; i++) 
  {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    //fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
   }
  for(int i = 139; i >= 0; i--) 
  {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    //fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
  /*for(int i = 0; i < 4; i++) {
    //ShowGOAL
    int cursor = 0;
    for(int k=0; k<=20;k++){
        //Serial.print(digits[digit][k]);
        if (digits[10][k]== 1){leds[cursor]=ledColor;}
         else if (digits[10][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
    cursor =21;
    for(int k=0; k<=20;k++){
        //Serial.print(digits[digit][k]);
        if (digits[11][k]== 1){leds[cursor]=ledColor;}
         else if (digits[11][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
    cursor =44;
    for(int k=0; k<=20;k++){
        //Serial.print(digits[digit][k]);
        if (digits[12][k]== 1){leds[cursor]=ledColor;}
         else if (digits[12][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
    cursor =65;
    for(int k=0; k<=20;k++){
        //Serial.print(digits[digit][k]);
        if (digits[13][k]== 1){leds[cursor]=ledColor;}
         else if (digits[13][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
    FastLED.show();    
    delay(100);
    //HideGoal
    for(int k=0; k<=140;k++){
        leds[cursor]=0x000000;
        cursor ++;
        };
    FastLED.show();   
    Serial.println("End Goal");
    delay(100);
  }
  
  */
}
 
void ScoreAdjust()
{
  if (Button == 4)
  {
    ScoreL += 1;
    ShowGoal();
  }
  if (Button == 5){
    if (ScoreL > 0){
      ScoreL -= 1;
    }
  }
  if (Button == 6){
    ScoreV += 1;
    ShowGoal();
  }
  if (Button == 7){
    if (ScoreV > 0){
      ScoreV -= 1;
    }
  }
} 
 
int GetTemp()
{
    int temp;
    tpElements_t tp;
    RTC.readTemperature(tp);
    if (tp.Temp != NO_TEMPERATURE) {
     
        temp = tp.Temp; //* 10 + tp.Decimal/10;
    }
    else {
        temp = 0;
    }
    //Serial.println(temp);
    return(temp);
}
 
void TimeToArray(int NewTime)
{
  int cursor = 0;
  //int NewTime = GetTime();  // Get time
  for(int i=1;i<=4;i++)
  {
    int digit = NewTime % 10; // get last digit in time
    if (i==1)
    {
      //Serial.print("Digit 4 is : ");Serial.print(digit);Serial.print(" ");
      cursor =65  ;
      for(int k=0; k<=20;k++)
      {
        //Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
        else if (digits[digit][k]==0){leds[cursor]=0x000000;};
        cursor ++;
        };
      //Serial.println();
    }
    else if (i==2)
    {
      //Serial.print("Digit 3 is : ");Serial.print(digit);Serial.print(" ");
      cursor =44;
      for(int k=0; k<=20;k++)
      {
        //Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
        else if (digits[digit][k]==0){leds[cursor]=0x000000;};
        cursor ++;
        };
      //Serial.println();
    }
    else if (i==3)
    {
      //Serial.print("Digit 2 is : ");Serial.print(digit);Serial.print(" ");
      cursor =21;
      for(int k=0; k<=20;k++)
      {
        //Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
        else if (digits[digit][k]==0){leds[cursor]=0x000000;};
        cursor ++;
        };
      //Serial.println();
    }
    else if (i==4)
    {
      //Serial.print("Digit 1 is : ");Serial.print(digit);Serial.print(" ");
      cursor =0;
      for(int k=0; k<=20;k++)
      {
        //Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
        else if (digits[digit][k]==0){leds[cursor]=0x000000;};
        cursor ++;
        };
      //Serial.println();
    }
    NewTime /= 10;
  }
//  FastLED.show();
}

void ScoreToArray(int ScoreV, int ScoreL)
{
  int cursor = 0;
  int digit = 0;
  cursor =86;
  if (ScoreV > 9)
  {  
    for(int k=0; k<=5;k++)
    {
      leds[cursor]=ledColor;
      cursor ++;
    };
  } 
  else 
  {
    for(int k=0; k<=5;k++)
    {
      //Serial.print(digits[digit][k]);
      leds[cursor]=0x000000;
      cursor ++;
    };
  }
  cursor =92;
  digit = ScoreV %10;
  for(int k=0; k<=20;k++)
  {
    if (digits[digit][k]== 1){leds[cursor]=ledColor;}
    else if (digits[digit][k]==0){leds[cursor]=0x000000;};
    cursor ++;
  };
  cursor =113;
  if (ScoreL > 9)
  {
    for(int k=0; k<=5;k++)
    {
      leds[cursor]=ledColor;
      cursor ++;
    };
  } 
  else 
  {
    for(int k=0; k<=5;k++)
    {
      leds[cursor]=0x000000;
      cursor ++;
    };
  }
  cursor =119;
  digit = ScoreL %10;
  for(int k=0; k<=20;k++)
  {
    if (digits[digit][k]== 1){leds[cursor]=ledColor;}
    else if (digits[digit][k]==0){leds[cursor]=0x000000;};
    cursor ++;
  };
}
 
void BlackArray()
{
  for(int k=86; k<=140;k++)
  {
    leds[k]=0x000000; 
  };
}
 
void TempToArray(int NewTime)
{
  int cursor = 0;
  for(int i=1;i<=4;i++)
  {
//    int digit = NewTemp % 10; // get last digit in temp
    if (i==1)
    {
      //Serial.print("C");
      cursor =65  ;
      for(int k=0; k<=20;k++)
      {
        //Serial.print(digits[digit][k]);
        if (digits[15][k]== 1){leds[cursor]=ledColor;}
        else if (digits[15][k]==0){leds[cursor]=0x000000;};
        cursor ++; 
      };    
    }
    else if (i==2)
    {
      //Serial.print("°");
      cursor =44;
      for(int k=0; k<=20;k++)
      {
        //Serial.print(digits[digit][k]);
        if (digits[14][k]== 1){leds[cursor]=ledColor;}
        else if (digits[14][k]==0){leds[cursor]=0x000000;};
        cursor ++;
      };
    }
    else if (i==3)
    {
      //Serial.print("Digit 2 is : ");Serial.print(digit);Serial.print(" ");
      cursor =21;
      int digit = NewTemp % 10; // get last digit in temp
      for(int k=0; k<=20;k++)
      {
        //Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
        else if (digits[digit][k]==0){leds[cursor]=0x000000;};
        cursor ++;
      };
      NewTemp /= 10;
    }
    else if (i==4)
    {
      //Serial.print("Digit 1 is : ");Serial.print(digit);Serial.print(" ");
      cursor =0;
      int digit = NewTemp % 10; // get last digit in temp
      for(int k=0; k<=20;k++)
      {
        //Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
        else if (digits[digit][k]==0){leds[cursor]=0x000000;};
        cursor ++;
      };
    }
  }
}
 
void ShowTime()
{
  NewTime = GetTime();
  if (NewTime != OldTime)
  {
    OldTime = NewTime;
    int hour = NewTime / 100;
    int min = NewTime % 100;
    printDec2(hour);Serial.print(":");printDec2(min);Serial.print(" ");
    Serial.println(" ");
  }
  BlackArray();
  TimeToArray(NewTime);
  if (Dot){leds[42]=ledColor;leds[43]=ledColor;}
  else {leds[42]=0x000000;leds[43]=0x000000;};
}
 
void ShowTemp()
{
  NewTemp = GetTemp();
  if (NewTemp != OldTemp)
  {
    OldTemp = NewTemp;
    printDec2(NewTemp);Serial.print("°");Serial.print("C");
    Serial.println(" ");
  }
  BlackArray();
  TempToArray(NewTemp);
  leds[42]=0x000000;leds[43]=0x000000;
}
 
void ShowScore()
{
  NewTime = GetTimer();
  if (NewTime != OldTime)
  {
    OldTime = NewTime;
    Serial.print("LLHC : ");Serial.print(ScoreL);Serial.print(" ");
    int hour = NewTime / 100;
    int min = NewTime % 100;
    printDec2(hour);Serial.print(":");printDec2(min);Serial.print(" ");
    Serial.print("VISITOR : ");Serial.print(ScoreV);
    Serial.println(" ");
  }
  ScoreToArray(ScoreV, ScoreL);
  TimeToArray(NewTime);
  leds[42]=ledColor;leds[43]=ledColor;
}
 
void loop()
{
  ReadSerial(); // for test purpose reading serial in place of button
                // 1: switch mode 1: clock, 2 timer, 3: temp
  switch(Mode)
  {
    case 1:  //the clock defaults to clockFunction()
      if (Button != 0)
      {
        TimeAdjust();
        Button = 0;
        //delay(1000);
      }
      ShowTime();
      FastLED.show();
    break;
    case 2:  //Switches to timerFunction()
      if (Button != 0)
      {
        if (Button == 2) {Started = !Started;}
        else 
        {
          if (Started){ScoreAdjust();}
          else {TimerReset();
          TimerAdjust();};
        }
        Button = 0;
        //delay(1000);
      }
      ShowScore();
      FastLED.show();
    break;
    case 3:  //Switches to timerFunction()
      if (Button != 0)
      {
        if (Celcius){Celcius = !Celcius;}
        Button = 0;
        //delay(1000);
      }
      ShowTemp();
      FastLED.show();
    break;
  }
  //delay(10000);
}
