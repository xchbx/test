#include  <string.h>
#include "xdra_freq_band.h"
#ifdef __CONFIG_MAKE_TABLE
#include "stdafx.h"
static const xdra_freq_band g_freq_band_l_8000[] =
{
	{ 26, 101.5625},
	{ 27, 105.4688}, 
	{ 27, 105.4688},
	{ 28, 109.3750},
	{ 30, 117.1875},
	{ 32, 125.0000},
	{ 34, 132.8125},
	{ 37, 144.5313},
	{ 41, 160.1563},
	{ 46, 179.6875},
	{ 53, 207.0313},
	{ 62, 242.1875},
	{ 74, 289.0625},
	{ 89, 347.6563},
	{109, 425.7813},
	{133, 519.5313},
	{162, 632.8125},
	{ 14,  54.6875}
};

static const xdra_freq_band g_freq_band_s_8000[] =
{
	{ 4, 125.00},
	{ 4, 125.00}, 
	{ 4, 125.00},
	{ 4, 125.00},
	{ 4, 125.00},
	{ 5, 156.25},
	{ 5, 156.25},
	{ 6, 187.50},
	{ 6, 187.50},
	{ 7, 218.75},
	{ 8, 250.00},
	{10, 312.50},
	{12, 375.00},
	{15, 468.75},
	{18, 562.50},
	{16, 500.00}
};

static const xdra_freq_band g_freq_band_l_11025[] =
{
	{ 19, 102.2827},
	{ 20, 107.6660}, 
	{ 20, 107.6660}, 
	{ 21, 113.0493}, 
	{ 22, 118.4326}, 
	{ 23, 123.8159}, 
	{ 25, 134.5825}, 
	{ 27, 145.3491}, 
	{ 30, 161.4990}, 
	{ 34, 183.0322},
	{ 39, 209.9487},
	{ 46, 247.6318},
	{ 54, 290.6982},
	{ 66, 355.2979},
	{ 80, 430.6641},
	{ 99, 532.9468},
	{120, 645.9961},
	{143, 769.8120},
	{136, 732.1289}
};
static const xdra_freq_band g_freq_band_s_11025[] =
{
	{ 4, 172.2656}, 
	{ 4, 172.2656}, 
	{ 4, 172.2656}, 
	{ 4, 172.2656}, 
	{ 4, 172.2656}, 
	{ 4, 172.2656}, 
	{ 4, 172.2656}, 
	{ 5, 215.3320}, 
	{ 6, 258.3984},
	{ 7, 301.4648},
	{ 8, 344.5313},
	{10, 430.6641},
	{12, 516.7969},
	{15, 645.9961},
	{18, 775.1953},
	{19, 818.2617}
};

static const xdra_freq_band g_freq_band_l_12000[] =
{
	{ 18, 105.4688},
	{ 18, 105.4688}, 
	{ 18, 105.4688}, 
	{ 19, 111.3281}, 
	{ 20, 117.1875}, 
	{ 21, 123.0469}, 
	{ 23, 134.7656}, 
	{ 25, 146.4844}, 
	{ 28, 164.0625}, 
	{ 31, 181.6406}, 
	{ 36, 210.9375},
	{ 42, 246.0938},
	{ 50, 292.9688},
	{ 60, 351.5625},
	{ 74, 433.5938},
	{ 91, 533.2031},
	{110, 644.5313},
	{131, 767.5781},
	{153, 896.4844},
	{ 56, 328.1250}
};

static const xdra_freq_band g_freq_band_s_12000[] =
{
	{ 4, 187.500}, 
	{ 4, 187.500}, 
	{ 4, 187.500},
	{ 4, 187.500}, 
	{ 4, 187.500}, 
	{ 4, 187.500}, 
	{ 4, 187.500}, 
	{ 5, 234.375}, 
	{ 6, 281.250},
	{ 7, 328.125},
	{ 9, 421.875},
	{11, 515.625},
	{13, 609.375},
	{16, 750.000},
	{18, 843.750},
	{15, 703.125}
};

