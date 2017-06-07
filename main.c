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
#include "log.h"

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

int win_w;
int win_h;

SDL_Surface         *winMain        = NULL;
static void help_version(void);
static void help_usage(void);
static void clean_up(void);

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

    win_w = curMode->current_w;
    win_h = curMode->current_h;

    win_w = WIN_WIDTH;
    win_h = WIN_HEIGHT;

    if (!(winMain = SDL_SetVideoMode(
         win_w
        ,win_h
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

sleep(5);
quit = true;
    // Main loop
    while (!quit) {
//        SDL_Event event;

    } // while (!quit)

    return ret;
}

static void help_version(void) {
    printf("%s v%s (BUILT: %s)\n", (APP_NAME), (APP_VERSION), (BUILD_DATE));
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

void clean_up(void) {
    SDL_Quit();

    log_end();
}
