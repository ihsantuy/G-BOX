/*MIT License

Copyright (c) [2025] [ihsantuy]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Button2.h>

// Pin untuk OLED dan Tombol
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST -1
#define BUTTON_ACC 13    // Putar / Tembak / Lompat
#define BUTTON_LEFT 12   // Kiri
#define BUTTON_RIGHT 2    // Kanan
#define BUTTON_MENU 16    // Menu

// Ukuran OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Deklarasi Objek
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
Button2 buttonAcc(BUTTON_ACC);
Button2 buttonLeft(BUTTON_LEFT);
Button2 buttonRight(BUTTON_RIGHT);
Button2 buttonMenu(BUTTON_MENU);

// Variabel Game
int gameStage = 0;
int menuIndex = 0;
String menuItems[] = {"Space Shooter", "Asteroids", "Dino Game", "Space Invaders", "Flappy Bird", "Kembali"}; // "Asteroids" ditambahkan, "Tetris" dihilangkan, "Space Invaders" dan "Flappy Bird" ditambahkan
bool gameRunning = false;
unsigned long lastUpdate = 0;
int score = 0;
int gameSelected = 0;
unsigned long menuDebounce = 200;
unsigned long lastMenuChange = 0;
bool menuReady = false;

// Variabel Space Shooter
int playerX;
int playerY;
int playerSize = 7;
int bulletX[5];
int bulletY[5];
bool bulletActive[5];
int enemyX[5];
int enemyY[5];
bool enemyActive[5];
int enemySpeed = 1;
int bulletSpeed = 7;
int shootDelay = 200;
unsigned long lastFireTime = 0;
int playerFlameLength = 0;
unsigned long lastFlameChange = 0;
int flameInterval = 80;
int starX[20];
int starY[20];
int bgStarX[10];
int bgStarY[10];
int bgStarSpeed[10];

// Variabel Asteroids
float asteroidX[10];
float asteroidY[10];
float asteroidSpeedX[10];
float asteroidSpeedY[10];
int asteroidSize[10];
bool asteroidActive[10];
int numAsteroids = 3;
int playerAsteroidX;
int playerAsteroidY;
float playerAngle = 0;
float playerSpeed = 2;
float playerRotationSpeed = 0.1;
bool isAccelerating = false;
bool isRotatingLeft = false;
bool isRotatingRight = false;
int asteroidBulletX[5];
int asteroidBulletY[5];
bool asteroidBulletActive[5];
float asteroidBulletAngle[5];
float asteroidBulletSpeed = 5;
unsigned long lastAsteroidFireTime = 0;
int asteroidShootDelay = 300; // Reduced delay for faster shooting
int lives = 3;
bool gameOverAsteroid = false; // Variabel untuk game over Asteroids
unsigned long gameOverTime;
bool respawnPlayer = false;
unsigned long respawnTime;
#define RESPAWN_DELAY 3000
#define MAX_BULLETS 5

// Variabel Dino Game
int dinoX = 20;
int dinoY = 40;
int dinoHeight = 16;
int dinoWidth = 8;
int dinoSpeedY = 10;
int gravity = 2;
bool isJumping = false;
unsigned long jumpStartTime = 0;
int obstacleX = 120;
int obstacleY = 40;
int obstacleWidth = 8;
int obstacleHeight = 8;
int obstacleSpeed = 2;
bool gameDinoOver = false;
unsigned long dinoLastUpdate = 0;
unsigned long obstacleLastUpdate = 0;
unsigned long scoreDinoLastUpdate = 0;
int dinoScore = 0;
int floorY = 48; // Lantai game dino
bool dinoCrouching = false;
int dinoFrame = 0;     // Frame animasi dino
unsigned long dinoFrameTime = 100; // Kecepatan animasi dino
unsigned long lastDinoFrameChange = 0;
int cloudX[3];     // Posisi awan
int cloudY[3];
int cloudSpeed[3];
unsigned long lastCloudUpdate = 0;
int parallaxFactor = 4; // Faktor paralaks untuk awan

// Variabel Space Invaders
int invaderX[8][5];
int invaderY[8][5];
bool invaderActive[8][5];
int invaderSpeedX = 1;
int invaderSpeedY = 5;
int invaderDirection = 1;
int invaderSize = 8;
int invaderBulletX[10];
int invaderBulletY[10];
bool invaderBulletActive[10];
int invaderBulletSpeed = 3;
unsigned long lastInvaderFireTime = 0;
int invaderFireRate = 500; //ms
int numInvadersAlive = 8 * 5;
bool gameOverInvader = false;
unsigned long gameOverInvaderTime;

// Variabel Flappy Bird
int birdX = 30;
int birdY = SCREEN_HEIGHT / 2;
int birdRadius = 3;
int birdSpeedYBird = 0;
int birdAcceleration = 1;
int pipeX[3];
int pipeYTop[3];
int pipeYBottom[3];
int pipeWidth = 10;
int pipeGap = 20;
int pipeSpeed = 2;
bool gameFlappyOver = false;
unsigned long flappyLastUpdate = 0;
int flappyScore = 0;
unsigned long gameOverFlappyTime;
bool passPipe[3]; // Track if bird has passed each pipe
bool gameStarted = false; // Track if the game has started
unsigned long gameStartTimeFlappy; // For initial delay

// Definisi Tetromino - Tidak digunakan di game ini
const int tetrominoShapes[7][4][4][2] = {
  // I-shape
  {{{0, 0}, {0, 1}, {0, 2}, {0, 3}},
   {{0, 1}, {1, 1}, {2, 1}, {3, 1}},
   {{0, 0}, {0, 1}, {0, 2}, {0, 3}},
   {{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
  // J-shape
  {{{0, 0}, {1, 0}, {1, 1}, {1, 2}},
   {{0, 1}, {1, 1}, {2, 1}, {2, 0}},
   {{1, 2}, {0, 2}, {0, 1}, {0, 0}},
   {{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
  // L-shape
  {{{0, 2}, {1, 2}, {1, 1}, {1, 0}},
   {{2, 1}, {1, 1}, {0, 1}, {0, 0}},
   {{0, 0}, {0, 1}, {0, 2}, {1, 0}},
   {{0, 2}, {1, 2}, {2, 2}, {2, 1}}},
  // O-shape
  {{{0, 0}, {0, 1}, {1, 0}, {1, 1}},
   {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
   {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
   {{0, 0}, {0, 1}, {1, 0}, {1, 1}}},
  // S-shape
  {{{0, 1}, {0, 2}, {1, 0}, {1, 1}},
   {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
   {{0, 1}, {0, 2}, {1, 0}, {1, 1}},
   {{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
  // T-shape
  {{{0, 1}, {1, 0}, {1, 1}, {1, 2}},
   {{0, 1}, {1, 1}, {1, 0}, {2, 1}},
   {{1, 0}, {1, 1}, {1, 2}, {2, 1}},
   {{0, 1}, {1, 1}, {1, 2}, {2, 1}}},
  // Z-shape
  {{{0, 0}, {0, 1}, {1, 1}, {1, 2}},
   {{0, 1}, {1, 0}, {1, 1}, {2, 0}},
   {{0, 0}, {0, 1}, {1, 1}, {1, 2}},
   {{0, 1}, {1, 0}, {1, 1}, {2, 0}}}
};

// Fungsi untuk menggambar pesawat Space Shooter
void drawSpaceShooterPlayer() {
  // Gambar pesawat yang lebih detail, seperti pesawat tempur
  display.fillTriangle(playerX, playerY, playerX - 5, playerY + playerSize, playerX + 5, playerY + playerSize, WHITE);
  display.fillRect(playerX - 3, playerY + playerSize, 6, 3, WHITE);
  display.drawLine(playerX, playerY + 2, playerX, playerY + 6, 0xAAAAAA);
  display.drawLine(playerX - 5, playerY + 3, playerX - 7, playerY + 1, WHITE);
  display.drawLine(playerX + 5, playerY + 3, playerX + 7, playerY + 1, WHITE);

  if (playerFlameLength > 0) {
    for (int i = 0; i < playerFlameLength; i++) {
      int flameY = playerY + playerSize + i;
      int flameX1 = playerX - 2 + random(-1, 2);
      int flameX2 = playerX + 2 + random(-1, 2);
      if (i % 2 == 0)
        display.drawLine(flameX1, flameY, flameX2, flameY, WHITE);
      else
        display.drawLine(flameX1, flameY, flameX2, flameY, WHITE);
    }
  }
}

// Fungsi untuk mengupdate posisi pesawat Space Shooter
void updateSpaceShooterPlayer() {
  if (buttonLeft.isPressed()) {
    playerX -= 3;
    if (playerX < playerSize) playerX = playerSize;
  }
  if (buttonRight.isPressed()) {
    playerX += 3;
    if (playerX > SCREEN_WIDTH - playerSize) playerX = SCREEN_WIDTH - playerSize;
  }
  if (buttonAcc.isPressed() && millis() - lastFireTime > shootDelay) {
    for (int i = 0; i < 5; i++) {
      if (!bulletActive[i]) {
        bulletX[i] = playerX;
        bulletY[i] = playerY - playerSize / 2;
        bulletActive[i] = true;
        lastFireTime = millis();
        break;
      }
    }
    playerFlameLength = 3;
    lastFlameChange = millis();
  }

  if (millis() - lastFlameChange > flameInterval) {
    playerFlameLength = random(0, 4);
    lastFlameChange = millis();
  }
}

// Fungsi untuk menggambar peluru Space Shooter
void drawSpaceShooterBullets() {
  for (int i = 0; i < 5; i++) {
    if (bulletActive[i]) {
      display.drawFastVLine(bulletX[i], bulletY[i], 10, 0x00FFFF);
      display.drawPixel(bulletX[i], bulletY[i], 0xFFFFFF);
    }
  }
}

// Fungsi untuk mengupdate posisi peluru Space Shooter
void updateSpaceShooterBullets() {
  for (int i = 0; i < 5; i++) {
    if (bulletActive[i]) {
      bulletY[i] -= bulletSpeed;
      if (bulletY[i] < 0) {
        bulletActive[i] = false;
      }
    }
  }
}

// Fungsi untuk menggambar musuh Space Shooter
void drawSpaceShooterEnemies() {
  for (int i = 0; i < 5; i++) {
    if (enemyActive[i]) {
      // Menggambar musuh dengan bentuk yang lebih menarik
      display.fillTriangle(enemyX[i], enemyY[i], enemyX[i] - 4, enemyY[i] + 4, enemyX[i] + 4, enemyY[i] + 4, WHITE); // Kepala
      display.fillRect(enemyX[i] - 4, enemyY[i] + 4, 8, 6, WHITE);       // Badan
      display.drawFastHLine(enemyX[i] - 6, enemyY[i] + 2, 4, WHITE);       // Sayap Kiri
      display.drawFastHLine(enemyX[i] + 2, enemyY[i] + 2, 4, WHITE);       // Sayap Kanan
      display.drawPixel(enemyX[i] - 2, enemyY[i] + 6, BLACK);         // Mata Kiri
      display.drawPixel(enemyX[i] + 2, enemyY[i] + 6, BLACK);         // Mata Kanan
    }
  }
}

// Fungsi untuk mengupdate posisi musuh Space Shooter
void updateSpaceShooterEnemies() {
  for (int i = 0; i < 5; i++) {
    if (enemyActive[i]) {
      enemyY[i] += enemySpeed;
      if (enemyY[i] > SCREEN_HEIGHT) {
        enemyY[i] = -15;
        enemyX[i] = random(10, SCREEN_WIDTH - 10);
      }
    }
  }
}

// Fungsi untuk membuat musuh Space Shooter
void createSpaceShooterEnemies() {
  for (int i = 0; i < 5; i++) {
    if (!enemyActive[i]) {
      enemyX[i] = random(10, SCREEN_WIDTH - 10);
      enemyY[i] = -15;
      enemyActive[i] = true;
      break;
    }
  }
}

// Fungsi untuk cek tabrakan Space Shooter
bool checkSpaceShooterCollision() {
  for (int i = 0; i < 5; i++) {
    if (bulletActive[i]) {
      for (int j = 0; j < 5; j++) {
        if (enemyActive[j]) {
          if (abs(bulletX[i] - enemyX[j]) < 6 && abs(bulletY[i] - enemyY[j]) < 8) {
            bulletActive[i] = false;
            enemyActive[j] = false;
            score += 20;
            return true;
          }
        }
      }
    }
  }
  return false;
}

// Asteroids Game Functions

// Function to draw the player's spaceship
void drawPlayerAsteroid() {
  if (!respawnPlayer) { // Only draw if not respawning
    display.drawTriangle(
      playerAsteroidX, playerAsteroidY - 10,
      playerAsteroidX - 8, playerAsteroidY + 8,
      playerAsteroidX + 8, playerAsteroidY + 8,
      WHITE
    );
  }
}

// Function to update the player's position
void updatePlayerAsteroid() {
  if (isAccelerating) {
    playerAsteroidX += cos(playerAngle) * playerSpeed;
    playerAsteroidY += sin(playerAngle) * playerSpeed;
  }

  if (isRotatingLeft) {
    playerAngle -= playerRotationSpeed;
  }
  if (isRotatingRight) {
    playerAngle += playerRotationSpeed;
  }

  // Wrap around the screen
  if (playerAsteroidX > SCREEN_WIDTH) playerAsteroidX = 0;
  if (playerAsteroidX < 0) playerAsteroidX = SCREEN_WIDTH;
  if (playerAsteroidY > SCREEN_HEIGHT) playerAsteroidY = 0;
  if (playerAsteroidY < 0) playerAsteroidY = SCREEN_HEIGHT;
}

// Function to draw the asteroids
void drawAsteroids() {
  for (int i = 0; i < numAsteroids; i++) {
    if (asteroidActive[i]) {
      display.drawCircle(asteroidX[i], asteroidY[i], asteroidSize[i], WHITE);
    }
  }
}

// Function to update the asteroids' positions
void updateAsteroids() {
  for (int i = 0; i < numAsteroids; i++) {
    if (asteroidActive[i]) {
      asteroidX[i] += asteroidSpeedX[i];
      asteroidY[i] += asteroidSpeedY[i];

      // Wrap around the screen
      if (asteroidX[i] > SCREEN_WIDTH + asteroidSize[i]) asteroidX[i] = -asteroidSize[i];
      if (asteroidX[i] < -asteroidSize[i]) asteroidX[i] = SCREEN_WIDTH + asteroidSize[i];
      if (asteroidY[i] > SCREEN_HEIGHT + asteroidSize[i]) asteroidY[i] = -asteroidSize[i];
      if (asteroidY[i] < -asteroidSize[i]) asteroidY[i] = SCREEN_HEIGHT + asteroidSize[i];
    }
  }
}

// Function to initialize the asteroids
void initAsteroids() {
  for (int i = 0; i < numAsteroids; i++) {
    asteroidX[i] = random(SCREEN_WIDTH);
    asteroidY[i] = random(SCREEN_HEIGHT / 2); // Start in the top half
    asteroidSize[i] = random(10, 20);
    asteroidSpeedX[i] = (float)random(-20, 20) / 10;
    asteroidSpeedY[i] = (float)random(10, 50) / 10; // Bias speed downwards
    asteroidActive[i] = true;
  }
}

// Function to handle shooting bullets
void shootAsteroidBullet() {
  if (millis() - lastAsteroidFireTime > asteroidShootDelay) {
    for (int i = 0; i < MAX_BULLETS; i++) {
      if (!asteroidBulletActive[i]) {
        asteroidBulletX[i] = playerAsteroidX;
        asteroidBulletY[i] = playerAsteroidY;
        asteroidBulletAngle[i] = playerAngle;
        asteroidBulletActive[i] = true;
        lastAsteroidFireTime = millis();
        break;
      }
    }
  }
}

// Function to draw the bullets
void drawAsteroidBullets() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (asteroidBulletActive[i]) {
      display.drawPixel(asteroidBulletX[i], asteroidBulletY[i], WHITE);
    }
  }
}

// Function to update the bullets' positions
void updateAsteroidBullets() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (asteroidBulletActive[i]) {
      asteroidBulletX[i] += cos(asteroidBulletAngle[i]) * asteroidBulletSpeed;
      asteroidBulletY[i] += sin(asteroidBulletAngle[i]) * asteroidBulletSpeed;

      // Deactivate bullets that go off-screen
      if (asteroidBulletX[i] < 0 || asteroidBulletX[i] > SCREEN_WIDTH || asteroidBulletY[i] < 0 || asteroidBulletY[i] > SCREEN_HEIGHT) {
        asteroidBulletActive[i] = false;
      }
    }
  }
}

// Function to check for collisions between bullets and asteroids
void checkAsteroidBulletCollisions() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (asteroidBulletActive[i]) {
      for (int j = 0; j < numAsteroids; j++) {
        if (asteroidActive[j]) {
          float distance = sqrt(pow(asteroidBulletX[i] - asteroidX[j], 2) + pow(asteroidBulletY[i] - asteroidY[j], 2));
          if (distance < asteroidSize[j]) {
            asteroidBulletActive[i] = false;
            asteroidActive[j] = false;
            score += 10; // Award points
            // Create smaller asteroids if the asteroid is large enough
            if (asteroidSize[j] > 15) {
              //spawn smaller asteroids
              for (int k = 0; k < 2; k++) {
                int newAsteroid = -1;
                for (int l = 0; l < numAsteroids; l++) {
                  if (!asteroidActive[l]) {
                    newAsteroid = l;
                    break;
                  }
                }
                if (newAsteroid != -1) {
                  asteroidActive[newAsteroid] = true;
                  asteroidX[newAsteroid] = asteroidX[j];
                  asteroidY[newAsteroid] = asteroidY[j];
                  asteroidSize[newAsteroid] = asteroidSize[j] / 2;
                  asteroidSpeedX[newAsteroid] = (float)random(-30, 30) / 10;
                  asteroidSpeedY[newAsteroid] = (float)random(-30, 30) / 10;
                }
              }
            }
            break;
          }
        }
      }
    }
  }
}

// Function to check for collisions between the player and asteroids
void checkPlayerAsteroidCollisions() {
  if (respawnPlayer) return; // No collision during respawn

  for (int i = 0; i < numAsteroids; i++) {
    if (asteroidActive[i]) {
      float distance = sqrt(pow(playerAsteroidX - asteroidX[i], 2) + pow(playerAsteroidY - asteroidY[i], 2));
      if (distance < asteroidSize[i] + 10) { // Add a little extra for the ship's size
        lives--;
        if (lives <= 0) {
          gameOverAsteroid = true;
          gameRunning = false;
          gameStage = 4; // Game over screen
          gameOverTime = millis();
        } else {
          respawnPlayer = true;
          respawnTime = millis();
        }
        asteroidActive[i] = false; // Destroy the asteroid on collision
        break;
      }
    }
  }
}

void resetPlayerPosition() {
  playerAsteroidX = SCREEN_WIDTH / 2;
  playerAsteroidY = SCREEN_HEIGHT / 2;
  playerAngle = 0;
  respawnPlayer = false; // Reset respawn flag
}

// Function to draw the game over screen for Asteroids
void drawGameOverAsteroid() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 20);
  display.print("Game Over");
  display.setTextSize(1);
  display.setCursor(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 10);
  display.print("Score: ");
  display.print(score);
  display.setCursor(SCREEN_WIDTH / 2 - 65, SCREEN_HEIGHT / 2 + 20);
  display.print("Press A to Restart");
  display.display();
}

// Fungsi untuk menggambar Dino
void drawDino() {
  if (dinoCrouching) {
    display.fillRect(dinoX, dinoY + dinoHeight / 2, dinoWidth * 2, dinoHeight / 2, WHITE); // Gambar dino yang sedang menunduk
    display.drawRect(dinoX, dinoY + dinoHeight / 2, dinoWidth * 2, dinoHeight / 2, BLACK);
  } else {
    // Animasi lari dengan mengubah bentuk kaki
    if (dinoFrame == 0) {
      display.fillRect(dinoX, dinoY, dinoWidth, dinoHeight, WHITE); // Badan
      display.fillRect(dinoX + dinoWidth, dinoY + 2, 3, 3, WHITE);       // Kepala
      display.drawLine(dinoX, dinoY + dinoHeight / 2, dinoX - 4, dinoY + dinoHeight, WHITE); // Kaki 1
      display.drawLine(dinoX + dinoWidth / 2, dinoY + dinoHeight / 2, dinoX + dinoWidth / 2- 4, dinoY + dinoHeight, WHITE); // Kaki 2
    } else {
      display.fillRect(dinoX, dinoY, dinoWidth, dinoHeight, WHITE); // Badan
      display.fillRect(dinoX + dinoWidth, dinoY + 2, 3, 3, WHITE);       // Kepala
      display.drawLine(dinoX, dinoY + dinoHeight / 2, dinoX - 2, dinoY + dinoHeight - 2, WHITE); // Kaki 1
      display.drawLine(dinoX + dinoWidth / 2, dinoY + dinoHeight / 2, dinoX + dinoWidth / 2 - 2, dinoY + dinoHeight - 2, WHITE); // Kaki 2
    }
  }
}

// Fungsi untuk menggambar rintangan
void drawObstacle() {
  display.fillRect(obstacleX, obstacleY, obstacleWidth, obstacleHeight, WHITE);
  display.drawRect(obstacleX, obstacleY, obstacleWidth, obstacleHeight, BLACK);
}

// Fungsi untuk mengupdate posisi Dino
void updateDino() {
  if (!isJumping) {
    if (buttonAcc.isPressed()) {
      isJumping = true;
      dinoSpeedY = -10;
      jumpStartTime = millis();
      dinoCrouching = false;
    } else if (buttonLeft.isPressed() || buttonRight.isPressed()) {
      dinoCrouching = true;
    } else {
      dinoCrouching = false;
    }
  }

  if (isJumping) {
    dinoY += dinoSpeedY;
    dinoSpeedY += gravity;
    if (dinoY >= floorY - dinoHeight) {
      dinoY = floorY - dinoHeight;
      isJumping = false;
      dinoSpeedY = 0;
    }
  }
  if (!isJumping && (buttonLeft.isPressed() || buttonRight.isPressed())) {
    dinoCrouching = true;
  } else {
    dinoCrouching = false;
  }

  // Animasi lari
  if (millis() - lastDinoFrameChange > dinoFrameTime) {
    dinoFrame = (dinoFrame + 1) % 2; // Ganti frame antara 0 dan 1
    lastDinoFrameChange = millis();
  }
}

// Fungsi untuk mengupdate posisi rintangan
void updateObstacle() {
  obstacleX -= obstacleSpeed;
  if (obstacleX < -obstacleWidth) {
    obstacleX = SCREEN_WIDTH;
    dinoScore += 10;
  }
}

// Fungsi untuk menggambar awan
void drawClouds() {
  for (int i = 0; i < 3; i++) {
    display.fillCircle(cloudX[i], cloudY[i], 10, WHITE);
    display.fillCircle(cloudX[i] + 15, cloudY[i] - 5, 12, WHITE);
    display.fillCircle(cloudX[i] + 30, cloudY[i], 10, WHITE);
  }
}

// Fungsi untuk mengupdate posisi awan
void updateClouds() {
  unsigned long currentTime = millis();
  if (currentTime - lastCloudUpdate > 200) { // Update setiap 200ms
    for (int i = 0; i < 3; i++) {
      cloudX[i] -= cloudSpeed[i];
      if (cloudX[i] < -30) {
        cloudX[i] = SCREEN_WIDTH + random(0, 50);
      }
    }
    lastCloudUpdate = currentTime;
  }
}

// Fungsi untuk mengecek tabrakan Dino dengan rintangan
bool checkDinoCollision() {
  if (dinoCrouching) {
    if (dinoX < obstacleX + obstacleWidth && dinoX + dinoWidth * 2 > obstacleX && dinoY + dinoHeight / 2 < obstacleY + obstacleHeight && dinoY + dinoHeight > obstacleY) {
      return true;
    }
  } else {
    if (dinoX < obstacleX + obstacleWidth && dinoX + dinoWidth > obstacleX && dinoY < obstacleY + obstacleHeight && dinoY + dinoHeight > obstacleY) {
      return true;
    }
  }
  return false;
}

// Fungsi untuk menggambar skor Dino
void drawDinoScore() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Skor: ");
  display.print(dinoScore);
}

// Fungsi untuk menggambar skor
void drawScore() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Skor: ");
  display.print(score);
}

// Fungsi untuk menampilkan menu
void drawMenu() {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 30);
  display.print("Menu");
  display.setTextSize(1);
  for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); i++) {
    int itemY = SCREEN_HEIGHT / 2 - 10 + (i - menuIndex) * 20;
    if (itemY > -20 && itemY < SCREEN_HEIGHT + 20) {
      display.setCursor(SCREEN_WIDTH / 2 - 50, itemY);
      if (menuIndex == i) {
        display.setTextColor(BLACK, WHITE);
      } else {
        display.setTextColor(WHITE);
      }
      display.print(menuItems[i]);
    }
  }
  display.setTextColor(WHITE);
  if (gameRunning) {
    display.drawRect(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 40, 100, 100, WHITE);
  }
}

// Fungsi untuk inisialisasi OLED
void setupOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD106 alokasi gagal"));
    while (1);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("frimware:by @ihsantuy");
  display.display();
  delay(2000);
}

// Fungsi untuk inisialisasi tombol
void setupButtons() {
  pinMode(BUTTON_ACC, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_MENU, INPUT_PULLUP);
}

// Fungsi untuk menggambar Space Invaders
void drawInvaders() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 5; j++) {
      if (invaderActive[i][j]) {
        display.fillRect(invaderX[i][j], invaderY[i][j], invaderSize, invaderSize, WHITE);
        display.drawRect(invaderX[i][j], invaderY[i][j], invaderSize, invaderSize, BLACK);
      }
    }
  }
}

// Fungsi untuk mengupdate posisi Space Invaders
void updateInvaders() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 5; j++) {
      invaderX[i][j] += invaderSpeedX;
    }
  }

  if (invaderX[0][0] > SCREEN_WIDTH - invaderSize || invaderX[7][4] < 0) {
    invaderSpeedX *= -1;
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 5; j++) {
        invaderY[i][j] += invaderSpeedY;
      }
    }
  }
}

// Fungsi untuk membuat invader
void createInvaders() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 5; j++) {
      invaderX[i][j] = 10 + i * (invaderSize + 4);
      invaderY[i][j] = 10 + j * (invaderSize + 4);
      invaderActive[i][j] = true;
    }
  }
  numInvadersAlive = 8 * 5;
}

// Fungsi untuk menggambar peluru Invader
void drawInvaderBullets() {
  for (int i = 0; i < 10; i++) {
    if (invaderBulletActive[i]) {
      display.drawFastVLine(invaderBulletX[i], invaderBulletY[i], 5, WHITE);
    }
  }
}

// Fungsi untuk mengupdate peluru Invader
void updateInvaderBullets() {
  for (int i = 0; i < 10; i++) {
    if (invaderBulletActive[i]) {
      invaderBulletY[i] += invaderBulletSpeed;
      if (invaderBulletY[i] > SCREEN_HEIGHT) {
        invaderBulletActive[i] = false;
      }
    }
  }
}

// Fungsi untuk invader menembak
void invaderShoot() {
  if (millis() - lastInvaderFireTime > invaderFireRate) {
    for (int i = 0; i < 10; i++) {
      if (!invaderBulletActive[i]) {
        int shooterX = random(0, 8);
        int shooterY = random(0, 5);
        if(invaderActive[shooterX][shooterY]){
          invaderBulletX[i] = invaderX[shooterX][shooterY] + invaderSize / 2;
          invaderBulletY[i] = invaderY[shooterY][shooterY] + invaderSize / 2;
          invaderBulletActive[i] = true;
          lastInvaderFireTime = millis();
          break;
        }
      }
    }
  }
}

// Fungsi untuk cek tabrakan peluru player dengan invader
bool checkInvaderBulletCollision() {
  for (int i = 0; i < 5; i++) { // Player bullets
    if (bulletActive[i]) {
      for (int x = 0; x < 8; x++) { // Invaders
        for (int y = 0; y < 5; y++) {
          if (invaderActive[x][y]) {
            if (abs(bulletX[i] - invaderX[x][y]) < invaderSize && abs(bulletY[i] - invaderY[x][y]) < invaderSize) {
              bulletActive[i] = false;
              invaderActive[x][y] = false;
              score += 50;
              numInvadersAlive--;
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

// Fungsi untuk cek tabrakan peluru invader dengan player
bool checkPlayerInvaderCollision() {
  for (int i = 0; i < 10; i++) { // Invader bullets
    if (invaderBulletActive[i]) {
      if (abs(invaderBulletX[i] - playerX) < playerSize && abs(invaderBulletY[i] - playerY) < playerSize) {
        gameOverInvader = true;
        gameRunning = false;
        gameStage = 4;
        gameOverInvaderTime = millis();
        return true;
      }
    }
  }
  return false;
}

// Fungsi untuk menggambar game over screen Invaders
void drawGameOverInvader() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 20);
  display.print("Game Over");
  display.setTextSize(1);
  display.setCursor(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 10);
  display.print("Score: ");
  display.print(score);
  display.setCursor(SCREEN_WIDTH / 2 - 65, SCREEN_HEIGHT / 2 + 20);
  display.print("Press A to Restart");
  display.display();
}

// Fungsi untuk menggambar burung Flappy Bird
void drawBird() {
  display.fillCircle(birdX, birdY, birdRadius, WHITE);
  display.drawCircle(birdX, birdY, birdRadius, WHITE);
}

// Fungsi untuk mengupdate posisi burung Flappy Bird
void updateBird() {
  if (buttonAcc.isPressed() || !gameStarted) { // Make bird flap on button press or at start
    birdSpeedYBird = -6;
    gameStarted = true; // Set game started on first flap
    if (millis() - gameStartTimeFlappy < 1000) {
      birdSpeedYBird = 0;
    }
  }
  birdY += birdSpeedYBird;
  birdSpeedYBird += birdAcceleration;

  if (birdY < birdRadius) {
    birdY = birdRadius;
    birdSpeedYBird = 0;
  }
  if (birdY > SCREEN_HEIGHT - birdRadius) {
    birdY = SCREEN_HEIGHT - birdRadius;
    birdSpeedYBird = 0;
    gameFlappyOver = true;
    gameRunning = false;
    gameStage = 4;
    gameOverFlappyTime = millis();
  }
}

// Fungsi untuk menggambar pipa Flappy Bird
void drawPipes() {
  for (int i = 0; i < 3; i++) {
    display.fillRect(pipeX[i], 0, pipeWidth, pipeYTop[i], WHITE);
    display.fillRect(pipeX[i], pipeYBottom[i], pipeWidth, SCREEN_HEIGHT - pipeYBottom[i], WHITE);
    display.drawRect(pipeX[i], 0, pipeWidth, pipeYTop[i], WHITE);
    display.drawRect(pipeX[i], pipeYBottom[i], pipeWidth, SCREEN_HEIGHT - pipeYBottom[i], WHITE);
  }
}

// Fungsi untuk mengupdate posisi pipa Flappy Bird
void updatePipes() {
  for (int i = 0; i < 3; i++) {
    pipeX[i] -= pipeSpeed;
    if (pipeX[i] < -pipeWidth) {
      pipeX[i] = SCREEN_WIDTH + 2 * pipeWidth;
      pipeYTop[i] = random(10, SCREEN_HEIGHT / 3);
      pipeYBottom[i] = pipeYTop[i] + pipeGap;
      passPipe[i] = false; // Reset passPipe status
    }
  }
}

// Fungsi untuk membuat pipa Flappy Bird
void createPipes() {
  for (int i = 0; i < 3; i++) {
    pipeX[i] = SCREEN_WIDTH + i * (SCREEN_WIDTH / 3);
    pipeYTop[i] = random(10, SCREEN_HEIGHT / 3);
    pipeYBottom[i] = pipeYTop[i] + pipeGap;
    passPipe[i] = false;
  }
}

// Fungsi untuk cek tabrakan Flappy Bird dengan pipa
bool checkFlappyBirdCollision() {
  for (int i = 0; i < 3; i++) {
    if (birdX + birdRadius > pipeX[i] && birdX - birdRadius < pipeX[i] + pipeWidth) {
      if (birdY - birdRadius < pipeYTop[i] || birdY + birdRadius > pipeYBottom[i]) {
        return true;
      }
    }
  }
  return false;
}

// Fungsi untuk update skor Flappy Bird
void updateFlappyBirdScore() {
  for (int i = 0; i < 3; i++) {
    if (birdX > pipeX[i] + pipeWidth && !passPipe[i]) {
      flappyScore++;
      passPipe[i] = true;
    }
  }
}

// Fungsi untuk menggambar skor Flappy Bird
void drawFlappyBirdScore() {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH / 2 - 20, 10);
  display.print(flappyScore);
}

// Function to draw the game over screen for Flappy Bird
void drawGameOverFlappy() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 20);
  display.print("Game Over");
  display.setTextSize(1);
  display.setCursor(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 + 10);
  display.print("Score: ");
  display.print(flappyScore);
  display.setCursor(SCREEN_WIDTH / 2 - 65, SCREEN_HEIGHT / 2 + 20);
  display.print("Press A to Restart");
  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  setupOLED();
  setupButtons();
  randomSeed(analogRead(0));
  for (int i = 0; i < 5; i++) {
    bulletActive[i] = false;
  }
  for (int i = 0; i < 5; i++) {
    enemyActive[i] = false;
  }
  playerX = SCREEN_WIDTH / 2;
  playerY = SCREEN_HEIGHT - 20;
  // Inisialisasi variabel Asteroids
  playerAsteroidX = SCREEN_WIDTH / 2;
  playerAsteroidY = SCREEN_HEIGHT / 2;
  initAsteroids();
  lives = 3;
  gameOverAsteroid = false;
  respawnPlayer = false;
  // Inisialisasi posisi bintang latar belakang untuk Space Shooter
  for (int i = 0; i < 10; i++) {
    bgStarX[i] = random(0, SCREEN_WIDTH);
    bgStarY[i] = random(0, SCREEN_HEIGHT);
    bgStarSpeed[i] = random(1, 3);
  }
  // Inisialisasi bintang untuk Space Shooter
  for (int i = 0; i < 20; i++) {
    starX[i] = random(0, SCREEN_WIDTH);
    starY[i] = random(0, SCREEN_HEIGHT);
  }
  // Inisialisasi awan untuk Dino Game
  for (int i = 0; i < 3; i++) {
    cloudX[i] = random(0, SCREEN_WIDTH);
    cloudY[i] = random(10, 30);
    cloudSpeed[i] = random(1, 2);
  }
  //inisialisasi Space Invaders
  createInvaders();
  for (int i = 0; i < 10; i++) {
    invaderBulletActive[i] = false;
  }
  gameOverInvader = false;
  //inisialisasi Flappy Bird
  createPipes();
  gameFlappyOver = false;
  flappyScore = 0;
  gameStarted = false;
  gameStartTimeFlappy = millis();
}

void loop() {
  buttonAcc.loop();
  buttonLeft.loop();
  buttonRight.loop();
  buttonMenu.loop();

  unsigned long currentTime = millis();
  if (currentTime - lastUpdate >= 100) {
    if (gameStage == 2) {
      updateSpaceShooterPlayer();
      updateSpaceShooterBullets();
      updateSpaceShooterEnemies();
      createSpaceShooterEnemies();
      checkSpaceShooterCollision();
      lastUpdate = currentTime;
    } else if (gameStage == 3) { // Asteroids
      updatePlayerAsteroid();
      updateAsteroids();
      updateAsteroidBullets();
      checkAsteroidBulletCollisions();
      checkPlayerAsteroidCollisions(); // Check collisions
      if (buttonAcc.isPressed()) {
        shootAsteroidBullet();
      }
      isAccelerating = buttonAcc.isPressed();
      isRotatingLeft = buttonLeft.isPressed();
      isRotatingRight = buttonRight.isPressed();
      lastUpdate = currentTime;
    } else if (gameStage == 5) { // Dino Game
      updateDino();
      updateObstacle();
      updateClouds(); // Update posisi awan
      if (checkDinoCollision()) {
        gameDinoOver = true;
        gameRunning = false;
        gameStage = 4;
      }
      lastUpdate = currentTime;
    } else if (gameStage == 6) { // Space Invaders
      updateSpaceShooterPlayer();
      updateInvaders();
      updateInvaderBullets();
      invaderShoot();
      checkInvaderBulletCollision();
      checkPlayerInvaderCollision();
      lastUpdate = currentTime;
    } else if (gameStage == 7) { // Flappy Bird
      if (gameStarted) {
        updateBird();
        updatePipes();
        updateFlappyBirdScore();
        if (checkFlappyBirdCollision()) {
          gameFlappyOver = true;
          gameRunning = false;
          gameStage = 4;
          gameOverFlappyTime = millis();
        }
      } else {
        updateBird(); //update bird only to make it fall
      }
      lastUpdate = currentTime;
    }
  }

  display.clearDisplay();
  if (gameStage == 0) {
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 - 10);
    display.print("G-BOX");
    display.setTextSize(1);
    display.setCursor(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 10);
    display.print("Tekan A untuk mulai");
    if (buttonAcc.isPressed()) {
      gameStage = 1;
      lastUpdate = millis();
    }
  } else if (gameStage == 1) {
    if (menuReady) {
      drawMenu();
      if (buttonAcc.isPressed() && millis() - lastMenuChange > menuDebounce) {
        gameSelected = menuIndex;
        if (gameSelected == 0) {
          gameStage = 2;
          gameRunning = true;
          lastUpdate = millis();
          score = 0;
          playerX = SCREEN_WIDTH / 2;
          playerY = SCREEN_HEIGHT - 20;
          for (int i = 0; i < 5; i++) {
            enemyActive[i] = false;
          }
          display.clearDisplay();
        } else if (gameSelected == 1) { // Asteroids
          gameStage = 3;
          gameRunning = true;
          lastUpdate = millis();
          score = 0;
          lives = 3;
          gameOverAsteroid = false;
          respawnPlayer = false;
          playerAsteroidX = SCREEN_WIDTH / 2;
          playerAsteroidY = SCREEN_HEIGHT / 2;
          playerAngle = 0;
          initAsteroids();
          display.clearDisplay();
        } else if (gameSelected == 2) { // Dino Game
          gameStage = 5;
          gameRunning = true;
          lastUpdate = millis();
          dinoScore = 0;
          obstacleX = SCREEN_WIDTH;
          gameDinoOver = false;
          display.clearDisplay();
        } else if (gameSelected == 3) { // Space Invaders
          gameStage = 6;
          gameRunning = true;
          lastUpdate = millis();
          score = 0;
          createInvaders();
          gameOverInvader = false;
          display.clearDisplay();
        } else if (gameSelected == 4) { // Flappy Bird
          gameStage = 7;
          gameRunning = true;
          lastUpdate = millis();
          flappyScore = 0;
          createPipes();
          gameFlappyOver = false;
          gameStarted = false;
          gameStartTimeFlappy = millis();
          display.clearDisplay();
        } else if (gameSelected == 5) {
          gameStage = 0;
          gameRunning = false;
          lastUpdate = millis();
          display.clearDisplay();
          menuReady = false;
        }
        lastMenuChange = millis();
      } else if (buttonLeft.isPressed() && millis() - lastMenuChange > menuDebounce) {
        menuIndex--;
        if (menuIndex < 0) {
          menuIndex = sizeof(menuItems) / sizeof(menuItems[0]) - 1;
        }
        lastMenuChange = millis();
      } else if (buttonRight.isPressed() && millis() - lastMenuChange > menuDebounce) {
        menuIndex++;
        if (menuIndex >= sizeof(menuItems) / sizeof(menuItems[0])) {
          menuIndex = 0;
        }
        lastMenuChange = millis();
      } else if(buttonAcc.isPressed() && buttonLeft.isPressed() && millis() - lastMenuChange > menuDebounce) { //tombol kembali
        menuIndex = 5;
        lastMenuChange = millis();
      }
    } else {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 - 10);
      display.print("G-BOX");
      display.display();
      if (buttonLeft.isPressed()) {
        menuReady = true;
        display.clearDisplay();
        lastUpdate = millis();
      }
    }
  } else if (gameStage == 2) {
    for (int i = 0; i < 10; i++) {
      bgStarY[i] += bgStarSpeed[i];
      if (bgStarY[i] > SCREEN_HEIGHT) {
        bgStarY[i] = 0;
        bgStarX[i] = random(0, SCREEN_WIDTH);
      }
      display.drawPixel(bgStarX[i], bgStarY[i], 0x404040);
    }
    for (int i = 0; i < 20; i++) {
      display.drawPixel(starX[i], starY[i], WHITE);
    }
    drawSpaceShooterPlayer();
    drawSpaceShooterBullets();
    drawSpaceShooterEnemies();
    drawScore();
  } else if (gameStage == 3) { // Asteroids
    if (!gameOverAsteroid) {
      drawPlayerAsteroid();
      drawAsteroids();
      drawAsteroidBullets();
      drawScore();
      display.setTextSize(1);
      display.setCursor(0, SCREEN_HEIGHT - 10);
      display.print("Lives: ");
      display.print(lives);
      if (respawnPlayer && millis() - respawnTime > RESPAWN_DELAY) {
        resetPlayerPosition();
      }
    } else {
      drawGameOverAsteroid(); // Show game over screen
      if (buttonAcc.isPressed()) {
        // Reset the game
        gameStage = 0;
        gameRunning = false;
        menuReady = false;
        gameOverAsteroid = false;
        score = 0;
        lives = 3;
        initAsteroids();
        playerAsteroidX = SCREEN_WIDTH / 2;
        playerAsteroidY = SCREEN_HEIGHT / 2;
        playerAngle = 0;
      }
    }
  } else if (gameStage == 5) { // Dino Game
    drawClouds(); // Gambar awan
    drawDino();
    drawObstacle();
    drawDinoScore();
    if (gameDinoOver) {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 - 10);
      display.print("Game Over");
      display.setTextSize(1);
      display.setCursor(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 10);
      display.print("Tekan A untuk main lagi");
      if (buttonAcc.isPressed()) {
        gameStage = 0;
        gameRunning = false;
        menuReady = false;
        gameDinoOver = false;
      }
    }
  } else if (gameStage == 6) { // Space Invaders
    if (!gameOverInvader) {
      drawInvaders();
      drawSpaceShooterPlayer();
      drawInvaderBullets();
      drawScore();
      if (numInvadersAlive == 0) {
        gameOverInvader = true;
        gameRunning = false;
        gameStage = 4;
        gameOverInvaderTime = millis();
      }
    } else {
      drawGameOverInvader();
      if (buttonAcc.isPressed()) {
        gameStage = 0;
        gameRunning = false;
        menuReady = false;
        gameOverInvader = false;
        score = 0;
        createInvaders();
        playerX = SCREEN_WIDTH / 2;
        playerY = SCREEN_HEIGHT - 20;
      }
    }
  } else if (gameStage == 7) { // Flappy Bird
    if (!gameFlappyOver) {
      drawBird();
      drawPipes();
      drawFlappyBirdScore();
    } else {
      drawGameOverFlappy();
      if (buttonAcc.isPressed()) {
        gameStage = 0;
        gameRunning = false;
        menuReady = false;
        gameFlappyOver = false;
        flappyScore = 0;
        createPipes();
        birdX = 30;
        birdY = SCREEN_HEIGHT / 2;
        birdSpeedYBird = 0;
        gameStarted = false;
        gameStartTimeFlappy = millis();
      }
    }
  } else if (gameStage == 4) { // General Game Over
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 - 10);
    display.print("Game Over");
    display.setTextSize(1);
    display.setCursor(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 10);
    display.print("Tekan A untuk main lagi");
    if (buttonAcc.isPressed()) {
      gameStage = 0;
      gameRunning = false;
      menuReady = false;
      gameOverAsteroid = false;
      gameDinoOver = false;
      gameOverInvader = false;
      gameFlappyOver = false;
      lives = 3;
      score = 0;
      flappyScore = 0;
      initAsteroids();
      playerAsteroidX = SCREEN_WIDTH / 2;
      playerAsteroidY = SCREEN_HEIGHT / 2;
      playerAngle = 0;
      createInvaders();
      playerX = SCREEN_WIDTH / 2;
      playerY = SCREEN_HEIGHT - 20;
      createPipes();
      birdX = 30;
      birdY = SCREEN_HEIGHT / 2;
      birdSpeedYBird = 0;
      gameStarted = false;
      gameStartTimeFlappy = millis();
    }
  }

  if (buttonMenu.isPressed()) {
    gameStage = 0;
    gameRunning = false;
    menuReady = false;
    display.clearDisplay();
    lastUpdate = millis();
  }

  display.display();
}
