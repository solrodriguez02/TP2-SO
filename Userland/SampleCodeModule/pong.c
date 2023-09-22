#include <pong.h>
#include <library.h>

/**
 * @brief El par coordenado (x,y) es el centro de la bola, y (dx,dy) es el vector de movimiento.
 */
typedef struct {
    int x;
    int y;
    int r;
    int dx;
    int dy;
} ball_format;

/**
 * @brief El tamaño de la paleta es el producto de h y w. El par coordenado (x,y) es la coordenada de la
 * esquina derecha.
 */
typedef struct {
    int x;
    int y;
    int h;
    int w;
} paddle_format;

ball_format ball;
paddle_format paddles[2];
int score[2];
char keys[PONG_KEYS];

/**
 * @brief Función correspondiente al módulo de "pong". Inicializo la pantalla para el minijuego "pong" hasta
 * la detección de la tecla "ESC", la cual vuelve a la Shell reiniciada. Para iniciar el juego se debe presionar
 * la tecla del salto de línea.
 */
void playPong() {
    enableDoubleBuffer(1);
    init_menu();
    startGame();
    int exit = play();
    while (!exit) {
        restart_menu();
        score[LEFT_PADDLE] = 0;
        score[RIGHT_PADDLE] = 0;
        if (startGame())
            exit = play();
        else 
            break;
    }
    quit();
}


/**
 * @brief Escribe en pantalla el título del juego, y cómo empezar y salir del juego.
 */
void init_menu() {
    printAt("PONG GAME", 450, 200);
    printAt("Press ENTER to play", 385, 300);
    printAt("Press ESC to quit", 395, 400);
    loadScreen();
}

/**
 * @brief Imprime el mensaje de fin de juego, mostrando el  ganador y las opciones de si quieren volver a
 * jugar o quieren salir. 
 */
void restart_menu() {
    clearScreen();
    printAt("GAME OVER", 450, 200);
    if(score[LEFT_PADDLE] == MAX_SCORE) {
        printAt("Winner: left player", 390, 300);
    }
    else {
        printAt("Winner: right player", 390, 300);
    }
    printAt("Press ENTER to play again", 350, 400);
    printAt("Press ESC to quit", 395, 500);
    loadScreen();
}

void quit() {
    enableDoubleBuffer(0);
}


/**
 * @brief Finaliza el juego en el caso de que se consiga la máxima puntuación por parte de uno de los jugadores.
 * 
 * @return Valor booleano si ya terminó el juego al llegar a una puntuación máxima. 
 */
int gameOver() {
    if(score[LEFT_PADDLE] == MAX_SCORE || score[RIGHT_PADDLE] == MAX_SCORE) {
        return 1;
    }
    return 0;
}

/**
 * @brief Elimina visualmente los objetos del minijuego.
 */
void clearScreen() {
    drawRectangle(0,0, SCREEN_HEIGHT, SCREEN_WIDTH, BACKGROUNG_COLOR);
}

/**
 * @brief Comienza el juego, y mueve las paletas dependiendo del valor leido para las teclas, desplaza a la
 * bola, actualiza la línea, los puntajes y la pantalla.
 * 
 * @return Valor booleano de si ya terminó el juego.
 */
int play() {
    while (!gameOver()) {
        getKeyboardState(keys);
        if(keys[0]) {
            return 1;
        }
        if(paddles[LEFT_PADDLE].y <= 10) {
            keys[UPL_KEY] = 0;
        }
        else if(paddles[LEFT_PADDLE].y >= SCREEN_HEIGHT-10) {
            keys[DOWNL_KEY] = 0;
        }
        if(paddles[RIGHT_PADDLE].y <= 10) {
            keys[UPR_KEY] = 0;
        }
        else if(paddles[RIGHT_PADDLE].y >= SCREEN_HEIGHT-10) {
            keys[DOWNR_KEY] = 0;
        }
        if(keys[UPL_KEY]) {
            movePaddle(LEFT_PADDLE, MOVE_UP);
        } else if(keys[DOWNL_KEY] ) {
           movePaddle(LEFT_PADDLE, MOVE_DOWN);
        }
        if(keys[UPR_KEY] ) {
           movePaddle(RIGHT_PADDLE, MOVE_UP);
        } else if(keys[DOWNR_KEY] ) {
           movePaddle(RIGHT_PADDLE, MOVE_DOWN);
        }
        drawMidLine();
        printScores();
        moveBall();
        loadScreen();
    }
    return 0;
}

