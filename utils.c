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

#include <stdio.h>
#include <time.h>           // clock_gettime() etc
#include <errno.h>          // errno etc
#include <unistd.h>         // sleep(), usleep() etc

#include "app.h"
#include "lang.h"
#include "utils.h"

// Some basic macros
#define MAX( a, b) (((a) > (b)) ? (a) : (b))
#define MIN( a, b) (((a) < (b)) ? (a) : (b))
#define DIFF(a, b) (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))

struct timespec ts_now;

inline unsigned long get_usecs(void) {
    int rtc_ret;

    if ((rtc_ret = clock_gettime(CLOCK_REALTIME, &ts_now)) != 0) {
        /* Issue occurred with realtime clock */
        fprintf(stderr, "%s: %s (%d)\n", _("ERROR"), _("Failed to get time from realtime clock"), errno);

        return -1;
    }

    return (ts_now.tv_sec * 1000000) + (ts_now.tv_nsec / 1000);
}

inline unsigned long get_msecs(void) {
    int rtc_ret;

    if ((rtc_ret = clock_gettime(CLOCK_REALTIME, &ts_now)) != 0) {
        /* Issue occurred with realtime clock */
        fprintf(stderr, "%s: %s (%d)\n", _("ERROR"), _("Failed to get time from realtime clock"), errno);

        return -1;
    }

    return (ts_now.tv_sec * 1000) + (ts_now.tv_nsec / 1000000);
}

inline void msleep(unsigned long ms) {
    unsigned long _secs,
                  _msecs;

    _secs = 0;
    _msecs = ms;

    while (_msecs > 1000) {
        _msecs -= 1000;
        ++_secs;
    }   

    if (_secs > 0) sleep(_secs);
    if (_msecs > 0) usleep(1000 * _msecs);
}
