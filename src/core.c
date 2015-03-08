#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "core.h"

#define LEN(a) (sizeof(a) / sizeof(a[0]))
char *mm_config_path = NULL;
char *mm_data_path = NULL;
char *mm_store_path = NULL;

mm_conf_t mm_confs[3] = {
    {.nm = "guesses", .val = MM_GUESSES},
    {.nm = "colors", .val = MM_COLORS},
    {.nm = "holes", .val = MM_HOLES},
};

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
			for (conf = mm_confs; conf < mm_confs + LEN(mm_confs);
			     conf++) {
				if (!strcmp(n, conf->nm))
					conf->val = d;
			}
		}
		free(n);
		fclose(f);
	}
	config->guesses = (uint8_t)mm_confs[0].val;
	config->colors = (uint8_t)mm_confs[1].val;
	config->holes = (uint8_t)mm_confs[2].val;
	return config;
}
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
unsigned mm_config_set(const char *name, const int value)
{
	mm_conf_t *conf = mm_confs;
	while (conf < mm_confs + LEN(mm_confs) && strcmp(conf->nm, name) != 0)
		conf++;
	if (conf == mm_confs + LEN(mm_confs))
		return -1;
	conf->val = value;
	mm_config_save();
	return 0;
}
mm_secret *mm_secret_new(mm_config *conf)
{
	char i;
	mm_secret *sec = (mm_secret *)malloc(sizeof(mm_secret));
	sec->val = (uint8_t *)malloc(sizeof(uint8_t) * conf->holes);
	sec->freq = (uint8_t *)malloc(sizeof(uint8_t) * conf->colors);
	srandom(time(NULL));
	for (i = 0; i < conf->colors; i++)
		sec->freq[i] = 0;
	for (i = 0; i < conf->holes; i++) {
		sec->val[i] = random() % conf->colors;
		sec->freq[sec->val[i]]++;
	}
	return sec;
}
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
		if (T[i] >= session->config->colors) {
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
	return 0;
}
mm_guess mm_play_last(mm_session *session)
{
	assert(session->guessed == 0);
	return session->panel[session->guessed - 1];
}
void mm_init()
{
	char *home = getenv("HOME");
	struct utsname unm;
	char *xdg_config = getenv("XDG_CONFIG_HOME");
	char *xdg_data = getenv("XDG_DATA_HOME");
	mm_data_path = (char *)malloc(sizeof(char) * 2000);
	mm_config_path = (char *)malloc(sizeof(char) * 2000);
	mm_store_path = (char *)malloc(sizeof(char) * 2000);
	if (getenv("APPDATA")) { // running on windows
		strcpy(mm_data_path, getenv("APPDATA"));
		strcpy(mm_config_path, getenv("APPDATA"));
	} else if (strcmp(unm.sysname, "Darwin") == 0) { // Mac OS
		sprintf(mm_config_path, "%s%s", home,
			"/Library/Application Support");
		if (access(mm_config_path, R_OK | W_OK | X_OK) == 0) {
			strcat(mm_config_path, "/" PACKAGE);
			strcpy(mm_data_path, mm_config_path);
		} else {
			sprintf(mm_config_path, "%s%s", home, "/." PACKAGE);
			strcpy(mm_data_path, mm_config_path);
		}
	} else { // Linux/Unix system ?
		// config dir; try use XDG based dir ~/.config/mastermind else
		// fallback to ~/.mastermind
		if (xdg_config)
			sprintf(mm_config_path, "%s%s", xdg_config,
				"/" PACKAGE);
		else
			sprintf(mm_config_path, "%s%s", home,
				"/.config/" PACKAGE);
		if (access(mm_config_path, W_OK | R_OK | X_OK) == 0)
			strcat(mm_config_path, "/" PACKAGE);
		else
			sprintf(mm_data_path, "%s%s", home, "/." PACKAGE);
		// data dir; try use XDG based dir ~/.local/share/mastermind
		// else fallback to ~/.mastermind
		if (xdg_data)
			sprintf(mm_data_path, "%s%s", xdg_data, "/" PACKAGE);
		else
			sprintf(mm_data_path, "%s%s", home,
				"/.local/share/" PACKAGE);
		if (access(mm_data_path, W_OK | R_OK | X_OK) == 0)
			strcat(mm_data_path, "/" PACKAGE);
		else
			sprintf(mm_data_path, "%s%s", home, "/." PACKAGE);
	}
	mkdir(mm_data_path, 0700);
	mkdir(mm_config_path, 0700);
	sprintf(mm_store_path, "%s%s", mm_data_path, "/store.data");
	strcat(mm_config_path, "/config");
	strcat(mm_data_path, "/data.txt");
}
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
unsigned int mm_session_save(mm_session *session)
{
	/* [...mm_session...]
	 * [...mm_config...]
	 * [...mm_secret->val...]
	 * [...mm_secret->freq...]
	 * [...mm_guess[0]->combination...]
	 * [...mm_guess[0]->in{place,secret}...]
	 * ....
	 */
	unsigned i;
	FILE *f = fopen(mm_store_path, "wb");
	if (!f)
		return -1;
	mm_session *_session = (mm_session *)malloc(sizeof(mm_session));
	memcpy(_session, session, sizeof(mm_session));
	_session->secret = NULL;
	_session->config = NULL;
	_session->panel = NULL;
	fwrite(_session, sizeof(mm_session), 1, f);
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
	free(_session);
	return 0;
}
mm_session *mm_session_restore()
{
	if (!mm_store_path)
		mm_init();
	FILE *f = fopen(mm_store_path, "rb");
	if (!f)
		return NULL;
	mm_session *session = (mm_session *)malloc(sizeof(mm_session));
	if (!fread(session, sizeof(mm_session), 1, f))
		goto session_err;
	if (session->config != NULL || session->secret != NULL ||
	    session->panel != NULL)
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