static const xdra_freq_band g_freq_band_l_16000[] =
{
	{ 13,  76.17188},
	{ 14,  82.03125}, 
	{ 14,  82.03125}, 
	{ 14,  82.03125}, 
	{ 15,  87.89063}, 
	{ 16,  93.75000}, 
	{ 17,  99.60938}, 
	{ 19, 111.32810}, 
	{ 21, 123.04690}, 
	{ 24, 140.62500},
	{ 27, 158.20310},
	{ 32, 187.50000},
	{ 38, 222.65630},
	{ 46, 269.53130},
	{ 56, 328.12500},
	{ 69, 404.29690},
	{ 84, 492.18750},
	{ 99, 580.07810},
	{116, 679.68750},
	{136, 796.87500},
	{154, 902.34380}
};

static const xdra_freq_band g_freq_band_s_16000[] =
{
	{ 4,  250.0}, 
	{ 4,  250.0}, 
	{ 4,  250.0}, 
	{ 4,  250.0}, 
	{ 4,  250.0}, 
	{ 4,  250.0}, 
	{ 4,  250.0}, 
	{ 5,  312.5}, 
	{ 6,  375.0},
	{ 8,  500.0},
	{ 9,  562.5},
	{11,  687.5},
	{13,  812.5},
	{15,  937.5},
	{18, 1125.0},
	{15,  937.5}
};

static const xdra_freq_band g_freq_band_l_22050[] =
{
	{ 10,  107.6660},
	{ 10,  107.6660}, 
	{ 10,  107.6660},
	{ 11,  118.4326}, 
	{ 11,  118.4326}, 
	{ 12,  129.1992}, 
	{ 13,  139.9658}, 
	{ 14,  150.7324}, 
	{ 16,  172.2656}, 
	{ 18,  193.7988}, 
	{ 21,  226.0986}, 
	{ 24,  258.3984},
	{ 29,  312.2314},
	{ 35,  376.8311},
	{ 43,  462.9639},
	{ 53,  570.6299},
	{ 64,  689.0625},
	{ 75,  807.4951},
	{ 88,  947.4609},
	{104, 1119.7270},
	{129, 1388.8920},
	{178, 1916.4550},
	{ 56,  602.9297}
};

static const xdra_freq_band g_freq_band_s_22050[] =
{
	{ 4,  344.5313}, 
	{ 4,  344.5313}, 
	{ 4,  344.5313},
	{ 4,  344.5313}, 
	{ 4,  344.5313}, 
	{ 4,  344.5313}, 
	{ 5,  430.6641}, 
	{ 6,  516.7969},
	{ 7,  602.9297},
	{ 9,  775.1953},
	{10,  861.3281},
	{12, 1033.5940},
	{14, 1205.8590},
	{18, 1550.3910},
	{23, 1981.0550}
};

static const xdra_freq_band g_freq_band_l_24000[] =
{
	{  9,  105.4688}, 
	{  9,  105.4688}, 
	{  9,  105.4688},
	{ 10,  117.1875}, 
	{ 10,  117.1875}, 
	{ 11,  128.9063}, 
	{ 12,  140.6250}, 
	{ 13,  152.3438}, 
	{ 14,  164.0625},
	{ 16,  187.5000},
	{ 19,  222.6563}, 
	{ 22,  257.8125},
	{ 26,  304.6875},
	{ 32,  375.0000},
	{ 39,  457.0313},
	{ 48,  562.5000},
	{ 58,  679.6875},
	{ 68,  796.8750},
	{ 80,  937.5000},
	{ 94, 1101.5630},
	{117, 1371.0940},
	{159, 1863.2810},
	{149, 1746.0940}
};

