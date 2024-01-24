#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET    -1
#define OLED_ADDR     0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int paddle1Y, paddle2Y;
int paddleHeight = 12;
int paddleWidth = 2;
int paddleSpeed = 2;

int ballX, ballY;
int ballSize = 2;
int ballSpeedX, ballSpeedY;

int player1Score = 0, player2Score = 0;
int winningScore = 4;
int scoreDifference = 2;

const int paddle1UpButton = 10;
const int paddle1DownButton = 11;
const int paddle2UpButton = 8;
const int paddle2DownButton = 9;

void setup() {
  pinMode(OLED_RESET, OUTPUT);
  digitalWrite(OLED_RESET, LOW);
  delay(50);
  digitalWrite(OLED_RESET, HIGH);

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) {}
  }

  display.display();
  delay(2000);

  display.clearDisplay();
  display.display();
  delay(2000);

  paddle1Y = paddle2Y = (SCREEN_HEIGHT - paddleHeight) / 2;

  pinMode(paddle1UpButton, INPUT_PULLUP);
  pinMode(paddle1DownButton, INPUT_PULLUP);
  pinMode(paddle2UpButton, INPUT_PULLUP);
  pinMode(paddle2DownButton, INPUT_PULLUP);


  displayCountdown(3);
}

void loop() {
  if (player1Score >= winningScore && player1Score >= player2Score + scoreDifference) {
    displayEndScreen(player1Score, player2Score);
    waitForButtonPress();
    resetGame();
    displayCountdown(3);
  } else if (player2Score >= winningScore && player2Score >= player1Score + scoreDifference) {
    displayEndScreen(player1Score, player2Score);
    waitForButtonPress();
    resetGame();
    displayCountdown(3);
  } else {
    updatePaddles();
    updateBall();

    display.clearDisplay();
    drawPaddles();
    drawBall();
    drawScores();
    display.display();
  }
}

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

void updateBall() {
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  if (ballX <= paddleWidth && ballY >= paddle1Y && ballY <= paddle1Y + paddleHeight) {
    ballSpeedX = -ballSpeedX;
  }
  if (ballX >= SCREEN_WIDTH - paddleWidth - ballSize && ballY >= paddle2Y && ballY <= paddle2Y + paddleHeight) {
    ballSpeedX = -ballSpeedX;
  }

  if (ballY <= 0 || ballY >= SCREEN_HEIGHT - ballSize) {
    ballSpeedY = -ballSpeedY;
  }

  if (ballX <= 0) {
    player2Score++;
    resetBall();
  } else if (ballX >= SCREEN_WIDTH - ballSize) {
    player1Score++;
    resetBall();
  }
}

void resetBall() {
  ballX = SCREEN_WIDTH / 2;
  ballY = SCREEN_HEIGHT / 2;
  ballSpeedX = random(1, 3);
  ballSpeedY = random(1, 3);

  if (random(2) == 0) {
    ballSpeedX = -ballSpeedX;
  }

  if (random(2) == 0) {
    ballSpeedY = -ballSpeedY;
  }
}

void drawPaddles() {
  display.fillRect(0, paddle1Y, paddleWidth, paddleHeight, WHITE);
  display.fillRect(SCREEN_WIDTH - paddleWidth, paddle2Y, paddleWidth, paddleHeight, WHITE);
}

void drawBall() {
  display.fillRect(ballX, ballY, ballSize, ballSize, WHITE);
}

void drawScores() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH / 2 - 35, 5);
  display.print("Punkte: ");
  display.print(player1Score);
  display.print(" - ");
  display.print(player2Score);
}

void displayEndScreen(int score1, int score2) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);

  if (score1 > score2) {
    display.print("Spieler 1 gewinnt!");
  } else {
    display.print("Spieler 2 gewinnt!");
  }

  display.setTextSize(1);
  display.setCursor(12, 55);
  display.print("Punkte: ");
  display.print(score1);
  display.print(" - ");
  display.print(score2);
}

void waitForButtonPress() {
  display.display();
  delay(500);

  while (digitalRead(paddle1UpButton) == HIGH && digitalRead(paddle1DownButton) == HIGH &&
         digitalRead(paddle2UpButton) == HIGH && digitalRead(paddle2DownButton) == HIGH) {
  }

  while (digitalRead(paddle1UpButton) == LOW || digitalRead(paddle1DownButton) == LOW ||
         digitalRead(paddle2UpButton) == LOW || digitalRead(paddle2DownButton) == LOW) {
  }
}

void resetGame() {
  player1Score = player2Score = 0;
  resetBall();
  paddle1Y = paddle2Y = (SCREEN_HEIGHT - paddleHeight) / 2;

  display.clearDisplay();
  display.display();
}

void displayCountdown(int seconds) {
  for (int i = seconds; i > 0; i--) {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor((SCREEN_WIDTH - 20) / 2, (SCREEN_HEIGHT - 20) / 2);
    display.print(i);
    display.display();
    delay(1000);
  }
  display.clearDisplay();
  display.display();
}
