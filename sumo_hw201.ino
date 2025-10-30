const int sensorIzquierdo = 2;
const int sensorDerecho = 3;

const int enableA = 9;
const int enableB = 10;
const int pinMotorA1 = 5;
const int pinMotorA2 = 4;
const int pinMotorB1 = 6;
const int pinMotorB2 = 7;

const int ledAtaque = 13;

const int velocidadExploracion = 150;
const int velocidadAtaque = 240;

bool oponenteDetectado = false;
unsigned long tiempoUltimaDeteccion = 0;
const unsigned long tiempoEsperaReconexion = 2000;

int ultimaDireccion = 0; // 0=ninguna, 1=izquierda, 2=derecha

void setup() {
  pinMode(sensorIzquierdo, INPUT);
  pinMode(sensorDerecho, INPUT);
  
  pinMode(enableA, OUTPUT);
  pinMode(enableB, OUTPUT);
  pinMode(pinMotorA1, OUTPUT);
  pinMode(pinMotorA2, OUTPUT);
  pinMode(pinMotorB1, OUTPUT);
  pinMode(pinMotorB2, OUTPUT);
  
  pinMode(ledAtaque, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(sensorIzquierdo), deteccionIzquierda, RISING);
  attachInterrupt(digitalPinToInterrupt(sensorDerecho), deteccionDerecha, RISING);
  
  Serial.begin(9600);
  Serial.println("Modo: Exploración");
  
  delay(2000);
}

void loop() {
  if (oponenteDetectado && (millis() - tiempoUltimaDeteccion > tiempoEsperaReconexion)) {
    oponenteDetectado = false;
    digitalWrite(ledAtaque, LOW);
    Serial.println("Perdí al oponente - Volviendo a modo exploración");
  }
  
  if (oponenteDetectado) {
    modoAtaque();
  } else {
    modoExploracion();
  }
  
  delay(50);
}
void deteccionIzquierda() {
  if (!oponenteDetectado) {
    Serial.println("carro mid en la izquierda");
  }
  oponenteDetectado = true;
  tiempoUltimaDeteccion = millis();
  ultimaDireccion = 1;
  digitalWrite(ledAtaque, HIGH);
}
void deteccionDerecha() {
  if (!oponenteDetectado) {
    Serial.println("carro mid en la derecha");
  }
  oponenteDetectado = true;
  tiempoUltimaDeteccion = millis();
  ultimaDireccion = 2;
  digitalWrite(ledAtaque, HIGH);
}

void modoAtaque() {
  if (ultimaDireccion == 1) {
    Serial.println("ATAQUE: Girando IZQUIERDA hacia oponente");
    girarIzquierda(velocidadAtaque);
  } else if (ultimaDireccion == 2) {
    Serial.println("ATAQUE: Girando DERECHA hacia oponente");
    girarDerecha(velocidadAtaque);
  } else {
    Serial.println("ATAQUE: Avanzando recto");
    avanzarRecto(velocidadAtaque);
  }
}

void modoExploracion() {
  static unsigned long ultimoCambio = 0;
  static int patronBusqueda = 0;
  if (millis() - ultimoCambio > 3000) {
    patronBusqueda = random(0, 3); // 0, 1 o 2
    ultimoCambio = millis();
    
    switch(patronBusqueda) {
      case 0:
        Serial.println("EXPLORACIÓN: Giro izquierda suave");
        break;
      case 1:
        Serial.println("EXPLORACIÓN: Giro derecha suave");
        break;
      case 2:
        Serial.println("EXPLORACIÓN: Avance recto breve");
        break;
    }
  }
  switch(patronBusqueda) {
    case 0:
      girarIzquierda(velocidadExploracion);
      break;
    case 1:
      girarDerecha(velocidadExploracion);
      break;
    case 2:
      avanzarRecto(velocidadExploracion);
      break;
  }
}

void avanzarRecto(int velocidad) {
  analogWrite(enableA, velocidad);
  analogWrite(enableB, velocidad);
  
  digitalWrite(pinMotorA1, HIGH);
  digitalWrite(pinMotorA2, LOW);
  digitalWrite(pinMotorB1, HIGH);
  digitalWrite(pinMotorB2, LOW);
}

void girarIzquierda(int velocidad) {
  analogWrite(enableA, velocidad);
  analogWrite(enableB, velocidad);
  
  digitalWrite(pinMotorA1, HIGH);
  digitalWrite(pinMotorA2, LOW);
  digitalWrite(pinMotorB1, LOW);
  digitalWrite(pinMotorB2, HIGH);
}

void girarDerecha(int velocidad) {
  analogWrite(enableA, velocidad);
  analogWrite(enableB, velocidad);
  
  digitalWrite(pinMotorA1, LOW);
  digitalWrite(pinMotorA2, HIGH);
  digitalWrite(pinMotorB1, HIGH);
  digitalWrite(pinMotorB2, LOW);
}

void retroceder(int velocidad) {
  analogWrite(enableA, velocidad);
  analogWrite(enableB, velocidad);
  
  digitalWrite(pinMotorA1, LOW);
  digitalWrite(pinMotorA2, HIGH);
  digitalWrite(pinMotorB1, LOW);
  digitalWrite(pinMotorB2, HIGH);
}

void detener() {
  analogWrite(enableA, 0);
  analogWrite(enableB, 0);
  
  digitalWrite(pinMotorA1, LOW);
  digitalWrite(pinMotorA2, LOW);
  digitalWrite(pinMotorB1, LOW);
  digitalWrite(pinMotorB2, LOW);
}