static const xdra_freq_band g_freq_band_s_24000[] =
{
	{ 4,  375.00},
	{ 4,  375.00}, 
	{ 4,  375.00},
	{ 4,  375.00}, 
	{ 4,  375.00}, 
	{ 4,  375.00}, 
	{ 5,  468.75}, 
	{ 6,  562.50},
	{ 7,  656.25},
	{ 9,  843.75},
	{10,  937.50},
	{12, 1125.00},
	{14, 1312.50},
	{19, 1781.25},
	{22, 2062.50}
};

static const xdra_freq_band g_freq_band_l_32000[] =
{
	{  7,  109.375},
	{  7,  109.375},
	{  7,  109.375},
	{  7,  109.375},
	{  8,  125.000}, 
	{  8,  125.000}, 
	{  9,  140.625},
	{ 10,  156.250}, 
	{ 11,  171.875}, 
	{ 12,  187.500}, 
	{ 14,  218.750}, 
	{ 17,  265.625}, 
	{ 20,  312.500},
	{ 24,  375.000},
	{ 30,  468.750},
	{ 36,  562.500},
	{ 44,  687.500},
	{ 52,  812.500},
	{ 61,  953.125},
	{ 72, 1125.000},
	{ 89, 1390.625},
	{123, 1921.875},
	{200, 3125.000},
	{156, 2437.500}
};

static const xdra_freq_band g_freq_band_s_32000[] =
{
	{ 4,  500},
	{ 4,  500}, 
	{ 4,  500}, 
	{ 4,  500}, 
	{ 4,  500}, 
	{ 4,  500}, 
	{ 5,  625}, 
	{ 6,  750},
	{ 7,  875},
	{ 8, 1000},
	{10, 1250},
	{13, 1625},
	{18, 2250},
	{32, 4000},
	{ 5,  625}
};

static const xdra_freq_band g_freq_band_l_44100[] =
{
	{  5,  107.6660},
	{  5,  107.6660},
	{  5,  107.6660},
	{  6,  129.1992},
	{  6,  129.1992}, 
	{  6,  129.1992}, 
	{  7,  150.7324}, 
	{  7,  150.7324}, 
	{  8,  172.2656},
	{  9,  193.7988}, 
	{ 11,  236.8652}, 
	{ 13,  279.9316},
	{ 15,  322.9980},
	{ 19,  409.1309},
	{ 23,  495.2637},
	{ 28,  602.9297},
	{ 34,  732.1289},
	{ 40,  861.3281},
	{ 46,  990.5273},
	{ 55, 1184.3260},
	{ 70, 1507.3240},
	{101, 2174.8540},
	{176, 3789.8440},
	{329, 7084.4240}
};

static const xdra_freq_band g_freq_band_s_44100[] =
{
	{ 4,  689.0625}, 
	{ 4,  689.0625}, 
	{ 4,  689.0625},
	{ 4,  689.0625}, 
	{ 4,  689.0625}, 
	{ 4,  689.0625}, 
	{ 5,  861.3281},
	{ 6, 1033.5940},
	{ 7, 1205.8590},
	{ 9, 1550.3910},
	{12, 2067.1880},
	{21, 3617.5780},
	{44, 7579.6880}
};
static const xdra_freq_band g_freq_band_l_48000[] =
{
	{  5,  117.1875}, 
	{  5,  117.1875}, 
	{  5,  117.1875},
	{  5,  117.1875},
	{  5,  117.1875},
	{  6,  140.6250},
	{  6,  140.6250},
	{  7,  164.0625},
	{  8,  187.5000},
	{  9,  210.9375},
	{ 10,  234.3750},
	{ 12,  281.2500},
	{ 15,  351.5625},
	{ 18,  421.8750},
	{ 22,  515.6250},
	{ 27,  632.8125},
	{ 32,  750.0000},
	{ 38,  890.6250},
	{ 44, 1031.2500},
	{ 53, 1242.1880},
	{ 69, 1617.1880},
	{102, 2390.6250},
	{189, 4429.6880},
	{332, 7781.2500}
};

