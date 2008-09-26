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

#ifndef _DATAHANDLERS_H
#define _DATAHANDLERS_H

int dh_date(int *);
int dh_humidity(int *);
int dh_intemp(int *);
int dh_outtemp(int *);
int dh_pressure(int *);
int dh_rain(int *);
int dh_time(int *);
int dh_windspeed(int *);
int dh_windgust(int *);
int dh_winddir(int *);
int dh_forecast(int *);
int dh_trend(int *);
int dh_windchill(int *);
int dh_unknown1(int *);

#endif
