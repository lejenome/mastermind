#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "core.h"
#ifdef POSIX
#include <unistd.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#else
#include <windows.h>
#endif

#define LEN(a) (sizeof(a) / sizeof(a[0]))
char *mm_config_path = NULL;
char *mm_score_path = NULL;
char *mm_store_path = NULL;

#define MM_POS_GUESSES 0
#define MM_POS_COLORS 1
#define MM_POS_HOLES 2
#define MM_POS_REMISE 3
#define MM_POS_SAVE_EXIT 4
#define MM_POS_SAVE_PLAY 5
mm_conf_t mm_confs[6] = {
	[MM_POS_GUESSES] = {.nm = "guesses",
			    .val = MM_GUESSES,
			    .min = 2,
			    .max = MM_GUESSES_MAX},
	[MM_POS_COLORS] = {.nm = "colors",
			   .val = MM_COLORS,
			   .min = 2,
			   .max = MM_COLORS_MAX},
	[MM_POS_HOLES] = {.nm = "holes",
			  .val = MM_HOLES,
			  .min = 2,
			  .max = MM_HOLES_MAX},
	[MM_POS_REMISE] = {.nm = "remise", .val = 0, .min = 0, .max = 1},
	[MM_POS_SAVE_EXIT] = {.nm = "save_on_exit",
			      .val = 0,
			      .min = 0,
			      .max = 1},
	[MM_POS_SAVE_PLAY] = {.nm = "save_on_play",
			      .val = 0,
			      .min = 0,
			      .max = 1},
};
mm_scores_t mm_scores = {.T = NULL, .max = 20, .len = 0};
#ifndef POSIX
#define srandom(var) srand(var)
#define random() rand()
#endif
/* create new mastermind session and initialize viables && config
 * return: mm_session* : new session object
 */
mm_session *mm_session_new()
{
	mm_session *session = (mm_session *)malloc(sizeof(mm_session));
	session->config = mm_config_load();
	session->guessed = 0;
	session->secret = mm_secret_new(session->config);
	session->state = MM_NEW;
	session->panel =
	    (mm_guess *)malloc(sizeof(mm_guess) * session->config->guesses);
	return session;
}
/* load global config from config file and create new session config based
 * on global config
 * return: mm_config* : new session config object
 */
mm_config *mm_config_load()
{
	mm_config *config;
	mm_conf_t *conf;
	char *n;
	int d;
	FILE *f;
	if (!mm_config_path)
		mm_init();
	config = (mm_config *)malloc(sizeof(mm_config));
	if ((f = fopen(mm_config_path, "r"))) {
		n = (char *)malloc(sizeof(char) * 40);
		while (fscanf(f, "%s %d", n, &d) != EOF) {
			conf = mm_confs;
			while (conf < mm_confs + LEN(mm_confs) &&
			       strcmp(n, conf->nm) != 0)
				conf++;
			if (conf == mm_confs + LEN(mm_confs)) {
#ifdef DEBUG
				printf(_("Error: config option not supported: "
					 "%s\n"),
				       n);
#endif
				continue;
			}
			if (d >= conf->min && d <= conf->max)
				conf->val = d;
#ifdef DEBUG
			else
				printf(
				    _("Error: config option value invalid: %s "
				      "= %d\n"),
				    n, d);
#endif
		}
		free(n);
		fclose(f);
	}
	if (mm_confs[MM_POS_REMISE].val &&
	    mm_confs[MM_POS_COLORS].val < mm_confs[MM_POS_HOLES].val) {
		// ensure colors are no less then holes on remise mode
		mm_confs[MM_POS_COLORS].val = mm_confs[MM_POS_HOLES].val;
	}
	config->guesses = (uint8_t)mm_confs[MM_POS_GUESSES].val;
	config->colors = (uint8_t)mm_confs[MM_POS_COLORS].val;
	config->holes = (uint8_t)mm_confs[MM_POS_HOLES].val;
	config->remise = (uint8_t)mm_confs[MM_POS_REMISE].val;
	return config;
}
/* save global config on the config file
 */
void mm_config_save()
{
	FILE *f = fopen(mm_config_path, "w");
	mm_conf_t *conf;
	if (f) {
		for (conf = mm_confs; conf < mm_confs + LEN(mm_confs); conf++)
			fprintf(f, "%s %d\n", conf->nm, conf->val);
		fclose(f);
	}
}
/* change global config with name to value then save to config file
 * param name: const char* : name of global config to change
 * param value: const int : the new value of global config name
 * return: unsigned : 0 on success , 1 if conf option not found, 2 if conf
 *         value is not valid
 */
