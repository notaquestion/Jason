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

String CurrentMenu = "IdleMenu";
String CurrentSelection = "";

String MenuDepths[5];
int CurrentMenuDepth = 0;

String MainMenuContent[] = {
  "KeyboardMenu",
  // "CommonWordMenu",
  // "StacyWordMenu",
  "MouseMenu",
  "OptionsMenu",
  "End"
};

    String KeyboardMenu[] = {
      "Type Letter",
      "BackSpace",
      "SpecialKeyMenu",
      "End"
    };

      String SpecialKeyMenu[] = {
        "Enter",
        "Tab",
        // "LeftArrow",
        // "RightArrow",
        // "UpArrow",
        // "DownArrow",
        // "CapsLock",
        //Home, End, INS, DEL, TAB, ALT (Select Once for Press, 2x for hold)
        "End"
      };

    String OptionsMenu[] = {
      "FasterCycleSpeed",
      "LongerCycleSpeed",
      "ShorterGoBackInput",
      "LongerGoBackInput",
      "End"
    };

// String SymbolMenu[] = {
//   ".",
//   ",",
//   "?",
//   "!", 
//   "@",
//   "#",
//   "$",
//   "%",
//   "^",
//   "&",
//   "*",
//   "(",
//   ")",
//   "-",
//   "_",
//   "+",
//   "=",
//   "/", 
//   "End"
// };

// //Long press to go back should be disabled in mouse menu
// String MouseMenu[] = {
//   "Same As Last",
//   "Back Menu",
//   "Up",
//   "Down",
//   "Left",
//   "Right",
//   "L-Click",
//   "R-Click",
//   "M-Click",
//   "Scroll Up",
//   "Scroll Down",
//   "Sensitivity Up",
//   "Sensitivikty Down",
//   "End"
// };







//eotha sinrd luymw fgcbp kvjqxz

char LetterClumps[6][6] = {
{'_', 'E', 'T', 'S', 'D', 'W'},
{'O', 'H', 'I', 'L', 'F', 'K'},
{'A', 'N', 'U', 'G', 'V', 'Z'},
{'R', 'Y', 'C', 'J', '1', '2'},
{'M', 'B', 'Q', '3', '4', '5'},
{'P', 'X', '6', '7', '8', '9'}
};

String LetterClumpsS[7] = {
  {"_/E/T/S/D/W"},
  {"O/H/I/L/F/K"},
  {"A/N/U/G/V/Z"},
  {"R/Y/C/J/1/2"},
  {"M/B/Q/3/4/5"},
  {"P/X/6/7/8/9"},
  {"./?/,/+/!"}
};


char CurrentCharChoices[6];

int cycleSpeed = 3000;
int GoBackHoldTime = 1500;
int inputDelay = 5;

const int PrimaryInput = 2;
const int SeccondInput = 11;
const int ThirdInput = 12;

float MouseTimer = 0.0;

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

    if(CurrentMenu == "IdleMenu")
    {
      int t = 0;

      doWhat = AwaitInput(cycleSpeed);

      if(doWhat == Back)
      {

        CurrentMenu = "MainMenuContent";
        delay(500);
        // strip.clear();
        // strip.show();
      }
    }
    if(CurrentMenu == "MainMenuContent")
    {
      doWhat = DisplayMenuOptions(MainMenuContent);
      //DisplayMenuOptions(MainMenuContent);

      if(doWhat == Back)
      {
        CurrentMenu = "IdleMenu";
      }
      else if(doWhat == Select1)
         {
          
            if(false)
            {

            }
            else
            {
              CurrentMenu = CurrentSelection;
              CurrentSelection = "";
            }
         }
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
            // else if(CurrentSelection == "")
            // {
            //   CurrentSelection = "";
            // }
            
            // else if(CurrentSelection == "IncreaseDelay")
            // {
            //   IncreaseDelay();
            //   CurrentSelection = "";
            // }
            // else if(CurrentSelection == "ReduceDelay")
            // {
            //   ShortenDelay();
            //   CurrentSelection = "";
            // }
            else
            {
              CurrentMenu = CurrentSelection;
              CurrentSelection = "";
            }
         }
    }

    else if(CurrentMenu == "Type Letter")
    {
      //(String[] options, int size, String seperator, String goBackMenu)
      String TypeWhat = DispalyGridOptionsAndType(LetterClumpsS, 7, '/', "KeyboardMenu");

      if(TypeWhat != "")
      {
        delay(1000);
        DisplayText(TypeWhat);
        CurrentMenu = "KeyboardMenu";
      }
    }

    
    
    else if(CurrentMenu == "SpecialKeyMenu")
    {
      doWhat = DisplayMenuOptions(SpecialKeyMenu);
      // KEY_LEFT_CTRL   
      // KEY_LEFT_SHIFT  
      // KEY_LEFT_ALT    
      // KEY_LEFT_GUI    
  
      // KEY_UP_ARROW    
      // KEY_DOWN_ARROW  
      // KEY_LEFT_ARROW  
      // KEY_RIGHT_ARROW 
      // KEY_RETURN
      // KEY_INSERT
      // KEY_DELETE
      // KEY_PAGE_UP
      // KEY_PAGE_DOWN
      // KEY_HOME
      // KEY_END
      // KEY_CAPS_LOCK
      // KEY_F1 ..

       if(doWhat == Back)
       {
        CurrentMenu = "KeyboardMenu";
       }
       else if(doWhat == Select1)
       {
          if(CurrentSelection == "Enter")
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
            Keyboard.write(KEY_CAPS_LOCK);
            CurrentSelection = "";
          }
          else if(CurrentSelection == "Escape")
          {
            Keyboard.write(KEY_ESC);
            CurrentSelection = "KEY_ESC";
          }
        }
    }
    else if(CurrentMenu == "OptionsMenu")
    {
      doWhat = DisplayMenuOptions(OptionsMenu);

         if(doWhat == Back)
         {
          CurrentMenu = "KeyboardMenu";
         }
         else if(doWhat == Select1)
         {
            if(CurrentSelection == "FasterCycleSpeed")
            {
              FasterCycleSpeed();
              CurrentSelection = "";
            }
            else if(CurrentSelection == "LongerCycleSpeed")
            {
              SlowerCycleSpeed();
              CurrentSelection = "";
            }
            else if(CurrentSelection == "ShorterGoBackInput")
            {
               ShortenDelay();
              CurrentSelection = "";
            }
            else if(CurrentSelection == "LongerGoBackInput")
            {
              IncreaseDelay();
              CurrentSelection = "";
            }
            else
            {
              CurrentMenu = CurrentSelection;
              CurrentSelection = "";
            }
         }
    }
    else if(CurrentMenu == "MouseMenu")
    {
      MouseFunctions();
    }
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

