#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#include "sdl.h"

#include "Buffer.h"

typedef struct _SETTINGS {
    // fov and screen settings
    unsigned int fov: 7; // max of 127
    unsigned int target_fps: 8; // max of 511
    
    // sound
    // 7 bits is minimum for 0-100
    unsigned int sound_main: 7;
    // put other sound types here (music, sfx, enemies, etc)

    // controls
    // make sure to have enough bits for keys
    // 
} SETTINGS;
SETTINGS settings;

// game state
typedef struct _GAME {
    // redundant
    SETTINGS settings;

    // audio, game objects/chunks, key events, rendering interface, network connection, etc
} GAME;
GAME game;

// default settings functions
SETTINGS default_settings() {
    // 
}

#define TARGET_UPDATESPS 1
#define MS_PER_UPDATE (1000.0 / TARGET_UPDATESPS)

#define MS_PER_FRAME (1000.0 / settings.target_fps)

#define log(g...) printf(g)
#define err(g...) fprintf(stderr, g)
#define fatal_err(g...) fprintf(stderr, g); exit(-1)

// key event + key event buffer
// key event buffer expands depending on how many keys are pressed at once
// but it never shrinks back down
// so hitting 200 keys in one run of the loop will permanently allocate 256*sizeof(KeyEvent) bytes
// (assuming buf->cap *= 2 each realloc)
// our key events will be simplified from sdl key events to manage memory
typedef struct _KeyEvent {
    // 
} KeyEvent;

KeyEvent* keyevent() {
    // 
}

typedef struct _KeyEventBuf {
    KeyEvent** events;
    int len;
    int cap;
    int cur;
} KeyEventBuf;

// assumes null termination obviously
int str_len(char* str) {
    int i;
    for (i = 0; str[i] != '\0'; i++);
    return i;
}

double tspec_to_double(struct timespec t) {
    double res = (t.tv_sec * 1000.0) + (t.tv_nsec / 1000000.0);
    return res;
}

double getcurrenttime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return tspec_to_double(t);
}

// GAME LOOP FUNCTIONS
// core game loop functions will use the global struct _GAME game state struct
// and other global variables
// and therefore take no input

void grab_input() {
    // 
}

void update() {
    // 
}

void render() {
    // 
}

int main() {

    char close = 0;

    // load settings
    // initialize game loader, server connection, etc
    
    // game loop
    // for each game loop:
    // - grab input
    // - update
    // - load new / dump old data/objects
    // - render
    // - send new data to the server
    // these will be simplified down to three things:
    // - process input - grab input
    // - update - update, load new / dump old data/objects, send data to server
    // - render - render
    double previous = getcurrenttime();
    double lag = 0.0;
    double frame_lag = 0.0;
    while (!close) {

        double current = getcurrenttime();
        double elapsed = current - previous;
        previous = current;
        lag += elapsed;

        if (settings.target_fps) {
            frame_lag += elapsed;
        }

        // process input
        grab_input();

        while(lag >= MS_PER_UPDATE) {
            // inside this while block runs TARGET_UPDATESPS times per second
            // update
            update();

            lag -= MS_PER_UPDATE;
        }

        if (settings.target_fps && frame_lag >= MS_PER_FRAME) {
            // inside this block will run TARGET_FPS times per second
            // render
            render();

            frame_lag -= MS_PER_FRAME;
        } else if (!settings.target_fps) {
            // this block runs every single loop if not using a target fps
            // render
            render();
        }
    }

    return 0;
}
