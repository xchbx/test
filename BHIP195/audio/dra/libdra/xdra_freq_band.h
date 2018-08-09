#ifndef __XDRA_FREQ_BAND_H
#define __XDRA_FREQ_BAND_H
#include "xdra_util.h"

typedef struct tagxdra_freq_band
{
	int num;
	double width;
}xdra_freq_band;

typedef struct tagxdra_freq_band_table
{
	int num;
	const xdra_freq_band *table;
}xdra_freq_band_table;


typedef struct tagxdra_freq_band_edge_table
{
	int num;
	const int16_t *table;
}xdra_freq_band_edge_table;
extern xdra_freq_band_edge_table g_freq_band_l_edge_tables[];
extern xdra_freq_band_edge_table g_freq_band_s_edge_tables[];
#endif
