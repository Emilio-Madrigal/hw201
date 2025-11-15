const int sensorIzquierdo = 2;
const int sensorDerecho = 3;

const int enableA = 9;
const int enableB = 10;  // Pin 11 para PWM confiable

const int pinMotorA1 = 5;
const int pinMotorA2 = 4;
const int pinMotorB1 = 7;
const int pinMotorB2 = 6;

const int ledAtaque = 13;

// Ultrasonido
const int trigPin = 11;
const int echoPin = 12;
const int distanciaUmbral = 8; // cm

const int velocidadExploracion = 160;
const int velocidadGiro = 200;      // Velocidad para girar buscando
const int velocidadAtaque = 240;    // Velocidad cuando avanza al ataque

bool sensorIzqActivo = false;
bool sensorDerActivo = false;

// Factores de calibración (ajusta si un motor es más rápido)
float factorA = 1.0;
float factorB = 1.0;

void setup() {
  pinMode(sensorIzquierdo, INPUT_PULLUP);
  pinMode(sensorDerecho, INPUT_PULLUP);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(enableA, OUTPUT);
  pinMode(enableB, OUTPUT);
  pinMode(pinMotorA1, OUTPUT);
  pinMode(pinMotorA2, OUTPUT);
  pinMode(pinMotorB1, OUTPUT);
  pinMode(pinMotorB2, OUTPUT);
  
  pinMode(ledAtaque, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Robot Sumo Iniciado");
  
  delay(2000);
}

void loop() {
  // Medir distancia con ultrasonido
  int distancia = medirDistancia();
  
  // Prioridad: si hay algo al frente dentro del umbral, embestir
  if (distancia > 0 && distancia <= distanciaUmbral) {
    Serial.print("Embestida - distancia: ");
    Serial.println(distancia);
    digitalWrite(ledAtaque, HIGH);
    avanzarRecto(velocidadAtaque);
  } else {
    // Leer estado de los sensores con lógica invertida (LOW = detecta, HIGH = no detecta)
    sensorIzqActivo = (digitalRead(sensorIzquierdo) == LOW);
    sensorDerActivo = (digitalRead(sensorDerecho) == LOW);
    
    // Decidir acción según sensores
    if (sensorIzqActivo && sensorDerActivo) {
      // Ambos sensores detectan: avanzar recto al ataque
      Serial.println("ATAQUE FRONTAL - Ambos sensores");
      digitalWrite(ledAtaque, HIGH);
      avanzarRecto(velocidadAtaque);
      
    } else if (sensorIzqActivo) {
      // Solo sensor izquierdo: girar izquierda hasta perderlo
      Serial.println("Girando IZQUIERDA - Siguiendo oponente");
      digitalWrite(ledAtaque, HIGH);
      girarIzquierda(velocidadGiro);
      
    } else if (sensorDerActivo) {
      // Solo sensor derecho: girar derecha hasta perderlo
      Serial.println("Girando DERECHA - Siguiendo oponente");
      digitalWrite(ledAtaque, HIGH);
      girarDerecha(velocidadGiro);
      
    } else {
      // Ningún sensor detecta: modo exploración
      Serial.println("Explorando...");
      digitalWrite(ledAtaque, LOW);
      avanzarRecto(velocidadExploracion);
    }
  }
  
  delay(10); // Pequeño delay para estabilidad
}

void avanzarRecto(int velocidad) {
  int velA = velocidad * factorA;
  int velB = velocidad * factorB;
  
  analogWrite(enableA, velA);
  analogWrite(enableB, velB);
  
  digitalWrite(pinMotorA1, HIGH);
  digitalWrite(pinMotorA2, LOW);
  digitalWrite(pinMotorB1, HIGH);
  digitalWrite(pinMotorB2, LOW);
}

void girarIzquierda(int velocidad) {
  int velA = velocidad * factorA;
  int velB = velocidad * factorB;
  
  analogWrite(enableA, velA);
  analogWrite(enableB, velB);
  
  // Motor A adelante, Motor B atrás
  digitalWrite(pinMotorA1, HIGH);
  digitalWrite(pinMotorA2, LOW);
  digitalWrite(pinMotorB1, LOW);
  digitalWrite(pinMotorB2, HIGH);
}

void girarDerecha(int velocidad) {
  int velA = velocidad * factorA;
  int velB = velocidad * factorB;
  
  analogWrite(enableA, velA);
  analogWrite(enableB, velB);
  
  // Motor A atrás, Motor B adelante
  digitalWrite(pinMotorA1, LOW);
  digitalWrite(pinMotorA2, HIGH);
  digitalWrite(pinMotorB1, HIGH);
  digitalWrite(pinMotorB2, LOW);
}

void retroceder(int velocidad) {
  int velA = velocidad * factorA;
  int velB = velocidad * factorB;
  
  analogWrite(enableA, velA);
  analogWrite(enableB, velB);
  
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

// --- Ultrasonido ---
int medirDistancia() {
  long suma = 0;
  int lecturasValidas = 0;
  
  for (int i = 0; i < 5; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duracionLocal = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
    
    if (duracionLocal > 0) {
      int dist = duracionLocal * 0.034 / 2; // cm
      if (dist > 2 && dist < 400) {
        suma += dist;
        lecturasValidas++;
      }
    }
    delay(10);
  }
  
  if (lecturasValidas > 0) {
    return suma / lecturasValidas;
  } else {
    return -1; // sin lectura válida
  }
}
