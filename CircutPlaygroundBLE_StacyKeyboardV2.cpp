#include <Adafruit_CircuitPlayground.h>
//#include <BLE52_Mouse_and_Keyboard.h> // This one litteraly breaks the Capacative touch. I have no idea why. Can't find anyone talking about it online. 
#include <TinyUSB_Mouse_and_Keyboard.h>

#include <avr/pgmspace.h>


///////////////////////GLOBAL VARIABLES///////////////////////
int CycleSpeed = 1000; //Delay befor showing a new clickable option.
int GoBackHoldTime = 1500; //Amount of time user must hold input to peform Back Command in AwaitInput.
int InputDelay = 5; //Delay before a new Input can be started after showing a new option.

float MouseTimer = 0.0;  //Timer used to determine mouse roation and angle in MouseFunctions();

enum Commands {None, Back, Select1}; //The commands we can issue with our hardware. Usually returned by AwaitInput()
Commands NextCommand = None; //What to do next.

//The old way, BAD.
//String CurrentMenu = "IdleMenu"; //The menu we're in, should be replaced by an enum.
//String CurrentSelection = ""; //A string sometimes representing the next menu and sometimes representing a function we want to perform.

///////////////////////MENU STUFF///////////////////////
enum MenuTree {
  NoMenu,
  IdleMenu, //This isn't really a menu, more of a waiting state.
  MainMenu, //Our first true Menu.
    KeyboardMenu,
      TypeLetter, //This one's weird, we cick it over to a different system.
      Backspace, //A function, not a menu. If this get's selected, do the function and return to previous menu.
      AutoComplete, //Weird like type letter.
      SpecialKeyMenu,
        Enter, //These are functions, press that special key.
        Tab,
    OptionsMenu,
      FasterCycleSpeed,
      LongerCycleSpeed,
      ShorterGoBackInput,
      LongerGoBackInput,
  MouseMenu
};

MenuTree CurrentMenu = IdleMenu;
MenuTree NextMenu = NoMenu;

static const char * MenuTreeAsStrings[] = {
  "None",
  "IdleMenu", 
  "MainMenu",
    "KeyboardMenu",
      "TypeLetter",
      "Backspace",
      "AutoComplete",
      "SpecialKeyMenu",
        "Enter",
        "Tab",
    "OptionsMenu",
      "FasterCycleSpeed",
      "LongerCycleSpeed",
      "ShorterGoBackInput",
      "LongerGoBackInput",
  "MouseMenu"
};

String MenuTreeToString(MenuTree menu)
{
  return MenuTreeAsStrings[menu];
}

//What Menus have sub menus? What MenuTree entries are subsidiary to each?
MenuTree M_MainMenu[] = { //Can't just be called MainMenu w/o compiler thinking I meant the enum element.
  KeyboardMenu,
  OptionsMenu,
  NoMenu //Serves as a terminatior when itterating through array.
};
MenuTree M_OptionsMenu[] = {
  FasterCycleSpeed,
  LongerCycleSpeed,
  ShorterGoBackInput,
  LongerGoBackInput,
  NoMenu //Serves as a terminatior when itterating through array.
};
MenuTree M_KeyboardMenu[] = {
  TypeLetter,
  Backspace,
  AutoComplete,
  SpecialKeyMenu,
  NoMenu //Serves as a terminatior when itterating through array.
};
MenuTree M_SpecialKeyMenu[] = {
  Enter,
  Tab,
  NoMenu //Serves as a terminatior when itterating through array.
};





///////////////////////COLORS///////////////////////
uint32_t ColorWhite = 0xFFFFFF;
uint32_t ColorRed = 0xFF0000;
uint32_t ColorYellow = 0x808000;
uint32_t ColorGreen = 0x00FF00;
uint32_t ColorTeal = 0x008080;
uint32_t ColorBlue = 0x0000FF;

uint32_t StartupColor = ColorTeal;
uint32_t PendingColor = ColorWhite;
uint32_t ConfirmColor = ColorGreen;
uint32_t GoBackColor  = ColorRed;
uint32_t MouseColor   = ColorBlue;



