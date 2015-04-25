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
 */
#include <Gamer.h>

struct Sprite {
  char x;
  char y;
};

typedef enum {
  READY,
  ON_THE_WAY_UP,
  ON_THE_WAY_DOWN
} JumpPosition;

Gamer g;

Sprite cloud1, cloud2;
Sprite runner;
Sprite obstacle1, obstacle2;
JumpPosition jump = READY;
int airtime = 0;

int bitmap[8][8] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

void setup() {
  g.begin();
  randomSeed(g.ldrValue());
  
  Serial.begin(9600);

  runner = { 1, 6 };
  cloud1 = { 8, 1 };
  obstacle1 = { 8, 6 };
  obstacle2 = { 16, 6 };
}

void loop() {
  updateGameState();
  updateScreen();

  if (isPositionEqual(runner, obstacle1) || isPositionEqual(runner, obstacle2)) {
    delay(2000);    
  }
  
  delay(140);
}

bool isPositionEqual(struct Sprite a, struct Sprite b) {
  return a.x == b.x && a.y == b.y;
}

void updateGameState() {
  cloud1.x--;
  if (cloud1.x < 0) {
    cloud1.x = 7;
    cloud1.y = random(0, 3);
  }

  cloud2.x--;
  if (cloud2.x < 0) {
    cloud2.x = random(8, 16);
    cloud2.y = random(0, 3);
  }
  
  obstacle1.x--;
  if (obstacle1.x < 0) {
    obstacle1.x = random(obstacle2.x, obstacle2.x + 16);
  }
  
  obstacle2.x--;
  if (obstacle2.x < 0) {
    obstacle2.x = random(obstacle1.x, obstacle1.x + 16);
  }

  if (jump == ON_THE_WAY_UP) {
    airtime++;
  }

  if (airtime > 3) {
    jump = ON_THE_WAY_DOWN;
  }

  if (jump != ON_THE_WAY_DOWN && g.isHeld(UP)) {
    if (jump == READY) {
      jump = ON_THE_WAY_UP;
    }
    
    if (jump == ON_THE_WAY_UP) {
      if (runner.y > 3) {
        runner.y--;
      }
    }
  }
  else {
    airtime = 0;
    if (jump == ON_THE_WAY_UP) {
      jump = ON_THE_WAY_DOWN;
    }
    
    if (jump == ON_THE_WAY_DOWN) {
      if (runner.y < 6) {
        runner.y++;
      }
      else {
        jump = READY;
      }
    }
  }
}

void updateScreen() {
  clearBitmap();
  updateBitmapWithGameState();
  
  g.updateDisplay();
}

void clearBitmap() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      g.display[i][j] = 0;
    }
  }
}

void updateBitmapWithGameState() {
  //ground
  for (int i = 0; i < 8; i++) {
    activateBit(i, 7);
  }

  //player
  activateBit(runner.x, runner.y);

  //clouds
//  activateBit(cloud1.x, cloud1.y);
//  activateBit(cloud2.x, cloud2.y);

  //obstacles
  activateBit(obstacle1.x, obstacle1.y);
  activateBit(obstacle2.x, obstacle2.y);
}

void activateBit(int x, int y) {
  if (x >= 0 && x < 8 && y >= 0 && y < 8) {
    g.display[x][y] = 1;
  }
}
