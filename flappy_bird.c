#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define JUMP_AMOUNT 2
#define WIDTH 80
#define HEIGHT 24
#define BIRD 20             // Bird x val is fixed
#define PIPE_INTERVAL 30
#define TOP_BOTTOM_PIPE_GAP 6
#define MS_GAME_CYCLE 100
#define HS_FILE "highscore.txt"


int bird_y;
int score;
int high_score;
int screen[WIDTH];
int next_pipe_counter;
int fall_counter;


void load_high_score() {
    FILE* file = fopen(HS_FILE, "r");
    if (file == NULL) {
        high_score = 0;
    } else {
        fscanf(file, "%d", &high_score);
        fclose(file);
    }
}

void save_high_score() {
    if (score > high_score) {
        high_score = score;
        FILE* file = fopen(HS_FILE, "w");
        if (file != NULL) {
            fprintf(file, "%d", high_score);
            fclose(file);
        }
        printf("New High Score: %d!\n", high_score);
    }
}

void load_game() {
    srand(time(NULL));
    bird_y = HEIGHT / 2;
    score = 0;
    load_high_score();
    for (int i =0; i < WIDTH; i++) {
        screen[i] = 0;
    }

    next_pipe_counter = 1;
    fall_counter = 0;
}

void draw() {

    printf("\033[2J\033[H");  // Some voodoo magic I got off stackoverflow that clears the screen
    // go through col by col so you can print the entire pipe at once
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == BIRD && y == bird_y) {
                printf("\033[1;33m@\033[0m");  // Draw the yellow bird the \033[ is start of ANSI 1;33m is saying bold;yellow @ is the bird and the rest is the end of ANSI
            } else if (screen[x] != 0) {
                if (y < screen[x] || y > screen[x] + TOP_BOTTOM_PIPE_GAP) {
                    // check the value of screen[x] this will have the min pipe amount. 
                    // if it is less than it, draw a green |, if it is more than it + the gap between bottom and top
                    // draw it. This took me longer than I admit because 0,0 is top left which is DUMB 
                    printf("\033[1;32m|\033[0m");                 
                } else if (y == screen[x]) {
                    printf("\033[1;32m-\033[0m");  // If it is equal to screen[x], it 
                                                   // is the bottom of the top pipe so draw a green -
                } else if (y == screen[x] + TOP_BOTTOM_PIPE_GAP) {
                    printf("\033[1;32m-\033[0m");  // Same thing
                } else {
                    printf(" "); 
                }
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
    printf("High score: %d\n", high_score);
}


void update_game() {
    bird_y++;
    // Move everything to the left
    for (int i = 0; i < WIDTH - 1; i++) {
        screen[i] = screen[i + 1];
    }
    screen[WIDTH-1] = 0;


    // CHecks collision, fiddle with it if feels too hard/easy
    if (screen[BIRD] != 0 && (bird_y <= screen[BIRD]+1 || bird_y >= screen[BIRD] + TOP_BOTTOM_PIPE_GAP)) {
        save_high_score();
        printf("Game Over! You hit a pipe.\nFinal score: %d\nHigh score: %d\n", score, high_score);
        exit(0);
    }

    if (bird_y < 0 ) {
        save_high_score();
        printf("Game Over! You hit the cieling.\nFinal score: %d\nHigh score: %d\n", score, high_score);
        exit(0);
    }

    if (bird_y > HEIGHT ) {
        save_high_score();
        printf("Game Over! You hit the ground.\nFinal score: %d\nHigh score: %d\n", score, high_score);
        exit(0);
    }

    // Add to the score if the prev col was a pipe
    if (screen[BIRD] == 0 && screen[BIRD-1] != 0) {
        score++;
    }

    next_pipe_counter--;
    if (next_pipe_counter == 0) {
        screen[WIDTH -1] = (rand() % (HEIGHT - TOP_BOTTOM_PIPE_GAP - 2)) +2; // Stops pipe from appearing too close to top or bottom, fiddle with this a bit
        next_pipe_counter = PIPE_INTERVAL;
    }

}

// Source: https://gist.github.com/vsajip/1864660
// No idea how the hell this works
// Took me a while of trial and error of random stuff online to finally get this to work
// who wouldve thought some code from 13yrs ago would have saved the day
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    
    return 0;
}


int main() {
    load_game();
    while(1) {
        draw();
        update_game();
        // if theres a bit go up (which is subtracting cuz stupid computer grid)
        if (kbhit()) {
            getchar();
            bird_y -= JUMP_AMOUNT;
        }
        usleep(1000 * MS_GAME_CYCLE);
    }

}