static const xdra_freq_band g_freq_band_s_48000[] =
{
	{ 4,  750.0},
	{ 4,  750.0}, 
	{ 4,  750.0}, 
	{ 4,  750.0}, 
	{ 4,  750.0}, 
	{ 4,  750.0}, 
	{ 5,  937.5},
	{ 6, 1125.0},
	{ 7, 1312.5},
	{ 9, 1687.5},
	{14, 2625.0},
	{27, 5062.5},
	{36, 6750.0}
};

static const xdra_freq_band g_freq_band_l_88200[] =
{
	{  4,   172.2656},
	{  4,   172.2656},
	{  4,   172.2656}, 
	{  4,   172.2656}, 
	{  4,   172.2656}, 
	{  4,   172.2656}, 
	{  4,   172.2656}, 
	{  5,   215.3320}, 
	{  6,   258.3984}, 
	{  7,   301.4648}, 
	{  8,   344.5313}, 
	{ 10,   430.6641},
	{ 12,   516.7969},
	{ 15,   645.9961},
	{ 18,   775.1953},
	{ 21,   904.3945},
	{ 25,  1076.6600},
	{ 30,  1291.9920},
	{ 39,  1679.5900},
	{ 59,  2540.9180},
	{114,  4909.5700},
	{402, 17312.7000},
	{225,  9689.9410}
};

static const xdra_freq_band g_freq_band_s_88200[] =
{
	{ 4,  1378.125},
	{ 4,  1378.125}, 
	{ 4,  1378.125}, 
	{ 4,  1378.125}, 
	{ 4,  1378.125}, 
	{ 4,  1378.125},
	{ 6,  2067.188},
	{ 9,  3100.781},
	{20,  6890.625},
	{69, 23772.660}
};

static const xdra_freq_band g_freq_band_l_96000[] =
{
	{  4,   187.500},
	{  4,   187.500}, 
	{  4,   187.500}, 
	{  4,   187.500}, 
	{  4,   187.500}, 
	{  4,   187.500}, 
	{  4,   187.500}, 
	{  5,   234.375}, 
	{  6,   281.250}, 
	{  7,   328.125}, 
	{  9,   421.875},
	{ 11,   515.625},
	{ 13,   609.375},
	{ 16,   750.000},
	{ 18,   843.750},
	{ 21,   984.375},
	{ 26,  1218.750},
	{ 33,  1546.875},
	{ 47,  2203.125},
	{ 83,  3890.625},
	{223, 10453.130},
	{478, 22406.250}
};

static const xdra_freq_band g_freq_band_s_96000[] =
{
	{ 4,  1500},
	{ 4,  1500},
	{ 4,  1500},
	{ 4,  1500},
	{ 4,  1500},
	{ 5,  1875},
	{ 7,  2625},
	{13,  4875},
	{44, 16500},
	{39, 14625}
};

static const xdra_freq_band g_freq_band_l_176400[] =
{
	{  4,   341.0156},
	{  4,   341.0156}, 
	{  4,   341.0156}, 
	{  4,   341.0156}, 
	{  4,   341.0156}, 
	{  4,   341.0156}, 
	{  5,   426.2695}, 
	{  6,   511.5234}, 
	{  7,   596.7773}, 
	{  9,   767.2852},
	{ 10,   852.5391},
	{ 12,  1023.0470},
	{ 14,  1193.5550},
	{ 18,  1534.5700},
	{ 25,  2131.3480},
	{ 44,  3751.1720},
	{114,  9718.9450},
	{736, 62746.8800}

};

static const xdra_freq_band g_freq_band_s_176400[] =
{
	{ 4,  2728.125}, 
	{ 4,  2728.125}, 
	{ 4,  2728.125}, 
	{ 4,  2728.125},
	{ 6,  4092.188},
	{15, 10230.470},
	{91, 62064.840}
};

