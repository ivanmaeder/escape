#include <Gamer.h>

/*
TODO Looks like bitmap can be dropped (the screen is not updated until updateScreen()
TODO Make JUMP_LENGTH the actual length of the jump (airtime should increment whilst ascending and descending)
TODO Test isPressed (this could simplify the jumping code)
TODO Try printImage for obstacles

 */

/* The maximum jump height the runner can achieve. Change this number to
 make the runner jump higher or lower.
 */
const int JUMP_HEIGHT = 3;

/* The maximum amount of time the runner can stay in the air. For super
 long jumps, make this number really high.
 */
const int JUMP_LENGTH = 2;

/* This value controls the running speed at the beginning of the game. The
 higher the number, the faster the runner will start.

 BEWARE! The runner goes faster and faster as the game progresses.
 */
const int INITIAL_SPEED = 60;

const int MIN_X = 0, MAX_X = 7, MIN_Y = 0, MAX_Y = 7;

struct Coordinate {
  char x;
  char y;
};

struct Sprite {
  Coordinate coordinate;
};

typedef enum {
  RUNNING,
  ASCENDING,
  DESCENDING
} JumpPosition;

Gamer g;

/* These are the objects that can move on the screen. */
Sprite runner;
Sprite obstacle1, obstacle2;

/* These variables are used by the program to keep track of state. For
 example, the score variable is incremented by one every time the runner
 jumps over an obstacle.
 */
JumpPosition jumpPosition;
byte airtime;
int score;
bool isAudioOn = true;

byte bitmap[8][8];

void setup() {
  g.begin();
  randomSeed(g.ldrValue());

  /* Initialise (or reset) state variables. */
  jumpPosition = RUNNING;
  airtime = 0;
  score = 0;

  /* Place sprites at their starting positions. */
  runner.coordinate = { 1, 6 };
  obstacle1.coordinate = { 8, 6 };
  obstacle2.coordinate = { 16, 6 };
}

void loop() {
  playPauseHandler();
  updateGameState();
  collisionDetectionHandler();
  updateScreen();

  delay(200 - INITIAL_SPEED - score);
}

void playPauseHandler() {
  if (g.isHeld(START)) {
    delay(500);

    while (!g.isHeld(START)) {
      if (g.isHeld(LEFT)) {
        isAudioOn = !isAudioOn;
        
        if (isAudioOn)
          g.printString("SOUND ON");
        else
          g.printString("SOUND OFF");
          
        memcpy(g.display, bitmap, sizeof(bitmap));
        g.updateDisplay();
      }
    }
  }
}

void collisionDetectionHandler() {
  if (collisionDetected()) {
    crashSound();
    flashScreen();

    g.showScore(min(score, 99));
    delay(1000);

    /* Restart the game. */
    setup();
  }
}

bool collisionDetected() {
  return isCoordinateEqual(runner.coordinate, obstacle1.coordinate) || isCoordinateEqual(runner.coordinate, obstacle2.coordinate);
}

bool isCoordinateEqual(struct Coordinate a, struct Coordinate b) {
  return a.x == b.x && a.y == b.y;
}

void flashScreen() {
  for (int i = 0; i < 2; i++) {
    g.clear();
    delay(300);

    memcpy(g.display, bitmap, sizeof(bitmap));
    g.updateDisplay();
    delay(300);
  }
}

void updateGameState() {
  obstacle1.coordinate.x--;
  if (obstacle1.coordinate.x < 0) {
    obstacle1.coordinate.x = random(max(obstacle2.coordinate.x, 7), obstacle2.coordinate.x + 16);
    score++;
  }

  obstacle2.coordinate.x--;
  if (obstacle2.coordinate.x < 0) {
    obstacle2.coordinate.x = random(max(obstacle1.coordinate.x, 7), obstacle1.coordinate.x + 16);
    score++;
  }

  updateJumpPosition();
}

void updateJumpPosition() {
  if (isJumping())
    airtime++;

  if (airtime >= JUMP_HEIGHT + JUMP_LENGTH)
    mustDescend();

  if (!isFalling() && g.isHeld(UP)) {
    if (isRunning())
      mustAscend();

    if (isJumping() && isBelowMaximumJumpHeight())
        ascend();
  }
  else {
    airtime = 0;

    if (isJumping())
      mustDescend();

    if (isFalling()) {
      if (runner.coordinate.y < 6)
        descend();
      else
        mustRun();
    }
  }
}

bool isRunning() {
  return jumpPosition == RUNNING;
}

bool isJumping() {
  return jumpPosition == ASCENDING;
}

bool isFalling() {
  return jumpPosition == DESCENDING;
}

void mustRun() {
  jumpPosition = RUNNING;
}

void mustAscend() {
  jumpSound();
  jumpPosition = ASCENDING;
}

void mustDescend() {
  jumpPosition = DESCENDING;
}

void ascend() {
  /* Because the top row of the screen is zero (the y-value is 0), the lower
   the y-value, the higher the row.

   The row on the bottom of the screen has a y-value of 7.
   */
   runner.coordinate.y--;
}

void descend() {
  /* The lower the y-value, the higher the screen row. */
  runner.coordinate.y++;
}

bool isBelowMaximumJumpHeight() {
  const int FLOOR_HEIGHT = 1;

  return runner.coordinate.y > (MAX_Y - FLOOR_HEIGHT - JUMP_HEIGHT);
}

void updateScreen() {
  clearBitmap();
  updateBitmapWithGameState();

  memcpy(g.display, bitmap, sizeof(bitmap));
  g.updateDisplay();
}

void clearBitmap() {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      bitmap[i][j] = 0;
}

void updateBitmapWithGameState() {
  for (int i = 0; i < 8; i++)
    activateBit(Coordinate { i, 7 });

  activateBit(runner.coordinate);
  activateBit(obstacle1.coordinate);
  activateBit(obstacle2.coordinate);
}

void activateBit(Coordinate coordinate) {
  if (coordinate.x >= 0 && coordinate.x < 8 && coordinate.y >= 0 && coordinate.y < 8)
    bitmap[coordinate.x][coordinate.y] = 1;
}

void jumpSound() {
  if (isAudioOn) {
    g.playTone(200);
    delay(20);
    g.stopTone();
  }
}

void crashSound() {
  if (isAudioOn) {
    for (int t = 200; t < 255; t++) {
      g.playTone(t);
      delay(10);
    }

    g.stopTone();
  }
}
