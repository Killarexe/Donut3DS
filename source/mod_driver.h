#ifndef MOD_DRIVER_H
#define MOD_DRIVER_H

#define CHANNEL_COUNT 2

#include <3ds.h>
#include <libmodplug/modplug.h>

typedef struct ModplugPlayer {
  ModPlugFile* file;
  ModPlug_Settings settings;
  ndspWaveBuf wave_buffers[CHANNEL_COUNT];
  size_t decoder_buffer_size;
  int next_buffer;
 } ModplugPlayer;

ModplugPlayer create_player();

void audio_callback(void* const data);

int play_mod(ModplugPlayer* player, const char* file_path);

void stop_mod(ModplugPlayer* player);

#endif