static const xdra_freq_band g_freq_band_l_192000[] =
{
	{  4,  375.00},
	{  4,  375.00}, 
	{  4,  375.00}, 
	{  4,  375.00}, 
	{  4,  375.00}, 
	{  4,  375.00}, 
	{  5,  468.75}, 
	{  6,  562.50}, 
	{  7,  656.25}, 
	{  9,  843.75},
	{ 10,  937.50},
	{ 12,  1125.00},
	{ 14,  1312.50},
	{ 19,  1781.25},
	{ 30,  2812.50},
	{ 61,  5718.75},
	{268, 25125.00},
	{559, 52406.25}
};

static const xdra_freq_band g_freq_band_s_192000[] =
{
	{ 4,  3000},
	{ 4,  3000}, 
	{ 4,  3000},
	{ 4,  3000},
	{ 7,  5250},
	{24, 18000},
	{81, 60750}
};
xdra_freq_band_table g_freq_band_l_tables[] = {
	{ sizeof(g_freq_band_l_8000)/sizeof(xdra_freq_band),    g_freq_band_l_8000},
	{ sizeof(g_freq_band_l_11025)/sizeof(xdra_freq_band),  g_freq_band_l_11025},
	{ sizeof(g_freq_band_l_12000)/sizeof(xdra_freq_band),  g_freq_band_l_12000},
	{ sizeof(g_freq_band_l_16000)/sizeof(xdra_freq_band),  g_freq_band_l_16000},
	{ sizeof(g_freq_band_l_22050)/sizeof(xdra_freq_band),  g_freq_band_l_22050},
	{ sizeof(g_freq_band_l_24000)/sizeof(xdra_freq_band),  g_freq_band_l_24000},
	{ sizeof(g_freq_band_l_32000)/sizeof(xdra_freq_band),  g_freq_band_l_32000},
	{ sizeof(g_freq_band_l_44100)/sizeof(xdra_freq_band),  g_freq_band_l_44100},
	{ sizeof(g_freq_band_l_48000)/sizeof(xdra_freq_band),  g_freq_band_l_48000},
	{ sizeof(g_freq_band_l_88200)/sizeof(xdra_freq_band),  g_freq_band_l_88200},
	{ sizeof(g_freq_band_l_96000)/sizeof(xdra_freq_band),  g_freq_band_l_96000},
	{sizeof(g_freq_band_l_176400)/sizeof(xdra_freq_band), g_freq_band_l_176400},
	{sizeof(g_freq_band_l_192000)/sizeof(xdra_freq_band), g_freq_band_l_192000}
};

xdra_freq_band_table g_freq_band_s_tables[] = {
	{ sizeof(g_freq_band_s_8000)/sizeof(xdra_freq_band),    g_freq_band_s_8000},
	{ sizeof(g_freq_band_s_11025)/sizeof(xdra_freq_band),  g_freq_band_s_11025},
	{ sizeof(g_freq_band_s_12000)/sizeof(xdra_freq_band),  g_freq_band_s_12000},
	{ sizeof(g_freq_band_s_16000)/sizeof(xdra_freq_band),  g_freq_band_s_16000},
	{ sizeof(g_freq_band_s_22050)/sizeof(xdra_freq_band),  g_freq_band_s_22050},
	{ sizeof(g_freq_band_s_24000)/sizeof(xdra_freq_band),  g_freq_band_s_24000},
	{ sizeof(g_freq_band_s_32000)/sizeof(xdra_freq_band),  g_freq_band_s_32000},
	{ sizeof(g_freq_band_s_44100)/sizeof(xdra_freq_band),  g_freq_band_s_44100},
	{ sizeof(g_freq_band_s_48000)/sizeof(xdra_freq_band),  g_freq_band_s_48000},
	{ sizeof(g_freq_band_s_88200)/sizeof(xdra_freq_band),  g_freq_band_s_88200},
	{ sizeof(g_freq_band_s_96000)/sizeof(xdra_freq_band),  g_freq_band_s_96000},
	{sizeof(g_freq_band_s_176400)/sizeof(xdra_freq_band), g_freq_band_s_176400},
	{sizeof(g_freq_band_s_192000)/sizeof(xdra_freq_band), g_freq_band_s_192000}
};




