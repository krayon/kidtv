/* vim:set ts=4 sw=4 tw=80 et cindent ai si cino=(0,ml,\:0:
 * ( settings from: http://datapax.com.au/code_conventions/ )
 */

/**********************************************************************
    KidTV
    Copyright (C) 2016-2017 Todd Harbour

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 2 ONLY, as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program, in the file COPYING or COPYING.txt; if
    not, see http://www.gnu.org/licenses/ , or write to:
      The Free Software Foundation, Inc.,
      51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 **********************************************************************/

#include <stdio.h>          // NULL etc
#include <stdbool.h>        // bool etc
#include <getopt.h>         // getopt_long() etc
#include <unistd.h>         // sleep() etc

#include <SDL/SDL.h>

#include "app.h"
#include "lang.h"
#include "git.h"
#include "utils.h"
#include "log.h"

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

#define BUTTON_DELAY      (2000)
#define QUIT_BUTTON_DELAY (BUTTON_DELAY)

// All available buttons
typedef enum e_button t_button;
enum e_button {
     button_NONE      = 0
    ,button_quit      = 1
    ,button_left      = 2
    ,button_right     = 3
    ,button_playpause = 4
    ,button_stopmenu  = 5
};
#define button_COUNT    6
const char *s_button[button_COUNT] = {
     "button_NONE"
    ,"button_quit"
    ,"button_left"
    ,"button_right"
    ,"button_playpause"
    ,"button_stopmenu"
};

typedef struct s_buttonstate t_buttonstate;
struct s_buttonstate {
    // The last time the button was known to be NOT pressed
    unsigned long last_up;
    // The last time the button was known to be depressed
    unsigned long last_down;
    // Is button down now?
    bool          down;
    // Is this a new press (so processing can occur on button release)
    bool          new_press;
};
t_buttonstate button[button_COUNT];

s_idims winres;

SDL_Surface         *winMain        = NULL;
static void help_version(void);
static void help_usage(void);
static void clean_up(void);
static void buttons_init(void);
static void button_down(const t_button but, const unsigned long msecs);
static void button_up(const t_button but, const unsigned long msecs);

