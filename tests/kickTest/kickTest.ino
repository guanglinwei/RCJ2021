#define KICKRELAY 12
#define CHARGERELAY 11
void setup() {
  // put your setup code here, to run once:
  pinMode(KICKRELAY, OUTPUT);
  pinMode(CHARGERELAY, OUTPUT);
  digitalWrite(KICKRELAY, LOW);
  digitalWrite(CHARGERELAY, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void charge() {
  kickOff();
  chargeOn();
  delay(700);
  chargeOff();
}
void kick() {
  chargeOff();
  kickOn();
  delay(400);
  kickOff();
  charge();
}


void chargeOn() {
  digitalWrite(CHARGERELAY, LOW);
}
void chargeOff() {
  digitalWrite(CHARGERELAY, HIGH);
}
void kickOn() {
  digitalWrite(KICKRELAY, LOW);
}
void kickOff() {
  digitalWrite(KICKRELAY, HIGH);
}
