/*
 * November 2017
 * Class: ENGR 102 intro to electronics (Fall 2017)
 * Software Version: Arduino 1.8.2
 * Source code for Discoteca 2.0 more information on the report
*/


#include <Keypad.h>
#include <Servo.h>

Servo servo1;

// constants and global variables
const int lightTargetPin = A0;
const int lightRefPin = A1;
int lightTargetVal;
int lightRefVal;

// this keeps track of the callibration samples
int const numOfSamples = 200;
int diffBetweenSensors[numOfSamples];
int counter = 0;
String defaultPassword = "#102";
String password;

const int servoClosedAngle = 150;
const int servoOpenAngle = 0;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

const int maxPasswordLength = 4;

byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

const int redLed = 13;
const int greenLed = 12;
const int buzzerPin = 11;
const int tempo = 113;
const int beats = 4;
const int numNotes = 8;
const int duration = beats * tempo;
const int NUM_NOTES = 8;


int GREEN_RG[2] = {0, 1};
int RED_RG[2] = {1, 0};
int RG_PINS[2] = {redLed, greenLed};
int OFF_RG[2] = {0, 0};

void turnGreenLights();

void setup() 
{
  Serial.begin(9600);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  setPasswordTo(defaultPassword);

  servo1.attach(10, 900, 2100);  //Connect the servo to pin 9
                //with a minimum pulse width of
                //900 and a maximum pulse width of
                //2100. 
}

void loop() 
{
  // make sure the doors are closed and keep listening for cars
  closeDoors();
  listenForCars();
}

void setPasswordTo(String newPassword)
{
  password = newPassword;
}

void listenForPassword()
{
  String attempt = readKeypadForAttempt(maxPasswordLength);

  if(attempt == password)
  {
    playCorrectPassword();
    turnGreenLights();
    openDoors();
    turnLightsOff();
  }
  else
  {
    playIncorrectPassword();
    turnRedLights();
    delay(500);
    turnLightsOff();
  }
  return;
}

void closeDoors()
{
  servo1.write(servoClosedAngle);
}

void openDoors()
{
  int currentDiff;
  do
  {
    servo1.write(servoOpenAngle);
    lightTargetVal = analogRead(lightTargetPin);
    lightRefVal = analogRead(lightRefPin);
    currentDiff = abs(lightTargetVal - lightRefVal);
    delay(5000);
    closeDoors();
  } while(laserIsCovered(currentDiff));
  
  return;
  
}

void listenForCars()
{
  lightTargetVal = analogRead(lightTargetPin);
  lightRefVal = analogRead(lightRefPin);

  int currentDiff = abs(lightTargetVal - lightRefVal);


  Serial.print("light target val: ");
  Serial.println(lightTargetVal);
  Serial.print("light ref val: ");
  Serial.println(lightRefVal);
  Serial.print("current diff: ");
  Serial.println(currentDiff);
  Serial.print("avg diff between sensors: ");
  Serial.println(avgDiffBetweenSensors());
  
  if(laserIsCovered(currentDiff) and (counter > numOfSamples - 1))
  {
    listenForPassword();
    return;
  }
  else
  {
    diffBetweenSensors[counter % numOfSamples] = currentDiff;
    counter++;
  }
}

boolean laserIsCovered(int currentDiff)
{
  return ((counter > 99) and (abs(currentDiff - avgDiffBetweenSensors()) > ((40.0/100.0) * float(avgDiffBetweenSensors())))); 
}

int avgDiffBetweenSensors()
{
  unsigned int sum = 0;
  for(int i = 0; i < numOfSamples; i++)
  {
    sum += diffBetweenSensors[i];
  }

  Serial.print("SUM");
  Serial.println(sum);
  
  if (counter > numOfSamples * 2)
  {
    counter = numOfSamples;
  }
  return sum / numOfSamples;
}

String readKeypadForAttempt(int maxLength)
{
  return readKeypadForString(maxLength);
}

String readKeypadForString(int maxLength)
{
  String string;
  while (string.length() < maxLength)
  {
    char customKey = customKeypad.getKey();
    if (customKey)
    {
      string += customKey;
    }
  }
  return string;
}

void playCorrectPassword()
{
  char notes[] = {'c'};
  int durations[] = {duration};
  soundMelody(buzzerPin, notes, 1, durations, 1, 8);
  delay(duration);
  notes[0] = {'g'};
  soundMelody(buzzerPin, notes, 1, durations, 1, 8);
  delay(duration);
  notes[0] = {'f'};
  soundMelody(buzzerPin, notes, 1, durations, 1, 8);
}

void turnLightsOff()
{
  turnRGLed(RG_PINS, OFF_RG);
}

void turnRedLights()
{
  turnRGLed(RG_PINS, RED_RG);
}

void turnGreenLights()
{
  turnRGLed(RG_PINS, GREEN_RG);
}

void playIncorrectPassword()
{
  char notes[] = {'C'};
  int durations[] = {duration};
  soundMelody(buzzerPin, notes, 1, durations, 1, 8);
  delay(duration);
  notes[0] = {'c'};
  soundMelody(buzzerPin, notes, 1, durations, 1, 8);
  delay(duration);
  notes[0] = {'C'};
  soundMelody(buzzerPin, notes, 1, durations, 1, 8);
}

void turnRGLed(int rgPins[],int colors[])
{
  for(int i = 0; i < 2; i++)
  {
    digitalWrite(rgPins[i], colors[i]);
  }
}

void soundMelody(int buzzer, char melodyNotes[], int numNotes, int durations[], int numDurations, int numSounds)
{
  for(int i = 0; i < numSounds; i++)
  {
    tone(buzzer, noteToFreq(melodyNotes[i % numNotes]), durations[i % numDurations]);
  }
}

int noteToFreq(char note)
{
  int i;

  // these are the notes
  char notesList[numNotes] = {
    'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'};

  // frequency[i] matches names[i]
  int frequencies[NUM_NOTES] = {
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
