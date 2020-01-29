#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "beep.h"

//CUSTOM BEEP
#include <alsa/asoundlib.h>
int abeep(double frequency, double duration);

/*
==========================
MIDI FILE PLAYER PROJECT..
::::::::::::::::::::::::::
MADE.BY.SEYYED NIMA MOEIN.
TAGHAVI.........9822762211
::::::::::::::::::::::::::
FOR.FOCP.SUPERVISED.BY.DR.
NOURI.BAYGI...............
::::::::::::::::::::::::::
TEARCHERS.ASSISTANTS......
@tooraj_trz...............
@af4rinz..................
@farzanehhlj..............
@behnam_hs................
@ghn_hamed................
@pariya_ab................
::::::::::::::::::::::::::
PROJECT LIMITS............
1.Format2.Midifiles.......
2.Files.with.note-on.witho
ut.note-of................
==========================

==========================================================================

==>Installing extra libraries:

//sudo apt install pulseaudio
//sudo apt install alsa-utils

==>Running instructions:

//before runing run this command : pulseaudio -k && sudo alsa force-reload
//./a.out
//compile with : gcc test.c beep.o sintable.o -lasound

==========================================================================


==========================================================================
								FEATURES
						========================
						1.Clean code(comment,functions)
						2.Complete user interface
						3.Custom beep
						4.Git:https://github.com/nimamt/focp
						
===========================================================================
								
*/

/* Constant haye barname */

//Console colors
#define KNRM "\x1B[0m"  		//Normal
#define KRED "\x1B[31m" 		//Ghermez
#define KGRN "\x1B[32m" 		//Sabz
#define KYEL "\x1B[33m" 		//Zard
#define KBLU "\x1B[34m" 		//Aabi
#define KMAG "\x1B[35m" 		//Banafsh
#define KCYN "\x1B[36m" 		//Aabi asemani
#define KWHT "\x1B[37m" 		//Sefid

//Sizes
#define MAX_PLAYLIST_FILES 100	//Tedad maximum file haye playlist
#define MAX_FILENAME_SIZE 100	//Maximum size esm kamel yek file
#define MAX_EVENT_SIZE 10000	//Maximum tedad event haye yek track
#define MAX_PATH_SIZE 200		//Maximum andaze adress folder music

/* Constant haye barname */

/* Global Variable haye barname */

//In moteghayer ha baraye kahesh dadan parametr haye function hamun global shodan
double tempo = -1;				//Tempo
int division;					//Division yek file midi
int a = 0; 						//Tedad note on haye yek file midi
int b = 0; 						//Tedad note off haye yek file midi

/* Global Variable haye barname */

/* Struct ha va enum haye barname */

typedef enum
{
	NOTE, 						//Note yaani note kamel frequency ba modat zaman
	MISC  						//Event haye gheyr note ke pakhsh nemishan valie deltatimeshun moheme
} event_type;

typedef struct
{
	event_type type;
	double deltaTime_ms; 		//Delta time in milliseconds
	double frequency; 			//Frequency duration va start_ticks faghat baraye event_type type == NOTE hastan!
	double duration;
	int start_ticks; 			//Start of note on 
	int skipto; 				//Event id in track array of note-off event
} event;

typedef struct
{
	int count; 					//Teedad event haye track
	event events[MAX_EVENT_SIZE];
} track;

/* Struct ha va enum haye barname */

/* Prototype haye barname */

int get_bytes(FILE *midi_file, int n); 		  //Function komaki baraye gereftan addad n bytie ke hadaxar n 4 hast
int get_vlv(FILE* midi_file, int* bytesRead); //Function baraye fread kardan Variable Length Value 
double note_to_freq(int note);                //Function baraye tabdil note 0-127 be frequency moadelesh
int readMidiEvent(FILE* midi_file, int* command, track tracks[], int t, int *current_time, int * bytesRead); //Function baraye proccess kardan Event haye yek track
void play_tracks(track tracks[], int count);  //Function baraye pakhsh kardan note haye kol track haye yek file midi
int proccess_midi_file(char file_name[]);	  //Function asli baraye proccess kardan yek file midi ba address file_name

//TEXT UI FUNCTIONS
void text_ui_inititialize(); 				  //Initialization Function for ~~Text UI~~ ^_^
void text_ui_draw_menu(); 					  //Option haye menu ro chap kon ta karbar bebine
void text_ui_proccess_command(); 			  //User input ro handle kon

/* Prototype haye barname */

int main()
{
	abeep(500,1000);						//Ye bughi bezane avale barname khube :)
	text_ui_inititialize(); 				//START TEXT UI
}

