/*
 * November 2017
 * Class: ENGR 102 intro to electronics (Fall 2017)
 * Software Version: Arduino 1.8.2
 * Source code for Discoteca 2.0 more information on the report
*/


#include <LiquidCrystal.h>
LiquidCrystal lcd(11, 13, 12, 7, 6, 5);

// some constants for abstraction and readability
const int softPot = A0;
const int flexSensor = A1;
const int buzzerPin = 8;
const int blueLed = 9;
const int greenLed = 10;
const int redLed = 16;
const int numLeds = 8;

int dataPin = 2; 
int clockPin = 3;
int latchPin = 4;

byte data = 0;

// tempo is constant, and is an empirical obtained value
const int tempo = 113;

void setup()
{
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);  
  pinMode(latchPin, OUTPUT);


  lcd.begin(16, 2); //Initialize the 16x2 LCD


  lcd.clear();  //Clear any old data displayed on the LCD
}

void loop()
{
  int beats;
  int flexVal;
  int softPotVal;
  int note; 

  // colors is an array of three integers, interpret as following:
  // - colors[i] = 1 if we want to light the LED related to i ON
  // i = 0 is red, i = 1 is green, i = 2 is blue
  int colors[3];

  // a list of notes is just an array of characters
  // interpret each character as a note
  char notesList[] = {'a', 'b', 'c', 'C', 'd', 'e', 'f', 'g', ' '};
  const int numNotes = 9;

  // read the sensors
  softPotVal = analogRead(softPot);
  flexVal = analogRead(flexSensor);

  Serial.println(flexVal);
  
  // beats is a function of the flex sensor
  switch (flexVal)
  {
    case 800 ... 900:
      beats = 1;
      break;
    case 720 ... 799:
      beats = 4;
      break;
    default:
      beats = 8;
      break;
  }

  Serial.println(softPotVal);
  // the notes are in function of the reading on the soft pot
  // also the colors match the notes
  switch (softPotVal)
  {
    case 0 ... 49:
      note = pickRandomNote(notesList, numNotes);

      // do not turn the leds on when note means stop
      if (note == ' ')
      {
        colors[0] = 0;
        colors[1] = 0;
        colors[2] = 0;
      }
      else
      {
        // if we succesfully get a note, pick a color at random that is not 000
        do
        {
          colors[0] = random(0, 2);
          colors[1] = random(0, 2);
          colors[2] = random(0, 2);
        } while ((colors[0] == 0) &&
                 (colors[1] == 0) &&
                 (colors[2] == 0));
      }
      break;

    case 50 ... 140:
      note = 'c';
      colors[0] = 0;
      colors[1] = 0;
      colors[2] = 1;
    break;

    case 141 ... 254:
      note = 'd';
      colors[0] = 0;
      colors[1] = 0;
      colors[2] = 1;
      break;

    case 255 ... 381:
      note = 'e';
      colors[0] = 0;
      colors[1] = 1;
      colors[2] = 0;
      break;

    case 382 ... 508:
      note = 'f';
      colors[0] = 0;
      colors[1] = 1;
      colors[2] = 1;
      break;

    case 509 ... 635:
      note = 'g';
      colors[0] = 1;
      colors[1] = 0;
      colors[2] = 0;
      break;

    case 636 ... 762:
      note = 'a';
      colors[0] = 1;
      colors[1] = 0;
      colors[2] = 1;
      break;

    case 763 ... 889:
      note = 'b';
      colors[0] = 1;
      colors[1] = 1;
      colors[2] = 0;
      break;

    default:
      note = 'C';
      colors[0] = 1;
      colors[1] = 1;
      colors[2] = 1;
      break;
  }

  printOnLCD(note, frequency(note, numNotes), beats);
  
  // play the buzzer
  tone(buzzerPin, frequency(note, numNotes), beats * tempo);

  // lights blink along with the buzzer
  blinkLightsON(colors, beats * tempo);

  // LEDs dance along with the frequency of the music
  danceLeds(note);

  delay(beats * tempo / 2);

  blinkLightsOFF();

  delay(beats * tempo / 2);

  // this is also taken empirically
  delay(beats * tempo / 5);
}

void blinkLightsOFF()
{
  digitalWrite(redLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(greenLed, LOW);
}

void blinkLedsOFF()
{
  for(int i = 0; i < 8; i++)
  {
    shiftWrite(i, LOW);
  }
}

// this function takes an array of colors and the speed in which to blink
// then makes the RGB Led blink as the parameters specify
void blinkLightsON(int colors[], int vel)
{
  digitalWrite(redLed, colors[0]);
  digitalWrite(greenLed, colors[1]);
  digitalWrite(blueLed, colors[2]);
}

// this function takes a note and assigns to it a frequency
// thanks to the SIK guide for this piece of code
int frequency(char note, int numNotes)
{
  int i;

  // these are the notes
  char notesList[numNotes] = {
    'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'};

  // frequency[i] matches names[i]
  int frequencies[numNotes] = {
    262, 294, 330, 349, 392, 440, 494, 523};

  // Now we'll search through the letters in the array, and if
  // we find it, we'll return the frequency for that note.
  for (i = 0; i < numNotes; i++)  // Step through the notes
  {
    if (notesList[i] == note)         // Is this the one?
    {
      return (frequencies[i]);    // Yes! Return the frequency and exit function.
    }
  }
  return 0;  // We looked through everything and didn't find it,
  // but we still need to return a value, so return 0.
}

char pickRandomNote(char notes[], int numNotes)
{
  return notes[random(0, numNotes)];
}

void shiftWrite(int desiredPin, boolean desiredState)
{
  //Serial.println(data);
  bitWrite(data, desiredPin, desiredState);
  //Serial.println(data);
  shiftOut(dataPin, clockPin, MSBFIRST, data);

  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}

void danceLeds(char note)
{
  Serial.println(note);

  for(int index = 0; index < numLeds; index++)
  {
    Serial.println(noteValue(note, 8));
    if(shouldBlink(numLeds - 1 - index, noteValue(note, 8)))
    {
      shiftWrite(numLeds - 1 - index, HIGH);
    }
    else
    {
      shiftWrite(numLeds - 1 - index, LOW);
    }
  }
}

boolean shouldBlink(int LED, int noteValue)
{
  return (LED >= noteValue);
}

int noteValue(char note, int numNotes)
{
  if (note == ' ')
  {
    return numNotes + 100;
  }
  
  char notesList[numNotes] = {
    'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'};

  for(int i = 0; i < 8; i++)
  {
    if (notesList[i] == note)
    {
      return i;
    }
    else
    {
      continue;
    }
  }

  return 0;
}

void printOnLCD(int note,int frequency,int beats)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Note: ");
  lcd.print(char(note));
  lcd.print("  Hz: ");
  lcd.print(frequency);
  lcd.setCursor(0, 1);
  lcd.print("Speed: ");

  int speedLevel;
  if(beats == 1)
  {
     speedLevel = 3;
  }
  else if (beats == 4)
  {
    speedLevel = 2;
  }
  else
  {
    speedLevel = 1;
  }
  lcd.print(speedLevel);
}

