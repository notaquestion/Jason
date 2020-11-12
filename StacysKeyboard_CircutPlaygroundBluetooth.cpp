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
#include <Adafruit_CircuitPlayground.h>
#include <BLE52_Mouse_and_Keyboard.h>

#include <avr/pgmspace.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    3

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 6

  // Declare our NeoPixel strip object:
Adafruit_CPlay_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
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
uint32_t MouseColor = strip.Color(50, 50, 255);

//INSTRUCTIONS: Hold 1.5s to go back 1 menu. Tap to select.


//int MenuNavigation[5]; //the int first position indicated which menu is open at depth 1, the int in seccond positon, at depth 2, and so on.

enum Commands {None, Back, Select1};

Commands NextCommand = None;

String CurrentMenu = "IdleMenu";
String CurrentSelection = "";

String MenuDepths[5];
int CurrentMenuDepth = 0;

String MainMenuContent[] = {
//  "MouseMenu",
  "KeyboardMenu",
  "StacyWordsMenu",
  // "StacyWordMenu",
 //"MouseMenuLinear",
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
const char Words_B[] PROGMEM = "BACK/BECAUSE/BRANDON/BURRITO/BIG FISH GAMES";
const char Words_C[] PROGMEM = "CAKE/CHEESEBURGER/CHERIE/CHEST/COME/COMEDY/COMPUTER/CONCERT/COOKIES/COULD";
const char Words_D[] PROGMEM = "DANIEL/DEONTE/DRAMA/DRINK STRAW";
const char Words_E[] PROGMEM = "EVEN";
const char Words_F[] PROGMEM = "FIND/FINGERS/FIRST/FRENCH FRIES/FROM/FUNNY";
const char Words_G[] PROGMEM = "GAMES/GINA/GIVE/GOOD/GOOD";
const char Words_H[] PROGMEM = "HAND/HAVE/HEAD/HIDDEN OBJECT/HIPS/HURT";
const char Words_I[] PROGMEM = "I WANT/I WANT TO/I WANT TO DO/I WANT TO GET/ICE CREAM SANDWICH/INTO";
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

void setup() 
{ // initialize the buttons' inputs:
  
  //  pinMode(PrimaryInput, INPUT);
  //  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  CircuitPlayground.begin();
  // initialize mouse control:

  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
   clock_prescale_set(clock_div_1);
  #endif

  Mouse.begin();
  Keyboard.begin();

  delay(6000);
  // colorWipe(CircuitPlayground.strip, PendingColor, 35);
  // delay(1000);
  // colorWipe(CircuitPlayground.strip, MouseColor, 35);
  // delay(1000);
  CircuitPlayground.clearPixels();
  delay(1000);
  int mw = 0;
  bool red = false;
  // CircuitPlayground.clearPixels();
  // colorWipe(CircuitPlayground.strip, MouseColor, 25);
  // delay(1000);
  // colorWipe(CircuitPlayground.strip, GoBackColor, 25);
  // delay(1000);
  while(!Mouse.isConnected()) 
  { 
    Serial.print("."); 
    delay(100);
    if(red)
    {
      colorWipe(CircuitPlayground.strip, MouseColor, 25);
    }
    else
    {
      colorWipe(CircuitPlayground.strip, GoBackColor, 25);
    }
    red = !red;

    ++mw;
    if(mw > 50) 
    {
      mw = 0; 
      Serial.println();
    }
  }

  // Serial.println("\nConnection successful");

  // END of Trinket-specific code.
  //
  //
  //  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  //  strip.show();            // Turn OFF all pixels ASAP
  //  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  //
  delay(3000);
  theaterChaseRainbow(CircuitPlayground.strip, 50);
  delay(500);
  CircuitPlayground.clearPixels();
  //  strip.clear();
  //  strip.show();

  // strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  // strip.show();            // Turn OFF all pixels ASAP
  // strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  //PopulateAutoCompleteDicOptions();
}

void loop() {
 
    Commands doWhat;

    
    
    delay(1);
    // Serial.println(CurrentMenu);
    // Serial.print("Looping: ");
    // Serial.println(CurrentMenu);


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
        CurrentMenu = "MouseMenu";
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
    // else if(CurrentMenu == "MouseMenuLinear"){
    //   MouseFunctionLinear();
    // }
}


int CurrentDisplayLength = 0;

void DisplayText (String text_)
{
  Serial.print("Going to print: ");
  Serial.println(text_);
  for(int c = 0; text_[c] != '\0'; ++c)
  {
    Keyboard.write(text_[c]);
    ++CurrentDisplayLength;
    delay(10);
  }
  Serial.println("Finsihed Print");
}

void ClearText (String text_)
{
  Serial.print("Clear Text: ");
  Serial.println(text_);
  for(int c = 0; text_[c] != '\0'; ++c)
  {
    Keyboard.write(KEY_BACKSPACE);
    delay(10);
  }
  Serial.print("Clear Text2 ");
  Serial.println(text_);
}

Commands DisplayMenuOptions(String menu_[])
{
  AddCurrsor();

  Commands doWhat;
  for(int M = 0; menu_[M] != "End"; ++M)
    {
      String selectionToDisplay = String(menu_[M]);
      
      Serial.print("Selection To Display1: ");
      Serial.println(selectionToDisplay);
      delay(100);

      DisplayText(selectionToDisplay);

      Serial.print("Selection To Display2: ");
      Serial.println(selectionToDisplay);

      // if(M < 6)
      // {
      //     strip.setPixelColor(M, PendingColor);         //  Set pixel's color (in RAM)
      //                      //  Pause for a moment
      // }

      doWhat = AwaitInput(cycleSpeed); //Will return true when input conditoin true, or false in 1000 frames.
      
      delay(100);
      Serial.print("Selection To Display3: ");
      Serial.println(selectionToDisplay);
      delay(100);

      ClearText(selectionToDisplay);

      Serial.print("Selection To Display4: ");
      Serial.println(selectionToDisplay);
      
      if(doWhat == Select1)
      {
        CurrentSelection = selectionToDisplay;

        Serial.print("Selection To Display5: ");
        Serial.println(selectionToDisplay);

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

String DispalyGridOptionsAndType(const char *options[], int size, char seperator, String goBackMenu)
{
  String selection = "";
    AddCurrsor();

    for(int g = 0; g < size; ++g)
    {

        String optionsString = RetriveString(*(options[g]));
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

bool TouchCondition()
{
  //Serial.println(CircuitPlayground.readCap(0));
  return CircuitPlayground.readCap(0) > 1000;
}

Commands AwaitInput(int FramesToWait)
{
  Serial.println("AwaitingInput");
  delay(50);
  bool hasBeenFalse = false; //Has this ever been false? TO prevent button from being held accidentily as part of last press.

  int i = 0;
  while (i < FramesToWait)
  {
    if(!TouchCondition()) //No Input
    {
      //Serial.println("NotTouching");

      ++i;
      delay(1);
      //digitalWrite(LED_BUILTIN, LOW);
      hasBeenFalse = true;
      CycleSpeedOptions();
      
    }
    else if(TouchCondition() && hasBeenFalse) 
    {
      //CircuitPlayground.playTone(50, 100, false);
      Serial.print("TouchStarted:"); Serial.println(FramesToWait);
      delay(100);
      int t = 0;

      //digitalWrite(LED_BUILTIN, HIGH);
      while(t <= GoBackHoldTime && TouchCondition())
      {
        fillOverTime(CircuitPlayground.strip, PendingColor, t, GoBackHoldTime);
        t += 1;
        delay(1);
      }
      Serial.println("Finished Touch");
      //digitalWrite(LED_BUILTIN, LOW);

      //CircuitPlayground.clearPixels();

      if(t >= GoBackHoldTime)
      {
        // //CircuitPlayground.playTone(50, 100, false);
        // Serial.println("Go Back");
        // DisplayText(" --Back-- ");
        // colorWipe(CircuitPlayground.strip, GoBackColor, 25); 
        // Serial.print("Back:"); Serial.println(t);
        // delay(500);
        // ClearText(" --Back-- ");
        
        // CircuitPlayground.clearPixels();
        return Back;
      }
      else if (t >= inputDelay)
      {
        // //CircuitPlayground.playTone(100, 100, false);
        // Serial.println("Select");

        // DisplayText(" --Selected-- ");
        // DisplayText(String(t));
        // colorWipe(CircuitPlayground.strip, ConfirmColor, 25); 
        // Serial.print("Select:"); Serial.println(t);
        // delay(500);
        // ClearText(String(t));
        // ClearText(" --Selected-- ");

        // CircuitPlayground.clearPixels();
        return Select1;
      }
      
    }

  }

  CircuitPlayground.clearPixels();
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
  //Serial.println("CyclingSpeedOptions");
  if(CircuitPlayground.leftButton())
  {
    int t = 0;

    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("CyclingSpeedOptions2");
    while(CircuitPlayground.leftButton())
    {
      t += 1;
      delay(1);
      Serial.println("CyclingSpeedOptions3");
    }
    digitalWrite(LED_BUILTIN, LOW);

  Serial.println("CyclingSpeedOptions4");
    if(t >= 200)
    {
      GoBackHoldTime = t;
      String message = String("  --  Hold Time for Back: " + String(GoBackHoldTime));
      DisplayText(message);
      delay(2000);
      ClearText(message);
    }
  }
  else if(CircuitPlayground.rightButton())
  {
    int t = 0;

    digitalWrite(LED_BUILTIN, HIGH);
    while(CircuitPlayground.rightButton())
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

      for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
        strip.setPixelColor(i, MouseColor);         //  Set pixel's color (in RAM)
      }
        strip.show();                          //  Update strip to match

      int CircleSpeed = 200;
      float CircleSize = 6;
      int MoveDelay = 500;
      bool xDirection = false;

      float xMax = CircleSize * (float)cos(MouseTimer);
      float yMax = CircleSize * (float)sin(MouseTimer);

      int xMove = (round(xMax));
      int yMove = (round(yMax));


      //Looks incrimentally cleaner than just moving
      for(int m = 0; m < 3; ++m)
      {
        if(m < 2)
        {
          Mouse.move(xMove/3, yMove/3);
          //Serial.print(xMove/3); Serial.print("\t"); Serial.println(yMove/3);
        }
        else
        {
          Mouse.move(xMove/3 + xMove % 3, yMove/3 + yMove%3);
          //Serial.print(xMove/3 + xMove % 3); Serial.print("\t"); Serial.println(yMove/3 + yMOve % 3);
        }

        //delay(10);
        int persistanceOfVisionDelay = 50;
        for(int d = 0; d < persistanceOfVisionDelay; ++d)
        {
          if(TouchCondition() || CircuitPlayground.leftButton() || CircuitPlayground.rightButton())
          {
            d = persistanceOfVisionDelay; 
            m = 3;
          }
          else
          {
            delay(1);
          }
        }
      }

      //Mouse.move(xMove, yMove);

      
      delay(CircleSpeed);

      int heldTime = 0;
      if(TouchCondition())
      {

        while(TouchCondition() && heldTime < MoveDelay)
        {
          ++heldTime;
          delay(1);
        }
        
        if(heldTime >= MoveDelay) //Should we start moving or Click?
        {
          heldTime = 0;
          while(TouchCondition())
          {
            Mouse.move(heldTime * 0.1 * xMove, heldTime * 0.1 * yMove);
            delay(100);
            ++heldTime;
            fillOverTime(CircuitPlayground.strip, PendingColor, heldTime, 60);
          }
          strip.clear();
          strip.show();

          if(heldTime > 60)
          {
            CurrentMenu = "MainMenuContent";
          }
        }
        else
        {
          colorWipe(CircuitPlayground.strip, ConfirmColor, 25);
          Mouse.click(MOUSE_LEFT);
        }
      }

      if(!TouchCondition())
      {
        MouseTimer += 0.1;


      }
}



char buffer[100];
String RetriveString(const char *StoredString[])
{
  strcpy_P(buffer, (char *)pgm_read_word(StoredString));  // Necessary casts and dereferencing, just copy.
  Serial.print("Retreived string");
  Serial.println(buffer);
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
    // DispalyGridOptionsAndType(autoCompleteOptions, 1, '/', "KeyboardMenu");
  }

  return "";
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(Adafruit_CPlay_NeoPixel strip_, uint32_t color, int wait) {
  // delay(100);
  // Serial.println("Going to color wipe");
  // delay(100);
  // strip_.clear();

  // strip_.setPixelColor(9, PendingColor);
  // strip_.show();
  // delay(1000);
  // strip_.clear();
  // strip_.setPixelColor(9, GoBackColor);
  // strip_.show();
  // delay(1000);
  // strip_.clear();

  // for(int j = 0; j < strip_.numPixels(); j++) { // For each pixel in strip...
  //   if(j != 0 && j != 1)
  //   {
  //     Serial.println(strip_.numPixels());
  //     //Doing these two to the CP strip causes arduino to hang and hang serial monitor for some reason?
  //     strip_.setPixelColor(strip_.numPixels(), color);         //  Set pixel's color (in RAM)
  //     //CircuitPlayground.setPixelColor(i, color);
  //     delay(wait);                           //  Pause for a moment
  //     strip_.show();                          //  Update strip to match
  //   }


  // }
  // //CircuitPlayground.clearPixels();
}

void theaterChaseRainbow(Adafruit_CPlay_NeoPixel strip_, int wait) {
  // int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  // for(int a=0; a<30; a++) {  // Repeat 30 times...
  //   for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
  //     strip_.clear();         //   Set all pixels in RAM to 0 (off)
  //     // 'c' counts up from 'b' to end of strip_ in increments of 3...
  //     for(int c=b; c<strip_.numPixels(); c += 3) {
  //       // hue of pixel 'c' is offset by an amount to make one full
  //       // revolution of the color wheel (range 65536) along the length
  //       // of the strip_ (strip_.numPixels() steps):
  //       int      hue   = firstPixelHue + c * 65536L / strip_.numPixels();
  //       uint32_t color = strip_.gamma32(strip_.ColorHSV(hue)); // hue -> RGB
  //       strip_.setPixelColor(c, color); // Set pixel 'c' to value 'color'
  //     }
  //     strip_.show();                // Update strip_ with new contents
  //     delay(wait);                 // Pause for a moment
  //     firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
  //   }
  // }
}

void fillOverTime(Adafruit_CPlay_NeoPixel strip, uint32_t color, int currentTime, int maxTime)
{
  // int numToFill = 1 + ((float)currentTime/(float)maxTime) * ((strip.numPixels() - 1));
  // //Serial.println(numToFill);

  // strip.clear();
  // for(int i = 0; i < numToFill; i++) { // For each pixel in strip...
  //   strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  // }                   //  Pause for a moment
  
  // strip.show();                          //  Update strip to match       
}