void text_ui_draw_menu()
{
	printf("%s~~MIDI FILE PLAYER (with user interface!)~~%s\n", KMAG, KBLU); //Printf %s KMAG rang terminal ro be magneta taghiir mide %s KBLUE be blue
	printf("0.Play a single midi file\n");
	printf("====Playlist Functions====\n");
	printf("1.Ezafe kon be playlist\n");
	printf("2.Hazf mored az playlist\n");
	printf("3.Reset playlist\n");
	printf("4.Neshan dadan playlist\n");
	printf("5.Paksh hame playlist files\n");
	printf("6.Paksh file moshakhas az playlist\n");
	printf("7.Menu dasturat ro neshun bede\n");
	printf("====Extra functions====\n");
	printf("8.Set midi music directory\n");
	printf("9.Save midi music directory\n");
	printf("10.Load midi music directory\n");
	printf("11.Save playlist\n");
	printf("12.Load playlist\n");
	printf("Other. Exit barname\n"); //User input be sabz bashe
}

void text_ui_proccess_command()
{
	int command, i; 										//Command dastur vared shodas, i ham bara for loop hast
	char file_name[MAX_FILENAME_SIZE];						//Filename string baraye yek file hast
	char end_file_name[MAX_FILENAME_SIZE+MAX_PATH_SIZE-1];	//Hasel strcat folder_path va file_name
	char folder_path[MAX_PATH_SIZE] = "";					//Yek folder bara file haye midi hast az un estefade beshe
	char playlist[MAX_PLAYLIST_FILES][MAX_FILENAME_SIZE];	//Playlist araye shamel filename hayist ke lazem darim
	int filecount = 0;										//Filecount tedad ozve haye araye playlist hast

	do
	{
		printf("\n%sDastoor ra vared konid:\n>>", KGRN);		//User input be green bashe ghashang tare
		scanf("%d", &command);								//Daryaft dastur
		if(command == 0)
		{
			printf("Filename or path:");
			scanf(" %s", file_name);						//Daryaft filename ya adress file baraye pakhsh
			end_file_name[0] = '\0';						//Reset
			strcat(end_file_name, folder_path);             //Ezafe kardan address folder
			strcat(end_file_name, file_name);				//Ezafe kardan address file
			proccess_midi_file(end_file_name);					//Pakhsh file midi
		}
		if(command == 1)
		{
			if(filecount == MAX_PLAYLIST_FILES - 1) 		//Last element in array is SIZE - 1
				printf("Playlist takmile, nemishe ezafe kard.\n");
			else
			{
				printf("Filename or path:");
				scanf(" %s", playlist[filecount]);
				filecount++;
			}
		}
		if(command == 2)
		{
			int id;
			scanf("%d", &id);
			if(id <= filecount && id > 0)
			{
				id--; //To araye item ha az 0 ta SIZE-1 hastan vali list az 1 ta SIZE be karbar neshun dade mishe
				for(i=id+1;i<=filecount;i++)
				{
					strcpy(playlist[i-1], playlist[i]);
				}
				filecount--;
			}
			else
			{
				printf("File entekhab shode eshtebah mibashad!\n");
			}
		}
		if(command == 3)
		{
			filecount=0;									//Reset kardan playlist be 0 ozve
		}
		if(command == 4)
		{
			printf("========");
			if(filecount == 0)
				printf("No file exists on the playlist.");
			else
				printf("\n");
			for(i=0; i<filecount; i++)
			{
				printf("#%dth file: %s\n", i+1, playlist[i]);
			}
			printf("========\n");
		}
		if(command == 5)
		{
			printf("\n");
			for(i=0; i<filecount; i++)
			{
				printf("%sSending %dth file: %s to player\n", KGRN, i+1, playlist[i]);	//Be karbar begu dari chikar mikoni
				end_file_name[0] = '\0';
				strcat(end_file_name, folder_path);
				strcat(end_file_name, playlist[i]);
				proccess_midi_file(end_file_name);										//File ro befrest play she
			}
		}
		if(command == 6)
		{
			int id;
			scanf("%d", &id);
			if(id <= filecount && id > 0)
			{
				id--; //To araye item ha az 0 ta SIZE-1 hastan vali list az 1 ta SIZE be karbar neshun dade mishe
				printf("%sSending %dth file: %s to player\n", KGRN, id+1, playlist[id]);
				end_file_name[0] = '\0';
				strcat(end_file_name, folder_path);
				strcat(end_file_name, playlist[id]);
				proccess_midi_file(end_file_name);
				printf("**6 ra dobare vared konid to yek file midi digar pakhsh shavad**\n");
			}
			else
			{
				printf("File entekhab shode eshtebah mibashad!\n");
			}

		}
		if(command == 7)
		{
			text_ui_draw_menu();
		}
		if(command == 8)
		{
			printf("Music folder path(with slash at end):");
			scanf(" %s", folder_path);
		}
		if(command == 9)
		{
			FILE* folder_file = fopen("folder.txt", "w");
			fputs(folder_path, folder_file);
			fclose(folder_file);
		}
		if(command == 10)
		{
			FILE* folder_file = fopen("folder.txt", "r");
			fgets(folder_path, MAX_PATH_SIZE,folder_file);
			fclose(folder_file);
		}
		if(command == 11)
		{
			FILE* playlist_file = fopen("playlist.txt", "w");
			for(i=0; i<filecount; i++)
			{
				fputs(playlist[i], playlist_file);
				fprintf(playlist_file, "\n");
			}
			fclose(playlist_file);
		}
		if(command == 12)
		{
			filecount = 0;
			FILE* playlist_file = fopen("playlist.txt", "r");
			while (fgets(playlist[filecount], MAX_FILENAME_SIZE, playlist_file))
			{
				playlist[filecount][strlen(playlist[filecount])-1] = '\0'; //Hazf character \n az akhar
				filecount++;
			}
			fclose(playlist_file);
		}
	} 
	while (command >=0 && command <=12);
	printf("==> Merci barname ma ro entekhab kardid, dar hale khoruj! :))\n");
}

