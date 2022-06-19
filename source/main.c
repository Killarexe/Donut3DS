#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

int k;

int main(int argc, char **argv)
{
    romfsInit();
	gfxInitDefault();

	PrintConsole top, bottom;

	consoleInit(GFX_TOP, &top);
	consoleInit(GFX_BOTTOM, &bottom);

	float A=0, B=0, i, j, z[1760];
    char b[1760];
    consoleSelect(&bottom);
    printf("\x1b[31mHold Start to exit.\x1b[0m");
    consoleSelect(&top);
    printf("\x1b[2J");
    for(; ; ) {
    	hidScanInput();
    	u32 kDown = hidKeysDown();
    	if (kDown & KEY_START) break;
        memset(b,32,1760);
        memset(z,0,7040);
        for(j=0; 6.28>j; j+=0.07) {
            for(i=0; 6.28 >i; i+=0.02) {
                float sini=sin(i),
                      cosj=cos(j),
                      sinA=sin(A),
                      sinj=sin(j),
                      cosA=cos(A),
                      cosj2=cosj+2,
                      mess=1/(sini*cosj2*sinA+sinj*cosA+5),
                      cosi=cos(i),
                      cosB=cos(B),
                      sinB=sin(B),
                      t=sini*cosj2*cosA-sinj* sinA;
                int x=25+25*mess*(cosi*cosj2*cosB-t*sinB),
                    y=15+10*mess*(cosi*cosj2*sinB +t*cosB),
                    o=x+80*y,
                    N=8*((sinj*sinA-sini*cosj*cosA)*cosB-sini*cosj*sinA-sinj*cosA-cosi *cosj*sinB);
                if(22>y&&y>0&&x>0&&80>x&&mess>z[o]){
                    z[o]=mess;
                    b[o]=".,-~:;=!*#$@"[N>0?N:0];
                }
            }
        }
        for(k=0; 1761>k; k++)
            putchar(k%80?b[k]:10);
        printf("\x1b[H");
        A+=0.08;
        B+= 0.04;

        gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
    }

	gfxExit();
	return 0;
}
