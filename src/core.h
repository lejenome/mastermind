#include <stdint.h>
#include "../config.h"

/*! \file core.h
 * \brief mastermind core funtions and types definition
 */

#ifndef __MM_CORE_H
#define __MM_CORE_H

#define MM_NEW 0
#define MM_PLAYING 1
#define MM_SUCCESS 2
#define MM_FAIL 4

#define MM_CONF_INT 0
#define MM_CONF_STR 1
#define MM_CONF_BOOL 2

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
	uint8_t inplace;      ///< nbre of items on right place
	uint8_t insecret;     ///< nbre of items on secret but not inplace
} mm_guess;

/// contains the secret generated at the beginning of session and the freq of
/// its colors.
typedef struct {
	uint8_t *val;  ///< len: config->holes
	uint8_t *freq; ///< len: config->colors
} mm_secret;

/// the session object containing the secret object, the inputed guesses,
/// session configuration and state.
typedef struct {
	uint8_t guessed;   ///< nbre of user guessed combination
	uint8_t state;     ///< current state of session
	char *account;     ///< account name or NULL for default account
	mm_secret *secret; ///< secret combination to guess
	mm_config *config; ///< session config
	mm_guess *panel;   ///< session panel
} mm_session;

/// session score, containing score value and account name
typedef struct {
	long unsigned score; ///< score value
	char *account;       ///< account name
} mm_score_t;

/// top scores history
typedef struct {
	mm_score_t *T; ///< top scores array
	unsigned max;  ///< max number of top scores to store
	unsigned len;  ///< current number of top scores stores
} mm_scores_t;

/// configuration option of type integer (conf.nbre.*)
typedef struct {
	uint8_t type; ///< configuration type: MM_CONF_INT
	char *name;   ///< configuration name
	int val;      ///< configuration value
	int min;      ///< minimal integer value configuration value acceptes
	int max;      ///< maximal integer value configuration value acceptes
} mm_conf_int_t;

/// configuration option of type string (conf.str.*)
typedef struct {
	uint8_t type; ///< configuration typr: MM_CONF_STR
	char *name;   ///< configuration type
	char *val;    ///< configuration value
	uint8_t len;  ///< length of string in value
} mm_conf_str_t;

/// configuration option od type boolean (conf.bool.*)
typedef struct {
	uint8_t type; ///< configuration type: MM_CONF_BOOL
	char *name;   ///< configuration name
	uint8_t val;  ///< 0 or 1
} mm_conf_bool_t;

/// general configuration type
typedef union {
	uint8_t type; ///< configuration type, share with all configurations
	mm_conf_str_t str;   ///< string configuration data
	mm_conf_bool_t bool; ///< boolean configration data
	mm_conf_int_t nbre;  ///< integer configuration data
} mm_conf_t;

extern mm_conf_t mm_confs[7];

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
