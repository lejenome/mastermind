#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "core.h"

char *mm_config_path = NULL;
char *mm_data_path = NULL;
char *mm_store_path = NULL;
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
	if (!mm_config_path)
		mm_init();
	mm_config *config = (mm_config *)malloc(sizeof(mm_config));
	config->guesses = MM_GUESSES;
	config->colors = MM_COLORS;
	config->holes = MM_HOLES;
	return config;
}
mm_secret *mm_secret_new(mm_config *conf)
{
	char i;
	mm_secret *sec = (mm_secret *)malloc(sizeof(mm_secret));
	sec->val = (unsigned char *)malloc(sizeof(unsigned char) * conf->holes);
	sec->freq =
	    (unsigned char *)malloc(sizeof(unsigned char) * conf->colors);
	srandom(time(NULL));
	for (i = 0; i < conf->colors; i++)
		sec->freq[i] = 0;
	for (i = 0; i < conf->holes; i++) {
		sec->val[i] = random() % conf->colors;
		sec->freq[sec->val[i]]++;
	}
	return sec;
}
unsigned mm_play(mm_session *session, unsigned char *T)
{
	char i;
	if (session->guessed >= session->config->guesses ||
	    session->state == MM_SUCCESS)
		return 1;
	mm_guess *G = session->panel + session->guessed;
	unsigned char *freq = (unsigned char *)malloc(sizeof(unsigned char) *
						      session->config->colors);
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
	char *xdg_config = getenv("XDG_CONFIG_HOME");
	char *xdg_data = getenv("XDG_DATA_HOME");
	mm_data_path = (char *)malloc(sizeof(char) * 2000);
	mm_config_path = (char *)malloc(sizeof(char) * 2000);
	mm_store_path = (char *)malloc(sizeof(char) * 2000);
	if (xdg_config)
		sprintf(mm_config_path, "%s%s", xdg_config, "/" PACKAGE);
	else
		sprintf(mm_config_path, "%s%s", home, "/.config/" PACKAGE);
	if (xdg_data)
		sprintf(mm_data_path, "%s%s", xdg_data, "/" PACKAGE);
	else
		sprintf(mm_data_path, "%s%s", home, "/.local/share/" PACKAGE);
	char *cmd = (char *)malloc(sizeof(char) * 4096);
	snprintf(cmd, 4096, "mkdir -p '%s' '%s' > /dev/null", mm_data_path,
		 mm_config_path);
	system(cmd);
	sprintf(mm_store_path, "%s%s", mm_data_path, "/store.data");
	strcat(mm_config_path, "/config");
	strcat(mm_data_path, "/data.txt");
	free(cmd);
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
	fwrite(session->secret->val, sizeof(unsigned char),
	       session->config->holes, f);
	fwrite(session->secret->freq, sizeof(unsigned char),
	       session->config->colors, f);
	for (i = 0; i < session->guessed; i++) {
		fwrite(session->panel[i].combination, sizeof(unsigned char),
		       session->config->holes, f);
		fwrite(&session->panel[i].inplace, sizeof(unsigned char), 1, f);
		fwrite(&session->panel[i].insecret, sizeof(unsigned char), 1,
		       f);
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
	session->secret->val = (unsigned char *)malloc(sizeof(unsigned char) *
						       session->config->holes);
	if (!fread(session->secret->val, sizeof(unsigned char),
		   session->config->holes, f))
		goto sec_val_err;
	session->secret->freq = (unsigned char *)malloc(
	    sizeof(unsigned char) * session->config->colors);
	if (!fread(session->secret->freq, sizeof(unsigned char),
		   session->config->colors, f))
		goto sec_freq_err;
	unsigned i = 0;
	session->panel =
	    (mm_guess *)malloc(sizeof(mm_guess) * session->config->guesses);
	for (i = 0; i < session->guessed; i++) {
		session->panel[i].combination = (unsigned char *)malloc(
		    sizeof(unsigned char) * session->config->holes);
		if (!fread(session->panel[i].combination, sizeof(unsigned char),
			   session->config->holes, f) ||
		    !fread(&session->panel[i].inplace, sizeof(unsigned char), 1,
			   f) ||
		    !fread(&session->panel[i].insecret, sizeof(unsigned char),
			   1, f))
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
