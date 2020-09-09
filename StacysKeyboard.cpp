/*
  KeyboardAndMouseControl

  Controls the mouse from five pushbuttons on an Arduino Leonardo, Micro or Due.

  Hardware:
  - five pushbuttons attached to D2, D3, D4, D5, D6

  The mouse movement is always relative. This sketch reads four pushbuttons, and
  uses them to set the movement of the mouse.

  WARNING: When you use the Mouse.move() command, the Arduino takes over your
  mouse! Make sure you have control before you use the mouse commands.

  created 15 Mar 2012
  modified 27 Mar 2012
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/KeyboardAndMouseControl
*/

#include <Adafruit_NeoPixel.h>
  #ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include "Keyboard.h"
#include "Mouse.h"

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    3

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 6

  // Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


uint32_t StartupColor = strip.Color(30, 30, 30);
uint32_t PendingColor = strip.Color(203, 245, 66);
uint32_t ConfirmColor = strip.Color(0, 255, 0);
uint32_t GoBackColor = strip.Color(255, 0, 0);

//INSTRUCTIONS: Hold 1.5s to go back 1 menu. Tap to select.


//int MenuNavigation[5]; //the int first position indicated which menu is open at depth 1, the int in seccond positon, at depth 2, and so on.

enum Commands {None, Back, Select1};

Commands NextCommand = None;

String CurrentMenu = "MainMenuContent";
String CurrentSelection = "";

String MenuDepths[5];
int CurrentMenuDepth = 0;

String MainMenuContent[] = {
  "Type",
  "Mouse",
  "End"
};

String SpecialMenu[] = {
  "Enter",
  "Tab",
  "LeftArrow",
  "RightArrow",
  "UpArrow",
  "DownArrow",
  "CapsLock",
  //Home, End, INS, DEL, TAB, ALT (Select Once for Press, 2x for hold)
  "End"
};

String SymbolMenu[] = {
  ".",
  ",",
  "?",
  "!", 
  "@",
  "#",
  "$",
  "%",
  "^",
  "&",
  "*",
  "(",
  ")",
  "-",
  "_",
  "+",
  "=",
  "/", 
  "End"
};

String KeyboardMenu[] = {
  "Type Letter",
  "BackSpace",
  "Enter", //Replace with Special MEnu
  "Tab", // Replace with Special Menu
  "IncreaseDelay",
  "ReduceDelay",
  "End"
};

//Long press to go back should be disabled in mouse menu
String MouseMenu[] = {
  "Same As Last",
  "Back Menu",
  "Up",
  "Down",
  "Left",
  "Right",
  "L-Click",
  "R-Click",
  "M-Click",
  "Scroll Up",
  "Scroll Down",
  "Sensitivity Up",
  "Sensitivikty Down",
  "End"
};



//eotha sinrd luymw fgcbp kvjqxz

char LettersByFrequency[] = {
  'e', 'o', 't', 'h', 'a', 's', 'i', 'n', 'r', 'd', 'l', 'u', 'y', 'm', 'w', 'f', 'g', 'c', 'b', 'p', 
  'k', 'v', 'j', 'q', 'x', 'z'};

char LetterClumps[6][6] = {
// {'_', 'e', 't', 's', 'd', 'w'},
// {'o', 'h', 'i', 'l', 'f', 'k'},
// {'a', 'n', 'u', 'g', 'v', 'z'},
// {'r', 'y', 'c', 'j', '1', '2'},
// {'m', 'b', 'q', '3', '4', '5'},
// {'p', 'x', '6', '7', '8', '9'}

{'_', 'E', 'T', 'S', 'D', 'W'},
{'O', 'H', 'I', 'L', 'F', 'K'},
{'A', 'N', 'U', 'G', 'V', 'Z'},
{'R', 'Y', 'C', 'J', '1', '2'},
{'M', 'B', 'Q', '3', '4', '5'},
{'P', 'X', '6', '7', '8', '9'}
};

  // {'e', 'o', 't', 'h', 'a', 's'},
  // {'i', 'n', 'r', 'd', 'l', 'u'},
  // {'y', 'm', 'w', 'f', 'g', 'c'},
  // {'b', 'p', 'k', 'v', 'j', 'q'},
  // {'x', 'z', '1', '2', '3', '4'},
  // {'5', '6', '7', '8', '9', '0'}

char CurrentCharChoices[6];

int cycleSpeed = 3000;
int shortInputDelay = 2500;
int longInputDelay = 3500;
int inputDelay = 5;

const int PrimaryInput = 2;
const int SeccondInput = 11;
const int ThirdInput = 12;

