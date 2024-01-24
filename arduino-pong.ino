#include <Adafruit_GFX.h>       // Adafruit_GFX-Bibliothek für die Grafik
#include <Adafruit_SSD1306.h>   // Adafruit_SSD1306-Bibliothek für das OLED-Display

#define SCREEN_WIDTH 128        // Breite des OLED-Displays in Pixeln
#define SCREEN_HEIGHT 64        // Höhe des OLED-Displays in Pixeln

#define OLED_RESET    -1        // Pin für den Reset des OLED-Displays (-1 bedeutet, dass der Reset-Pin nicht verwendet wird)
#define OLED_ADDR     0x3C      // I2C-Adresse des OLED-Displays

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  // Instanziierung des Display-Objekts

// Initialisierung von Variablen für die Paddel, den Ball, die Geschwindigkeiten und die Punkte
int paddle1Y, paddle2Y;
int paddleHeight = 12;
int paddleWidth = 2;
int paddleSpeed = 2;

int ballX, ballY;
int ballSize = 2;
int ballSpeedX, ballSpeedY;

int player1Score = 0, player2Score = 0;
int winningScore = 4;          // Punktzahl, bei der ein Spieler gewinnt
int scoreDifference = 2;       // Mindestunterschied zwischen den Punktzahlen zum Gewinnen

// Pins für die Tasten zur Steuerung der Paddel
const int paddle1UpButton = 10;
const int paddle1DownButton = 11;
const int paddle2UpButton = 8;
const int paddle2DownButton = 9;

void setup() {
  pinMode(OLED_RESET, OUTPUT);   // Setze den Reset-Pin des Displays als Ausgangspin
  digitalWrite(OLED_RESET, LOW);  // Setze den Reset-Pin auf LOW, um das Display zurückzusetzen
  delay(50);
  digitalWrite(OLED_RESET, HIGH);  // Setze den Reset-Pin auf HIGH, um das Display zu aktivieren

  // Initialisiere das Display und überprüfe, ob die Initialisierung erfolgreich war
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) {}
  }

  display.display();  // Zeige den aktuellen Display-Inhalt an
  delay(2000);

  display.clearDisplay();  // Lösche den Display-Inhalt
  display.display();
  delay(2000);

  // Setze die Anfangspositionen der Paddel
  paddle1Y = paddle2Y = (SCREEN_HEIGHT - paddleHeight) / 2;

  // Konfiguriere die Tasten für die Paddelsteuerung
  pinMode(paddle1UpButton, INPUT_PULLUP);
  pinMode(paddle1DownButton, INPUT_PULLUP);
  pinMode(paddle2UpButton, INPUT_PULLUP);
  pinMode(paddle2DownButton, INPUT_PULLUP);

  displayCountdown(3);  // Starte einen 3-Sekunden-Countdown vor dem Spielbeginn
}

void loop() {
  // Überprüfe, ob ein Spieler gewonnen hat
  if (player1Score >= winningScore && player1Score >= player2Score + scoreDifference) {
    displayEndScreen(player1Score, player2Score);
    waitForButtonPress();
    resetGame();
    displayCountdown(3);  // Starte einen 3-Sekunden-Countdown vor dem Neustart
  } else if (player2Score >= winningScore && player2Score >= player1Score + scoreDifference) {
    displayEndScreen(player1Score, player2Score);
    waitForButtonPress();
    resetGame();
    displayCountdown(3);  // Starte einen 3-Sekunden-Countdown vor dem Neustart
  } else {
    updatePaddles();  // Aktualisiere die Paddelpositionen basierend auf den Tasteneingaben
    updateBall();     // Aktualisiere die Position des Balls und überprüfe Kollisionen

    display.clearDisplay();  // Lösche den aktuellen Display-Inhalt
    drawPaddles();          // Zeichne die Paddel
    drawBall();             // Zeichne den Ball
    drawScores();           // Zeichne die Punktzahlen
    display.display();      // Zeige den neuen Display-Inhalt an
  }
}

// Funktion zum Aktualisieren der Paddelpositionen basierend auf den Tasteneingaben
void updatePaddles() {
  if (digitalRead(paddle1UpButton) == LOW && paddle1Y > 0) {
    paddle1Y -= paddleSpeed;
  }
  if (digitalRead(paddle1DownButton) == LOW && paddle1Y < SCREEN_HEIGHT - paddleHeight) {
    paddle1Y += paddleSpeed;
  }

  if (digitalRead(paddle2UpButton) == LOW && paddle2Y > 0) {
    paddle2Y -= paddleSpeed;
  }
  if (digitalRead(paddle2DownButton) == LOW && paddle2Y < SCREEN_HEIGHT - paddleHeight) {
    paddle2Y += paddleSpeed;
  }
}

// Funktion zum Aktualisieren der Ballposition und Überprüfen von Kollisionen
void updateBall() {
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // Überprüfe Kollision mit dem linken Paddel
  if (ballX <= paddleWidth && ballY >= paddle1Y && ballY <= paddle1Y + paddleHeight) {
    ballSpeedX = -ballSpeedX;  // Ändere die Richtung des Balls horizontal
  }
  // Überprüfe Kollision mit dem rechten Paddel
  if (ballX >= SCREEN_WIDTH - paddleWidth - ballSize && ballY >= paddle2Y && ballY <= paddle2Y + paddleHeight) {
    ballSpeedX = -ballSpeedX;  // Ändere die Richtung des Balls horizontal
  }

  // Überprüfe Kollision mit den oberen und unteren Rändern des Bildschirms
  if (ballY <= 0 || ballY >= SCREEN_HEIGHT - ballSize) {
    ballSpeedY = -ballSpeedY;  // Ändere die Richtung des Balls vertikal
  }

  // Überprüfe, ob der Ball das linke oder rechte Ende des Bildschirms erreicht hat
  if (ballX <= 0) {
    player2Score++;  // Spieler 2 bekommt einen Punkt
    resetBall();      // Setze den Ball zurück
  } else if (ballX >= SCREEN_WIDTH - ballSize) {
    player1Score++;  // Spieler 1 bekommt einen Punkt
    resetBall();      // Setze den Ball zurück
  }
}

// Funktion zum Zurücksetzen der Ballposition und -geschwindigkeit
void resetBall() {
  ballX = SCREEN_WIDTH / 2;
  ballY = SCREEN_HEIGHT / 
