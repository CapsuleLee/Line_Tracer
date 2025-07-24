#include <IRremote.h>

#define PIN_IR_RECEIVE 3

#define PIN_MOTOR_LEFT_FORWARD 9
#define PIN_MOTOR_LEFT_BACKWARD 10
#define PIN_MOTOR_RIGHT_FORWARD 5
#define PIN_MOTOR_RIGHT_BACKWARD 6

#define PIN_LINE_DETECT_LEFT A1
#define PIN_LINE_DETECT_RIGHT A0
#define REF_VALUE_LINE_DETECT 500

int motorSpeedLeft = 0;   // -255 ~ 255
int motorSpeedRight = 0;
int isStartLineTracing = 0;

int detectValueLeft = 0;
int detectValueRight = 0;

// === 모터 제어 함수 ===
void processMotor() {
  if (motorSpeedLeft >= 0) {
    analogWrite(PIN_MOTOR_LEFT_FORWARD, motorSpeedLeft);
    analogWrite(PIN_MOTOR_LEFT_BACKWARD, 0);
  } else {
    analogWrite(PIN_MOTOR_LEFT_FORWARD, 0);
    analogWrite(PIN_MOTOR_LEFT_BACKWARD, -motorSpeedLeft);
  }

  if (motorSpeedRight >= 0) {
    analogWrite(PIN_MOTOR_RIGHT_FORWARD, motorSpeedRight);
    analogWrite(PIN_MOTOR_RIGHT_BACKWARD, 0);
  } else {
    analogWrite(PIN_MOTOR_RIGHT_FORWARD, 0);
    analogWrite(PIN_MOTOR_RIGHT_BACKWARD, -motorSpeedRight);
  }
}

// === 라인 센서 값 읽기 ===
void updateLineSensor() {
  detectValueLeft = analogRead(PIN_LINE_DETECT_LEFT);
  detectValueRight = analogRead(PIN_LINE_DETECT_RIGHT);
}

// === 모터 속도 계산 ===
void updateMotorSpeed() {
  if (!isStartLineTracing) {
    motorSpeedLeft = 0;
    motorSpeedRight = 0;
    return;
  }

  updateLineSensor();

  if ((detectValueLeft > REF_VALUE_LINE_DETECT) && (detectValueRight > REF_VALUE_LINE_DETECT)) {
    motorSpeedLeft = 180;
    motorSpeedRight = 180;
  }
  else if (detectValueLeft > REF_VALUE_LINE_DETECT) {
    motorSpeedLeft = 160;
    motorSpeedRight = 50;
  }
  else if (detectValueRight > REF_VALUE_LINE_DETECT) {
    motorSpeedLeft = 50;
    motorSpeedRight = 160;
  }
}

void setup() {
  IrReceiver.begin(PIN_IR_RECEIVE, ENABLE_LED_FEEDBACK);

  pinMode(PIN_MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(PIN_MOTOR_LEFT_BACKWARD, OUTPUT);
  pinMode(PIN_MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(PIN_MOTOR_RIGHT_BACKWARD, OUTPUT);

  analogWrite(PIN_MOTOR_LEFT_FORWARD, 0);
  analogWrite(PIN_MOTOR_LEFT_BACKWARD, 0);
  analogWrite(PIN_MOTOR_RIGHT_FORWARD, 0);
  analogWrite(PIN_MOTOR_RIGHT_BACKWARD, 0);
}

void loop() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.protocol == NEC) {
      if (IrReceiver.decodedIRData.command == 0x16) { // '*' 버튼
        isStartLineTracing = 1;
      }
      else if (IrReceiver.decodedIRData.command == 0x0D) { // '#' 버튼
        isStartLineTracing = 0;
      }
    }
    IrReceiver.resume();
  }

  updateMotorSpeed();
  processMotor();
}
