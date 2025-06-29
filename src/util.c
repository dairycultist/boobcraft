// isolation/abstraction is good :)

#ifndef UTIL_DEFINED

#define UTIL_DEFINED

#define TRUE 1
#define FALSE 0
#define DEG2RAD (M_PI / 180)

void log_error(const char *msg) {
	
	if (strlen(SDL_GetError()) == 0) {
		fprintf(stderr, "\n%s: <No error given>\n\n", msg);
	} else {
		fprintf(stderr, "\n%s: %s\n\n", msg, SDL_GetError());
	}
}

typedef struct {

	char data[65536 * 8];
	int bytecount;

} EZArray;

void append_ezarray(EZArray *array, void *data, int data_length) {

	memcpy(&array->data[array->bytecount], data, data_length);
	array->bytecount += data_length;
}

#endif