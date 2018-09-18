/*
 * Rafael Laya
 * Professor Basham's ENGR102 class at Shoreline Community College
 * The reader can use this code as they please but I won't be liable of anything
 */

// some constants for abstraction and readability
const int softPot = A0;
const int flexSensor = A1;
const int buzzerPin = 3;
const int blueLed = 9;
const int greenLed = 10;
const int redLed = 11;

// tempo is constant, and is an empirical obtained value
const int tempo = 113;

void setup()
{
  Serial.begin(9600); 
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
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

  // the notes are in function of the reading on the soft pot
  // also the colors match the notes
  switch (softPotVal)
  {
    case 0 ... 127:
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

    case 128 ... 254:
      note = 'b';
      colors[0] = 0;
      colors[1] = 0;
      colors[2] = 1;
      break;

    case 255 ... 381:
      note = 'c';
      colors[0] = 0;
      colors[1] = 1;
      colors[2] = 0;
      break;

    case 382 ... 508:
      note = 'C';
      colors[0] = 0;
      colors[1] = 1;
      colors[2] = 1;
      break;

    case 509 ... 635:
      note = 'd';
      colors[0] = 1;
      colors[1] = 0;
      colors[2] = 0;
      break;

    case 636 ... 762:
      note = 'e';
      colors[0] = 1;
      colors[1] = 0;
      colors[2] = 1;
      break;

    case 763 ... 889:
      note = 'f';
      colors[0] = 1;
      colors[1] = 1;
      colors[2] = 0;
      break;

    default:
      note = 'g';
      colors[0] = 1;
      colors[1] = 1;
      colors[2] = 1;
      break;
  }

  // play the buzzer
  tone(buzzerPin, frequency(note, numNotes), beats * tempo);

  // lights blink along with the buzzer
  blinkLights(colors, beats * tempo);

  // this is also taken empirically
  delay(beats * tempo / 5);
}

// this function takes an array of colors and the speed in which to blink
// then makes the RGB Led blink as the parameters specify
void blinkLights(int colors[], int vel)
{
  digitalWrite(redLed, colors[0]);
  digitalWrite(greenLed, colors[1]);
  digitalWrite(blueLed, colors[2]);
  delay(vel / 2);
  digitalWrite(redLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(greenLed, LOW);
  delay(vel / 2);
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

