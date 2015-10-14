#include <TimerOne.h>

void setup() {
  Serial.begin(9600);
  Serial.println("Hello world\n");
  // put your setup code here, to run once:
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(21, OUTPUT);

  pinMode(0, INPUT);
  attachInterrupt(digitalPinToInterrupt(0), change, CHANGE);

  Timer1.initialize(1);
  Timer1.attachInterrupt(pump);
  Joystick.useManualSend(true);
}

int count = 0;
int channel = 0;
int channels[16];
int clock = 0;

void change() {
  if (digitalRead(0))
    rise();
  else
    fall();
}

int joyPos(int channel) {
  return 512 + (channels[channel] - 400);
}

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
  
  if (count > 10000) {
    channel = 0;
    joySend();
  } else {
    if (count > 1500)
      channels[channel] = 800;
    else if (count > 700)
      channels[channel] = count - 700;
    else
      channels[channel] = 0;
    channel++;
  }
}

void rise() {
  count = 0;
  Timer1.restart();
}


void pump() {
  count++;
  clock++;
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(14, channels[0] > 0);
  digitalWrite(15, channels[0] > 100);
  digitalWrite(16, channels[0] > 200);
  digitalWrite(17, channels[0] > 300);
  digitalWrite(18, channels[0] > 400);
  digitalWrite(19, channels[0] > 500);
  digitalWrite(20, channels[0] > 600);
  digitalWrite(18, channels[0] > 700);
  digitalWrite(19, channels[0] > 800);
  digitalWrite(20, channels[0] > 900);
  digitalWrite(21, channels[0] > 1000);

  if ((clock % 10000) == 0) {
    char temp[100];
    snprintf(temp, 100, "channel0: %d 1: %d 2: %d 3: %d 4: %d 5: %d 6: %d 7: %d",
    channels[0], channels[1], channels[2], channels[3], channels[4],
    channels[5], channels[6], channels[7]);
    Serial.println(temp);
  }
}
