#if 0
vox2v3a.exe: vox2v3a.c; cl vox2v3a.c /Ox /G6Fy /MD /link /opt:nowin98
    del vox2v3a.obj
!if 0
#endif

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char **argv)
{
    FILE *fil;
    long x, y, z, xsiz, ysiz, zsiz;
    unsigned char ch, palette[256][3], *voxel;

    if (argc < 3)
    {
        printf("VOX to V3A converter                       by Ken Silverman (05/14/2007)\n");
        printf("Example #1: >vox2v3a model.vox model.v3a\n");
        exit(0);
    }
    fil = fopen(argv[1],"rb"); if (!fil) { printf("File not found: %s",argv[1]); exit(0); }
    fread(&xsiz,4,1,fil);
    fread(&ysiz,4,1,fil);
    fread(&zsiz,4,1,fil);
    voxel = (char *)malloc(xsiz*ysiz*zsiz);
    fread(voxel,xsiz*ysiz*zsiz,1,fil);
    fread(palette,768,1,fil);
    fclose(fil);

    fil = fopen(argv[2],"rb");
    if (fil)
    {
        fclose(fil);
        printf("%s already exists. Overwrite? (y/n)",argv[2]);
        do
        {
            ch = getch();
            if ((ch == 27) || (ch == 'N') || (ch == 'n')) { printf("\nOperation cancelled."); exit(0); }
            if ((ch == 0) || (ch == 0xe0)) ch = getch();
        } while ((ch != 'Y') && (ch != 'y'));
    }

    fil = fopen(argv[2],"wb"); if (!fil) { printf("Can't write file: %s",argv[2]); exit(0); }
    fprintf(fil,"VERSION 1.0\r\n");
    fprintf(fil,"TYPE VoxelCubic\r\n");
    fprintf(fil,"DIMENSION %d.0 %d.0 %d.0\r\n",xsiz,ysiz,zsiz);
    fprintf(fil,"SIZE %d %d %d\r\n",xsiz,ysiz,zsiz);
    fprintf(fil,"DATA ");
    for(x=0;x<xsiz;x++)
    {
        for(y=ysiz-1;y>=0;y--)
        {
            for(z=zsiz-1;z>=0;z--)
            {
                ch = voxel[x*ysiz*zsiz + y*zsiz + z];
                if (ch == 255) fprintf(fil,"-1 -1 -1 -1 ");
                else fprintf(fil,"%d %d %d %d ",palette[ch][0],palette[ch][1],palette[ch][2],255);
            }
            fprintf(fil,"\r\n");
        }
        fprintf(fil,"\r\n");
    }
    fclose(fil);
}

#if 0
!endif
#endif