String DispalyGridOptionsAndType(String options[], int size, char seperator, String goBackMenu)
{
  String selection = "";
    AddCurrsor();

    for(int g = 0; g < size; ++g)
    {
        DisplayText(" >");
        DisplayText(options[g]);
        DisplayText("<");

        Commands doWhatNow = AwaitInput(cycleSpeed);
        
        ClearText(" >");
        ClearText(options[g]);
        ClearText("<");

        if(doWhatNow == Select1)
        {
            selection = ParseStringAndPresentOptions(options[g], seperator);
           
           if(selection == "")
           {
            //We wanted to go back.
            g = -1; //This'll become 0 in the next go round.
           }
           else
           {
            g = size;
            //delay(3000);
           }

        }
        else if(doWhatNow == Back)
        {
          CurrentMenu = goBackMenu;
          //g = size;
          RemoveCursor();
          return "";
        }
    }
    RemoveCursor();

    if(selection != "")
    {
      //DisplayText(selection);
      return selection;
    }
}

String ParseStringAndPresentOptions(String parseMe, char seperator)
{
  String GridSelection;
  Commands doWhatNow;

  int c = 0;
  String chocies[10];

  int p = 0;
  String CurrentString = "";

  while(parseMe[p] != '\0' && c < 10)
  {
    if(parseMe[p] == seperator)
    {
      chocies[c] = CurrentString;
      ++c;
      CurrentString = "";
    }
    else
    {
      CurrentString += parseMe[p];
    }

    if(parseMe[p + 1] == '\0')
    {
      chocies[c] = CurrentString;
      ++c;
      CurrentString = "";
    }

    ++p;
  }
  

  // for(int s = 0; s < c; ++s) //We're going to offer you the choice of each thing in choices[]. 
  // {
  int s = 0;
  while(true)
  {
    DisplayText("<");
    for(int w = 0; w < c; ++w) //We're going to write each option in choices.
    {
      if(s == w)
      {
            DisplayText(" |");
            DisplayText(chocies[w]);
            DisplayText("| ");
      }
      else
      {
        DisplayText(chocies[w]);
      }
      // Keyboard.write(options[g][L]);
    }
    DisplayText("<");

    doWhatNow = AwaitInput(cycleSpeed);

    if(doWhatNow == Select1)
    {
      GridSelection =  chocies[s];

      CurrentMenu = "KeyboardMenu";
      CurrentSelection = "";
      s = c;
    }
    else if(doWhatNow == Back)
    {
       GridSelection = "";
       s = c;
    }

    ClearText("> ||<");
    for(int clearWord = 0; clearWord < c; ++clearWord) //We're going to clear
    {
      ClearText(chocies[clearWord]);
    }

    ++s;
    if(s == c)
      s = 0;

    if(doWhatNow == Select1 || doWhatNow == Back)
    {
      return GridSelection;
    }

    // for(int d = 0; d < 11 ; ++d)
    // {
    //   Keyboard.write(KEY_BACKSPACE);
    // }


  }
}

