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

#include <avr/pgmspace.h>

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
  "StacyWordsMenu",
  // "StacyWordMenu",
  "MouseMenu",
  "MouseMenuLinear",
  "OptionsMenu",
  "End"
};

    String KeyboardMenu[] = {
      "Type Letter",
      "BackSpace",
      "AutoComplete",
      "SpecialKeyMenu",
      "End"
    };

      String SpecialKeyMenu[] = {
        "Enter",
        "Tab",
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
//   ".",//   ",",//   "?",//   "!", //   "@",//   "#",//   "$",//   "%",//   "^",//   "&",//   "*",//   "(",//   ")",//   "-",//   "_",//   "+",//   "=",//   "/", //   "End"
// };

// //Long press to go back should be disabled in mouse menu
// String MouseMenu[] = {//   "Same As Last",//   "Back Menu",//   "Up",//   "Down",//   "Left",//   "Right",//   "L-Click",//   "R-Click",//   "M-Click",//   "Scroll Up",//   "Scroll Down",//   "Sensitivity Up",//   "Sensitivikty Down",//   "End"
// }; 

//eotha sinrd luymw fgcbp kvjqxz

// String LetterClumps[7] = {
//   {"_/E/T/S/D/W"},
//   {"O/H/I/L/F/K"},
//   {"A/N/U/G/V/Z"},
//   {"R/Y/C/J/1/2"},
//   {"M/B/Q/3/4/5"},
//   {"P/X/6/7/8/9"},
//   {"./?/,/+/!"}
// };

const char LetterClumps_1[] PROGMEM = "_/E/T/S/D/W";
const char LetterClumps_2[] PROGMEM = "O/H/I/L/F/K";
const char LetterClumps_3[] PROGMEM = "A/N/U/G/V/Z";
const char LetterClumps_4[] PROGMEM = "R/Y/C/J/1/2";
const char LetterClumps_5[] PROGMEM = "M/B/Q/3/4/5";
const char LetterClumps_6[] PROGMEM = "P/X/6/7/8/9";
const char LetterClumps_7[] PROGMEM = "./?/,/+/!";
const char *const LetterClumps[] PROGMEM = {LetterClumps_1, LetterClumps_2, LetterClumps_3, LetterClumps_4, LetterClumps_5, LetterClumps_6, LetterClumps_7};
const int LetterClumps_SIZE = 7;



    String StacyWordsMenu[] = {
      "Words_Verbs",
      "Words_Things",
      "Words_Fun",
      "Words_Foods",
      "Words_Body",
      "Words_Adjectives",
      "Words_People",
      "End"
    };
//

String CurrentTypedWord = "";
String AutoCompleteOptions = "";

const char Words_A[] PROGMEM = "ABOUT/ACTION/ADVENTURE CAPATALIST/AFTER/ALSO/AMANDA";
const char Words_B[] PROGMEM = "BACK/BECAUSE/BRANDON/BURRITO";
const char Words_C[] PROGMEM = "CAKE/CHEESEBURGER/CHERIE/CHEST/COME/COMEDY/COMPUTER/CONCERT/COOKIES/COULD";
const char Words_D[] PROGMEM = "DANIEL/DEONTE/DRAMA/DRINK STRAW";
const char Words_E[] PROGMEM = "EVEN";
const char Words_F[] PROGMEM = "FIND/FINGERS/FIRST/FRENCH FRIES/FROM/FUNNY";
const char Words_G[] PROGMEM = "GAMES/GINA/GIVE/GOOD/GOOD";
const char Words_H[] PROGMEM = "HAND/HAVE/HEAD/HIDDEN OBJECT/HIPS/HURT";
const char Words_I[] PROGMEM = "ICE CREAM SANDWICH/INTO";
const char Words_J[] PROGMEM = "JASON/JIGWORDS/JOSE/JUST";
const char Words_K[] PROGMEM = "KNOW";
const char Words_L[] PROGMEM = "LASAGNA/LEMONADE WATER/LETTERS/LIKE/LIKE/LOOK/LOVE";
const char Words_M[] PROGMEM = "MACARONI/MAKE/MARCY/MICHAEL/MIKE/MONTI/MOST/MOVIE";
const char Words_N[] PROGMEM = "NECK/NICK";
const char Words_O[] PROGMEM = "ONLY/OTHER/OVER";
const char Words_P[] PROGMEM = "PAINFUL/PAUL/PEOPLE/PILLOW/PINEAPPLE WATER/PLAY";
const char Words_S[] PROGMEM = "SHEET/SOME/STEAM";
const char Words_T[] PROGMEM = "TAKE/TANGELO/TANGELO/THAN/THAT/THEIR/THEM/THEN/THERE/THESE/THEY/THINK/THIS/TIME/TIRED/TOES/TURN/TV SERIES";
const char Words_W[] PROGMEM = "WANT/WANT/WELL/WENT/WHAT/WHEELCHAIR/WHEN/WHICH/WILL/WITH/WORDS/WORK/WOULD/WRITE";
const char Words_Y[] PROGMEM = "YEAR/YOUR";

const char *const AutoSuggestDic[] PROGMEM = {Words_A, Words_B, Words_C, Words_D, Words_E, Words_F, Words_G, Words_H, Words_I, Words_J, Words_K, Words_L, Words_M, Words_N, Words_O, Words_P, Words_S, Words_T, Words_W, Words_Y, };
const int AutoSuggestDic_SIZE = 20;



const char Words_Verbs_0[] PROGMEM =  "Want/Went/Saw/Find/Turn/Go";
const char Words_Verbs_1[] PROGMEM =  "Like/Love/Hurt/Play/Write";
const char *const Words_Verbs[] PROGMEM = {Words_Verbs_0, Words_Verbs_1};
const int Words_Verbs_SIZE = 2;


const char Words_Things_0[] PROGMEM =  "Wheelchair/Bed/Sheet/Pillow/Van";
const char *const Words_Things[] PROGMEM = {Words_Things_0};
const int Words_Things_SIZE = 1;


const char Words_Fun_0[] PROGMEM =  "Games/Tangelo/Steam/Computer/Movie";
const char Words_Fun_1[] PROGMEM =  "Concert/Drama/Action/Comedy/TV series";
const char Words_Fun_2[] PROGMEM =  "Tangelo/Letters/Words";
const char Words_Fun_3[] PROGMEM =  "Adventure Capatalist/Hidden Object/Jigwords";
const char *const Words_Fun[] PROGMEM = {Words_Fun_0, Words_Fun_1, Words_Fun_2, Words_Fun_3};
const int Words_Fun_SIZE = 4;


const char Words_Foods_0[] PROGMEM =  "Macaroni/Lasagna/Cheeseburger/French fries";
const char Words_Foods_1[] PROGMEM =  "Burrito/Cake/Ice cream sandwich/Cookies";
const char Words_Foods_2[] PROGMEM =  "Drink straw/Lemonade water/Pineapple water";
const char *const Words_Foods[] PROGMEM = {Words_Foods_0, Words_Foods_1, Words_Foods_2};
const int Words_Foods_SIZE = 3;


const char Words_Body_0[] PROGMEM =  "Arm/Fingers/Hand/Leg/Toes";
const char Words_Body_1[] PROGMEM =  "Hips/Chest/Head/Neck";
const char *const Words_Body[] PROGMEM = {Words_Body_0, Words_Body_1};
const int Words_Body_SIZE = 2;


const char Words_Adjectives_0[] PROGMEM =  "Bad/Painful/Funny/Tired/New";
const char *const Words_Adjectives[] PROGMEM = {Words_Adjectives_0};
const int Words_Adjectives_SIZE = 1;


const char Words_People_0[] PROGMEM =  "Mom/Dad/Nick/Gina/Jason/Daniel";
const char Words_People_1[] PROGMEM =  "Michael/Paul/Deonte/Amanda/Jose";
const char Words_People_2[] PROGMEM =  "Cherie/Monti/Mike/Marcy/Brandon";
const char *const Words_People[] PROGMEM = {Words_People_0, Words_People_1, Words_People_2};
const int Words_People_SIZE = 3;


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

  //PopulateAutoCompleteDicOptions();
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

              CurrentTypedWord.remove(CurrentTypedWord.length() - 1);
              PopulateAutoCompleteDicOptions();
            }
            else if(CurrentSelection == "AutoComplete")
            {
              if(AutoCompleteOptions.length() > 0)
              {
                String typeWhat = ParseStringAndPresentOptions(AutoCompleteOptions, '/');

                if(typeWhat == "")
                {
                  CurrentMenu = "KeyboardMenu";
                  CurrentSelection = "";
                }
                else
                {
                  ClearText(CurrentTypedWord);
                  DisplayText(typeWhat);
                  DisplayText(" ");
                  AutoCompleteOptions = "";
                  CurrentTypedWord = "";
                }
                CurrentSelection = "";
              }
              else
              {
                CurrentMenu = "KeyboardMenu";
                CurrentSelection = "";
              }
            }
            
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
      String TypeWhat = DispalyGridOptionsAndType(LetterClumps, LetterClumps_SIZE, '/', "KeyboardMenu");

      if(TypeWhat == "_")
      {
        TypeWhat = " ";
        CurrentTypedWord = "";
        AutoCompleteOptions = "";
      }

      if(TypeWhat != "")
      {
        if(TypeWhat != " ")
        {
          CurrentTypedWord += TypeWhat;
        }
        DisplayText(TypeWhat);

        // DisplayText("       CurrentTypedWord:");
        // DisplayText(CurrentTypedWord);
        // delay(1000);
        // ClearText("       CurrentTypedWord:");
        // ClearText(CurrentTypedWord);


        CurrentMenu = "KeyboardMenu";

        PopulateAutoCompleteDicOptions();
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
    else if(CurrentMenu == "StacyWordsMenu")
    {
      doWhat = DisplayMenuOptions(StacyWordsMenu);

      if(doWhat == Back)
      {
        CurrentMenu = "MainMenuContent";
      }
      else if(doWhat == Select1)
      {
        CurrentMenu = CurrentSelection;
        CurrentSelection = "";
      }
    }
      else if(CurrentMenu == "Words_Verbs") {
        String TypeWhat = DispalyGridOptionsAndType(Words_Verbs, Words_Verbs_SIZE, '/', "StacyWordsMenu");

        if(TypeWhat != "")
        {
          DisplayText(TypeWhat);
          CurrentMenu = "MainMenuContent";
        } 
      }
      else if(CurrentMenu == "Words_Things") {
        String TypeWhat = DispalyGridOptionsAndType(Words_Things, Words_Things_SIZE, '/', "StacyWordsMenu");

        if(TypeWhat != "")
        {
          DisplayText(TypeWhat);
          CurrentMenu = "MainMenuContent";
        }
      }
      else if(CurrentMenu == "Words_Fun") {
        String TypeWhat = DispalyGridOptionsAndType(Words_Fun, Words_Fun_SIZE, '/', "StacyWordsMenu");

        if(TypeWhat != "")
        {
          DisplayText(TypeWhat);
          CurrentMenu = "MainMenuContent";
        }
      }
      else if(CurrentMenu == "Words_Foods") {
        String TypeWhat = DispalyGridOptionsAndType(Words_Foods, Words_Foods_SIZE, '/', "StacyWordsMenu");

        if(TypeWhat != "")
        {
          DisplayText(TypeWhat);
          CurrentMenu = "MainMenuContent";
        }
      }
      else if(CurrentMenu == "Words_Body") {
        String TypeWhat = DispalyGridOptionsAndType(Words_Body, Words_Body_SIZE, '/', "StacyWordsMenu");

        if(TypeWhat != "")
        {
          DisplayText(TypeWhat);
          CurrentMenu = "MainMenuContent";
        }
      }
      else if(CurrentMenu == "Words_Adjectives") {
        String TypeWhat = DispalyGridOptionsAndType(Words_Adjectives, Words_Adjectives_SIZE, '/', "StacyWordsMenu");

        if(TypeWhat != "")
        {
          DisplayText(TypeWhat);
          CurrentMenu = "MainMenuContent";
        }
      }
      else if(CurrentMenu == "Words_People") {
        String TypeWhat = DispalyGridOptionsAndType(Words_People, Words_People_SIZE, '/', "StacyWordsMenu");

        if(TypeWhat != "")
        {
          DisplayText(TypeWhat);
          CurrentMenu = "MainMenuContent";
        }
      }
    else if(CurrentMenu == "MouseMenu"){
      MouseFunctions();
    }
    else if(CurrentMenu == "MouseMenuLinear"){
      MouseFunctionLinear();
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

String DispalyGridOptionsAndType(const char* options[], int size, char seperator, String goBackMenu)
{
  String selection = "";
    AddCurrsor();

    for(int g = 0; g < size; ++g)
    {

        String optionsString = RetriveString(&(options[g]));
        DisplayText(" >");
        DisplayText(optionsString);
        DisplayText("<");

        Commands doWhatNow = AwaitInput(cycleSpeed);
        
        ClearText(" >");
        ClearText(optionsString);
        ClearText("<");

        if(doWhatNow == Select1)
        {
            selection = ParseStringAndPresentOptions(optionsString, seperator);
           
           if(selection == "")
           {
            //We wanted to go back.
            g = -1; //This'll become 0 in the next go round.
           }
           else
           {
            g = size ;
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

        if(g == (size - 1))
        {
          g = -1;
        }
    }
    RemoveCursor();

    if(selection.length() > 1)
    {
      selection = " " + selection + " ";
    }

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
    DisplayText(">");
    for(int w = 0; w < c; ++w) //We're going to write each option in choices.
    {
      if(s == w)
      {
            DisplayText(" |");
            DisplayText(chocies[w]);
            DisplayText("| /");
      }
      else
      {
        DisplayText(chocies[w]);
        DisplayText("/");
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

    ClearText("> || <");
    for(int clearWord = 0; clearWord < c; ++clearWord) //We're going to clear
    {
      ClearText(chocies[clearWord]);
      // if(clearWord != s)
      // {
        ClearText("/");
      // }
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

      int CircleSpeed = 300;
      float CircleSize = 10;
      int MoveDelay = 500;
      bool xDirection = false;

      float xMax = CircleSize * (float)cos(MouseTimer);
      float yMax = CircleSize * (float)sin(MouseTimer);

      int xMove = (round(xMax));
      int yMove = (round(yMax));

      Mouse.move(xMove, yMove);
      delay(CircleSpeed);

      int heldTime = 0;
      if(digitalRead(PrimaryInput))
      {

        while(digitalRead(PrimaryInput) && heldTime < MoveDelay)
        {
          ++heldTime;
          delay(1);
        }
        
        if(heldTime >= MoveDelay) //Should we start moving or Click?
        {
          heldTime = 0;
          while(digitalRead(PrimaryInput))
          {
            Mouse.move(heldTime * 0.1 * xMove, heldTime * 0.1 * yMove);
            delay(100);
            ++heldTime;
            fillOverTime(PendingColor, heldTime, 1000);
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

float MouseMoveIteration = 0.0;
void MouseFunctionLinear()
{
    int CrossSpeed = 30;
    float CrossSize = 70;
    int MoveDelay = 500;
    bool xDirection = false;

    int xMove = 0;
    int yMove = 0;

    if(MouseMoveIteration >= 0 && MouseMoveIteration < CrossSize)
    {
      xMove = 1; yMove = 0;
    }
    else if( MouseMoveIteration > CrossSize && MouseMoveIteration < (2 * CrossSize))
    {
      xMove = -1; yMove = 0;
    }
    else if( MouseMoveIteration > (2 * CrossSize) && MouseMoveIteration < (3 * CrossSize))
    {
      xMove = 0; yMove = 1;
    }
    else if( MouseMoveIteration > (3 * CrossSize) && MouseMoveIteration < (4 * CrossSize))
    {
      xMove = 0; yMove = -1;
    }

    MouseMoveIteration += 1.0;
    if(MouseMoveIteration >= ( 4 * CrossSize))
    {
      MouseMoveIteration = 0;
    }

    Mouse.move(xMove, yMove);
    delay(CrossSpeed);

    int heldTime = 0;
    if(digitalRead(PrimaryInput))
    {

      while(digitalRead(PrimaryInput) && heldTime < MoveDelay)
      {
        ++heldTime;
        delay(1);
      }
      
      if(heldTime >= MoveDelay) //Should we start moving or Click?
      {
        heldTime = 0;
        while(digitalRead(PrimaryInput))
        {
          Mouse.move(heldTime * 0.3 * xMove, heldTime * 0.3 * yMove);
          delay(50);
          ++heldTime;
          fillOverTime(PendingColor, heldTime, 1000);
        }

        if(heldTime > 1000)
        {
          CurrentMenu = "MainMenuContent";
        }
        MouseMoveIteration = 0;
      }
      else
      {
        colorWipe(ConfirmColor, 25);
        Mouse.click(MOUSE_LEFT);
        MouseMoveIteration = 0;
      }
    }

    if(!digitalRead(PrimaryInput))
    {
      MouseTimer += 0.1;
    }
}

char buffer[100];
String RetriveString(const char* StoredString[])
{
  strcpy_P(buffer, (char *)pgm_read_word(StoredString));  // Necessary casts and dereferencing, just copy.
  return buffer;
}


String PopulateAutoCompleteDicOptions()
{
  AutoCompleteOptions = "";
  if(CurrentTypedWord.length() > 0)
  {
    bool flag = false;
    char WordsStartingWith[105] = {0};
    for(int a = 0; a < AutoSuggestDic_SIZE; ++a)
    {
      strcpy_P(WordsStartingWith, (char *)pgm_read_word(&(AutoSuggestDic[a])));

      if(WordsStartingWith[0] == CurrentTypedWord[0])
      {
        a = AutoSuggestDic_SIZE;
        flag = true;
      }
    }
    int MaxAutocompleteLength = 5;
    int AutoCompleteLength = 0;
    int W = 0;
    String currentWord;
    int currentWordPlace = 0;
    if(flag = true)
    {
      for (int i = 0; i < 105; ++i)
      {
        //delay(100);
        /* code */
        char newLetter = WordsStartingWith[i];
        if(newLetter != 0)
        {
          if ('/' == newLetter)
          {
            if(currentWord.length() > 0)
            {
              //DisplayText(currentWord);
              AutoCompleteOptions += currentWord;
              AutoCompleteOptions += '/';
              currentWord = "";
              currentWordPlace = 0;
              ++AutoCompleteLength;

              if(AutoCompleteLength >= MaxAutocompleteLength)
              {
                i = 105;
              }
            }
          }
          else if(newLetter == CurrentTypedWord[currentWordPlace] || currentWordPlace >= CurrentTypedWord.length())
          {
            currentWord += newLetter;
            ++currentWordPlace;
            // DisplayText(currentWord);
            // DisplayText(" - ");
          }
          else
          {
            currentWord = "";
            currentWordPlace = 0;
          }
        }
        else
        {
          i = 105;
        }
      }
    }
    if(AutoCompleteOptions.length() > 0)
    {
      AutoCompleteOptions.remove(AutoCompleteOptions.length() - 1, 1);
      DisplayText(" --- Auto Complete --- ");
      DisplayText(AutoCompleteOptions);
      Commands doWhatNow = AwaitInput(cycleSpeed);
      ClearText(AutoCompleteOptions);
      ClearText(" --- Auto Complete --- ");

      if(doWhatNow == Select1)
      {
        String typeWhat = ParseStringAndPresentOptions(AutoCompleteOptions, '/');

        if(typeWhat == "")
        {
          CurrentMenu = "KeyboardMenu";
        }
        else
        {
          ClearText(CurrentTypedWord);
          DisplayText(typeWhat);
          DisplayText(" ");
          AutoCompleteOptions = "";
          CurrentTypedWord = "";
        }
      }

    }

    //ParseStringAndPresentOptions(AutoCompleteOptions, '/');
    //DispalyGridOptionsAndType(autoCompleteOptions, 1, '/', "KeyboardMenu");
  }

  return "";
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