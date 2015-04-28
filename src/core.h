#include <stdint.h>
#include "../config.h"

/*!
 * \file core.h
 * \brief mastermind core funtions and types definition
 */

#ifndef __MM_CORE_H
#define __MM_CORE_H

/// different session states
enum { MM_NEW,     ///< just created session and didn't play any guess yet
       MM_PLAYING, ///< played at least one valid guess but session not ended
       MM_SUCCESS, ///< session ended with success (user find the secret)
       MM_FAIL     ///< session ended with failure (user didn't find the secret)
};

/// supported types of configuration options
enum { MM_CONF_INT, ///< configuration value is an integer
       MM_CONF_STR, ///< configuration value is a string
       MM_CONF_BOOL ///< configuration value is boolean (0 or 1)
};

/// contains current session configuration
typedef struct {
	uint8_t guesses; ///< max guesses on panel
	uint8_t colors;  ///< max nbre of colors
	uint8_t holes;   ///< nbre of holes (items) in a combination
	uint8_t remise;  ///< do/don't repeat color on combination
} mm_config;

/// the guess object, containing the combination by the user and its score
typedef struct {
	uint8_t *combination; ///< given combination (guess)
	uint8_t right_pos;    ///< nbre of items on right place
	uint8_t wrong_pos;    ///< nbre of items on wrong place
} mm_guess;

/// contains the secret generated at the beginning of session and the freq of
/// its colors.
typedef struct {
	uint8_t *val;  ///< secret combination (len: config->holes)
	uint8_t *freq; ///< colors freq (len: config->colors)
} mm_secret;

/// the session object containing the secret object, the inputed guesses,
/// session configuration and state.
typedef struct {
	uint8_t guessed;   ///< nbre of user guessed combination
	uint8_t state;     ///< current state of session
	char *account;     ///< account name or NULL for default account
	mm_secret *secret; ///< secret combination to guess
	mm_config *config; ///< session config
	mm_guess *panel;   ///< session panel. FIXME: update doc
} mm_session;

/// session score, containing score value and account name
typedef struct {
	long unsigned score; ///< score value. TODO: do we need long unsigned ??
	char *account;       ///< account name
} mm_score_t;

/// top scores history
typedef struct {
	mm_score_t *T; ///< top scores array
	unsigned max;  ///< max number of top scores to store
	unsigned len;  ///< current number of top scores stores
} mm_scores_t;

/// mastermind configuration option of type integer (conf.nbre.*)
typedef struct {
	unsigned type; ///< configuration type: MM_CONF_INT
	char *name;    ///< configuration name
	int val;       ///< configuration value
	int min;       ///< minimal integer value configuration value acceptes
	int max;       ///< maximal integer value configuration value acceptes
} mm_conf_int_t;

/// mastermind configuration option of type string (conf.str.*)
typedef struct {
	unsigned type; ///< configuration typr: MM_CONF_STR
	char *name;    ///< configuration type
	char *val;     ///< configuration value
	uint8_t len;   ///< length of string in value
} mm_conf_str_t;

/// mastermind configuration option od type boolean (conf.bool.*)
typedef struct {
	unsigned type; ///< configuration type: MM_CONF_BOOL
	char *name;    ///< configuration name
	uint8_t val;   ///< 0 or 1
} mm_conf_bool_t;

/// mastermind configuration option common data
typedef struct {
	unsigned type; ///< configuration type: MM_CONF_INT
	char *name;    ///< configuration name
} mm_conf_common_t;

/// mastermind general configuration type
typedef union {
	unsigned type; ///< configuration type, share with all configurations
	mm_conf_common_t common; ///< common configuration data
	mm_conf_str_t str;   ///< string configuration data
	mm_conf_bool_t bool; ///< boolean configration data
	mm_conf_int_t nbre;  ///< integer configuration data
} mm_conf_t;

/// config options indexes on mm_confs
enum { MM_POS_GUESSES,
       MM_POS_COLORS,
       MM_POS_HOLES,
       MM_POS_REMISE,
       MM_POS_ACCOUNT,
       MM_POS_SAVE_EXIT,
       MM_POS_SAVE_PLAY,
       MM_POS_LEN ///< length of mm_confs arrays
};

/// config options arrays of length MM_POS_LEN
extern mm_conf_t mm_confs[];

mm_session *mm_session_new();
mm_session *mm_session_restore();
unsigned int mm_session_save(mm_session *);
void mm_session_free(mm_session *);
void mm_session_exit(mm_session *);

mm_config *mm_config_new();
void mm_config_load();
void mm_config_save();
unsigned mm_config_set(const char *, const char *);

const mm_scores_t *mm_scores_get();
long unsigned mm_score(mm_session *session);

mm_secret *mm_secret_new(mm_config *);
unsigned mm_play(mm_session *, uint8_t *);
void mm_init(const char *);
#endif
