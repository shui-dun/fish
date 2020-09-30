#include <curses.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct Fish {
    int size;
    int cols;
    int lines;
} Fish;

int score = 9;
int map[200][200];
pthread_t thread_enemy, thread_create_enemy, thread_refresh, thread_input;
int delay[] = {25000, 50000, 75000, 100000, 125000};
char input_ch;
int cur_x = 0, cur_y = 2, cur_grade = 0;

char *ME9[] = {
    "  o>  ",
    "oo xo>",
    "  o>  ",
};

char *ME17[] = {" >     ", " oooo> ", "o oo xo", " oooo> ", " >     "};

char *ME31[] = {" ooo>     ", "   ooo>   ", " o   ooo> ", "oo    o xo",
                " o   ooo> ", "   ooo>   ", " ooo>     "};

char *ME45[] = {" ooooo>     ", "   ooooo>   ", "     ooooo> ", "ooo  ooo xo>",
                "     ooooo> ", "   ooooo>   ", " ooooo>     "};

char *FISH5[] = {"<5ooo"};

char *FISH10[] = {
    "  <   ", " o o  ", "o10 oo", " o o  ", "  <   ",
};

char *FISH24[] = {"   <oo ", "  o    ", " <oooo ", "o24  oo",
                  " <oooo ", "  o    ", "   <oo "};

char *FISH36[] = {
    "   <        ooo", "  < ooooooooo  ", "o 36     oooooo",
    "  < ooooooooo  ", "   <        ooo",
};

char *FISH100[] = {"          o        ", "  oooooooooo      o",
                   "oooooooooooo     oo", "o  100  oooooo  ooo",
                   "ooo      oooooooooo", "ooooooooooooooo ooo",
                   " oo oooooooo     oo", "   oo             o"};

char **fish[] = {ME9, ME17, ME31, ME45, FISH5, FISH10, FISH24, FISH36, FISH100};

Fish info[] = {{-1, 6, 3}, {-1, 7, 5}, {-1, 10, 7}, {-1, 12, 7}, {5, 5, 1},
               {10, 6, 5}, {24, 7, 7}, {36, 15, 5}, {100, 19, 8}};

void update_map(char ch, int s, int x, int y) {
    int flag = mvaddch(y, x, ch);
    if (flag == 0) map[y][x] = s;
}

void game_over() {
    move(LINES / 2 - 1, (COLS - 10) / 2);
    if (score < 60)
        printw("Game Over!");
    else
        printw("You Win!");
    move(LINES / 2, (COLS - 23) / 2);
    printw("Your Fianl Score is %d", score);
    refresh();
    sleep(5);
    endwin();
    exit(0);
}

void draw_fish(int nu, int x0, int y0) {
    for (int y = 0; y < info[nu].lines; y++)
        for (int x = 0; x < info[nu].cols; x++)
            if (fish[nu][y][x] != ' ')
                update_map(fish[nu][y][x], info[nu].size, x + x0, y + y0);
}

void erase_fish(int nu, int x0, int y0) {
    for (int y = 0; y < info[nu].lines; y++)
        for (int x = 0; x < info[nu].cols; x++)
            if (fish[nu][y][x] != ' ') update_map(' ', 0, x + x0, y + y0);
}

bool erase_enemy(int nu, int x0, int y0) {
    bool ret = true;
    for (int y = 0; y < info[nu].lines; y++)
        for (int x = 0; x < info[nu].cols; x++)
            if (fish[nu][y][x] != ' ') {
                if (ret && x + x0 >= 0 && x + x0 < COLS && y + y0 >= 0 &&
                    y + y0 < LINES && map[y + y0][x + x0] == -1)
                    ret = false;
                update_map(' ', 0, x + x0, y + y0);
            }
    return ret;
}

void init() {
    srand((unsigned)time(NULL));
    initscr();
    noecho();
    curs_set(0);
    draw_fish(0, 0, 2);
    refresh();
}

void *move_enemy(void *param) {
    int nu = rand() % 5 + 4;
    int x = COLS - 1;
    int y = rand() % (LINES + 10) - 5;
    int speed = rand() % 5;
    draw_fish(nu, x, y);
    while (1) {
        usleep(delay[speed]);
        if (!erase_enemy(nu, x, y)) {
            if (info[nu].size > score)
                game_over();
            else {
                score += nu - 2;
                pthread_exit(NULL);
            }
        }
        if (x-- == -19) pthread_exit(NULL);
        draw_fish(nu, x, y);
    }
}

void *create_enemy(void *param) {
    while (1) {
        sleep(rand() % 3 + 1);
        pthread_create(&thread_enemy, NULL, move_enemy, NULL);
    }
}

void *update_screen(void *param) {
    while (1) {
        usleep(50000);
        mvprintw(0, 0, "Score:\t%d", score);
        refresh();
    }
}

void move_me() {
    while (1) {
        erase_fish(cur_grade, cur_x, cur_y);
        if (score < 17)
            cur_grade = 0;
        else if (score < 31)
            cur_grade = 1;
        else if (score < 45)
            cur_grade = 2;
        else if (score < 60)
            cur_grade = 3;
        else
            game_over();
        switch (input_ch) {
            case 'w':
            case 'W':
                cur_y -= 2;
                break;
            case 'S':
            case 's':
                cur_y += 2;
                break;
            case 'A':
            case 'a':
                cur_x -= 4;
                break;
            case 'D':
            case 'd':
                cur_x += 4;
                break;
        }
        input_ch = 0;
        draw_fish(cur_grade, cur_x, cur_y);
        usleep(100000);
    }
}

void *wait_input(void *param) {
    while (1) input_ch = getch();
}

int main(int argc, char const *argv[]) {
    init();
    pthread_create(&thread_create_enemy, NULL, create_enemy, NULL);
    pthread_create(&thread_refresh, NULL, update_screen, NULL);
    pthread_create(&thread_input, NULL, wait_input, NULL);
    move_me();
    return 0;
}