void text_ui_inititialize()
{
	text_ui_draw_menu();
	text_ui_proccess_command();
}

int get_bytes(FILE *midi_file, int n)
{
	if(!(n >= 0 && n <= 4))
		return -1;
    unsigned char ch;
    int i, return_val = 0;
    for(i=0; i<n; i++)
    {
        fread(&ch, sizeof(unsigned char), 1, midi_file);
        return_val = (return_val << 8) | ch;
    }
    return return_val;
}

int get_vlv(FILE* midi_file, int* bytesRead) {
	//Variable length value === vlv
	//Delta-time
	//Sysex
	//Meta events length
	//....
	int output = 0;
	unsigned char ch = 0;
	fread(&ch, sizeof(unsigned char), 1, midi_file); //Hadaghal yek byte ke has
	*bytesRead += 1;
	output = (output << 7) | (0x7f & ch);
	while (ch >= 0x80) //MSB 1 hast pas edame bede va byte haye baadi ro ham bekhun
	{
		fread(&ch, sizeof(unsigned char), 1, midi_file);
		*bytesRead += 1;
		output = (output << 7) | (0x7f & ch);
	}
	return output;
}

double note_to_freq(int note){
	//Note on, byte 1sh az 0-127 yek id note dare ke har kodum be yek frequency eshare mikone
	//addad 0-127 migire va frequencsh ro ke az roo doc bardashte shode barmigardune

	float freq[] = {8.18,8.66,9.18,9.72,10.30,10.91,
	11.56,12.25,12.98,13.75,14.57,15.43,16.35,17.32,
	18.35,19.45,20.60,21.83,23.12,24.50,25.96,27.50,
	29.14,30.87,32.70,34.65,36.71,38.89,41.20,43.65,
	46.25,49.00,51.91,55.00,58.27,61.74,65.41,69.30,
	73.42,77.78,82.41,87.31,92.50,98.00,103.83,110.00,
	116.54,123.47,130.81,138.59,146.83,155.56,164.81,
	174.61,185.00,196.00,207.65,220.00,233.08,246.94,
	261.63,277.18,293.66,311.13,329.63,349.23,369.99,
	392.00,415.30,440.00,466.16,493.88,523.25,554.37,
	587.33,622.25,659.26,698.46,739.99,783.99,830.61,
	880.00,932.33,987.77,1046.50,1108.73,1174.66,
	1244.51,1318.51,1396.91,1479.98,1567.98,1661.22,
	1760.00,1864.66,1975.53,2093.00,2217.46,2349.32,
	2489.02,2637.02,2793.83,2959.96,3135.96,3322.44,
	3520.00,3729.31,3951.07,4186.01,4434.92,4698.64,
	4978.03,5274.04,5587.65,5919.91,6271.93,6644.88,
	7040.00,7458.62,7902.13,8372.02,8869.84,9397.27,
	9956.06,10548.08,11175.30,11839.82,12543.85};
    return freq[note];
}