void xdra_band_make_edege_l()
{
	TRACE("\n");
	TRACE("\n");
	TRACE("\n");
	TRACE("\n");
	TRACE("\n");
	int i;
	int tables_num = sizeof(g_freq_band_l_tables)/sizeof(xdra_freq_band_table);
	for( i= 0; i < tables_num; i++)
	{
		const xdra_freq_band *table = g_freq_band_l_tables[i].table;
		int num = g_freq_band_l_tables[i].num;

		float freq_total = 0;
		int edge = 0;
		TRACE("static const int16_t g_freq_band_l_edge%02d[] = \n", i);
		TRACE("{\n");
		TRACE("	");
		int j;
		for(j = 0; j < num - 1; j++)
		{

			edge += table[j].num;
			TRACE("%d, ", edge);
			freq_total += table[j].width;
		}
		edge += table[j].num;
		TRACE("%d\n", edge);
		freq_total += table[j].width;

		TRACE("};\n");
		TRACE("\n");

		//TRACE("freq_total = %f\n", freq_total);
	}
	TRACE("xdra_freq_band_edge_table g_freq_band_l_edge_tables[] = \n", i);
	TRACE("{\n");
	for(i= 0; i < tables_num - 1; i++)
	{
		TRACE("	{%d, g_freq_band_l_edge%02d}, \n", g_freq_band_l_tables[i].num, i);
	}
	TRACE("	{%d, g_freq_band_l_edge%02d} \n", g_freq_band_s_tables[i].num, i);
	TRACE("};\n");

	TRACE("\n");
	TRACE("\n");
	TRACE("\n");
	TRACE("\n");
	TRACE("\n");

}


void xdra_band_make_edege_s()
{
	TRACE("\n");
	TRACE("\n");
	TRACE("\n");
	TRACE("\n");
	TRACE("\n");
	int i;
	int tables_num = sizeof(g_freq_band_s_tables)/sizeof(xdra_freq_band_table);
	for( i= 0; i < tables_num; i++)
	{
		const xdra_freq_band *table = g_freq_band_s_tables[i].table;
		int num = g_freq_band_s_tables[i].num;

		float freq_total = 0;
		int edge = 0;
		TRACE("static const int16_t g_freq_band_s_edge%02d[] = \n", i);
		TRACE("{\n");
		TRACE("	");
		int j;
		for(j = 0; j < num - 1; j++)
		{

			edge += table[j].num;
			TRACE("%d,", edge);
			freq_total += table[j].width;
		}
		edge += table[j].num;
		TRACE("%d\n", edge);
		freq_total += table[j].width;

		TRACE("};\n");
		TRACE("\n");

		//TRACE("freq_total = %f\n", freq_total);
	}
	TRACE("xdra_freq_band_edge_table g_freq_band_s_edge_tables[] = \n", i);
	TRACE("{\n");
	for(i= 0; i < tables_num - 1; i++)
	{
		TRACE("	{%d, g_freq_band_s_edge%02d}, \n", g_freq_band_s_tables[i].num, i);
	}
	TRACE("	{%d, g_freq_band_s_edge%02d} \n", g_freq_band_s_tables[i].num, i);
	TRACE("};\n");


	TRACE("\n");
	TRACE("\n");
	TRACE("\n");
	TRACE("\n");
	TRACE("\n");

}

void xdra_band_make_edge_table()
{
	xdra_band_make_edege_l();
	xdra_band_make_edege_s();
}
#else
void xdra_band_make_edge_table()
{
}
#endif