/**
 * @brief Comienza e inicializa los objetos del minijuego. Espera en el menú hasta que se inicie o se salga del
 * minijuego.
 * 
 * @return Valor booleano que indica si quiere iniciar o salir del minijuego.
 */
int startGame() {
    clearScreen();
    int option = getChar();
    while (option != START_KEY && option != ESC_KEY) {
        option = getChar();
    }
    if(option == ESC_KEY) {
        return 0;
    }
    keys[0]=keys[1]=keys[2]=keys[3]=keys[4]=0;
    score[0]=score[1]=0;
    setBall(INIT_BALL_X, INIT_BALL_Y, BALL_RADIUS, BALL_SPEED, BALL_SPEED);
    setPaddle(INIT_RPADDLE_X, INIT_PADDLE_Y, PADDLE_HEIGHT, PADDLE_WIDTH, RIGHT_PADDLE);
    setPaddle(INIT_LPADDLE_X, INIT_PADDLE_Y, PADDLE_HEIGHT, PADDLE_WIDTH, LEFT_PADDLE);
    drawMidLine();
    drawBall();
    drawPaddle(RIGHT_PADDLE);
    drawPaddle(LEFT_PADDLE);   
    return 1;
}

/**
 * @brief Inicializa las paletas utilizadas por ambos jugadores.
 * 
 * @param x Posición izquierda de la paleta.
 * @param y Posición inferior de la paleta.
 * @param h Altura de la paleta.
 * @param w Ancho de la paleta.
 * @param pos Index de la paleta.
 */
void setPaddle(int x, int y, int h, int w, int pos) {
    paddle_format paddle;
    paddle.h = h;
    paddle.w = w;
    paddle.x = x;
    paddle.y = y;
    paddles[pos] = paddle;
}

/**
 * @brief Inicializa la bola del minijuego.
 * 
 * @param x Posición horizontal del centro de la bola.
 * @param y Posición vertical del centro de la bola.
 * @param r Radio de la bola.
 * @param dy Vector vertical de movimiento de la bola.
 * @param dx Vector horizontal de movimiento de la bola.
 */
void setBall(int x, int y, int r, int dy, int dx) {
    ball.x = x;
    ball.y = y;
    ball.r = r;
    ball.dx = dx;
    ball.dy = dy;
}

/**
 * @brief Escribe sobre la pantalla la puntuación de cada jugador.
 */
void printScores() {
    char buffer[1];
    numToStr(score[0], 10, buffer);
    printAt(buffer, 460, 50);
    numToStr(score[1], 10, buffer);
    printAt(buffer, 550 , 50);
}

/**
 * @brief Dibuja la línea media que separa ambos lados de los jugadores.
 */
void drawMidLine() {
    for(int y=0; y<SCREEN_HEIGHT ; y++) {
        if (y%10 < 5) {
           drawRectangle(MID_SCREEN_X, y, 1, 1, WHITE);
        }
    }
}

void movePaddle(int idx, int direction) {
    if((paddles[idx].y < 10 && direction == MOVE_UP)||(paddles[idx].y + PADDLE_HEIGHT > SCREEN_HEIGHT - 10 && direction == MOVE_DOWN)) {
        return;
    }
    deletePaddle(idx);
    paddles[idx].y = paddles[idx].y + MOVE_VALUE * direction;
    drawPaddle(idx);
}

/**
 * @brief Desplaza la bola un diferencial en cada dirección. Analiza los casos de colisión con la pared y
 * las paletas y cambia la dirección, módulo y sentido.
 */
