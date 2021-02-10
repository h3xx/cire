/*
 * ***************************************
 * Cire: An Open-Source Diary Program in C
 *
 * Copyright (C) 2004, Andrew Morritt, <andymorritt@rogers.com>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://cire.sourceforge.net/license.php
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * File is Managed by Ian Menard (deadalien). Get Permission Before Commiting.
 *
 * $Id: text_ui.c,v 1.22 2004/10/14 15:19:07 andrewm1 Exp $
 *  ***************************************
*/

#ifdef BUILD_TEXT_UI

#include "main.h"

/* private function(s) */
static int    text_ui_init();
static int    text_ui_cleanup();

static void   set_msg(const char * str);
static void   print_stat();
static void   menu();
static char   get_input();
static int    handle_input(char n);
static char * get_line(char * msg);
static char * file_to_mem(char * filename);

/* private variable(s) */
static char * MSG;
static char * PROMPT;
static struct text_ui_cire CORE;
int QUIT=0;

int text_ui_main(int argc, char * argv[])
{
    const char * argv_file;
    char * key;

    /* init */
    text_ui_init();
    prefs_init(argc, argv);

    /* how pretty... and polite */
    system("clear");
    set_msg("Welcome to Cire!\n");

    /* check for file on command line */
    argv_file = prefs_get_argv_file();
    
    if(argv_file != NULL)
    {
	CORE.the_diary = diary_new();
	
	if(misc_file_is_encrypted(argv_file))
	    key = get_line("Password?");
	else
	    key = NULL;

	if(diary_load_from_file(CORE.the_diary, argv_file, key))
	    set_msg("There was a problem opening the file you specified.\n");
	else
	{
	    CORE.current_file = malloc(strlen(argv_file) +1);
	    strcpy(CORE.current_file, argv_file);
	}
    }
    
    /* main loop */
    do
    {
	system("clear");
	menu();
	print_stat();
	printf("%s", MSG);
	handle_input(get_input());
	
    } while(!QUIT);

    /* cleanup */
    text_ui_cleanup();
    prefs_free();

    return 0;
}


/* Diary Menu --------------------------------------------------- */

int
text_ui_diary_new()
{

    if(CORE.the_diary != NULL)
    {
        if(CORE.current_file != NULL)
            diary_save_to_file(CORE.the_diary, CORE.current_file, NULL);

        diary_free(CORE.the_diary);
    }
    
    CORE.the_diary = diary_new();
    
    CORE.saved = 0;
    set_msg("You now have a new diary to work with\n");

    return 0;
}

int
text_ui_diary_open()
{
    /* get file name to open */
    char * filename;

    filename = get_line("Please Enter a Filename to open");

    if(filename == NULL)
    {
        set_msg("Out of Memory\n");
        return 1;
    }

    if(CORE.the_diary == NULL)
        CORE.the_diary = diary_new();

    if(diary_load_from_file(CORE.the_diary, filename, NULL))
    {
        set_msg("There was a problem loading the diary from that file\n");
	free(filename);
	diary_free(CORE.the_diary);
	CORE.the_diary = NULL;
        return 1;
    }

    if(CORE.current_file != NULL)
	free(CORE.current_file);

    CORE.current_file = malloc(strlen(filename) +1);
    strcpy(CORE.current_file, filename);

    CORE.saved = 1;
    set_msg("Diary opened.\n");

    free(filename);
    return 0;
}

int
text_ui_diary_save()
{
    if(CORE.the_diary == NULL)
    {
        set_msg("There is no open diary to save at this time.\n");
        return 1;
    }

    if(CORE.current_file == NULL)
    {
        text_ui_diary_save_as();
        return 0;
    }

    diary_save_to_file(CORE.the_diary, CORE.current_file, NULL);

    return 0;
}

int
text_ui_diary_save_as()
{
    /* get file name to save as */
    char * filename;

    filename = get_line("Please enter a file name to save the diary as");

    if(filename == NULL)
    {
        printf("Out of Memory\n");
        return 1;
    }
    CORE.current_file = filename;

    if(diary_save_to_file(CORE.the_diary, CORE.current_file, NULL))
    {
        set_msg("There was a problem saving to file\n");
        return 1;
    }

    set_msg("Diary saved.\n");

    return 0;
}


