/*
 Then
 ----
 4. More ideas:
    - Music!
    - Sound effects!
    - Pressing start asks to restart; pressing again confirms
    - Walls you can cross by ducking (press DOWN)
    - Gaps in floor you have to clear
    - Jumping up and down onto different levels
    - Score based on time playing
    - Winning! Being rewarded for finishing the game
        - It having an ending and leaving you wanting more
        - More can be: more games or making changes to the game yourself
        
        
NOTES
- The API global vars should have a prefix or be in an object. E.g., I tried
  making a var called "player" and got errors. This would be confusing to a novice
  programmer. This happens with Arduino stuff (e.g., 'FALLING') but we shouldn't
  add to the confusion.
- Score doesn't do numbers > 99
- 
 */
#include <Gamer.h>

struct Coordinate {
  char x;
  char y;
};

struct Sprite {
  Coordinate coordinate;
};

typedef enum {
  READY,
  ON_THE_WAY_UP,
  ON_THE_WAY_DOWN
} JumpPosition;

Gamer g;

Sprite runner;
Sprite obstacle1, obstacle2;
JumpPosition jump = READY;
byte airtime = 0;
int score;

byte bitmap[8][8];

void setup() {
  g.begin();
  randomSeed(g.ldrValue());
  
  Serial.begin(9600);

  score = 0;
  runner.coordinate = { 1, 6 };
  obstacle1.coordinate = { 8, 6 };
  obstacle2.coordinate = { 16, 6 };
}

void loop() {
  //we should be able to copy game state: so we can have the previous and the new
  updateGameState();

  if (isCoordinateEqual(runner.coordinate, obstacle1.coordinate) || isCoordinateEqual(runner.coordinate, obstacle2.coordinate)) {
    memcpy(bitmap, g.display, sizeof(bitmap));

    g.clear();
    delay(300);
    memcpy(g.display, bitmap, sizeof(bitmap));
    g.updateDisplay();
    delay(300);
    g.clear();
    delay(300);
    memcpy(g.display, bitmap, sizeof(bitmap));
    g.updateDisplay();
    delay(300);
    g.showScore(min(score, 99));
    delay(1000);
    setup();
  }

  updateScreen();
  
  delay(140);
}

bool isCoordinateEqual(struct Coordinate a, struct Coordinate b) {
  return a.x == b.x && a.y == b.y;
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

  if (jump == ON_THE_WAY_UP)
    airtime++;

  if (airtime > 3)
    jump = ON_THE_WAY_DOWN;

  if (jump != ON_THE_WAY_DOWN && g.isHeld(UP)) {
    if (jump == READY)
      jump = ON_THE_WAY_UP;
    
    if (jump == ON_THE_WAY_UP && runner.coordinate.y > 3)
        runner.coordinate.y--;
  }
  else {
    airtime = 0;
    if (jump == ON_THE_WAY_UP)
      jump = ON_THE_WAY_DOWN;
    
    if (jump == ON_THE_WAY_DOWN)
      if (runner.coordinate.y < 6)
        runner.coordinate.y++;
      else
        jump = READY;
  }
}

void updateScreen() {
  clearBitmap();
  updateBitmapWithGameState();
  
  memcpy(g.display, bitmap, sizeof(bitmap));
  g.updateDisplay();
}

void clearBitmap() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      bitmap[i][j] = 0;
    }
  }
}

void updateBitmapWithGameState() {
  for (int i = 0; i < 8; i++)
    activateBit(i, 7);
  
  activateBit(runner.coordinate.x, runner.coordinate.y);
  activateBit(obstacle1.coordinate.x, obstacle1.coordinate.y);
  activateBit(obstacle2.coordinate.x, obstacle2.coordinate.y);
}

void activateBit(int x, int y) {
  if (x >= 0 && x < 8 && y >= 0 && y < 8)
    bitmap[x][y] = 1;
}