void moveBall() {
    deleteBall();
    ball.x += ball.dx;
    ball.y += ball.dy;
    // turns the ball around if it hits the edge of the screen
    if (ball.x < 10) {
        beep();
        score[1] += 1;
        setBall(INIT_BALL_X, INIT_BALL_Y, BALL_RADIUS, BALL_SPEED, BALL_SPEED);
    }
    if (ball.x > SCREEN_WIDTH - 10) { 
        beep();
        score[0] += 1;
        setBall(INIT_BALL_X, INIT_BALL_Y, BALL_RADIUS, -1*BALL_SPEED, -1*BALL_SPEED);
    }
    if (ball.y < 20 || ball.y > SCREEN_HEIGHT - 10) {
        ball.dy = -ball.dy;
    }
    for(int idx=0; idx<2; idx++) {
        if(checkCollision(idx)) {
            // the ball is moving to the left
            if (ball.dx < 0) {
                ball.dx -= 1;
            // the ball is moving to the right
            } else {    
                ball.dx += 1;
            }
            // changes the ball direction
            ball.dx = -ball.dx;
            changeBallAngle(idx);
        }
    }
    drawBall();
}

/**
 * @brief Coloca visualmente una bola de color blanco sobre la pantalla.
 */
void drawBall() {
    int r = ball.r;
    while (r!=0) {
        drawCircle(ball.x, ball.y, r, WHITE);
        r--;
    }
}

/**
 * @brief Cambia el ángulo de la bola dependiendo de la posición de la paleta en la cual golpeó.
 * 
 * @param idx 
 */
void changeBallAngle(int idx) {
    int impact_point = ball.y - paddles[idx].y;
    double div_factor = 1/7;
    if(impact_point < PADDLE_HEIGHT*div_factor) {
        ball.dy = 5;
    }
    else if (impact_point < PADDLE_HEIGHT*2*div_factor) {
        ball.dy = 3;
    }
    else if (impact_point < PADDLE_HEIGHT*3*div_factor) {
        ball.dy = 1;
    }
    else if (impact_point < PADDLE_HEIGHT*4*div_factor) {
        ball.dy = 0;
    }
    else if (impact_point < PADDLE_HEIGHT*5*div_factor) {
        ball.dy = -1;
    }
     else if (impact_point < PADDLE_HEIGHT*6*div_factor) {
        ball.dy = -3;
    }
    else {
        ball.dy = -5;
    }
}

/**
 * @brief Analiza si la bola colisionó con alguna de las paletas.
 * 
 * @param idx Index de la paleta seleccionada.
 * @return Valor booleano si detectó una colisión.
 */
int checkCollision(int idx) {
    if (ball.x + BALL_RADIUS > paddles[idx].x - 10 &&
        ball.x - BALL_RADIUS < paddles[idx].x + paddles[idx].w + 10 &&
        ball.y + BALL_RADIUS + 10 > paddles[idx].y &&
        ball.y - BALL_RADIUS - 10 < paddles[idx].y + paddles[idx].h) {
        return 1; 
    }
    return 0;  
}

/**
 * @brief Elimina visualmente la bola del minijuego.
 */
void deleteBall() {
    int r = ball.r;
    while (r!=0) {
        drawCircle(ball.x, ball.y, r, BACKGROUNG_COLOR);
        r--;
    }
}

/**
 * @brief Coloca visualmente la paleta con el index correspondiente recibido como parámetro.
 * 
 * @param idx Index correspondiente al número de jugador. El jugador izquierdo tiene index 0, mientras que el
 * derecho tiene index 1.
 */
void drawPaddle(int idx) {
    drawRectangle(paddles[idx].x, paddles[idx].y, paddles[idx].h, paddles[idx].w, WHITE);
}

/**
 * @brief Elimina visualmente la paleta con el index correspondiente recibido como parámetro.
 * 
 * @param idx Index correspondiente al número de jugador. El jugador izquierdo tiene index 0, mientras que el
 * derecho tiene index 1.
 */
void deletePaddle(int idx) {
    drawRectangle(paddles[idx].x, paddles[idx].y, paddles[idx].h, paddles[idx].w, BACKGROUNG_COLOR);
}