void setup() { // initialize the buttons' inputs:
  
  pinMode(PrimaryInput, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  // initialize mouse control:
  Mouse.begin();
  Keyboard.begin();

    // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
    // END of Trinket-specific code.


  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  //delay(100);
  theaterChaseRainbow(50);
  strip.show();
  delay(500);
  strip.clear();
  strip.show();

  // strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  // strip.show();            // Turn OFF all pixels ASAP
  // strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {

   Commands doWhat;
    
    delay(1);

    if(CurrentMenu == "MainMenuContent")
    {

      int t = 0;

      doWhat = AwaitInput(cycleSpeed);

      if(doWhat == Back)
      {

        CurrentMenu = "KeyboardMenu";
        delay(500);
        // strip.clear();
        // strip.show();
      }
        
        //DisplayMenuOptions(MainMenuContent);
    }
    else if(CurrentMenu == "KeyboardMenu")
    {
         doWhat = DisplayMenuOptions(KeyboardMenu);

         if(doWhat == Back)
         {
          CurrentMenu = "MainMenuContent";
         }
         else if(doWhat == Select1)
         {
          
            if(CurrentSelection == "BackSpace")
            {
              Keyboard.write(KEY_BACKSPACE);
              CurrentSelection = "";
            }
            else if(CurrentSelection == "Enter")
            {
              Keyboard.write(KEY_RETURN);
              CurrentSelection = "";
            }
            else if(CurrentSelection == "Tab")
            {
              Keyboard.write(KEY_TAB);
              CurrentSelection = "";
            }
            else if(CurrentSelection == "CapsLock")
            {
              CurrentSelection = "";
            }
            else if(CurrentSelection == "Escape")
            {
              CurrentSelection = "";
            }
            else if(CurrentSelection == "IncreaseDelay")
            {
              IncreaseDelay();
              CurrentSelection = "";
            }
            else if(CurrentSelection == "ReduceDelay")
            {
              ShortenDelay();
              CurrentSelection = "";
            }
            else
            {
              CurrentMenu = CurrentSelection;
              CurrentSelection = "";
            }
         }
    }
    else if(CurrentMenu == "Type Letter")
    {
      AddCurrsor();

      for(int g = 0; g < 6; ++g)
      {
        Keyboard.write(' ');
        Keyboard.write('>');
        
       
         for(int i = 0; i < 6; ++i)
         {
             Keyboard.write(LetterClumps[g][i]);
             CurrentCharChoices[i] = LetterClumps[g][i];
         }
          Keyboard.write('<');
          Keyboard.write(' ');
         doWhat = AwaitInput(cycleSpeed);
         for(int b = 0; b < 10; ++b)//Delete cursor and letter choices
         {
           Keyboard.write(KEY_BACKSPACE);
         }

         if(doWhat == Select1)
         {
            CurrentMenu = "Type Letter Specific";

            g = 6;
         }
         else if(doWhat == Back)
         {
          CurrentMenu = "KeyboardMenu";

            g = 6;
         }
      }
      RemoveCursor();
    }

    else if(CurrentMenu == "Type Letter Specific")
    {
      AddCurrsor();

      char charToType = '\0';
      Keyboard.write('>');
      for(int i = 0; i < 6 && charToType == '\0'; ++i)
      {
        for(int j = 0; j < 6 ; ++j)
        {
          if(j == i)
          {
            Keyboard.write(' ');
            Keyboard.write('|');
            Keyboard.write(CurrentCharChoices[i]);
            Keyboard.write('|');
            Keyboard.write(' ');
            
          }
          else
          {
            Keyboard.write(CurrentCharChoices[j]);
          }
        }
        Keyboard.write('<');
        //Keyboard.write(CurrentCharChoices[i]);
        doWhat = AwaitInput(cycleSpeed);

        if(doWhat == Select1)
        {
          charToType =  CurrentCharChoices[i];

          CurrentMenu = "KeyboardMenu";
          CurrentSelection = "";
        }
        else if(doWhat == Back)
        {
           CurrentMenu = "Type Letter";
           i = 6;
        }
        for(int d = 0; d < 11 ; ++d)
        {
          Keyboard.write(KEY_BACKSPACE);
        }
      }
      
      Keyboard.write(KEY_BACKSPACE);
      RemoveCursor();
      if(charToType != '\0')
       {
        if(charToType == '_')
        {
          Keyboard.write(' ');
        }
        else
        {
          Keyboard.write(charToType);
        }
        charToType = '\0';
       }
       
    }
    
    
    else if(CurrentMenu == "Arrow Keys")
    {

    }
    else if(CurrentMenu == "Other Keys")
    {

    }
    
    

//    for(int g = 0; g < 6; ++g)
//    {
//      for(int i = 0; i < 6; ++i)
//      {
//          Keyboard.write(LetterClumps[g][i]);
//      }
//
//      delay(1000);
//
//      for(int b = 0; b < 6; ++b)
//      {
//        Keyboard.write(KEY_BACKSPACE);
//      }
//
//    }


    //Keyboard.write('m');
}


int CurrentDisplayLength = 0;

void DisplayText (String text_)
{
  for(int c = 0; text_[c] != '\0'; ++c)
      {
        Keyboard.write(text_[c]);
        ++CurrentDisplayLength;
        delay(10);
      }
}

void ClearText (String text_)
{
    for(int c = 0; text_[c] != '\0'; ++c)
    {
      Keyboard.write(KEY_BACKSPACE);
      delay(10);
    }
}

Commands DisplayMenuOptions(String menu_[])
{
  AddCurrsor();

  Commands doWhat;
  for(int M = 0; menu_[M] != "End"; ++M)
    {
      String selectionToDisplay = menu_[M];

      DisplayText(selectionToDisplay);

      doWhat = AwaitInput(cycleSpeed); //Will return true when input conditoin true, or false in 1000 frames.
      
      ClearText(selectionToDisplay);
      
      if(doWhat == Select1)
      {
        CurrentSelection = selectionToDisplay;
        RemoveCursor();
        return doWhat;
      }
      else if(doWhat == Back)
      {
        RemoveCursor();
        return doWhat;
      }

      
    }
    RemoveCursor();
    return doWhat;
}



Commands AwaitInput(int FramesToWait)
{
  delay(100);
  

  int i = 0;
  while (i < FramesToWait)
  {
    //fillOverTime(PendingColor, i, FramesToWait);

    if(digitalRead(PrimaryInput) == LOW) //No Input
    {
      ++i;
      delay(1);
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if(digitalRead(PrimaryInput) == HIGH) //Input Detected
    {
      int t = 0;

      digitalWrite(LED_BUILTIN, HIGH);
      while(t <= longInputDelay && digitalRead(PrimaryInput) == HIGH)
      {
        fillOverTime(PendingColor, t, FramesToWait);
        t += 1;
        delay(1);
      }
      digitalWrite(LED_BUILTIN, LOW);

      strip.clear();
      strip.show();

      if(t >= longInputDelay)
      {
        DisplayText(" --Back--");
        colorWipe(strip.Color(  255, 0,   0), 25); // Green\
        strip.show();
        delay(500);
        ClearText(" --Back--");
        
        strip.clear();
        strip.show();
        return Back;
      }
      else if (t >= inputDelay)
      {
        DisplayText(" --Selected--");
        colorWipe(strip.Color(  0, 255,   0), 25); // Green
        strip.show();
        delay(500);
        ClearText(" --Selected--");

        strip.clear();
        strip.show();
        return Select1;
      }
      
    }

  }

  strip.clear();
  strip.show();
  return None;
}

void AddCurrsor()
{
  Keyboard.write('_');
  Keyboard.write(' ');
  Keyboard.write('<');
  Keyboard.write('-');
  Keyboard.write('-');
  Keyboard.write(' ');
  Keyboard.write(' ');
}

void RemoveCursor()
{
  Keyboard.write(KEY_BACKSPACE);
  Keyboard.write(KEY_BACKSPACE);
  Keyboard.write(KEY_BACKSPACE);
  Keyboard.write(KEY_BACKSPACE);
  Keyboard.write(KEY_BACKSPACE);
  Keyboard.write(KEY_BACKSPACE);
  Keyboard.write(KEY_BACKSPACE);
}

void ShortenDelay()
{
  shortInputDelay -=200;
  longInputDelay -=200;
  cycleSpeed *= 0.9;
  Serial.println(cycleSpeed);
}

void IncreaseDelay()
{
  shortInputDelay += 400;
  longInputDelay += 400;
  cycleSpeed *= 1.2;
  Serial.println(cycleSpeed);
}



void MoveMouse()
{
   /*
    for(int i = 0; i < 300; ++i)
    {
       Mouse.move(-1, 0);
       delay(1);
    }
    delay(1000);

    for(int i = 0; i < 400; ++i)
    {
       Mouse.move(0, 1);
       delay(1);
    }
    delay(1000);

    for(int i = 0; i < 500; ++i)
    {
       Mouse.move(1, -1);
       delay(1);
    }
    delay(1000);
    
   // Mouse.click(MOUSE_LEFT);
   */
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void fillOverTime(uint32_t color, int currentTime, int maxTime)
{
  int numToFill = 1 + ((float)currentTime/(float)maxTime) * ((strip.numPixels() - 1));
  Serial.println(numToFill);

  strip.clear();
  for(int i=0; i<numToFill; i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  }                   //  Pause for a moment
  
  strip.show();                          //  Update strip to match       



}