int
text_ui_diary_close()
{
    char q;
    const char * promptstr = "Save (y/n)> ";
	
    /*
     * This Is part of a Workaround implemented by
     * AndrewM that allows you to change the Prompt
     * When you call get_input
     */
    if(PROMPT != NULL)
        free(PROMPT);
    PROMPT = malloc(strlen(promptstr) +1 );
    strcpy(PROMPT, promptstr);
	
    if(!CORE.saved)
    {
	printf("Would you like to save?\n");
	q = get_input();
    
	if(q == 'y' || q == 'Y')
	    text_ui_diary_save();
    }
    
    text_ui_cleanup();
    
    QUIT=1;
    return 0;
}

int
text_ui_diary_quit()
{
    char q;
    const char * promptstr = "Quit (y/n)> ";
    
    /* I copied over Andrew's code here  
     */
    if(PROMPT != NULL)
        free(PROMPT);
    PROMPT = malloc(strlen(promptstr) +1 );
    strcpy(PROMPT, promptstr);

    printf("Are you sure you want to quit?");
    q = get_input();

    if(q == 'y' || q == 'Y')
    {
	prefs_save();
	text_ui_diary_close();
    }    
    return 0;
}

int
text_ui_diary_about()
{
    /*
     * This Function shows the About Message
     * Using CIRE_ABOUT_MESSAGE, defined in
     * Main.h. Implemented by AndrewM.
     */

    char * tmp;
    printf("%s",CIRE_ABOUT_MESSAGE);
    printf("\nPress Any Key to Continue");
    tmp = get_line("");
    system("clear");

    return 0;
}


/* Entry Menu ----------------------------------------------- */

int
text_ui_entry_view_all()
{
    int count;
    int i;
    char * wait;
    
    if(CORE.the_diary == NULL)
    {
	set_msg("There is no open diary to view.\n");
	return 1;
    }

    count = diary_get_entry_count(CORE.the_diary);

    for(i = 0; i < count; ++i)
    {
	printf("[%d]: %s\n", i,
	       entry_get_title(diary_get_entry_by_offset(CORE.the_diary, i)));
    }

    wait = get_line("Type something to return to the menu.");
    
    return 0;
}

int
text_ui_entry_view()
{
    if(CORE.the_entry == NULL)
    {
	set_msg("You must select an entry to view it.\n");
	return 1;
    }

    system("clear");

    printf("%s\n\n", entry_get_contents(CORE.the_entry));

    get_line("Type something to return to the menu");
    
    return 0;
}

int
text_ui_entry_select()
{
    int i, count, time_len;
    char * index;
    char * time_s;
    struct entry * tmp_e;
    
    if(CORE.the_diary == NULL)
    {
	set_msg("There is no open diary to select from.\n");
	return 1;
    }

    time_len = 100;
    time_s = malloc(time_len +1);
    count = diary_get_entry_count(CORE.the_diary);

    system("clear");
    printf("Entries in diary: \n\n");
    
    for(i = 0; i < count; ++i)
    {
	tmp_e = diary_get_entry_by_offset(CORE.the_diary, i);

	entry_get_timef(tmp_e, time_s, time_len, prefs_get_timef());
	
	printf("[%d] %15s %s\n", i, time_s, entry_get_title(tmp_e));
    }

    free(time_s);
    
    printf("\nEnter a number less than %d.\n" , count);
    index = get_line("to select an entry");

    if(index == NULL)
    {
	set_msg("Out of memory - unable to select entry.");
	return 1;
    }

    if(atoi(index) <= count)
    {
	CORE.the_entry = diary_get_entry_by_offset(CORE.the_diary, atoi(index));
	CORE.offset = atoi(index);
    }
    else
    {
	set_msg("Your input was invalid.");
	return 1;
    }

    free(index);
    return 0;
}

