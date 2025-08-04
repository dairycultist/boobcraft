
// need to include all engine-side implemented stuff so the game-side can correctly call those upon linkage


// game-side implemented
char *get_title();

void on_start();

void on_terminate();

void process_tick();

void process_event(SDL_Event event);