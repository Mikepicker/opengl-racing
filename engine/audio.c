#include "audio.h"

static void list_audio_devices(const ALCchar *devices) {
  const ALCchar *device = devices, *next = devices + 1;
  size_t len = 0;

  fprintf(stdout, "[audio_init] devices list:\n");
  fprintf(stdout, "----------\n");
  while (device && *device != '\0' && next && *next != '\0') {
    fprintf(stdout, "%s\n", device);
    len = strlen(device);
    device += (len + 1);
    next += (len + 2);
  }
  fprintf(stdout, "----------\n");
}

int audio_init() {
  audio_device = alcOpenDevice(NULL);
  if (!audio_device) {
    printf("[audio_init] error: could not initialize device\n");
    return -1;
  }

	fprintf(stdout, "[audio_init] error: using device: %s\n", alcGetString(audio_device, ALC_DEVICE_SPECIFIER));

  ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");;
  if (enumeration) {
    list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
  } else {
    printf("[audio_init] enumeration not available\n");
  }

  alGetError();

  audio_context = alcCreateContext(audio_device, NULL);
  if (!alcMakeContextCurrent(audio_context)) {
    printf("[audio_init] error: could not initialize context\n");
    return -1;
  } 

  AUDIO_TEST_ERROR("[audio_init] error: make default context");

  ALfloat listener_ori[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f  };
  alListener3f(AL_POSITION, 0, 0, 1.0f);
  AUDIO_TEST_ERROR("[audio_init] error: listener position");
  alListener3f(AL_VELOCITY, 0, 0, 0);
  AUDIO_TEST_ERROR("[audio_init] error: listener velocity");
  alListenerfv(AL_ORIENTATION, listener_ori);
  AUDIO_TEST_ERROR("[audio_init] error: listener orientation");

  alutInitWithoutContext(NULL, NULL);

  audio_buffers_count = 0;

  return 1;
}

int audio_load_sound(const char* filename, ALuint* out_buffer) {
  audio_buffers[audio_buffers_count] = alutCreateBufferFromFile(filename);
  //audio_buffer = alutCreateBufferHelloWorld();
  AUDIO_TEST_ERROR("[audio_load_sound] error: buffer generation");
  *out_buffer = audio_buffers[audio_buffers_count];
  audio_buffers_count++;
}

int audio_add_source(ALuint* source, ALuint buffer) {
  alGenSources(1, source);
  AUDIO_TEST_ERROR("[audio_add_source] error: source generation");

  alSourcef(*source, AL_PITCH, 1);
  AUDIO_TEST_ERROR("[audio_add_source] error: source pitch");
  alSourcef(*source, AL_GAIN, 1);
  AUDIO_TEST_ERROR("[audio_add_source] error: source gain");
  alSource3f(*source, AL_POSITION, 0, 0, 0);
  AUDIO_TEST_ERROR("[audio_add_source] error: source position");
  alSource3f(*source, AL_VELOCITY, 0, 0, 0);
  AUDIO_TEST_ERROR("[audio_add_source] error: source velocity");
  alSourcei(*source, AL_LOOPING, AL_FALSE);
  AUDIO_TEST_ERROR("[audio_add_source] error: source looping");

  alSourcei(*source, AL_BUFFER, buffer);
  AUDIO_TEST_ERROR("[audio_add_source] error: source-buffer binding");
}

int audio_play_source(ALuint source) {
  alSourcePlay(source);
  AUDIO_TEST_ERROR("[audio_play_source] error: play source");
}

int audio_source_playing(ALuint source) {
  ALint source_state;
  alGetSourcei(source, AL_SOURCE_STATE, &source_state);
  AUDIO_TEST_ERROR("[audio_source_playing] error: get source state");
  return source_state == AL_PLAYING;
}

int audio_move_listener(vec3 pos) {
  alListener3f(AL_POSITION, pos[0], pos[1], pos[2]);
  AUDIO_TEST_ERROR("[audio_init] error: listener position");
}

int audio_move_source(ALuint source, vec3 pos) {
  alSource3f(source, AL_POSITION, pos[0], pos[1], pos[2]);
  AUDIO_TEST_ERROR("[audio_add_source] error: source position");
}

int audio_loop_source(ALuint source, ALboolean loop) {
  alSourcei(source, AL_LOOPING, loop);
  AUDIO_TEST_ERROR("[audio_add_source] error: source looping");
}

int audio_init_object(object* object, ALuint source, int loop) {
  ALuint s;
  audio_add_source(&s, source);
  if (loop) { audio_loop_source(s, AL_TRUE); }
  else { audio_loop_source(s, AL_FALSE); }
  audio_play_source(s);
  object->audio_source = s;
}

void audio_free() {
  for (int i = 0; i < AUDIO_MAX_BUFFERS; i++) {
    alDeleteBuffers(1, &audio_buffers[i]);
  }
  audio_device = alcGetContextsDevice(audio_context);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(audio_context);
  alcCloseDevice(audio_device);
  alutExit();
}
