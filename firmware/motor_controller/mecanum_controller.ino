/**
 * mecanum_4roues.ino
 * Contrôle 4 moteurs TT — Châssis LewanSoul Mecanum
 * 2x L298N — Communication Serial JSON
 *
 * Format commande :
 *   {"vx": 0.5, "vy": 0.0, "wz": 0.0}
 *
 * vx = avance / recule
 * vy = glissement latéral (Mecanum)
 * wz = rotation
 *
 * Branchement :
 *   L298N #1 → Moteurs Avant (FL + FR)
 *   L298N #2 → Moteurs Arrière (RL + RR)
 */

// =====================
// PINS L298N #1 (AVANT)
// =====================
#define FL_EN   5    // PWM Avant Gauche
#define FL_IN1  22
#define FL_IN2  23

#define FR_EN   6    // PWM Avant Droit
#define FR_IN1  24
#define FR_IN2  25

// =====================
// PINS L298N #2 (ARRIÈRE)
// =====================
#define RL_EN   7    // PWM Arrière Gauche
#define RL_IN1  26
#define RL_IN2  27

#define RR_EN   8    // PWM Arrière Droit
#define RR_IN1  28
#define RR_IN2  29

// =====================
// CONSTANTES
// =====================
#define TIMEOUT_MS  500
#define MAX_PWM     200
#define BAUD_RATE   115200

// =====================
// VARIABLES
// =====================
float vx = 0;
float vy = 0;
float wz = 0;

unsigned long last_cmd_time = 0;
String serial_buffer = "";

// =====================
// SETUP
// =====================
void setup() {
  int pins[] = {
    FL_EN, FL_IN1, FL_IN2,
    FR_EN, FR_IN1, FR_IN2,
    RL_EN, RL_IN1, RL_IN2,
    RR_EN, RR_IN1, RR_IN2
  };

  for (int p : pins) pinMode(p, OUTPUT);

  stopAll();
  Serial.begin(BAUD_RATE);
  Serial.println("{\"status\":\"ready\", \"mode\":\"mecanum_4roues\"}");

  //testRotation();
}

// =====================
// LOOP
// =====================
void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      parseCommand(serial_buffer);
      serial_buffer = "";
    } else {
      serial_buffer += c;
    }
  }

  if (millis() - last_cmd_time > TIMEOUT_MS) {
    stopAll();
  } else {
    driveMecanum(vx, vy, wz);
  }
}

// =====================
// PARSE JSON
// =====================
void parseCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  vx = extractFloat(cmd, "vx");
  vy = extractFloat(cmd, "vy");
  wz = extractFloat(cmd, "wz");

  last_cmd_time = millis();

  Serial.print("{\"vx\":"); Serial.print(vx);
  Serial.print(",\"vy\":"); Serial.print(vy);
  Serial.print(",\"wz\":"); Serial.print(wz);
  Serial.println("}");
}

float extractFloat(String json, String key) {
  String search = "\"" + key + "\":";
  int idx = json.indexOf(search);
  if (idx == -1) return 0.0;
  idx += search.length();
  int end = json.indexOf(",", idx);
  if (end == -1) end = json.indexOf("}", idx);
  return json.substring(idx, end).toFloat();
}

// =====================
// CINÉMATIQUE MECANUM
// =====================
void driveMecanum(float vx, float vy, float wz) {
  // Calcul vitesse par roue
  float fl =  vx - vy - wz;   // Avant Gauche
  float fr =  vx + vy + wz;   // Avant Droit
  float rl =  vx + vy - wz;   // Arrière Gauche
  float rr =  vx - vy + wz;   // Arrière Droit

  // Normalisation [-1, 1]
  float maxVal = 1.0f;
  if (abs(fl) > maxVal) maxVal = abs(fl);
  if (abs(fr) > maxVal) maxVal = abs(fr);
  if (abs(rl) > maxVal) maxVal = abs(rl);
  if (abs(rr) > maxVal) maxVal = abs(rr);
  fl /= maxVal;
  fr /= maxVal;
  rl /= maxVal;
  rr /= maxVal;

  // Appliquer aux 4 moteurs
  setMotor(FL_EN, FL_IN1, FL_IN2, fl);
  setMotor(FR_EN, FR_IN1, FR_IN2, fr);
  setMotor(RL_EN, RL_IN1, RL_IN2, rl);
  setMotor(RR_EN, RR_IN1, RR_IN2, rr);
}

// =====================
// CONTRÔLE MOTEUR
// =====================
void setMotor(int en, int in1, int in2, float speed) {
  int pwm = (int)(abs(speed) * MAX_PWM);
  pwm = constrain(pwm, 0, MAX_PWM);

  if (speed > 0.05) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if (speed < -0.05) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    pwm = 0;
  }
  analogWrite(en, pwm);
}

// =====================
// STOP
// =====================
void stopAll() {
  setMotor(FL_EN, FL_IN1, FL_IN2, 0);
  setMotor(FR_EN, FR_IN1, FR_IN2, 0);
  setMotor(RL_EN, RL_IN1, RL_IN2, 0);
  setMotor(RR_EN, RR_IN1, RR_IN2, 0);
}

// =====================
// TEST AU DÉMARRAGE
// =====================
void testRotation() {
  delay(1000);                  // Attends 1 sec après démarrage
  driveMecanum(1, 0, 0);
  delay(3000);                  // Pendant 3 secondes
  stopAll();                    // Stop
  Serial.println("{\"test\":\"done\"}");
}