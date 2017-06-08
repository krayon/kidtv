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

#ifndef   UTILS_H
#define   UTILS_H

// Generic coordinates structure
typedef struct Sipoint s_ipoint;
struct Sipoint {
    int x;
    int y;
};

// Generic dimensions structure
typedef struct Sidims s_idims;
struct Sidims {
    int w;
    int h;
};

extern unsigned long get_usecs(void);
extern unsigned long get_msecs(void);
extern void msleep(unsigned long ms);

#endif /* UTILS_H */