int
text_ui_entry_add()
{
    char * syscmd;
    char * new_title;
    char * tmpfile;
    char * contents;
    FILE * tmpfd;
    struct entry * e;

    if(CORE.the_diary == NULL)
    {
	set_msg("There is no open diary to add an entry to.");
	return 1;
    }

    e = entry_new();

    new_title = get_line("Please enter a title for the new entry.");
    if(new_title == NULL)
    {
	set_msg("Error reading title.\n");
	return 1;
    }
    entry_set_title(e, new_title);

    tmpfile = malloc(strlen("/tmp/cire.tmp") +1);
    if(tmpfile == NULL)
    {
	set_msg("Out of memory - error opening tmp file for entry.");
	return 1;
    }
    strcpy(tmpfile, "/tmp/cire.tmp");

    tmpfd = fopen(tmpfile, "w+");
    if(tmpfd == NULL)
    {
	set_msg("Could not open tmp file for entry.");
	return 1;
    }
    fclose(tmpfd);

    syscmd = malloc(strlen(prefs_get_editor()) + strlen(tmpfile) + 10);
    if(syscmd == NULL)
    {
	set_msg("Out of memory - unable to create system command.");
	return 1;
    }

    strcpy(syscmd, prefs_get_editor());
    strcat(syscmd, " ");
    strcat(syscmd, tmpfile);
    strcat(syscmd, "\0");

    system(syscmd);

    system("clear");
    
    contents = file_to_mem(tmpfile);
    if(contents == NULL)
    {
	set_msg("Error reading file into memory.");
	return 1;
    }
    entry_set_contents(e, contents);

    diary_add_entry(CORE.the_diary, e);

    CORE.offset = diary_get_entry_count(CORE.the_diary) - 1;
    CORE.the_entry = e;
    CORE.saved = 0;
    
    free(contents);
    free(syscmd);
    free(new_title);
    free(tmpfile);
    return 0;
}

int
text_ui_entry_delete()
{
    if(CORE.the_diary == NULL)
    {
	set_msg("There is no open diary to delete from.");
	return 1;
    }
    if(CORE.the_entry == NULL)
    {
	set_msg("You must select an entry to delete it.");
	return 1;
    }

    if(diary_remove_entry_by_offset(CORE.the_diary, CORE.offset))
    {
	set_msg("Error removing entry.");
	return 1;
    }

    CORE.saved = 0;

    CORE.the_entry = NULL;
    CORE.offset = 0;
    
    set_msg("Entry Removed\n");    
    return 0;
}

int
text_ui_entry_rename()
{
    char * new_name;

    if(CORE.the_entry == NULL)
    {
	set_msg("You must 'Select' an entry first.\n");
	return 1;
    }

    new_name = get_line("Please enter a new name for the entry");

    if(new_name == NULL)
    {
	printf("Out of memory - unable to rename\n");
	return 1;
    }

    CORE.saved = 0;
    entry_set_title(CORE.the_entry, new_name);

    free(new_name);
    return 0;
}

/* private functions ---------------------------------- */

int text_ui_init()
{
    CORE.the_diary = NULL;
    CORE.the_entry = NULL;
    CORE.current_file = NULL;
    CORE.saved = 1;

    return 1;
}

int text_ui_cleanup()
{
    if(CORE.the_diary != NULL)
    {
	diary_free(CORE.the_diary);
	CORE.the_diary = NULL;
    }
    if(CORE.the_entry != NULL)
    {
	CORE.the_entry = NULL;
    }
    if(CORE.current_file != NULL)
    {
	free(CORE.current_file);
	CORE.current_file = NULL;
    }
    if(MSG != NULL)
    {
	free(MSG);
	MSG = NULL;
    }
    if(PROMPT != NULL)
    {
	free(PROMPT);
	PROMPT = NULL;
    }
    return 0;
}

void menu()
{
    int i;

    /* make an attempt to fit inside 80x22 here */    
    char * display_menu[] = {
	/*                          28                          56 */
	"+--------------------------+---------------------------+\n",
	"| Diary                    |  Entry                    |\n",
	"+--------------------------+---------------------------+\n",
	"| n: New       o: Open     |  a: Add       e: Select   |\n",
	"| s: Save      w: Save As  |  d: Delete    r: Rename   |\n",
	"| c: Close     i: About    |  v: View                  |\n",
	"| q: Quit                  |  p: Print Entries         |\n",
	"+--------------------------+---------------------------+\n",
	NULL
    };

    i = 0;
    while(display_menu[i] != NULL)
    {
	printf("%s", display_menu[i]);
	++i;
    }
}

