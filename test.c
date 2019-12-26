#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "beep.h"
#include <unistd.h>

//Project Fundementals of computer programming
//Made by Seyyed nima moeintaghavi

float getfrequency(char target_note[])
{
    char str[1000];
    FILE *header_file;
    header_file = fopen("MIDI_header_complete_it_yourself.h","r");
    //Read until you reach a #define line then read values and compare
    while(fscanf(header_file, "%s", str) != EOF)
    {
        if(strcmp(str, "#define") == 0)
        {

            char note[4] = "";

            fscanf(header_file, "%s", note);

            float freq;

            fscanf(header_file, "%f", &freq);

            if(strcmp(note, target_note) == 0)
            {
                return freq;
            }
        }
    }
    fclose(header_file);
}

void playnotes()
{
    int i;
    char str[1000];
    FILE *notes_file;
    notes_file = fopen("NOTES.txt", "r");
    //Read until you reach a #define line then read values and beep
    while(fscanf(notes_file, "%s", str) != EOF)
    {
        if(strcmp(str, "-N") == 0)
        {
            char note[4] = "";

            fscanf(notes_file, "%s", note);

            //Replace all # with s
            for(i=0; i<strlen(note); i++)
            {
                if(note[i] == '#')
                {
                    note[i] = 's';
                }
            }

            char dummy[4] = "";

            //-d is dummy string (garbage)

            fscanf(notes_file, "%s", dummy);

            int length = 0;

            fscanf(notes_file, "%d", &length);
            beep(getfrequency(note), length);
        }
    }
}

//PHASE TWO:

int read_bytes(int FD_ID, int length)
{
    
}

int main()
{
    printf("** Midi File Player Project **\nPlease enter phase: (1 to 4)\n");
    int phase = -1;
    while(!(phase >= 1 && phase <=4))
        scanf("%d", &phase);
    if(phase == 1)
    {
        printf("==>Playing notes from NOTES.txt");
        playnotes();
    }
    else if(phase == 2)
    {   
    }
    else
    {
        printf("Not implemented yet >.> pls try later, kay?");
    }
    return 0;
}

//sudo apt install pulseaudio
//sudo apt install alsa-utils
//before runing run this command : pulseaudio -k && sudo alsa force-reload
//./a.out
//compile with : gcc test.c beep.o sintable.o -lasound