unsigned mm_config_set(const char *name, const int value)
{
	mm_conf_t *conf = mm_confs;
	while (conf < mm_confs + LEN(mm_confs) && strcmp(conf->nm, name) != 0)
		conf++;
	if (conf == mm_confs + LEN(mm_confs))
		return 1;
	if (value < conf->min || value > conf->max)
		return 2;
	conf->val = value;
	mm_config_save();
	return 0;
}
/* create the secret part of mastermind using session config
 * this fuction use random and save it on mm_secret->val && save freq of every
 * color on mm_secret->freq
 * param conf : mm_config* : config of current session
 * return : mm_secret* : secret objet for this session
 */
mm_secret *mm_secret_new(mm_config *conf)
{
	char i;
	mm_secret *sec = (mm_secret *)malloc(sizeof(mm_secret));
	sec->val = (uint8_t *)malloc(sizeof(uint8_t) * conf->holes);
	sec->freq = (uint8_t *)malloc(sizeof(uint8_t) * conf->colors);
	for (i = 0; i < conf->colors; i++)
		sec->freq[i] = 0;
	for (i = 0; i < conf->holes; i++) {
		do {
			sec->val[i] = random() % conf->colors;
		} while (conf->remise && sec->freq[sec->val[i]] > 0);
		sec->freq[sec->val[i]]++;
	}
	return sec;
}
void mm_scores_load()
{
	FILE *f;
	mm_scores.T =
	    (long unsigned *)malloc(sizeof(long unsigned) * mm_scores.max);
	f = fopen(mm_score_path, "r");
	if (f == NULL)
		return;
	while (mm_scores.len < mm_scores.max &&
	       fscanf(f, "%ld", mm_scores.T + mm_scores.len) != EOF)
		mm_scores.len++;
	fclose(f);
}
const mm_scores_t *mm_scores_get()
{
	if (mm_scores.T == NULL)
		mm_scores_load();
	return &mm_scores;
}
void mm_scores_save(mm_session *session)
{
	long unsigned score;
	unsigned i, j;
	FILE *f;
	if (mm_scores.T == NULL)
		mm_scores_load();
	score = mm_score(session);
	i = 0;
	if (mm_scores.len && mm_scores.T[mm_scores.len - 1] >= score) {
		if (mm_scores.len == mm_scores.max)
			return;
		else
			i = mm_scores.len;
	}
	// find where to insert score
	while (i < mm_scores.len && mm_scores.T[i] > score)
		i++;
	if (mm_scores.T[i] == score)
		return;
	// unshift position of the rest (scores lower than score)
	for (j = i; j < mm_scores.len; j++)
		mm_scores.T[j + 1] = mm_scores.T[j];
	mm_scores.T[i] = score;
	mm_scores.len++;
	f = fopen(mm_score_path, "w+");
	if (f == NULL)
		return;
	for (i = 0; i < mm_scores.len; i++)
		fprintf(f, "%ld\n", mm_scores.T[i]);
	fclose(f);
}
/* This function is the most important function in the code
 * this function accept new guess combination , add it to
 * the session if it's not ended and calculed the score of
 * the current guess  then update session status
 * param session : mm_session* : current session
 * param t : uint8_t : the new guess combination
 * return : unsigned :
 * 		0 on success
 *  	1 on failure
 * 			- session already ended
 * 			- combination is not valid
 */