Commands AwaitInput(int FramesToWait)
{
  delay(50);
  bool hasBeenFalse = false; //Has this ever been false? TO prevent button from being held accidentily as part of last press.

  int i = 0;
  while (i < FramesToWait)
  {
    if(digitalRead(PrimaryInput) == LOW) //No Input
    {
      ++i;
      delay(1);
      digitalWrite(LED_BUILTIN, LOW);

      hasBeenFalse = true;

      CycleSpeedOptions();
    }
    else if(digitalRead(PrimaryInput) == HIGH && hasBeenFalse) 
    {
      Serial.print("TouchStarted:"); Serial.println(FramesToWait);
      int t = 0;

      digitalWrite(LED_BUILTIN, HIGH);
      while(t <= GoBackHoldTime && digitalRead(PrimaryInput) == HIGH)
      {
        fillOverTime(PendingColor, t, GoBackHoldTime);
        t += 1;
        delay(1);
      }
      digitalWrite(LED_BUILTIN, LOW);

      strip.clear();
      strip.show();

      if(t >= GoBackHoldTime)
      {
        DisplayText(" --Back-- ");
        colorWipe(GoBackColor, 25); 
        strip.show();
        Serial.print("Back:"); Serial.println(t);
        delay(500);
        ClearText(" --Back-- ");
        
        strip.clear();
        strip.show();
        return Back;
      }
      else if (t >= inputDelay)
      {
        DisplayText(" --Selected-- ");
        DisplayText(String(t));
        colorWipe(ConfirmColor, 25); 
        strip.show();
        Serial.print("Select:"); Serial.println(t);
        delay(500);
        ClearText(String(t));
        ClearText(" --Selected-- ");

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
  GoBackHoldTime -= 250;
  String message = String("  --  Hold Time for Back: " + String(GoBackHoldTime));
  DisplayText(message);
  delay(2000);
  ClearText(message);
  //Serial.println(cycleSpeed);
}

void IncreaseDelay()
{
  GoBackHoldTime += 500;
  String message = String("  --  Hold Time For Back: " + String(GoBackHoldTime));
  DisplayText(message);
  delay(2000);
  ClearText(message);
  //Serial.println(cycleSpeed);
}

void FasterCycleSpeed()
{
  cycleSpeed *= 0.9;
  String message = String("  --  CycleSpeed: " + String(cycleSpeed));
  DisplayText(message);
  delay(2000);
  ClearText(message);
}

void SlowerCycleSpeed()
{
  cycleSpeed *= 1.2;
  String message = String("  --  CycleSpeed: " + String(cycleSpeed));
  DisplayText(message);
  delay(2000);
  ClearText(message);
}

void CycleSpeedOptions()
{
  if(digitalRead(SeccondInput))
  {
    int t = 0;

    digitalWrite(LED_BUILTIN, HIGH);
    while(digitalRead(SeccondInput) == HIGH)
    {
      t += 1;
      delay(1);
    }
    digitalWrite(LED_BUILTIN, LOW);


    if(t >= 200)
    {
      GoBackHoldTime = t;
      String message = String("  --  Hold Time for Back: " + String(GoBackHoldTime));
      DisplayText(message);
      delay(2000);
      ClearText(message);
    }
  }
  else if(digitalRead(ThirdInput))
  {
    int t = 0;

    digitalWrite(LED_BUILTIN, HIGH);
    while(digitalRead(ThirdInput) == HIGH)
    {
      t += 1;
      delay(1);
    }
    digitalWrite(LED_BUILTIN, LOW);


    if(t >= 300)
    {
      cycleSpeed = t;
      String message = String("  --  CycleSpeed: " + String(cycleSpeed));
      DisplayText(message);
      delay(2000);
      ClearText(message);
    }
  }
}

void MouseFunctions()
{
  //doWhat = AwaitInput(1);

      float MouseSpeed = 4;
      int MoveDelay = 500;
      int Moves = 6;
      bool xDirection = false;

      float xMax = MouseSpeed * (float)cos(MouseTimer);
      float yMax = MouseSpeed * (float)sin(MouseTimer);

      int xMove = (round(xMax));
      int yMove = (round(yMax));

      Mouse.move(xMove, yMove);
      delay(70);

      int heldTime = 0;
      if(digitalRead(PrimaryInput))
      {

        while(digitalRead(PrimaryInput) && heldTime < MoveDelay)
        {
          ++heldTime;
          delay(1);
        }
        
        if(heldTime >= MoveDelay)
        {
          heldTime = 0;
          while(digitalRead(PrimaryInput))
          {
            Mouse.move(heldTime * 0.1 * xMove, heldTime * 0.1 * yMove);
            delay(50);
            ++heldTime;
            fillOverTime(PendingColor, heldTime, 100);
          }

          if(heldTime > 100)
          {
            CurrentMenu = "MainMenuContent";
          }
        }
        else
        {
          colorWipe(ConfirmColor, 25);
          Mouse.click(MOUSE_LEFT);
        }
      }

      if(!digitalRead(PrimaryInput))
      {
        MouseTimer += 0.1;
      }
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
  int numToFill = 1 + ((float)currentTime/(float)maxTime) * ((strip.numPixels() - 2));
  Serial.println(numToFill);

  strip.clear();
  for(int i=0; i<numToFill; i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  }                   //  Pause for a moment
  
  strip.show();                          //  Update strip to match       



}