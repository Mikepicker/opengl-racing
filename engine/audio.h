#ifndef audio_h
#define audio_h

#include "engine.h"

#define AUDIO_MAX_BUFFERS 16

ALCenum error;
#define AUDIO_TEST_ERROR(_msg)		\
	error = alGetError();		\
	if (error != AL_NO_ERROR) {	\
		fprintf(stderr, _msg "\n");	\
		return -1;		\
	}

ALCdevice* audio_device;
ALCcontext* audio_context;
ALuint audio_buffers[AUDIO_MAX_BUFFERS];
int audio_buffers_count;

int audio_init();
int audio_load_sound(const char* filename, ALuint* out_buffer);
int audio_add_source(ALuint* source, ALuint buffer);
int audio_play_source(ALuint source);
int audio_source_playing(ALuint source);
int audio_move_listener(vec3 pos);
int audio_move_source(ALuint source, vec3 pos);
int audio_loop_source(ALuint source, ALboolean loop);
void audio_free();

#endif
