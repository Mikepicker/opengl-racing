#ifndef audio_h
#define audio_h

#include "engine.h"

ALCenum error;
#define AUDIO_TEST_ERROR(_msg)		\
	error = alGetError();		\
	if (error != AL_NO_ERROR) {	\
		fprintf(stderr, _msg "\n");	\
		return -1;		\
	}

ALCdevice* audio_device;
ALCcontext* audio_context;
ALuint audio_buffer;

int audio_init();
int audio_load_sound(const char* filename);
int audio_add_source(ALuint* source);
int audio_play_source(ALuint source);
int audio_source_playing(ALuint source);
void audio_free();

#endif
