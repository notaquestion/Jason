#include <Adafruit_CircuitPlayground.h>
#include <BLE52_Mouse_and_Keyboard.h>

#include <avr/pgmspace.h>


// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


uint32_t StartupColor = CircuitPlayground.strip.Color(30, 30, 30);
uint32_t PendingColor = CircuitPlayground.strip.Color(203, 245, 66);
uint32_t ConfirmColor = CircuitPlayground.strip.Color(0, 255, 0);
uint32_t GoBackColor = CircuitPlayground.strip.Color(255, 0, 0);
uint32_t MouseColor = CircuitPlayground.strip.Color(50, 50, 255);

const char str[] = "My very long string";

const char LetterClumps_1[] = "_/E/T/S/D/W";
const char LetterClumps_2[] = "O/H/I/L/F/K";
const char LetterClumps_3[] = "A/N/U/G/V/Z";
const char LetterClumps_4[] = "R/Y/C/J/1/2";
const char LetterClumps_5[] = "M/B/Q/3/4/5";
const char LetterClumps_6[] = "P/X/6/7/8/9";
const char LetterClumps_7[] = "./?/,/+/!";
const char *const LetterClumps[] = {LetterClumps_1, LetterClumps_2, LetterClumps_3, LetterClumps_4, LetterClumps_5, LetterClumps_6, LetterClumps_7};
const int LetterClumps_SIZE = 7;


const char Words_A[] = "ABOUT/ACTION/ADVENTURE CAPATALIST/AFTER/ALSO/AMANDA";
const char Words_B[] = "BACK/BECAUSE/BRANDON/BURRITO/BIG FISH GAMES";
const char Words_C[] = "CAKE/CHEESEBURGER/CHERIE/CHEST/COME/COMEDY/COMPUTER/CONCERT/COOKIES/COULD";
const char Words_D[] = "DANIEL/DEONTE/DRAMA/DRINK STRAW";
const char Words_E[] = "EVEN";
const char Words_F[] = "FIND/FINGERS/FIRST/FRENCH FRIES/FROM/FUNNY";
const char Words_G[] = "GAMES/GINA/GIVE/GOOD/GOOD";
const char Words_H[] = "HAND/HAVE/HEAD/HIDDEN OBJECT/HIPS/HURT";
const char Words_I[] = "I WANT/I WANT TO/I WANT TO DO/I WANT TO GET/ICE CREAM SANDWICH/INTO";
const char Words_J[] = "JASON/JIGWORDS/JOSE/JUST";
const char Words_K[] = "KNOW";
const char Words_L[] = "LASAGNA/LEMONADE WATER/LETTERS/LIKE/LIKE/LOOK/LOVE";
const char Words_M[] = "MACARONI/MAKE/MARCY/MICHAEL/MIKE/MONTI/MOST/MOVIE";
const char Words_N[] = "NECK/NICK";
const char Words_O[] = "ONLY/OTHER/OVER";
const char Words_P[] = "PAINFUL/PAUL/PEOPLE/PILLOW/PINEAPPLE WATER/PLAY";
const char Words_S[] = "SHEET/SOME/STEAM";
const char Words_T[] = "TAKE/TANGELO/TANGELO/THAN/THAT/THEIR/THEM/THEN/THERE/THESE/THEY/THINK/THIS/TIME/TIRED/TOES/TURN/TV SERIES";
const char Words_W[] = "WANT/WANT/WELL/WENT/WHAT/WHEELCHAIR/WHEN/WHICH/WILL/WITH/WORDS/WORK/WOULD/WRITE";
const char Words_Y[] = "YEAR/YOUR";

const char *const AutoSuggestDic[] = {Words_A, Words_B, Words_C, Words_D, Words_E, Words_F, Words_G, Words_H, Words_I, Words_J, Words_K, Words_L, Words_M, Words_N, Words_O, Words_P, Words_S, Words_T, Words_W, Words_Y, };
const int AutoSuggestDic_SIZE = 20;


void setup() 
{ // initialize the buttons' inputs:
  

  Serial.begin(115200);
  CircuitPlayground.begin();
  // initialize mouse control:

  Mouse.begin();
  Keyboard.begin();

  delay(1000);
  colorWipe(CircuitPlayground.strip, PendingColor, 35);
  delay(1000);
  colorWipe(CircuitPlayground.strip, MouseColor, 35);
  delay(1000);
  CircuitPlayground.clearPixels();

  int mw = 0;
  bool red = false;

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
  CircuitPlayground.clearPixels();


}

char buffer[200];  // make sure this is large enough for the largest string it must hold
int bufferSize = 200;

void loop() {
  /* Using the string table in program memory requires the use of special functions to retrieve the data.
   The strcpy_P function copies a string from program space to a string in RAM ("buffer").
   Make sure your receiving string in RAM is large enough to hold whatever
   you are retrieving from program space. */

  strcpy_P(buffer, (char *)pgm_read_word(&(LetterClumps[0])));  // Necessary casts and dereferencing, just copy.
  Serial.println("Start Test");
//  Serial.println(buffer);
//  Serial.println(LetterClumps[0]);
//  Serial.println(LetterClumps_1);
  delay(500);

  WhichArray(LetterClumps);
  Serial.println("End Test");

  for(int i = 0; i < 100; ++i)
  {
    Serial.println(to_string(i));
    delay(1);
  }
}

void WhichArray(const char* const* array)
{
  Serial.println(array[0]);
}

void colorWipe(Adafruit_CPlay_NeoPixel strip_, uint32_t color, int wait) {
  for(int i=0; i< strip_.numPixels(); i++) { // For each pixel in strip...
    strip_.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip_.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}