int readMidiEvent(FILE* midi_file, int* command, track tracks[], int t, int *current_time, int *bytesRead) {

	/*========EVENT TYPE========*/
	tracks[t].events[tracks[t].count].type = MISC;
	/*========EVENT TYPE========*/

	// Delta time yek Variable Length Value hast be ticks ham mibashad
	int deltaTime = get_vlv(midi_file, bytesRead);
	
	*current_time += deltaTime;
	
	if(tempo < 0)											//Vaghti tempo nadarim nemishe az formula estefade kard 0 gharar midim
        tracks[t].events[tracks[t].count].deltaTime_ms = 0;
    else													//Tempo hast pas mishe az formula estefade kard
        tracks[t].events[tracks[t].count].deltaTime_ms = 60000.0 / (tempo * division) * deltaTime;

	int status = 1, i;										//Status hamunie ke akhar return mishe, baraye end of tracke
	unsigned char ch = 0;
	char byte1, byte2;
	fread(&ch, sizeof(unsigned char), 1, midi_file);
	*bytesRead += 1;
	if (ch < 0x80) {
		// running status: command byte is previous one in data stream
	} else {
		// midi command byte
		*command = ch;
		fread(&ch, sizeof(unsigned char), 1, midi_file);
		*bytesRead += 1;
	}
	byte1 = ch;
	switch (*command & 0xf0) {
		case 0x80:    // note-off
        case 0x81:	  //15 Channels exist
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x8B:
        case 0x8C:
        case 0x8D:
        case 0x8E:
        case 0x8F:
			//printf("%d\t", *command & 0xf0 - 0x80); //DEBUG: Show channel

			if(!(byte1 >= 0 && byte1 <= 127))	//Note ha bayad tu ye range moshakhas bashan!
			{
				printf("Error: note off ba note out of range peyda shod!\n");
					return 0;	//Be khundan event ha edame nade
			}

			b++; //b hamun note-off hast ke shomorde mishe

			int flag = 0;
			fread(&ch, sizeof(unsigned char), 1, midi_file);
			*bytesRead += 1;
			byte2 = ch;
            for(i=0; i<tracks[t].count; i++) //Search kon tooye event haye ghabli
            {
                if(tracks[t].events[i].type == NOTE) //Agar az noe note bud
                {
                    if(tracks[t].events[i].frequency == note_to_freq(byte1) && tracks[t].events[i].duration < 0) //Agar frequencish barabar bud va noteoff nadasht male hamune
                    {
						flag = 1; //Noteonsh ro peyda kard
                        int deltaticks = *current_time - tracks[t].events[i].start_ticks; //Ekhtelaf zaman dota note be ticks
                        tracks[t].events[i].duration = 60000 / (tempo * division) * deltaticks; //Tabdilesh kon be ms baraye duration beep
						tracks[t].events[i].skipto = tracks[t].count;
						break; //Baghiye note ona ro vel kon match shodi dg ( porru nasho :) )
                    }
                }
            }

			if(flag == 0) //Ba hichki match nashod badbakht tanha mund
				printf("#note-on not found for note-of: (Frequency:%lf)(Time:%dticks)\n", note_to_freq(byte1),*current_time);

			break;
		case 0x90:    // note-on
		case 0x91:	  // 15 Channels Exist
		case 0x92:
		case 0x93:
		case 0x94:
		case 0x95:
		case 0x96:
		case 0x97:
		case 0x98:
		case 0x99:
		case 0x9A:
		case 0x9B:
		case 0x9C:
		case 0x9D:
		case 0x9E:
		case 0x9F:
			//printf("%d\t", *command & 0xf0 - 0x80); //DEBUG: Show channel

			if(!(byte1 >= 0 && byte1 <= 127))	//Note ha bayad tu ye range moshakhas bashan!
			{
				printf("Error: note on ba note out of range peyda shod!\n");
					return 0; //Be khundan event ha edame nade
			}

			a++; //a hamun note-on hast ke shomorde mishe

            tracks[t].events[tracks[t].count].type = NOTE;                     //Farz ke in yek MISC hast ghalate va yek NOTE hast
            tracks[t].events[tracks[t].count].frequency = note_to_freq(byte1); //Frequencish Moshakhas kon
            tracks[t].events[tracks[t].count].duration = -1;                   //-1 Yaeni set nashode va note offesh peyda nashode ke halate defaulte
            tracks[t].events[tracks[t].count].skipto = -1;                     //-1 Mesl duration
			tracks[t].events[tracks[t].count].start_ticks = *current_time;     //Shoroe note be ticks ke baedan lazem mishe

			fread(&ch, sizeof(unsigned char), 1, midi_file);
			*bytesRead += 1;
			byte2 = ch;
			break;
		case 0xA0:    // aftertouch
			fread(&ch, sizeof(unsigned char), 1, midi_file);
			*bytesRead += 1;
			byte2 = ch;
			break;
		case 0xB0:    // continuous controller
			fread(&ch, sizeof(unsigned char), 1, midi_file);
			*bytesRead += 1;
			byte2 = ch;
			break;
		case 0xE0:    // pitch-bend
			fread(&ch, sizeof(unsigned char), 1, midi_file);
			*bytesRead += 1;
			byte2 = ch;
			break;
		case 0xC0:    // patch change: Baazi ha ke mesle in faghat break khorde faghat 1 byte data daran ke ghablan gerefte shode
			break;
		case 0xD0:    // channel pressure
			break;
		case 0xF0:    // various system bytes: variable bytes
			switch (*command) {
				case 0xf0: //Avalin packetesh
					break;
				case 0xf7: //Hamun continuation event
					{
					ungetc(byte1, midi_file);
					int length = get_vlv(midi_file, bytesRead);
					for (i=0; i<length; i++) {
						fread(&ch, sizeof(unsigned char), 1, midi_file);
						*bytesRead += 1;
						if (ch < 0x10) {

						} else {

						}
					}
					}
					break;
				case 0xf1:
					break;
				case 0xf2:
					break;
				case 0xf3:
					break;
				case 0xf4:
					break;
				case 0xf5:
					break;
				case 0xf6:
					break;
				case 0xf8:
					break;
				case 0xf9:
					break;
				case 0xfa:
					break;
				case 0xfb:
					break;
				case 0xfc:
					break;
				case 0xfd:
					break;
				case 0xfe:
					printf("Error: command hanuz handle nashode!\n");
					return 0;
					break;
				case 0xff:	//FF ha kolan hamun meta event ha hastan
					{
					int metatype = ch;
					int length = get_vlv(midi_file, bytesRead); //Hamashun ba ye variable length value shuru mishan ke cheghadr data daran neshun mide
					switch (metatype) {

						case 0x00:  // sequence number //Datash: ss ss
						   {
						   fread(&ch, sizeof(unsigned char), 1, midi_file);
						   *bytesRead += 1;
						   int number = ch;
						   fread(&ch, sizeof(unsigned char), 1, midi_file);
						   *bytesRead += 1;
						   number = (number << 8) | ch;
						   
						   printf("*Sequence number: %d\n", number);
						   
						   }
						   break;

						case 0x20: // MIDI channel prefix
						case 0x21: // MIDI port
						   // display single-byte decimal number
						   fread(&ch, sizeof(unsigned char), 1, midi_file);
						   *bytesRead += 1;

						   printf("*Midi Port: %d\n", ch);

						   break;

						case 0x51: // Tempo
						    {
						    int number = 0;
						    fread(&ch, sizeof(unsigned char), 1, midi_file);
							*bytesRead += 1;
						    number = (number << 8) | ch;
						    fread(&ch, sizeof(unsigned char), 1, midi_file);
							*bytesRead += 1;
						    number = (number << 8) | ch;
						    fread(&ch, sizeof(unsigned char), 1, midi_file);
							*bytesRead += 1;
						    number = (number << 8) | ch;
                            tempo = 1000000.0 / number * 60.0;

							printf("*Tempo: %lf\n", tempo);
						    
							}
						    
							break;

						case 0x54: // SMPTE offset
						    fread(&ch, sizeof(unsigned char), 1, midi_file); //h saat
							*bytesRead += 1;
						    fread(&ch, sizeof(unsigned char), 1, midi_file); //m daghighe
							*bytesRead += 1;
						    fread(&ch, sizeof(unsigned char), 1, midi_file); //s saniye
							*bytesRead += 1;
						    fread(&ch, sizeof(unsigned char), 1, midi_file); //f frame
							*bytesRead += 1;
						    fread(&ch, sizeof(unsigned char), 1, midi_file); //sf subframe
							*bytesRead += 1;
						    break;

						case 0x58: // time signature
						    fread(&ch, sizeof(unsigned char), 1, midi_file); //numerator
							*bytesRead += 1;
						    fread(&ch, sizeof(unsigned char), 1, midi_file); //denominator power
							*bytesRead += 1;
						    fread(&ch, sizeof(unsigned char), 1, midi_file); //clocks per beat
							*bytesRead += 1;
						    fread(&ch, sizeof(unsigned char), 1, midi_file); //32nd notes per beat
							*bytesRead += 1;
						    break;

						case 0x59: // key signature
						    fread(&ch, sizeof(unsigned char), 1, midi_file); //Accidentals
							*bytesRead += 1;
						    fread(&ch, sizeof(unsigned char), 1, midi_file); //Mode
							*bytesRead += 1;
						    break;

						case 0x01: printf("Text: ");// text
							for (i=0; i<length; i++) {
						      fread(&ch, sizeof(unsigned char), 1, midi_file);
							  *bytesRead += 1;
						      printf("%c", ch);
						   }
						   printf("\n");
						   break;
						case 0x02: printf("Copyright: ");// copyright
							for (i=0; i<length; i++) {
						      fread(&ch, sizeof(unsigned char), 1, midi_file);
							  *bytesRead += 1;
						      printf("%c", ch);
						   }
						   printf("\n");
						   break;
						case 0x03: printf("Track name: ");// track name
							for (i=0; i<length; i++) {
						      fread(&ch, sizeof(unsigned char), 1, midi_file);
							  *bytesRead += 1;
						      printf("%c", ch);
						   }
						   printf("\n");
						   break;
						case 0x04: printf("Instrument name: ");// instrument name
							for (i=0; i<length; i++) {
						      fread(&ch, sizeof(unsigned char), 1, midi_file);
							  *bytesRead += 1;
						      printf("%c", ch);
						   }
						   printf("\n");
						   break;
						case 0x05: printf("Lyric: ");// lyric
							for (i=0; i<length; i++) {
						      fread(&ch, sizeof(unsigned char), 1, midi_file);
							  *bytesRead += 1;
						      printf("%c", ch);
						   }
						   printf("\n");
						   break;
						case 0x06: printf("Marker: ");// marker
							for (i=0; i<length; i++) {
						      fread(&ch, sizeof(unsigned char), 1, midi_file);
							  *bytesRead += 1;
						      printf("%c", ch);
						   }
						   printf("\n");
						   break;
						case 0x07: printf("Cue point: ");// cue point
							for (i=0; i<length; i++) {
						      fread(&ch, sizeof(unsigned char), 1, midi_file);
							  *bytesRead += 1;
						      printf("%c", ch);
						   }
						   printf("\n");
						   break;
						case 0x08: printf("Program name: ");// program name
							for (i=0; i<length; i++) {
						      fread(&ch, sizeof(unsigned char), 1, midi_file);
							  *bytesRead += 1;
						      printf("%c", ch);
						   }
						   printf("\n");
						   break;
						case 0x09: printf("Device name: ");// device name
						   for (i=0; i<length; i++) {
						      fread(&ch, sizeof(unsigned char), 1, midi_file);
							  *bytesRead += 1;
						      printf("%c", ch);
						   }
						   printf("\n");
						   break;
						default:
						   for (i=0; i<length; i++) {
						      fread(&ch, sizeof(unsigned char), 1, midi_file);
							  *bytesRead += 1;
						      printf("%c", ch);
						      if (ch < 0x10) {
						      }
						   }
					}
					switch (metatype) //Baraye inke hich meta eventi ja nayofte :)))
					{				  //baazi ha tekrari bude bara hamin comment shode!
						case 0x00: printf("*sequence number\n");     break; //Sequence number ro neshun mide
						//case 0x01: printf("*text\n");                break; //Comment ha va etelaat dige
						//case 0x02: printf("*copyright notice\n");    break; //Shamel character (C) va sal va saheb copyright
						//case 0x03: printf("*track name\n");          break; //Ya sequence name dar asl Sequence/Track name
						//case 0x04: printf("*instrument name\n");     break; //Rahnamaei mikone che instrumenti baraye track estefade shode
						//case 0x05: printf("*lyric\n");               break; //Kalamati ke khunde mishan
						//case 0x06: printf("*marker\n");              break; //Label words in a sequence
						//case 0x07: printf("*cue point\n");           break; //Tozih mide che chizi toye yek film etefagh miyofte
						//case 0x08: printf("*program name\n");        break; //Komak mikone karbar ba estefade az sakht afzar motefavet az narmafzar dorost estefade kone
						//case 0x09: printf("*device name\n");         break; //Esm sakhtafzari ke baraye tolid file estefade shode
						case 0x20: printf("*MIDI channel prefix\n"); break;
						case 0x21: printf("*MIDI port\n");           break; //Kheili az system ha chand ta port midi daran ke bandwidth issue va 16 channel limit ro dor bezanan
						case 0x51: printf("*tempo\n");               break; //In ke dige malume :) be time per beat hastesh
						case 0x54: printf("*SMPTE offset\n");        break; //SMPTE offset ro neshun mide hr mn se fr ff
						case 0x58: printf("*time signature\n");      break; //Time signature ro neshun mide
						case 0x59: printf("*key signature\n");       break; //Key signature ro neshun mide
						case 0x7f: printf("*system exclusive\n");    break; //Manufacturer's ID code ro neshun mide, sequencer-specific info neshun mide
						case 0x2f:
						   printf("*End of track\n");
						   status = 0;
						   break;
						default:
							printf("*Meta-message ke noesh moshakhas nist!\n");
					}
					}
					break;

			}
			break;
	}
	return status;
}