unsigned mm_play(mm_session *session, uint8_t *T)
{
	char i;
	if (session->guessed >= session->config->guesses ||
	    session->state == MM_SUCCESS)
		return 1;
	mm_guess *G = session->panel + session->guessed;
	uint8_t *freq =
	    (uint8_t *)malloc(sizeof(uint8_t) * session->config->colors);
	for (i = 0; i < session->config->colors; i++)
		freq[i] = 0;
	G->combination = T;
	G->inplace = 0;
	G->insecret = 0;
	for (i = 0; i < session->config->holes; i++) {
		if (T[i] >= session->config->colors ||
		    (session->config->remise && freq[T[i]] > 0)) {
			free(freq);
			return 1;
		}
		freq[T[i]]++;
		if (freq[T[i]] <= session->secret->freq[T[i]])
			G->insecret++;
		if (session->secret->val[i] == T[i])
			G->inplace++;
	}
	session->guessed++;
	if (G->inplace == session->config->holes)
		session->state = MM_SUCCESS;
	else if (session->guessed == session->config->guesses)
		session->state = MM_FAIL;
	else
		session->state = MM_PLAYING;
	free(freq);
	if (mm_confs[MM_POS_SAVE_PLAY].val == 1)
		mm_session_save(session);
	if (session->state == MM_SUCCESS)
		mm_scores_save(session);
	return 0;
}
long unsigned mm_score(mm_session *session)
{
	unsigned i;
	long unsigned score =
	    session->config->holes * session->config->colors * MM_GUESSES_MAX;
	for (i = 0; i < session->guessed; i++)
		score -=
		    (session->config->holes * 2) -
		    (session->panel[i].inplace + session->panel[i].insecret);
	if (session->config->remise)
		score *= 0.75;
	return score;
}
/* get last gusess objet
 * param session : mm_session* : current objet
 * return : session : mm_session
 */
mm_guess mm_play_last(mm_session *session)
{
	assert(session->guessed == 0);
	return session->panel[session->guessed - 1];
}
/* This function initialize data && config && store files path using system
 * and core default standard
 * NOTE : this is an internal function
 */
void mm_init()
{
	mm_score_path = (char *)malloc(sizeof(char) * 2000);
	mm_config_path = (char *)malloc(sizeof(char) * 2000);
	mm_store_path = (char *)malloc(sizeof(char) * 2000);
	srandom(time(NULL));
#ifdef POSIX
	struct utsname unm;
	char *home = getenv("HOME");
	if (strcmp(unm.sysname, "Darwin") == 0) { // Mac OS
		sprintf(mm_config_path, "%s%s", home,
			"/Library/Application Support");
		if (access(mm_config_path, R_OK | W_OK | X_OK) == 0) {
			strcat(mm_config_path, "/" PACKAGE);
			strcpy(mm_score_path, mm_config_path);
		} else {
			sprintf(mm_config_path, "%s%s", home, "/." PACKAGE);
			strcpy(mm_score_path, mm_config_path);
		}
	} else { // Linux/Unix system ?
		char *xdg_config = getenv("XDG_CONFIG_HOME");
		char *xdg_data = getenv("XDG_DATA_HOME");
		// config dir; try use XDG based dir ~/.config/mastermind else
		// fallback to ~/.mastermind
		if (xdg_config)
			strcpy(mm_config_path, xdg_config);
		else
			sprintf(mm_config_path, "%s%s", home, "/.config");
		if (access(mm_config_path, W_OK | R_OK | X_OK) == 0)
			strcat(mm_config_path, "/" PACKAGE);
		else
			sprintf(mm_score_path, "%s%s", home, "/." PACKAGE);
		// data dir; try use XDG based dir ~/.local/share/mastermind
		// else fallback to ~/.mastermind
		if (xdg_data)
			strcpy(mm_score_path, xdg_data);
		else
			sprintf(mm_score_path, "%s%s", home, "/.local/share");
		if (access(mm_score_path, W_OK | R_OK | X_OK) == 0)
			strcat(mm_score_path, "/" PACKAGE);
		else
			sprintf(mm_score_path, "%s%s", home, "/." PACKAGE);
	}
	mkdir(mm_score_path, 0700);
	mkdir(mm_config_path, 0700);
#else
	if (getenv("APPDATA")) { // MS Windows
		sprintf(mm_score_path, "%s%s", getenv("APPDATA"), "/" PACKAGE);
		sprintf(mm_config_path, "%s%s", getenv("APPDATA"), "/" PACKAGE);
		CreateDirectory(mm_score_path, NULL);
		CreateDirectory(mm_config_path, NULL);
	} else {
		strcpy(mm_config_path, ".");
		strcpy(mm_score_path, ".");
	}
#endif
	sprintf(mm_store_path, "%s%s", mm_score_path, "/store.data");
	strcat(mm_config_path, "/config");
	strcat(mm_score_path, "/score.txt");
}
/* free session object
 * param session : mm_session* : session object
 */
void mm_session_free(mm_session *session)
{
	while (session->guessed--)
		free(session->panel[session->guessed].combination);
	free(session->panel);
	free(session->secret->freq);
	free(session->secret->val);
	free(session->secret);
	free(session->config);
	free(session);
}
/* save session if not ended && save_on_exit = 1 then free object
 * param session : mm_session* : session object
 */
