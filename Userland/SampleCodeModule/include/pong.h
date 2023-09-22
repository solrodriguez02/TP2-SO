#ifndef _PONG_H_
#define _PONG_H_

#define BALL_RADIUS 7
#define INIT_BALL_X SCREEN_WIDTH/2
#define INIT_BALL_Y SCREEN_HEIGHT/2
#define PADDLE_WIDTH 7
#define PADDLE_HEIGHT 150
#define INIT_RPADDLE_X SCREEN_WIDTH-10
#define INIT_LPADDLE_X 10
#define INIT_PADDLE_Y 300
#define MOVE_VALUE 10
#define MOVE_UP -1
#define MOVE_DOWN 1
#define ESC_KEY 27
#define START_KEY '\n'
#define UPR_KEY 3
#define UPL_KEY 1
#define DOWNR_KEY 4
#define DOWNL_KEY 2
#define LEFT_PADDLE 0
#define RIGHT_PADDLE 1
#define MAX_SCORE 10
#define BALL_SPEED 4
#define PONG_KEYS 5

void playPong();
void init_menu();
void restart_menu();
void quit();
int gameOver();
void clearScreen();
int play();
int startGame();
void setPaddle(int x, int y, int h, int w, int pos);
void setBall(int x, int y, int r, int dy, int dx);
void printScores();
void drawMidLine();
void movePaddle(int idx, int direction);
void moveBall();
void drawBall();
void changeBallAngle(int idx);
int checkCollision(int idx);
void deleteBall();
void drawPaddle(int idx);
void deletePaddle(int idx);


#endif