const int echoPin0 = 6;
const int echoPin1 = 11;
const int echoPin2 = 8;
const int echoPin3 = 9;
const int echoPin4 = 5;
const int echoPin5 = 7;
const int echoPin6 = 12;
const int echoPin7 = 13;
const int trigPin = 3;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin0, INPUT);
  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);
  pinMode(echoPin3, INPUT);  // Add this line
  pinMode(echoPin4, INPUT);  // Add this line
  pinMode(echoPin5, INPUT);  // Add this line
  pinMode(echoPin6, INPUT);  // Add this line
  pinMode(echoPin7, INPUT);  // Add this line
  Serial.begin(9600);
}
#define MEASURE_TIMEOUT 25000UL

float getDistance(int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH,MEASURE_TIMEOUT);
  float distance = (duration*.0343)/2;
  if (distance == 0) {
    distance = 100000;
  }
  
  return distance;
}

void printDistance(int echoPin) {
  float distance = getDistance(echoPin);
  Serial.print(echoPin);
  Serial.print(": ");
  Serial.println(distance);
}

void Debug(){
  printDistance(echoPin0);
  printDistance(echoPin1);
  printDistance(echoPin2);
  printDistance(echoPin3);
  printDistance(echoPin4);
  printDistance(echoPin5);
  printDistance(echoPin6);
  printDistance(echoPin7);
}

const int NUM_PINS = 8;
float distances[NUM_PINS];
int echoPin[NUM_PINS] = {echoPin0,echoPin1,echoPin2,echoPin3,echoPin4,echoPin5,echoPin6,echoPin7};

void loop() {

  for (int i = 0; i < NUM_PINS; i++) {
      distances[i] = getDistance(echoPin[i]);
      //delay(25);
  }
  

  float minDistance = distances[0];
  for (int i = 1; i < NUM_PINS; i++) {
      minDistance = min(minDistance, distances[i]);
  }
  //Debug();
  //printDistance(echoPin0);
  //printDistance(echoPin1);
  //printDistance(echoPin2);
  //printDistance(echoPin3);
  //printDistance(echoPin4);
  //printDistance(echoPin5);
  //printDistance(echoPin1);
  Serial.println(minDistance);
}