int main(int argc, char *argv[]) {
    bool quit = false;

    // Return value
    int ret = 0;

    // Command options
    int option_index;

    atexit(&clean_up);

    log_init();

    while (1) {
        int mode;
        static struct option long_options[] = {
            /* name, has_arg, flag, val
             *   name:
             *     long name
             *   has_arg:
             *     0 = no
             *     1 = required
             *     2 = optional
             *   flag:
             *     NULL  = getopt_long returns val
             *     other = getopt_long returns 0, flag = val (if option found)
             *   val:
             *     value to return, or be loaded into flag (when flag != NULL)
             */
            {"help",    0, NULL, 'h'},
            {"version", 0, NULL, 'V'},
            {0, 0, 0, 0}
        };  

        mode = getopt_long(argc, argv, "hV",
                long_options, &option_index);

        if (mode == -1) break;

        switch (mode) {
            // -V|--version
            case 'V':
                dlog(LOG, "-V|--version\n");
                help_version();
                return ret;

            // -h|--help
            case 'h':
                dlog(LOG, "-h|--help\n");
                help_usage();
                return ret;

            case 0:
                /* Long (only) option
                 *
                 * If this is the case option_index will be the long
                 * options index number:
                 *   long_options[option_index].name will be the name.
                 */
                break;

            case '?':
                // Unrecognised option
                break;

            default:
                printf("?? getopt AHHHHHH 0%o/%d:", mode, mode);
                while (*argv) printf(" %s", *argv++);
                printf("\n");
        } // switch (mode)
    } // while (1)

    /*
     * num remaining arguments: argc - optind
     * remaining arguments: argv[optind] -> argv[argc]
     */

    // Main entry point

    dlog(LOG, "Starting...");

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;

    const SDL_VideoInfo *curMode;
    if ((curMode = SDL_GetVideoInfo()) == NULL) {
        fprintf(stderr, "ERROR: SDL_GetVideoInfo() failed: %s", SDL_GetError());
        return 1;
    }
    dlog(LOG, "Current Video Resolution: %dx%d"
        ,curMode->current_w
        ,curMode->current_h
    );

    winres.w = curMode->current_w;
    winres.h = curMode->current_h;

    winres.w = WIN_WIDTH;
    winres.h = WIN_HEIGHT;

    if (!(winMain = SDL_SetVideoMode(
         winres.w
        ,winres.h
        ,0
        ,SDL_HWSURFACE|SDL_DOUBLEBUF /* |SDL_FULLSCREEN */
    ))) {
        // Failed to create window
        fprintf(stderr, "ERROR: Failed to create window\n");
        SDL_Quit();
        return 1;
    }

    // Set window title
    SDL_WM_SetCaption(APP_NAME, APP_ICON_NAME);

    // Disable repeating keys
    SDL_EnableKeyRepeat(0, 0);

    // Initialise buttons array
    buttons_init();

    // Main loop
    while (!quit) {
        SDL_Event event;

        while (!quit && SDL_PollEvent(&event)) {
            unsigned long msecs = get_msecs();

            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;

                case SDL_KEYDOWN: {
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            button_down(button_quit     , msecs);
                            break;

                        case SDLK_SPACE:
                            button_down(button_playpause, msecs);
                            break;

                        case SDLK_RETURN:
                            button_down(button_stopmenu , msecs);
                            break;

                        case SDLK_LEFT:
                            button_down(button_left     , msecs);
                            break;

                        case SDLK_RIGHT:
                            button_down(button_right    , msecs);
                            break;

                        // Comment out default to generate an compile error
                        // containing a handy list of SDLK_*
                        default:
                            break;
                    }

                    break;
                } // case SDL_KEYDOWN:

                case SDL_KEYUP: {
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            button_up(button_quit     , msecs);
                            break;

                        case SDLK_SPACE:
                            button_up(button_playpause, msecs);
                            break;

                        case SDLK_RETURN:
                            button_up(button_stopmenu , msecs);
                            break;

                        case SDLK_LEFT:
                            button_up(button_left     , msecs);
                            break;

                        case SDLK_RIGHT:
                            button_up(button_right    , msecs);
                            break;

                        // Comment out default to generate an compile error
                        // containing a handy list of SDLK_*
                        default:
                            break;
                    }

                    break;
                } // case SDL_KEYUP:
            } // switch (event.type)
        } // while (SDL_PollEvent(&event))

        // Quit button has been released
        if (button[button_quit].new_press && !button[button_quit].down) {
            if (button[button_quit].last_up != 0) {
                dlog(LOG_BUTTON
                    ,"!quit: %ld (last_down), %ld (last_up), %ld (a-b)\n"
                    ,button[button_quit].last_down
                    ,button[button_quit].last_up
                    ,button[button_quit].last_down - button[button_quit].last_up
                );

                if (button[button_quit].last_down - button[button_quit].last_up > QUIT_BUTTON_DELAY) {
                    quit = true;
                }
            }

            button[button_quit].new_press = false;
        } // if (!button[button_quit].down)
    } // while (!quit)

    // NOTE: cleanup() is called here (via atexit())

    return ret;
}

static void help_version(void) {
    printf("%s v%s\n(BUILT: %s)\n"
        ,(APP_NAME)
        ,(APP_VERSION)
        ,(BUILD_DATE)
    );
    printf("%s\n", (APP_COPYRIGHT));
    printf("%s\n", (APP_SUMMARY));
    printf("%s\n", (APP_URL));
}

static void help_usage(void) {
    help_version();

    printf("\
\n\
%s %s\n\
\n\
%s: %s -h|--h[elp]\n\
       %s -V|--v[ersion]\n\
\n\
-h|--h[elp]             - %s\n\
-V|--v[ersion]          - %s %s %s\n\
\n\
%s: %s\n\
",
     APP_NAME, _("is a thang.")

    ,_("Usage"),    BIN_NAME /* -h|--h[elp] */
    ,               BIN_NAME /* -V|--v[ersion] */

    ,_("Displays this help")
    ,_("Displays"), APP_NAME, _("version")
    ,_("Example"),  BIN_NAME
    );
}

static void clean_up(void) {
    SDL_Quit();

    log_end();
}

static void buttons_init(void) {
    t_button i;

    for (i = 0; i < button_COUNT; ++i) {
        button[i].new_press = false;
        button[i].down      = false;
        button[i].last_up   = 0;
        button[i].last_down = 0;
    }
}

static void button_down(const t_button but, const unsigned long msecs) {
    if (!button[but].down) {
        dlog(LOG_BUTTON, "DOWN: %s\n", s_button[but]);

        button[but].down      = true;
        button[but].new_press = true;
        button[but].last_up   = msecs;
    }
}

static void button_up(const t_button but, const unsigned long msecs) {
    dlog(LOG_BUTTON, "UP:   %s\n", s_button[but]);

    button[but].down      = false;
    button[but].last_down = msecs;
}