void mm_session_exit(mm_session *session)
{
	if (session->state == MM_PLAYING && mm_confs[MM_POS_SAVE_EXIT].val == 1)
		mm_session_save(session);
	mm_session_free(session);
}
/* save session object on mm_store_path file
 * param session : mm_session* : current session object
 * return : unsigned : 0 on success , 1 on failure
 */
unsigned int mm_session_save(mm_session *session)
{
	/* [----------- 8 bits -----------]
	 * [ mm_session->guessed          ]
	 * [ mm_session->state            ]
	 * [ mm_config->guesses           ]
	 * [ mm_config->colors            ]
	 * [ mm_config->holes             ]
	 * [ mm_config->remise            ]
	 * [............mm_secret->val.............] // 8 bits x config.holes
	 * [............mm_secret->freq............] // 8 bits x config.colors
	 * [...mm_session->panel[0].combination....] // 8 bits x config.holes
	 * [ mm_session->panel[0].inplace ]
	 * [ mm_session->panel[0].secret  ]
	 * [ ............ ] // panel[0] ... panel[session.guessed - 1]
	 */
	unsigned i;
	FILE *f = fopen(mm_store_path, "wb");
	if (!f)
		return -1;
	fwrite(&session->guessed, sizeof(uint8_t), 1, f);
	fwrite(&session->state, sizeof(uint8_t), 1, f);
	fwrite(session->config, sizeof(mm_config), 1, f);
	fwrite(session->secret->val, sizeof(uint8_t), session->config->holes,
	       f);
	fwrite(session->secret->freq, sizeof(uint8_t), session->config->colors,
	       f);
	for (i = 0; i < session->guessed; i++) {
		fwrite(session->panel[i].combination, sizeof(uint8_t),
		       session->config->holes, f);
		fwrite(&session->panel[i].inplace, sizeof(uint8_t), 1, f);
		fwrite(&session->panel[i].insecret, sizeof(uint8_t), 1, f);
	}
	fclose(f);
	return 0;
}
/* this function restore session object from mm_store_path file
 * return mm_session* : NULL on failure , session object pointeur on
 * success
 */
mm_session *mm_session_restore()
{
	if (!mm_store_path)
		mm_init();
	FILE *f = fopen(mm_store_path, "rb");
	if (!f)
		return NULL;
	mm_session *session = (mm_session *)malloc(sizeof(mm_session));
	if (!fread(&session->guessed, sizeof(uint8_t), 1, f))
		goto session_err;
	if (!fread(&session->state, sizeof(uint8_t), 1, f))
		goto session_err;
	session->config = (mm_config *)malloc(sizeof(mm_config));
	if (!fread(session->config, sizeof(mm_config), 1, f))
		goto conf_err;
	session->secret = (mm_secret *)malloc(sizeof(mm_secret));
	session->secret->val =
	    (uint8_t *)malloc(sizeof(uint8_t) * session->config->holes);
	if (!fread(session->secret->val, sizeof(uint8_t),
		   session->config->holes, f))
		goto sec_val_err;
	session->secret->freq =
	    (uint8_t *)malloc(sizeof(uint8_t) * session->config->colors);
	if (!fread(session->secret->freq, sizeof(uint8_t),
		   session->config->colors, f))
		goto sec_freq_err;
	unsigned i = 0;
	session->panel =
	    (mm_guess *)malloc(sizeof(mm_guess) * session->config->guesses);
	for (i = 0; i < session->guessed; i++) {
		session->panel[i].combination =
		    (uint8_t *)malloc(sizeof(uint8_t) * session->config->holes);
		if (!fread(session->panel[i].combination, sizeof(uint8_t),
			   session->config->holes, f) ||
		    !fread(&session->panel[i].inplace, sizeof(uint8_t), 1, f) ||
		    !fread(&session->panel[i].insecret, sizeof(uint8_t), 1, f))
			goto guess_err;
	}
	fclose(f);
	remove(mm_store_path);
	return session;

guess_err:
	while (i--)
		free(session->panel[i].combination);
	free(session->panel);
sec_freq_err:
	free(session->secret->freq);
sec_val_err:
	free(session->secret->val);
	free(session->secret);
conf_err:
	free(session->config);
session_err:
	free(session);
	fclose(f);
	remove(mm_store_path);
	return NULL;
}
