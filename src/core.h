#include "../config.h"

#define MM_COLORS_MAX 10

#define MM_NEW 0
#define MM_PLAYING 1
#define MM_SUCCESS 2
#define MM_FAIL 4

typedef struct {
	unsigned char guesses;	// max guesses on panel
	unsigned char colors;	// max nbre of colors
	unsigned char holes;	// nbre of holes (items) in a combination
} mm_config;
typedef struct {
	unsigned char *combination;	// given combination (guess)
	unsigned char inplace;		// nbre of items on right place
	unsigned char insecret;		// nbre of items on secret but not inplace
} mm_guess;
typedef struct {
	unsigned char *val;		// len: config->holes
	unsigned char *freq;		// len: config->colors
} mm_secret;
typedef struct {
	unsigned char guessed;	// nbre of user guessed combination
	unsigned char state;	// current state of session
	mm_secret *secret;	// secret combination to guess
	mm_config *config;	// session config
	mm_guess *panel;	// session panel
} mm_session;

mm_session *mm_session_new();
mm_session *mm_session_restore();
unsigned int mm_session_save(mm_session*);
mm_config *mm_config_load();
mm_secret *mm_secret_new(mm_config*);
unsigned mm_play(mm_session *, unsigned char *);
void mm_init();