//8000
static const int16_t g_freq_band_l_edge00[] = 
{
	0, 26, 53, 80, 108, 138, 170, 204, 241, 282, 328, 381, 443, 517, 606, 715, 848, 1010, 1024
};
//11025
static const int16_t g_freq_band_l_edge01[] = 
{
	0, 19, 39, 59, 80, 102, 125, 150, 177, 207, 241, 280, 326, 380, 446, 526, 625, 745, 888, 1024
};
//12000
static const int16_t g_freq_band_l_edge02[] = 
{
	0, 18, 36, 54, 73, 93, 114, 137, 162, 190, 221, 257, 299, 349, 409, 483, 574, 684, 815, 968, 1024
};
//16000
static const int16_t g_freq_band_l_edge03[] = 
{
	0, 13, 27, 41, 55, 70, 86, 103, 122, 143, 167, 194, 226, 264, 310, 366, 435, 519, 618, 734, 870, 1024
};
//22050
static const int16_t g_freq_band_l_edge04[] = 
{
	0, 10, 20, 30, 41, 52, 64, 77, 91, 107, 125, 146, 170, 199, 234, 277, 330, 394, 469, 557, 661, 790, 968, 1024
};
//24000
static const int16_t g_freq_band_l_edge05[] = 
{
	0, 9, 18, 27, 37, 47, 58, 70, 83, 97, 113, 132, 154, 180, 212, 251, 299, 357, 425, 505, 599, 716, 875, 1024
};
//32000
static const int16_t g_freq_band_l_edge06[] = 
{
	0, 7, 14, 21, 28, 36, 44, 53, 63, 74, 86, 100, 117, 137, 161, 191, 227, 271, 323, 384, 456, 545, 668, 868, 1024
};
//441000
static const int16_t g_freq_band_l_edge07[] = 
{
	0, 5, 10, 15, 21, 27, 33, 40, 47, 55, 64, 75, 88, 103, 122, 145, 173, 207, 247, 293, 348, 418, 519, 695, 1024
};
//48000
static const int16_t g_freq_band_l_edge08[] = 
{
	0, 5, 10, 15, 20, 25, 31, 37, 44, 52, 61, 71, 83, 98, 116, 138, 165, 197, 235, 279, 332, 401, 503, 692, 1024
};
//88200
static const int16_t g_freq_band_l_edge09[] = 
{
	0, 4, 8, 12, 16, 20, 24, 28, 33, 39, 46, 54, 64, 76, 91, 109, 130, 155, 185, 224, 283, 397, 799, 1024
};
//96000
static const int16_t g_freq_band_l_edge10[] = 
{
	0, 4, 8, 12, 16, 20, 24, 28, 33, 39, 46, 55, 66, 79, 95, 113, 134, 160, 193, 240, 323, 546, 1024
};
//176400
static const int16_t g_freq_band_l_edge11[] = 
{
	0, 4, 8, 12, 16, 20, 24, 29, 35, 42, 51, 61, 73, 87, 105, 130, 174, 288, 1024
};
//192000
static const int16_t g_freq_band_l_edge12[] = 
{
	0, 4, 8, 12, 16, 20, 24, 29, 35, 42, 51, 61, 73, 87, 106, 136, 197, 465, 1024
};

xdra_freq_band_edge_table g_freq_band_l_edge_tables[] = 
{
	{19, g_freq_band_l_edge00}, 
	{20, g_freq_band_l_edge01}, 
	{21, g_freq_band_l_edge02}, 
	{22, g_freq_band_l_edge03}, 
	{24, g_freq_band_l_edge04}, 
	{24, g_freq_band_l_edge05}, 
	{25, g_freq_band_l_edge06}, 
	{25, g_freq_band_l_edge07}, 
	{25, g_freq_band_l_edge08}, 
	{24, g_freq_band_l_edge09}, 
	{23, g_freq_band_l_edge10}, 
	{19, g_freq_band_l_edge11}, 
	{19, g_freq_band_l_edge12} 
};









