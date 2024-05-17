// Nombre total de pins echo utilisées
const int NUM_ECHO_PINS = 9;

// Tableau des pins Echo
const int echoPins[NUM_ECHO_PINS] = {5, 6, 7, 8, 9, 10, 11, 12, 13};

// Pin Trigger
const int trigPin = 3;

// Définir le timeout pour la mesure de la distance
#define MEASURE_TIMEOUT 25000UL

// Initialisation des pins et de la communication série
void setup() {
  // Configurer la pin trig en sortie
  pinMode(trigPin, OUTPUT); 

  // Configurer toutes les pins echo en entrée
  for (int i = 0; i < NUM_ECHO_PINS; i++) {
    pinMode(echoPins[i], INPUT);
  }

  // Initialiser la communication série à 9600 baud
  Serial.begin(9600);
}

// Fonction pour obtenir la distance mesurée par un capteur ultrason
float getDistance(int echoPin) {
  // Envoyer une impulsion ultrason
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Mesurer la durée de l'impulsion retournée
  float duration = pulseIn(echoPin, HIGH, MEASURE_TIMEOUT);

  // Calculer la distance en centimètres
  float distance = (duration * 0.0343) / 2;

  // Si la distance est 0 (erreur de mesure), assigner une valeur "infinie"
  if (distance == 0) {
    distance = 100000; 
  }
  
  return distance;
}

// Fonction pour imprimer la distance mesurée par un capteur ultrason
void printDistance(int echoPin) {
  float distance = getDistance(echoPin);
  Serial.print("Pin ");
  Serial.print(echoPin);
  Serial.print(": ");
  Serial.println(distance);
}

// Fonction pour déboguer les distances mesurées par tous les capteurs
void debugDistances() {
  for (int i = 0; i < NUM_ECHO_PINS; i++) {
    printDistance(echoPins[i]);
  }
}

// Boucle principale
void loop() {
  float distances[NUM_ECHO_PINS];

  // Mesurer les distances pour toutes les pins echo
  for (int i = 0; i < NUM_ECHO_PINS; i++) {
    distances[i] = getDistance(echoPins[i]);
  }

  // Trouver la distance minimale mesurée
  float minDistance = distances[0];
  for (int i = 1; i < NUM_ECHO_PINS; i++) {
    if (distances[i] < minDistance) {
      minDistance = distances[i];
    }
  }

  // Imprimer la distance minimale qui sera lue par l'ESP32
  Serial.println(minDistance);

  // Décommenter pour déboguer les distances mesurées par tous les capteurs
  // debugDistances();
}