///////////////////////TYPE LETTER STORAGE///////////////////////
const char LetterClumps_1[] PROGMEM = "_/E/T/S/D/W";
const char LetterClumps_2[] PROGMEM = "O/H/I/L/F/K";
const char LetterClumps_3[] PROGMEM = "A/N/U/G/V/Z";
const char LetterClumps_4[] PROGMEM = "R/Y/C/J/1/2";
const char LetterClumps_5[] PROGMEM = "M/B/Q/3/4/5";
const char LetterClumps_6[] PROGMEM = "P/X/6/7/8/9";
const char LetterClumps_7[] PROGMEM = "./?/,/+/!";
const char *const LetterClumps[] PROGMEM = {LetterClumps_1, LetterClumps_2, LetterClumps_3, LetterClumps_4, LetterClumps_5, LetterClumps_6, LetterClumps_7};
const int LetterClumps_SIZE = 7;


///////////////////////FREQUENTLY USED WORD STORAGE///////////////////////
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


///////////////////////END///////////////////////

void setup() 
{ // initialize the buttons' inputs:
  
  Mouse.begin(); // I think these might sometimes cause the serial monitor to freeze the Circut Playground and IDE, not sure though.
  Keyboard.begin();

  //Some begining fuctions.
  Serial.begin(115200); //Please make sure your Baud rate is set in Serial Monitor to 115200.
  CircuitPlayground.begin();


  //Just an Everythings OK show.
  delay(1000);
  colorWipe(PendingColor, 35);
  delay(1000);
  CircuitPlayground.clearPixels();


  
  //Now that everythigns connected, rainbows!
  theaterChaseRainbow(20, 2000);
  CircuitPlayground.clearPixels();


  // for (int i = 0; i < 10000; ++i)
  // {
  //  fillOverTime(CircuitPlayground.strip, PendingColor, i, 10000);
  //   delay(1);
  // }
  // CircuitPlayground.clearPixels();

  Serial.println(MenuTreeToString(MainMenu));



}

char buffer[200];  // make sure this is large enough for the largest string it must hold
int bufferSize = 200;

void loop() {

  //NextCommand = AwaitInput(CycleSpeed);

  if(NextMenu != NoMenu)
  {
    CurrentMenu = NextMenu;
    NextMenu = NoMenu;
  }

  //Big state machine based off MenuTree enum. 
  switch(CurrentMenu) {
    
    case NoMenu:
        while(true)
        {
          Serial.println("Error: NO MENU?");
          delay(1);
        }
        break;

    case IdleMenu: 
        Commands input = AwaitInput(CycleSpeed);

        if(input == Select1)
        {
            DisplayText("Select Pressed, Hold to Use Mouse");
            delay(1000);
            ClearText("Select Pressed, Hold to Use Mouse");
        }
        else if(input == Back)
        {
            DisplayText("Going to Mouse Menu");
            delay(1000);
            ClearText("Going to Mouse Menu");

            CurrentMenu = MainMenu;
        }

        break;

    case MainMenu: 
        Commands input = DisplayMenuOptions(M_MainMenu);
        if(input == Back)
        {
          CurrentMenu = MouseMenu;
        }
        break;

      case KeyboardMenu:
          Commands input = DisplayMenuOptions(M_KeyboardMenu);
          if(input == Back)
          {
            CurrentMenu = MainMenu;
          }
          break;

        case TypeLetter:
            String TypeWhat = DispalyGridOptionsAndType(LetterClumps, LetterClumps_SIZE, '/', KeyboardMenu);
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
              PopulateAutoCompleteDicOptions();
              CurrentMenu = KeyboardMenu;
            }

            break;

        case Backspace: 
            Keyboard.write(KEY_BACKSPACE);
            CurrentTypedWord.remove(CurrentTypedWord.length() - 1);

            bool again = true;
            while(again)
            {
              DisplayText("<--REPEAT?--");
              Commands input = AwaitInput(cycleSpeed);

              if(input == None)
              {
                again = false;
              }
              else
              {
                ClearText("<--REPEAT?--");
                Keyboard.write(KEY_BACKSPACE);
                CurrentSelection = "";

                CurrentTypedWord.remove(CurrentTypedWord.length() - 1);
              }
              delay(1);

            }
            ClearText("<--REPEAT?--");

            CurrentMenu = KeyboardMenu;
            break;

        case SpecialKeyMenu:
            Commands input = DisplayMenuOptions(M_SpecialKeyMenu);
            if(input == Back)
            {
              CurrentMenu = KeyboardMenu;
            }
            break;

          case Enter: 
              Keyboard.write(KEY_RETURN);
              CurrentMenu = SpecialKeyMenu;
              break;

          case Tab:
              Keyboard.write(KEY_TAB);
              CurrentMenu = SpecialKeyMenu;
              break;

              //Keyboard.write(KEY_CAPS_LOCK);
              //Keyboard.write(KEY_ESC);

      case OptionsMenu:
          Commands input = DisplayMenuOptions(M_OptionsMenu);

          if(input == Back)
          {
            CurrentMenu = MainMenu;
          }
          break;

        case FasterCycleSpeed:
            IncreaseCycleSpeed();
            CurrentMenu = OptionsMenu;
            break;

        case LongerCycleSpeed:
            DecreaseCycleSpeed();
            CurrentMenu = OptionsMenu;
            break;

        case ShorterGoBackInput:
            ShortenDelay();
            CurrentMenu = OptionsMenu;
            break;

        case LongerGoBackInput:
            IncreaseDelay();
            CurrentMenu = OptionsMenu;
            break;

    case MouseMenu:
        MouseFunctions();
        break;

    default:
        Serial.println("This Current Menu doesnt exist.");
        break;
  }

}