void print_stat()
{
    char * d_title;
    const char * e_title;
    char * time_f;
    int timef_len;
    int total;

    misc_str_assign(&d_title, "No Diary Open");
    total = 0;
    
    if(CORE.the_diary == NULL && CORE.current_file == NULL)
    {
	misc_str_assign(&d_title, "No Diary Open");	
	total = 0;
    }
    else if(CORE.the_diary != NULL && CORE.current_file == NULL)
    {
	misc_str_assign(&d_title, "Diary not saved to file yet");
	total = diary_get_entry_count(CORE.the_diary);
    }
    else if(CORE.the_diary != NULL && CORE.current_file != NULL)
    {
	d_title = CORE.current_file;
	total = diary_get_entry_count(CORE.the_diary);
    }
    
    if(CORE.the_entry == NULL)
    {
	e_title = "None Selected";
	/*malloc(strlen("None Selected") +1);
	  strcpy(e_title, "None Selected"); */
    }
    else
	e_title = entry_get_title(CORE.the_entry);

  
    
    printf("\n");
    printf("+--------------------------+---[ Current Information ]-+\n");
    printf("| Diary: %s\n", d_title);
    printf("| Entry: %s\n", e_title);
    if(CORE.the_entry != NULL)
    {
	timef_len = 100;
	time_f = malloc( timef_len );
	entry_get_timef(CORE.the_entry, time_f, timef_len, prefs_get_timef());
	printf("| Selected Entry: %d of [0 - %d]\n", CORE.offset, total -1);
	printf("| Entry Time: %s\n", time_f);
	free(time_f);
    }
    printf("+--------------------------+---------------------------+\n");
    printf("\n\n");


}

char get_input()
{
    char in, junk;


    const char * promptstr = "Cire> ";


    /*
     * This Is part of a Workaround implemented by
     * AndrewM that allows you to change the Prompt
     * When you call get_input
     */
    if(PROMPT == NULL)
    {
	PROMPT = malloc(strlen(promptstr) +1 );
	strcpy(PROMPT, promptstr);
    }

    printf("\n%s",PROMPT);

    if(PROMPT != NULL) { free(PROMPT); PROMPT = NULL; }

    in = fgetc(stdin);
    while((junk = fgetc(stdin)) != '\n');  /* swallow until the end of line*/

    return in;
}

int handle_input(char n)
{

    switch(n)
    {
	/* diary menu */
    case 'n':
	text_ui_diary_new();
	break;

    case 'o':
	text_ui_diary_open();
	break;

    case 's':
	text_ui_diary_save();
	break;

    case 'w':
	text_ui_diary_save_as();
	break;

    case 'c':
	text_ui_diary_close();
	break;

    case 'q':
	text_ui_diary_quit();
	break;


    case 'i':
	text_ui_diary_about();
	break;


	/* entry menu */
    case 'p':
	text_ui_entry_view_all();
	break;
    case 'v':
	text_ui_entry_view();
	break;
	
    case 'a':
	text_ui_entry_add();
	break;

    case 'e':
	text_ui_entry_select();
	break;

    case 'd':
	text_ui_entry_delete();
	break;

    case 'r':
	text_ui_entry_rename();
	break;

    default:
	set_msg("Your input was invalid.\n");
	break;

    }

    return 0;
}


void set_msg(const char * str)
{
    if(MSG != NULL)
	free(MSG);
    MSG = malloc(strlen(str) +1 );
    strcpy(MSG, str);
}

char * get_line(char * msg)
{
    int len;
    char * ret;

    set_msg(msg);
    printf("%s\n> ", msg);

    len = 100;
    ret = malloc(len +1);
    fgets(ret, len, stdin);

    ret[strlen(ret)-1] = '\0';

    return ret;
}

char * file_to_mem(char * filename)
{
    FILE * fd;
    char c;
    int len;
    int i;
    char * data;

    fd = fopen(filename, "r");
    len = 0;

    while( (c = fgetc(fd)) != EOF)
	++len;

    data = malloc(len +1);
    if(data == NULL)
	return NULL;

    rewind(fd);

    for(i = 0; i < len; ++i)
    {
	data[i] = fgetc(fd);
    }

    return data;
}

#endif
