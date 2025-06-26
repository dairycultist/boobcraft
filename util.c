// isolation/abstraction is good :)

#ifndef UTIL_DEFINED

#define UTIL_DEFINED

#define TRUE 1
#define FALSE 0

void log_error(const char *msg) {
	
	if (strlen(SDL_GetError()) == 0) {
		fprintf(stderr, "\n%s: <No error given>\n\n", msg);
	} else {
		fprintf(stderr, "\n%s: %s\n\n", msg, SDL_GetError());
	}
}

#endif