void play_tracks(track tracks[], int count)
{
	printf("Note haye chap shode note haye hamzaman hastan be formate (Frequency,Duration) baraye beep.\n");
	int i = 0, j, k;
    for(i=0; i<count; i++)								//Count ta track to har file midi hast
    {
        for(j=0; j< tracks[i].count; j++)				//Har kudum uni tracks[i].count ta track daran
        {
            sleep(tracks[i].events[j].deltaTime_ms/1000); //Faseleye zamaniye har do event bayad rayat she faregh az note budan ya na
            if(tracks[i].events[j].type == NOTE)
            {
				if(tracks[i].events[j].duration > 0)
				{
					beep(tracks[i].events[j].frequency, tracks[i].events[j].duration);
					sleep(tracks[i].events[j].duration/1000);		 //Dar hengam pakhsh barname nabayad kari bokone
					/* Pakhsh overlap ha ya pakhsh nakardan, masale in ast... (SKIP TYPE 1) (Hame overlap ha ro hazf mikone!)
					j = tracks[i].events[j].skipto+1; 				 //Boro be event baade note-off esh
					for(k=i+1; k<=tracks[i].events[j].skipto-1; k++) //Note haye hamzaman ke pakhsh nemishan ro chap kon khode note-on va note-off ke hesab nist
					{
						if(tracks[i].events[k].type == NOTE)
						{
							if(tracks[i].events[k].duration > 0)
							{
								printf("(F%6.2lf,D%6.2lf)\t", tracks[i].events[k].frequency, tracks[i].events[k].duration); //Soraat khundan kutah mishe bara chap!!
								//6.2 baraye tamiz tar amadan ebarat haye chap shode dade mishe
								//Rah dige in bud ke skip nakonim, zamanbandi ghalat dar miyumad va ahang eshtebah mishod vali note haye bishtari pakhsh mishod.
								//Ahang ba skip kardan kheili fargh mikone!!! :(
							}
						}
					}
					*/

					///* Skip type 2 (Note haye daghighan hamzaman skip mishan!)
					double timepassed = 0;
					k = j+1;
					while((k != tracks[i].count - 1) && timepassed == 0 && tracks[i].events[k].type != NOTE) 
					//Ta zamani ke be akharin event narsidi va zaman separi shode 0 hast na be note narsidi edame bede va zaman ro behesab
					{
						timepassed += tracks[i].events[k].deltaTime_ms;   //Hesab kardan zaman
						k++;											  //Edame
					}
					if(timepassed == 0 && tracks[i].events[k].type == NOTE)
					{
						printf("(F%6.2lf,D%6.2lf)",tracks[i].events[k].frequency, tracks[i].events[k].duration); //Note skip shode ro chap kon
						j = k+1;																				 //Be event bade un note skip kon
					}
					//*/

				}
            }
        }
    }
}

