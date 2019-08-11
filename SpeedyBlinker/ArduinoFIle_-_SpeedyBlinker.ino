/*
 * Rafael Laya
 * Professor Basham's ENGR102 class at Shoreline Community College
 * 
 * Purpose and usage:
 * Blinks a LED connected to pin 13
 * while varying the duration of on/off time
 * when user presses the button at pin 12, serial monitor will output the result
 * for re-testing just press the button again (or you can also press the Arduino reset button)
 */
// Set up some constants for abstraction and portability of the code
const int redLed = 13;
const int pushButton = 12;
int buttonState;

void setup()
{
  pinMode(redLed, OUTPUT);
  pinMode(pushButton, INPUT);
  buttonState = 0;

  // getting serial monitor ready for user interaction purposes
  Serial.begin(9600);
  Serial.println("********************** Serial monitor has just been initiated **********************");
}

void loop() 
{
  delay(1000);
  
  // advise the user the test has begun
  Serial.println("New Test initiated");

  // try from 1ms up to 100ms delay in steps of one
  for(int delayTime = 1; delayTime <= 100; delayTime++)
  { 
    // blink 150 times during each try
    for(int k = 0; k <= 149; k++)
    {
      buttonState = digitalRead(pushButton);
      blinkRedLed(delayTime);

      // when the user decides he can see the LED blinking he'll push the button
      if (buttonState == HIGH)
      {
        // output the results
        Serial.print("You can see the blinking light when it blinks every ");
        Serial.print(delayTime);
        Serial.println(" milliseconds");
        testEnded();
        return;
      }
    }
  }
  // wait 5 seconds before re-testing (in case the user never pressed the button)
  delay(5000);
}

void blinkRedLed(int delayTime)
{
  digitalWrite(redLed, HIGH);
  delay(delayTime);
  digitalWrite(redLed, LOW);
  delay(delayTime);  
}

void testEnded(void)
{
  delay(1000);
  /* 
   *  user will press the button after the test has ended
   *  if it is the case that he wants to take another test
   */
  while (true)
  {
    buttonState = digitalRead(pushButton);
    if (buttonState == HIGH)
    {
      buttonState = 0;
      break;
    }
    else
    {
      continue;
    }
  }
  return;
}

