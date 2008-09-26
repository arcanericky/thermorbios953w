/* Thermor / BIOS Weather Station Server
 *
 * This file is file is part of ws9xxd.
 *
 * ws9xxd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ws9xxd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ws9xxd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DATADISPLAY_H
#define _DATADISPLAY_H

void output_data(const char *fmt, ...);

int display_humidity(int, int);
int display_intemp(int, int);
int display_outtemp(int, int);
int display_pressure(int, int);
int display_rain(int, int);
int display_windspeed(int, int);
int display_windgust(int, int);
int display_windchill(int, int);
int display_unknown1(int, int);

void set_pp_default_text(void);
void set_pp_data_displayers(void);

#endif
