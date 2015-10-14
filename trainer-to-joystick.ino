#include <TimerOne.h>

/*
 * This code is used to parse the serial signal that my tx generates from the trainer port.
 * It parses up to 16 channels, and fills up the channels[16] array with the values.
 *
 * This code was written for the Teensy 3.1 arduino clone, just because thats the only
 * device i had available, Im not sure it will work with other arduino bords at all. 
 * https://www.pjrc.com/teensy/teensy31.html
 */

int count = 0;
int channel = 0;
int channels[16];
int clock = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello world\n");

  // Connect the rx to pin0
  pinMode(0, INPUT);

  // Fire a interrupt everytime it changes state
  attachInterrupt(digitalPinToInterrupt(0), change, CHANGE);

  // This timer is used to count the time between pulses
  Timer1.initialize(1);
  Timer1.attachInterrupt(pump);

  // Manuelly trigger when joystick library should send its pulses.
  Joystick.useManualSend(true);
}

void change() {
  if (digitalRead(0))
    rise();
  else
    fall();
}

// The rx sends between 0 -> 800 pulses, but the joystick driver expects 0 -> 1024.
// Align the center of the tx 400 pulses, to the joysticks 512.
int joyPos(int channel) {
  return 512 + (channels[channel] - 400);
}

// Use arduino joystick driver, to send signal to the pc.
void joySend() {
  Joystick.X(joyPos(0));
  Joystick.Y(joyPos(1));
  Joystick.Z(joyPos(2));
  Joystick.Zrotate(joyPos(3));
  Joystick.sliderLeft(joyPos(4));
  Joystick.sliderRight(joyPos(5));
  Joystick.send_now();
}

void fall() {
  // This is the timeout, that marks the the tx is starting from channel 0 again.
  if (count > 10000) {
    channel = 0;

    // Send the values to the pc.
    joySend();
  } else {
    // The minimum pulse with is always 700 ticks, (Range 700 -> 1500)
    // Cut out the 700 first ticks, and cap the value to bee between 0 -> 800, with 400 as center stick.
    if (count > 1500)
      channels[channel] = 800;
    else if (count > 700)
      channels[channel] = count - 700;
    else
      channels[channel] = 0;

    // Next fall triggered shoud apply to next channel
    channel++;
  }
}

// On signal rise, restart timer and reset count value.
void rise() {
  count = 0;
  Timer1.restart();
}

// This is run every ms, just increment the value of count once.
void pump() {
  count++;
  clock++;
}

void loop() {

  // Write a serial log every 10000 ticks
  if ((clock % 10000) == 0) {
    char temp[100];
    snprintf(temp, 100, "channel0: %d 1: %d 2: %d 3: %d 4: %d 5: %d 6: %d 7: %d",
    channels[0], channels[1], channels[2], channels[3], channels[4],
    channels[5], channels[6], channels[7]);
    Serial.println(temp);
  }
}
