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