int proccess_midi_file(char file_name[])
{
	/*========INITIALIZATION========*/
	a = 0; //Reset baraye file haye baadi
	b = 0;
	tempo = -1;
	//Division chon scan mishe lazem nist reset she
	/*========INITIALIZATION========*/

	printf("\n========%s==> Dar hale navakhtan file midi ba name %s\n", KCYN, file_name);
    
	FILE *midi_file;
    midi_file = fopen(file_name, "rb");
    unsigned char buffer[5];
    fread(buffer, sizeof(unsigned char), 4, midi_file);
    buffer[4] = '\0';
	
	if(strcmp(buffer, "MThd") != 0)
	{
		printf("Error: not a midi file, header chunck does not exist.\n");
		fclose(midi_file);
		return -60;
	}

    printf("==> Dar hale khandan header file midi\n");

    //Read byte size of header

    unsigned char ch;
    int hlength = 0;
    fread(&ch, sizeof(unsigned char), 1, midi_file);
    hlength = (hlength << 8) | ch;
    fread(&ch, sizeof(unsigned char), 1, midi_file);
    hlength = (hlength << 8) | ch;
    fread(&ch, sizeof(unsigned char), 1, midi_file);
    hlength = (hlength << 8) | ch;
    fread(&ch, sizeof(unsigned char), 1, midi_file);
    hlength = (hlength << 8) | ch;
    printf("*Length:%d\n", hlength);

    //Read bytes of header type
    
	int htype = 0;
    fread(&ch, sizeof(unsigned char), 1, midi_file);
    htype = (htype << 8) | ch;
    fread(&ch, sizeof(unsigned char), 1, midi_file);
    htype = (htype << 8) | ch;
    printf("*File type: %d\n", htype);
    
	//Read bytes of track count
    
	int ntracks = 0;
    fread(&ch, sizeof(unsigned char), 1, midi_file);
    ntracks = (ntracks << 8) | ch;
    fread(&ch, sizeof(unsigned char), 1, midi_file);
    ntracks = (ntracks << 8) | ch;
    printf("*Tracks count: %d\n", ntracks);
    
	//Read bytes of division
    
	division = 0;
    fread(&ch, sizeof(unsigned char), 1, midi_file);
    division = (division << 8) | ch;
    int bits;
    printf("*Division:\nBinary Representation of first byte:");
	
	//Print kon byte avale division ro be surate binary
    
	for(bits = 7; bits > -1; bits--)
    {
        if(ch & (1 << bits))
            printf("1");
        else
            printf("0");
    }
    if(ch & (1 << 7))
    {

		//In noe timing support nemishavad
		printf("Error: Timecode is not supported\n");
		fclose(midi_file);
		return -61;

        //Last bit is 1
        printf("Timecode: ");
        //ch &= ~(1UL << 7);
        printf("%d fps", division);
        fread(&ch, sizeof(unsigned char), 1, midi_file);
        printf("/ %d subdivision per frame", ch);
    }
    else
    {
		//In timing khube
        printf("\nMetrical timing: ");
        fread(&ch, sizeof(unsigned char), 1, midi_file);
        division = (division << 8) | ch;
        printf("%d ppqn\n", division);
    }

	printf("==> Khandan header tamum shod!\n");

	if(htype == 2)	//Faghat format 0 va 1 poshtibani mishavad!
	{
		printf("Format file dade shode support nemishavad!");
	}
	else
	{
	
    track tracks[ntracks];

    printf("==> Dar hale khandan track ha...\n");
    int i, j, count = 0;
    for (i=0; i<ntracks; i++) {

    	tracks[i].count = 0;

        fread(buffer, sizeof(unsigned char), 4, midi_file);
        while(strcmp(buffer, "MTrk") != 0)
        {
            fread(buffer, sizeof(unsigned char), 4, midi_file);
        }
        buffer[4] = '\0';
        //Finally a midi track
        int tlength = get_bytes(midi_file ,4);
        printf("\nTule track:%d byte mibashad\n", tlength);

        int command = 0;
        int current_time = 0;

		int readBytes = 0;
        while (readMidiEvent(midi_file, &command, tracks, i, &current_time, &readBytes)) {
            tracks[i].count++;
        };

		if(readBytes != tlength)
		{
			printf("Error: Byte haye khande shode az track ba track length barabar nist! Ekhtelaf (read - bayad read mishod): %d\n", readBytes - tlength);
			fclose(midi_file);
			return -62;
		}
		else
		{
			printf("Track #%d handle shod!\n", i+1);
		}
		

        count++;
    }
    printf("\n*SUM tracks read: %d", count);
	
	printf("\n==> SUM note-on events: %d SUM note-off events: %d\n", a, b); //Tedad note-on ha va note-off ha chon beepe kheili moheme
	if(a == 0 || b == 0)
	{
		printf("ERROR: Midi files with 0 note-on or 0 note-off events cannot be played!\n");
		return -63;
	}
	else
	{
		printf("\n==>Dar hal pakhsh track ha\n");

    	play_tracks(tracks, ntracks);

		printf("\n==>Navakhtan track ha tamum shod!\n");
		}
	}
    fclose(midi_file);
}

