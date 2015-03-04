#include "core.h"
#include <stdlib.h>
#include <time.h>
#include <assert.h>

mm_session *mm_session_new() {
	mm_session *session = (mm_session *)malloc(sizeof(mm_session));
	session->config = mm_config_load();
	session->guessed = 0;
	session->secret = mm_secret_new(session->config);
	session->state = MM_NEW;
	session->panel = (mm_guesse *)malloc(sizeof(mm_guesse) *
			session->config->guesses);
	return session;
}
mm_config *mm_config_load() {
	mm_config *config = (mm_config *)malloc(sizeof(mm_config));
	config->guesses = MM_GUESSES;
	config->colors  = MM_COLORS;
	config->holes   = MM_HOLES;
	return config;
}
mm_secret *mm_secret_new(mm_config *conf) {
	char i;
	mm_secret *sec = (mm_secret *)malloc(sizeof(mm_secret));
	sec->val = (unsigned char*)malloc(sizeof(unsigned char) * conf->holes);
	sec->freq = (unsigned char*)malloc(sizeof(unsigned char) * conf->colors);
	srandom(time(NULL));
	for(i = 0; i < conf->colors; i++)
		sec->freq[i] = 0;
	for(i = 0; i < conf->holes; i++) {
		sec->val[i] = random() % conf->colors;
		sec->freq[sec->val[i]]++;
	}
	return sec;
}
unsigned mm_play(mm_session *session, unsigned char *T) {
	char i;
	if(session->guessed >= session->config->guesses ||
			session->state == MM_SUCCESS)
		return 1;
	mm_guesse *G = session->panel + session->guessed;
	unsigned char *freq = (unsigned char*)malloc(sizeof(unsigned char) *
			session->config->colors);
	for(i=0; i < session->config->colors; i++)
		freq[i] = 0;
	G->combination = T;
	G->inplace = 0;
	G->insecret = 0;
	for(i=0; i < session->config->holes; i++) {
		if (T[i] >= session->config->colors) {
			free(freq);
			return 1;
		}
		freq[T[i]]++;
		if(freq[T[i]] <= session->secret->freq[T[i]])
			G->insecret++;
		if(session->secret->val[i] == T[i])
			G->inplace++;
	}
	session->guessed++;
	if(G->inplace == session->config->holes)
		session->state = MM_SUCCESS;
	else if(session->guessed == session->config->guesses)
		session->state = MM_FAIL;
	else
		session->state = MM_PLAYING;
	free(freq);
	return 0;
}
mm_guesse mm_play_last(mm_session *session) {
	assert(session->guessed == 0);
	return session->panel[session->guessed - 1];
}
