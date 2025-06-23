#include <LiquidCrystal.h>

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 13);

// Pines
const int joyX = A2;
const int joyY = A0;
const int botonA = 8;
const int botonB = 9;
const int buzzer1 = 6;  // música
const int buzzer2 = 7;  // efectos

// Notas musicales
const int Do = 261, Do2 = 523, Do3 = 1046;
const int Re = 293, Re2 = 587, Re3 = 1174;
const int Mi = 329, Ree = 331;
const int Fa = 349, Fa2 = 698;
const int Sol = 392, Soll = 415, Sol2 = 784, Soll2 = 830;
const int La = 440, Laa = 466, La2 = 880, Laa2 = 932;
const int Si = 493, Si1 = 247;

const int prim = 100, segun = 200;

// Personajes
byte pacman[8] = {B00000,B00000,B01110,B11011,B11100,B01110,B00000,B00000};
byte fantasma[8] = {B00000,B00000,B01110,B10101,B11111,B11111,B10101,B00000};
byte punto[8] = {B00000,B00000,B00000,B01110,B01110,B00000,B00000,B00000};

// Juego
const int MAXX = 15;
const int MAXY = 1;

int xpac = 2, ypac = 1;
int xfant = 15, yfant = 0;
bool puntos[MAXX + 1][MAXY + 1];
bool jugando = false;
int score = 0;
long t_pac = 0, t_fant = 0;
int VEL_PAC = 200;
int VEL_FANT = 1000;

// ---------------------------
// Funciones de sonido
// ---------------------------

void nota(int freq, int dur) {
  tone(buzzer1, freq, dur);
  delay(dur);
  noTone(buzzer1);
  delay(50);
}

void reproducirMelodia() {
  nota(Re,prim); nota(Re,prim); nota(Re2,prim); nota(La,prim); nota(Soll,prim);
  nota(Sol,prim); nota(Fa,segun); nota(Re,prim); nota(Fa,prim); nota(Sol,prim);
  nota(Do,prim); nota(Do,prim); nota(Re2,prim); nota(La,prim); nota(Soll,prim);
  nota(Sol,prim); nota(Fa,segun); nota(Re,prim); nota(Fa,prim); nota(Sol,prim);
  nota(Si1,prim); nota(Si1,prim); nota(Re2,prim); nota(La,prim); nota(Soll,prim);
  nota(Sol,prim); nota(Fa,segun); nota(Re,prim); nota(Fa,prim); nota(Sol,prim);
}

void sonidoMuerte() {
  tone(buzzer2, 400, 300); delay(300);
  tone(buzzer2, 300, 300); delay(300);
  tone(buzzer2, 200, 500); delay(500);
  noTone(buzzer2);
}

// ---------------------------
// Funciones del juego
// ---------------------------

int leerDireccion() {
  int x = analogRead(joyX);
  int y = analogRead(joyY);
  const int umbral = 100;

  if (x < 512 - umbral) return 0;   // Izquierda
  if (x > 512 + umbral) return 3;   // Derecha
  if (y > 512 + umbral) return 1;   // Arriba
  if (y < 512 - umbral) return 2;   // Abajo
  return -1;
}

void moverPacman(int dx, int dy) {
  int ox = xpac, oy = ypac;
  xpac = constrain(xpac + dx, 0, MAXX);
  ypac = constrain(ypac + dy, 0, MAXY);

  lcd.setCursor(ox, oy);
  if (puntos[ox][oy]) lcd.write(2); else lcd.print(" ");

  lcd.setCursor(xpac, ypac);
  lcd.write((byte)0);

  if (puntos[xpac][ypac]) {
    puntos[xpac][ypac] = false;
    score++;
  }

  // Verificar si quedan puntos
  bool vacio = true;
  for (int i = 0; i <= MAXX; i++)
    for (int j = 0; j <= MAXY; j++)
      if (puntos[i][j]) vacio = false;

  if (vacio) iniciarNivel();  // reiniciar puntos
}

void moverFantasma() {
  int ox = xfant, oy = yfant;

  if (xfant < xpac) xfant++;
  else if (xfant > xpac) xfant--;
  else if (yfant < ypac) yfant++;
  else if (yfant > ypac) yfant--;

  lcd.setCursor(ox, oy);
  if (puntos[ox][oy]) lcd.write(2); else lcd.print(" ");

  lcd.setCursor(xfant, yfant);
  lcd.write(1);
}

void pantallaInicio() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("DOOMBOX-PACMAN");
  lcd.setCursor(0, 1); lcd.print("Pulse A to start");
  while (digitalRead(botonA) == HIGH);
  delay(300);
}

void pantallaGameOver() {
  sonidoMuerte();
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("GAME OVER LOSER ");
  lcd.setCursor(0, 1); lcd.print("Puntaje: "); lcd.print(score);
  while (digitalRead(botonB) == HIGH);
  delay(300);
  pantallaInicio();
  reproducirMelodia();
  iniciarNivel();
}

void iniciarNivel() {
  lcd.clear();
  for (int i = 0; i <= MAXX; i++) {
    for (int j = 0; j <= MAXY; j++) {
      puntos[i][j] = true;
      lcd.setCursor(i, j); lcd.write(2);
    }
  }

  xpac = 2; ypac = 1;
  xfant = 15; yfant = 0;
  score = 0;
  jugando = true;
  t_pac = millis();
  t_fant = millis();

  lcd.setCursor(xpac, ypac); lcd.write((byte)0);
  lcd.setCursor(xfant, yfant); lcd.write((byte)1);
}

// ---------------------------
// Setup y Loop
// ---------------------------

void setup() {
  pinMode(botonA, INPUT_PULLUP);
  pinMode(botonB, INPUT_PULLUP);
  pinMode(buzzer1, OUTPUT);
  pinMode(buzzer2, OUTPUT);

  lcd.begin(16, 2);
  lcd.createChar(0, pacman);
  lcd.createChar(1, fantasma);
  lcd.createChar(2, punto);

  pantallaInicio();
  reproducirMelodia();
  iniciarNivel();
}

void loop() {
  if (!jugando) return;

  if (millis() - t_pac > VEL_PAC) {
    int dir = leerDireccion();
    switch (dir) {
      case 0: moverPacman(1, 0); break;   // Derecha
      case 3: moverPacman(-1, 0); break;  // Izquierda
      case 1: moverPacman(0, -1); break;  // Arriba
      case 2: moverPacman(0, 1); break;   // Abajo
    }
    t_pac = millis();
  }

  if (millis() - t_fant > VEL_FANT) {
    moverFantasma();
    t_fant = millis();
  }

  if (xpac == xfant && ypac == yfant) {
    jugando = false;
    pantallaGameOver();
  }
}