/*========BEEP========*/
//abeep: beep custom ba library alsa beep custom nomre ezafe

#define PCM_DEVICE "default" //Device be library dade mishe baraye pakhsh seda

#include <alsa/pcm.h>		 //Baraye pakhsh seda
#include <math.h> 			 //Baraye sine wave ha

//#define BUFFER_LEN 48000     //48Khz behtarin sample rate (standard) baraye dastgah haye musighi

static char *device = "default";   //soundcard
snd_output_t *output = NULL;

int abeep(double frequency, double duration)
{
	int err;
    int j,k;

    //int f = frequency;                //frequency (Taghyir: Int deghatesh kamtare hamun frequency estefade mikonam)
    int fs = 48000;                   //sampling frequency

	int BUFFER_LEN = fs*duration/1000*4; //andaze musighi ro bar asas andaze tanzim kon! / 1000 chon duration ms hast bayat tu formul physici second dashte bashim
	
	float buffer [BUFFER_LEN];		   //Negah darande moje sinusi mun
	
    snd_pcm_t *handle;
    snd_pcm_sframes_t frames;

    // ERROR HANDLING

    if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
            printf("Playback open error: %s\n", snd_strerror(err));
            exit(EXIT_FAILURE);
    }

    if ((err = snd_pcm_set_params(handle,
                                  SND_PCM_FORMAT_FLOAT,
                                  SND_PCM_ACCESS_RW_INTERLEAVED,
                                  1,
                                  48000,		  //Hamun fs
                                  1,
                                  7)) < 0) {   //Latency takhire, ziad khub nist, kamam khub nist, ini ke gozashtam bad nabud, dg be nazar harki fargh mikone :)))
            printf("Playback open error: %s\n", snd_strerror(err));
            exit(EXIT_FAILURE);
    }

	//int samples = fs * duration / 1000.0;		 //Tedad sample ha, harchi fs bozorgtar bashe dastemun baz tare (RAVESH AVALAM BUD)

    // SINE WAVE
    for (k=0; k<BUFFER_LEN; k++){
        //buffer[k] = (sin(2*M_PI*f/fs*k));    
		buffer[k] = (sin(2*M_PI*frequency/fs*k));   	 //Maghadir moje sinusi ro besaz (M_PI hamun PI khodemune)                     
    }       

	frames = snd_pcm_writei(handle, buffer, BUFFER_LEN);    //Ferestadan stream moje sinusi be sound driver

	//sleep(duration/1000);
    snd_pcm_close(handle);						//Karemun tamum shod
    return 0;
}
/*========BEEP========*/