//////////////////////////////MOUSE/////////////////////////////////////////
//This Function gets evaluated every frame. 
//When the input is not held, MouseTimer is incrimented.
//  //New Sin/Cos movment values are compute from MouseTimer such that the mouse moves in a circle.
//When the input starts, we determine if a hold or a press is occuring.
//  //If a press is occuring, click.
//  //If a hold is occuring, continue moving at your current vector, i.e. a tangent to the circle. (Gotta see it in action)
//
void MouseFunctions()
{
  //doWhat = AwaitInput(1);

      //Base Color
      for(int i=0; i < 10; i++) { // For each pixel in strip...
        CircuitPlayground.setPixelColor(i, MouseColor);         //  Set pixel's color (in RAM)
      }                        

      //MOUSE SETTINGS
      //int CircleSpeed = 200; // Delay between each frame of the rotating circle, lower value, faster movement.
      int CircleSpeed = 10; 
      float CircleSize = 6; // Circle Radius
      
      int MoveDelay = 500; // Hold time/delay before mouse starts moving instead of clicking.
      
      int MouseLinearSpeed = 100; //The Delay between each frame of the mouse moving in a line. Longer is slower.
      int HoldToGoBackLength = 60; //How many MouseLinearSpeed delays we should wait before going back to the MainMenu from this Mouse Menu
      
      //INTERNAL
      bool xDirection = false;
      float xMax = CircleSize * (float)cos(MouseTimer);
      float yMax = CircleSize * (float)sin(MouseTimer);

      int xMove = (round(xMax));
      int yMove = (round(yMax));


      //MOVE THE MOUSE
      //Uses a special 3rds technique to make the circle look a little more circular and less like discrete chunks. 
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

        int persistanceOfVisionDelay = 50; //Delay stuff just a little to make it look like a continuous motion.
        for(int d = 0; d < persistanceOfVisionDelay; ++d)
        {
          if(TouchCondition())
          {
            d = persistanceOfVisionDelay; //Break out
            m = 3;
          }
          else
          {
            delay(1);
          }
        }
      }

      delay(CircleSpeed); //Here we have the real option to slow down/spead up the circle. 

      //Clicking and moving the mouse in a straight line.
      int heldTime = 0;
      if(TouchCondition())
      {
        CircuitPlayground.playTone(50, 100, false);

        //While the user TouchCondition(), everything stops, at least for MoveDelay amount of time. Longer than move dealy would be a HOLD action.
        while(TouchCondition() && heldTime < MoveDelay)
        {
          ++heldTime;
          delay(1);
        }
        
        //Are we holding the TouchCondition for greater than MoveDelay? If so, we're holding and we should move the mouse!
        if(heldTime >= MoveDelay) 
        {
          CircuitPlayground.playTone(50, 100, false);
          
          heldTime = 0; //Lets use this again.

          //As long as we're still holding this. Move, and accelerate our speed, and fill our HeldTime/fill over time meter.
          //If heldTime Goes over HoldToGoBackLength, we go back to typing Menu.
          while(TouchCondition()) 
          {
            Mouse.move(heldTime * 0.1 * xMove, heldTime *  0.1 * yMove); //Using heldTime as a scaler results in a linear acceleration.
            delay(MouseLinearSpeed);
            ++heldTime;
            fillOverTime(PendingColor, heldTime, HoldToGoBackLength); //Let us know how long until we trigger a go back to main menu.
          }

          if(heldTime > HoldToGoBackLength)
          {
            CurrentMenu = MainMenu;
            CircuitPlayground.playTone(50, 300, false);
            colorWipe(GoBackColor, 50);
          }
          else
          {
            CircuitPlayground.playTone(150, 100, false);
          }
        }
        else
        {
          CircuitPlayground.playTone(100, 100, false);
          colorWipe(ConfirmColor, 25);
          Mouse.click(MOUSE_LEFT);
        }
      }
      //The other case, just incriment the timer so our rotation continues. 
      else if(!TouchCondition())
      {
        MouseTimer += 0.1;
      }
}



