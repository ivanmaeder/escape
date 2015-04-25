/*
 At a minimum
 ------------
 1. Randomly generated background (the top 2-3 rows)
 2. Randomly generated obstacles, with a minimum clearance between them
 3. Press UP to jump obstacles:

          .             ...       .       .        ..
         . .    .      .   .     . .     . .      .  .
        .   .  . .    .  #  .   .   .   . # .    . ## .
     ...  #  .. # ....   #   ...  ## ... ### ....  ##  ..
    ######################################################
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
  making a var called player and got errors. This would be confusing to a novice
  programmer
 */
#include <Gamer.h>

struct Cloud {
  char x;
  char y;
};

typedef enum {
  READY,
  JUMP,
  FALL
} JumpPosition;

Gamer g;

Cloud cloud1, cloud2;
int runner = 6;
JumpPosition jump = READY;

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

  cloud1 = { 8, 1 };
}

void loop() {
  updateGameState();
  updateScreen();

  delay(180);
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

  /* Not bad, but it would be better if users had to lift again after 
   the character hits the ground.
   */
  if (jump != FALL && g.isHeld(UP)) {
    if (jump == READY) {
      jump = JUMP;
    }
    
    if (jump == JUMP) {
      if (runner > 3) {
        runner--;
      }
    }
  }
  else {
    if (jump == JUMP) {
      jump = FALL;
    }
    
    if (jump == FALL) {
      if (runner < 6) {
      runner++;
      }
      else {
        jump = READY;
      }
    }
  }

    
  /*
  if (g.isHeld(UP)) {
    if (jump != FALL) {
      jump = JUMP;

      if (runner > 3) {
        runner--;
      }
    }
  }
  else {
    if (jump == JUMP) {
      jump = FALL;
    }

    if (runner < 6) {
      runner++;
    }
    else {
      jump = READY;
    }
  }
  */
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
    g.display[i][7] = 1;
  }

  //clouds
  g.display[cloud1.x][cloud1.y] = 1;
  g.display[cloud2.x][cloud2.y] = 1;

  //player
  g.display[1][runner] = 1;
}
