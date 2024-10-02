#include <3ds.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "mod_driver.h"

#define BOTTOM_SCREEN_WIDTH 40
#define BOTTOM_SCREEN_HEIGHT 30

int main() {
  gfxInitDefault();
  PrintConsole top_screen, bottom_screen;
  consoleInit(GFX_TOP, &top_screen);
  consoleInit(GFX_BOTTOM, &bottom_screen);
  ndspInit();
  romfsInit();

  consoleSelect(&bottom_screen);
  printf("\x1b[37;44m");
  for (u8 y = 0; y < BOTTOM_SCREEN_HEIGHT; y++) {
    for (u8 x = 0; x < BOTTOM_SCREEN_WIDTH; x++) {
      char c = ' ';
      if ((x == 0 || x == BOTTOM_SCREEN_WIDTH - 1) && (y == 0 || y == BOTTOM_SCREEN_HEIGHT - 1)) {
        c = '.';
      } else if (y == 0) {
        c = '-';
      } else if (y == BOTTOM_SCREEN_HEIGHT - 1) {
        c = '_';
      } else if (x == 0 || x == BOTTOM_SCREEN_WIDTH - 1) {
        c = '|';
      }
      printf("%c", c);
    }
  }

  ModplugPlayer player = create_player();
  int result = play_mod(&player, "romfs:/main_theme.mod");
  if (result != 0) {
    printf("Failed to play MOD file.\n");
  }

  printf("\x1b[2;2HDonut 3DS        (Press START to quit)");
  printf("\x1b[4;2HBased on the orignal donut.c by Andy.S");
  printf("\x1b[6;2HInitial concept by jornmann.");
  printf("\x1b[8;2HPorted by Killar.");
  printf("\x1b[10;2HMusic by Liljedahl.");

  consoleSelect(&top_screen);
  
  float A = 0, B = 0;
  float i, j;
  int k;
  float z[1500];
  char b[1500];

  while (aptMainLoop()) {
    hidScanInput();
    u32 keyDown = hidKeysDown();
    if (keyDown & KEY_START) {
      break;
    }

    memset(b, 32, 1500);
    memset(z, 0, 1500 * sizeof(float));
    for (j = 0; j < 6.28; j += 0.07) {
        for (i = 0; i < 6.28; i += 0.02) {
            float c = sin(i);
            float d = cos(j);
            float e = sin(A);
            float f = sin(j);
            float g = cos(A);
            float h = d + 2;
            float D = 1 / (c * h * e + f * g + 5);
            float l = cos(i);
            float m = cos(B);
            float n = sin(B);
            float t = c * h * g - f * e;
            int x = 25 + 30 * D * (l * h * m - t * n);
            int y = 15 + 20 * D * (l * h * n + t * m);
            int o = x + 50 * y;
            int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);
            if (30 > y && y > 0 && x > 0 && 50 > x && D > z[o]) {
                z[o] = D;
                b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
            }
        }
    }
    printf("\x1b[1;1H");
    for (k = 0; k < 1500; k++) {
        putchar(k % 50 ? b[k] : 10);
    }
    A += 0.08;
    B += 0.04;

    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
  }

  stop_mod(&player);

  romfsExit();
  ndspExit();
  gfxExit();
	return 0;
}