//8000
static const int16_t g_freq_band_s_edge00[] = 
{
	0,4,8,12,16,20,25,30,36,42,49,57,67,79,94,112,128
};
//11025
static const int16_t g_freq_band_s_edge01[] = 
{
	0,4,8,12,16,20,24,28,33,39,46,54,64,76,91,109,128
};
//12000
static const int16_t g_freq_band_s_edge02[] = 
{
	0,4,8,12,16,20,24,28,33,39,46,55,66,79,95,113,128
};
//16000
static const int16_t g_freq_band_s_edge03[] = 
{
	0,4,8,12,16,20,24,28,33,39,47,56,67,80,95,113,128
};
//22050
static const int16_t g_freq_band_s_edge04[] = 
{
	0,4,8,12,16,20,24,29,35,42,51,61,73,87,105,128
};
//24000
static const int16_t g_freq_band_s_edge05[] = 
{
	0,4,8,12,16,20,24,29,35,42,51,61,73,87,106,128
};
//32000
static const int16_t g_freq_band_s_edge06[] = 
{
	0,4,8,12,16,20,24,29,35,42,50,60,73,91,123,128
};
//44100
static const int16_t g_freq_band_s_edge07[] = 
{
	0,4,8,12,16,20,24,29,35,42,51,63,84,128
};
//48000
static const int16_t g_freq_band_s_edge08[] = 
{
	0,4,8,12,16,20,24,29,35,42,51,65,92,128
};
//88200
static const int16_t g_freq_band_s_edge09[] = 
{
	0,4,8,12,16,20,24,30,39,59,128
};
//96000
static const int16_t g_freq_band_s_edge10[] = 
{
	0,4,8,12,16,20,25,32,45,89,128
};
//176400
static const int16_t g_freq_band_s_edge11[] = 
{
	0,4,8,12,16,22,37,128
};
//192000
static const int16_t g_freq_band_s_edge12[] = 
{
	0,4,8,12,16,23,47,128
};

xdra_freq_band_edge_table g_freq_band_s_edge_tables[] = 
{
	{17, g_freq_band_s_edge00}, 
	{17, g_freq_band_s_edge01}, 
	{17, g_freq_band_s_edge02}, 
	{17, g_freq_band_s_edge03}, 
	{16, g_freq_band_s_edge04}, 
	{16, g_freq_band_s_edge05}, 
	{16, g_freq_band_s_edge06}, 
	{14, g_freq_band_s_edge07}, 
	{14, g_freq_band_s_edge08}, 
	{11, g_freq_band_s_edge09}, 
	{11, g_freq_band_s_edge10}, 
	{8, g_freq_band_s_edge11}, 
	{8, g_freq_band_s_edge12} 
};


#include "stdio.h"
void xdra_check_freq_band(int file_offset, xdra_freq_band_edge_table *table)
{
	FILE *fp = fopen( "E:\\librmdradec.so","rb");
	if(fp == NULL)
	{
		return; 
	}

	fseek(fp, file_offset, SEEK_SET);
	for(int i = 0; i < table->num - 1; i++)
	{
		int16_t num;
		fread(&num, 1, 2, fp);
		int diff = table->table[i + 1] - table->table[i];
		if(num != diff)
		{
			num = num;
			//break;
		}
		
	}
	fclose(fp);

}
void xdra_check_all_freq_bands()
{
	int offsets[26] = { 0xB2EC, 0xB2CC, 0xB2A4, 0xB284, 0xB25C, 0xB23C, 0xB210, 0xB1F0, 
	                    0xB1C0, 0xB1A0, 0xB170, 0xB150, 0xB120, 0xB100, 0xB0D0, 0xB0B4,
						0xB084, 0xB068, 0xB038, 0xB024, 0xAFF8, 0xAFE4, 0xAFC0, 0xAFB0, 0xAF8C, 0xAF7C,
	};
	for(int i = 0; i < 13; i++)
	{
		xdra_check_freq_band(offsets[i * 2 + 0], &g_freq_band_l_edge_tables[i]);
		xdra_check_freq_band(offsets[i * 2 + 1], &g_freq_band_s_edge_tables[i]);
		
	}

}