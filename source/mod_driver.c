#include "mod_driver.h"

#include <stdio.h>
#include <sys/stat.h>
#include <malloc.h>

ModplugPlayer create_player() {
  ModplugPlayer player;
  player.settings.mFlags = MODPLUG_ENABLE_OVERSAMPLING | MODPLUG_ENABLE_NOISE_REDUCTION;
  player.settings.mChannels = CHANNEL_COUNT;
  player.settings.mBits = 16;
  player.settings.mFrequency = 44100;
  player.settings.mResamplingMode = MODPLUG_RESAMPLE_LINEAR;

  /* Fill with modplug defaults */
  player.settings.mStereoSeparation = 128;
  player.settings.mMaxMixChannels = 32;
  player.settings.mReverbDepth = 0;
  player.settings.mReverbDelay = 0;
  player.settings.mBassAmount = 0;
  player.settings.mBassRange = 0;
  player.settings.mSurroundDepth = 0;
  player.settings.mSurroundDelay = 0;
  player.settings.mLoopCount = -1;
  player.next_buffer = 0;

  ModPlug_SetSettings(&player.settings);
  player.decoder_buffer_size = 3200;
  return player;
}

void audio_callback(void* const data) {
  ModplugPlayer* player = (ModplugPlayer*)(data); // Not safe at all...
  if (player->wave_buffers[player->next_buffer].status == NDSP_WBUF_DONE) {
    size_t decoded = ModPlug_Read(player->file, player->wave_buffers[player->next_buffer].data_pcm16, player->decoder_buffer_size);
    if (decoded != 0) {
      player->wave_buffers[player->next_buffer].nsamples = ((decoded / 2) / sizeof(int16_t));
      DSP_FlushDataCache(player->wave_buffers[player->next_buffer].data_pcm16, decoded);
      ndspChnWaveBufAdd(0, &player->wave_buffers[player->next_buffer]);
      player->next_buffer++;
      if (player->next_buffer == CHANNEL_COUNT) {
        player->next_buffer = 0;
      }
    }
  }
}

int play_mod(ModplugPlayer* player, const char* file_path) {
  struct stat file_stat;
  stat(file_path, &file_stat);
  size_t buffer_size = file_stat.st_size;

  FILE* file = fopen(file_path, "rb");
  void* buffer = (void*)malloc(buffer_size);
  fread(buffer, buffer_size, 1, file);
  player->file = ModPlug_Load(buffer, buffer_size);
  
  free(buffer);
  fclose(file);
  
  if (player->file == 0) {
    return -1;
  }

  ModPlug_SetMasterVolume(player->file, 128);
  
  ndspChnReset(0);
  ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);
  ndspChnSetRate(0, 44100);
  ndspChnSetInterp(0, NDSP_INTERP_LINEAR);

  for (u8 i = 0; i < CHANNEL_COUNT; i++) {
    player->wave_buffers[i].data_pcm16 = linearAlloc(player->decoder_buffer_size);
    player->wave_buffers[i].looping = false;
    player->wave_buffers[i].status = NDSP_WBUF_DONE;
  }
  for (u8 i = 0; i < CHANNEL_COUNT; i++) {
    audio_callback(player);
  }

  ndspSetCallback(audio_callback, player);
  return 0;
}

void stop_mod(ModplugPlayer* player) {
  ModPlug_Unload(player->file);
  for (u8 i = 0; i < CHANNEL_COUNT; i++) {
    linearFree(player->wave_buffers[0].data_pcm16);
  }
}