//////////////////////////////INPUT FUNCTIONS/////////////////////////////////
bool TouchCondition()
{
  //Serial.print(" CT0("); Serial.print(CircuitPlayground.readCap(0));Serial.print(')');
  return CircuitPlayground.readCap(0) > 1000;
}

Commands AwaitInput(int FramesToWait)
{
  Serial.println("\nAwaiting Input - ");
  bool hasBeenFalse = false; //Has this ever been false? TO prevent button from being held accidentily as part of last press.

  int i = 0;
  //Serial.println(TouchCondition());
  while (i < FramesToWait)
  {
    if(!TouchCondition()) //No Input
    {
      //Serial.print(".");

      i += 100;
      delay(100);
      //++i;
      //delay(1);

      hasBeenFalse = true;
      
    }
    else if(TouchCondition() && hasBeenFalse) 
    {
      CircuitPlayground.playTone(50, 100, false);
      Serial.print("\nTouch Started...");
      int t = 0;


      while(t <= GoBackHoldTime && TouchCondition())
      {
        fillOverTime(PendingColor, t, GoBackHoldTime);
        t += 100;
        delay(100);
      }
      Serial.print("\nFinished Touch. Duration "); Serial.print(t); Serial.print(" frames. Result: ");

      CircuitPlayground.clearPixels();

      if(t >= GoBackHoldTime)
      {
        CircuitPlayground.playTone(125, 100, false);
        // DisplayText(" --Back-- ");
        colorWipe(GoBackColor, 25); 
        Serial.print("Go Back. Awaiting Release:");
        delay(500);
        // ClearText(" --Back-- ");

        while(TouchCondition())
        {
          delay(20);
          Serial.print('.');
        }
        Serial.println("Released");
        CircuitPlayground.playTone(150, 100, false);
        
        CircuitPlayground.clearPixels();
        return Back;
      }
      
      else if (t >= InputDelay)
      {
        CircuitPlayground.playTone(100, 100, false);

        // DisplayText(" --Selected-- ");
        // DisplayText(String(t));
        colorWipe(ConfirmColor, 25); 
        Serial.println("Select");
        delay(500);
        // ClearText(String(t));
        // ClearText(" --Selected-- ");

        CircuitPlayground.clearPixels();
        return Select1;
      }
      
    }

  }
  Serial.println("\n - End Await Input");
  CircuitPlayground.clearPixels();
  return None;
}


////////////////////////////TEXT DISPLAY//////////////////////////////////////
void DisplayText (String text_)
{
  // Serial.print("Going to print: ");
  // Serial.println(text_);
  for(int c = 0; text_[c] != '\0'; ++c)
  {
    Keyboard.write(text_[c]);
    //delay(10);
  }
  // Serial.println("Finished Print");
}

void ClearText (String text_)
{
  // Serial.print("Clear Text: ");
  // Serial.println(text_);
  for(int c = 0; text_[c] != '\0'; ++c)
  {
    Keyboard.write(KEY_BACKSPACE);
    //delay(10);
  }
  // Serial.print("Clear Text2 ");
  // Serial.println(text_);
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

////////////////////////////LETTER/WORD DISPLAY//////////////////////////////////////
//(const char* const* array)

Commands DisplayMenuOptions(MenuTree menu_[]) 
{
  AddCurrsor();

  Commands doWhat;
  for(int M = 0; menu_[M] != NoMenu; ++M)
    {
      String selectionToDisplay = MenuTreeToString(menu_[M]);
      
      delay(100);

      DisplayText(selectionToDisplay);

      // if(M < 6)
      // {
      //     strip.setPixelColor(M, PendingColor);         
      //                      //  Pause for a moment
      // }

      doWhat = AwaitInput(CycleSpeed); //Will return true when input conditoin true, or false in 1000 frames.
      
      delay(100);

      delay(100);

      ClearText(selectionToDisplay);
      
      if(doWhat == Select1)
      {
        NextMenu = menu_[M];

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

String DispalyGridOptionsAndType(const char* const* options, int size, char seperator, MenuTree goBackMenu)
{
    String selection = "";
    AddCurrsor();

    for(int g = 0; g < size; ++g)
    {

        String optionsString = String(options[g]); //String cast not needed?
        DisplayText(" >");
        DisplayText(optionsString);
        DisplayText("<");

        Commands doWhatNow = AwaitInput(CycleSpeed);
        
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

    doWhatNow = AwaitInput(CycleSpeed);

    if(doWhatNow == Select1)
    {
      GridSelection =  chocies[s];

      CurrentMenu = KeyboardMenu;
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


/////////////////////////////ONBOARD OPTIONS/SETTINGS FUNCTIONS//////////////////////////////////////
void ShortenDelay()
{
  GoBackHoldTime -= 250;
  String message = String("  --  Hold Time for Back: " + String(GoBackHoldTime));
  DisplayText(message);
  delay(2000);
  ClearText(message);
  //Serial.println(CycleSpeed);
}

void IncreaseDelay()
{
  GoBackHoldTime += 500;
  String message = String("  --  Hold Time For Back: " + String(GoBackHoldTime));
  DisplayText(message);
  delay(2000);
  ClearText(message);
  //Serial.println(CycleSpeed);
}

void IncreaseCycleSpeed()
{
  CycleSpeed *= 0.9;
  String message = String("  --  CycleSpeed: " + String(CycleSpeed));
  DisplayText(message);
  delay(2000);
  ClearText(message);
}

void DecreaseCycleSpeed()
{
  CycleSpeed *= 1.2;
  String message = String("  --  CycleSpeed: " + String(CycleSpeed));
  DisplayText(message);
  delay(2000);
  ClearText(message);
}


/////////////////////////////LED FUNCTIONS//////////////////////////////////////

void colorWipe(uint32_t color, int wait) {
   for(int i=0; i< 10; i++) { // For each pixel in strip...
     CircuitPlayground.setPixelColor(i, color);
     delay(wait);                           //  Pause for a moment
   }
}

void theaterChaseRainbow(int spinSpeed, int duration){
  
  int counter = 0;

  while(counter < duration)
  {
    for(int i=0; i< 10; i++) { // For each pixel in strip...
      uint8_t colorIndex = i * 255/10 + counter;
      CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(colorIndex));
    }
    delay(spinSpeed);
    counter += spinSpeed;
  }
}

void fillOverTime(uint32_t color, int currentTime, int maxTime)
{
  int numToFill = 1 + ((float)currentTime/(float)maxTime * (float)10.0);
  //Serial.println(10 * (float)currentTime/(float)maxTime);
  //for(int i=0; i< numToFill; i++) { // For each pixel in strip...
    CircuitPlayground.setPixelColor(numToFill, color);
  //}
}