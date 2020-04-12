/**********************************************************************************
* Copyright (c)  2008-2019  Guangdong OPPO Mobile Comm Corp., Ltd
* VENDOR_EDIT
* Description: OPPO Charge Module Device Tree
* 
* Version: 1.0
* Date: 2019-06-10
* ------------------------------ Revision History: --------------------------------
* <version>           <date>                <author>                            <desc>
*                   2019-07-03            Yichun.Chen                     add common function
***********************************************************************************/
#ifndef __OP_CHARGE_H__
#define __OP_CHARGE_H__

struct ntc_table {
	int resistance;		/* ohm */
	int temperature;	/* 0.1*celsius */
};

struct usb_temp {
	int temperature;	/* 0.1*celsius */
	int interval;		/* ms */
};

#define ADC_SAMPLE_COUNT		5
#define ADC_SAMPLE_INTERVAL		10

#define PULL_UP_VOLTAGE_1800MV		1800	/* 1800mV */
#define PULL_UP_RESISTANCE_51KOHM	51000	/* 51kohm */

#define DETECT_INTERVAL_50MS		50	/* detect interval 50ms */
#define DETECT_INTERVAL_300MS		300	/* detect interval 300ms */
#define VALID_LOW_TEMPERATURE		200	/* 20C - 100C */
#define VALID_HIGH_TEMPERATURE		1000	/* 20C - 100C */
#define CRITICAL_TEMPERATURE		570	/* 57C */
#define CRITICAL_USBTEMP_ABOVE_BATTEMP	150	/* usb_temp - bat_temp >= 10C */
#define CRITICAL_RISE_TEMPERATURE	30	/* rise 3Celsius in 1500ms */
#define CRITICAL_RISE_INTERVAL		1500	/* rise 3Celsius in 1500ms */
#define TEMPERATURE_LIST_LENGTH		30	/* 1.5s / 50ms */

struct usb_temp temperature_list1[TEMPERATURE_LIST_LENGTH];	/* length = 1.5s / 50ms */
struct usb_temp temperature_list2[TEMPERATURE_LIST_LENGTH];	/* length = 1.5s / 50ms */

/*func:				check if dischg_enable
*temperature1 temperature2:	usb_temp (0.1*Celsius)
*critical_rise1 critical_rise2:	if rise 3Cesius in 1500ms
*bat_temp:			bat_temp (0.1*Celsius)
*dischg_enable:			dischg_enable		*/
#define check_dischg_enable(temperature1, temperature2, critical_rise1, critical_rise2, bat_temp, dischg_enable)	\
	do {														\
		dischg_enable = false;											\
		if (temperature1 >= VALID_LOW_TEMPERATURE && temperature1 <= VALID_HIGH_TEMPERATURE) {			\
			if (temperature1 >= CRITICAL_TEMPERATURE)							\
				dischg_enable = true;									\
			if (temperature1 - bat_temp >= CRITICAL_USBTEMP_ABOVE_BATTEMP && critical_rise1 == true)	\
				dischg_enable = true;									\
		}													\
		if (temperature2 >= VALID_LOW_TEMPERATURE && temperature2 <= VALID_HIGH_TEMPERATURE) {			\
			if (temperature2 >= CRITICAL_TEMPERATURE)							\
				dischg_enable = true;									\
			if (temperature2 - bat_temp >= CRITICAL_USBTEMP_ABOVE_BATTEMP && critical_rise2 == true)	\
				dischg_enable = true;									\
		}													\
	} while (0)

/*temp1 temp2:	usb_temp (0.1*Celsius)
*time:		thread interval (ms)
*i:		index of ntc_table	*/
#define add_temperature_list(temp1, temp2, time, i)							\
	do {												\
		for (i = TEMPERATURE_LIST_LENGTH - 1; i > 0; i --) {					\
			temperature_list1[i].temperature = temperature_list1[i - 1].temperature;	\
			temperature_list1[i].interval = temperature_list1[i - 1].interval;		\
			temperature_list2[i].temperature = temperature_list2[i - 1].temperature;	\
			temperature_list2[i].interval = temperature_list2[i - 1].interval;		\
		}											\
		temperature_list1[0].temperature = temp1;						\
		temperature_list1[0].interval = time;							\
		temperature_list2[0].temperature = temp2;						\
		temperature_list2[0].interval = time;							\
	} while (0)

/* check if rise 3Cesius within 1500ms */
#define check_temperature_list(list, total_time, critical_rise, i)					\
	do {												\
		critical_rise = false;									\
		total_time = 0;										\
		for (i = 0; i < TEMPERATURE_LIST_LENGTH; i ++) {					\
			total_time += list[i].interval;							\
			if (list[i].temperature == 0 && list[i].interval == 0)				\
				break;									\
			if (total_time - list[i].interval >= CRITICAL_RISE_INTERVAL)			\
				break;									\
			if (list[0].temperature - list[i].temperature >= CRITICAL_RISE_TEMPERATURE) {	\
				critical_rise = true;							\
				break;									\
			}										\
		}											\
	} while (0)

#define clear_temperature_list(i)					\
	do {								\
		for (i = 0; i < TEMPERATURE_LIST_LENGTH; i ++) {	\
			temperature_list1[i].temperature = 0;		\
			temperature_list1[i].interval = 0;		\
			temperature_list2[i].temperature = 0;		\
			temperature_list2[i].interval = 0;		\
		}							\
	} while (0)

/*R:	NTC resistance (ohm)
*T:	NTC temperature (0.1*Celsius)
*L:	ARRAY_SIZE(ntc_table)
*i:	index of ntc_table
*table:	ntc_table		*/
#define resistance_convert_temperature(R, T, i, table)			\
	do {								\
		for (i = 0; i < ARRAY_SIZE(table); i ++)		\
			if (table[i].resistance <= R)			\
				break;					\
		if (i == 0)						\
			T = table[0].temperature;			\
		else if (i == ARRAY_SIZE(table))			\
			T = table[ARRAY_SIZE(table) - 1].temperature;	\
		else						\
			T = table[i].temperature * (R - table[i - 1].resistance) / (table[i].resistance - table[i - 1].resistance) +	\
				table[i - 1].temperature * (table[i].resistance - R) / (table[i].resistance - table[i - 1].resistance);	\
	} while (0)

/*vadc:			usb_temp_adc voltage (mV)
*pull_up_voltage:	pull_up_voltage (mV)
*pull_up_resistance:	pull_up_resistance (ohm)	*/
#define voltage_convert_resistance(vadc, pull_up_voltage, pull_up_resistance)	\
	(pull_up_resistance / (pull_up_voltage - vadc) * vadc)

/* ntc1 100Kohm when 25c */
struct ntc_table ntc_table_100K[] = {
	/* {ohm, 0.1*celsius} */
	{4251000, -400},	{3962000, -390},	{3695000, -380},	{3447000, -370},	{3218000, -360},
	{3005000, -350},	{2807000, -340},	{2624000, -330},	{2454000, -320},	{2296000, -310},
	{2149000, -300},	{2012000, -290},	{1885000, -280},	{1767000, -270},	{1656000, -260},
	{1554000, -250},	{1458000, -240},	{1369000, -230},	{1286000, -220},	{1208000, -210},
	{1135000, -200},	{1068000, -190},	{1004000, -180},	{945000, -170},		{889600, -160},
	{837800, -150},		{789300, -140},		{743900, -130},		{701300, -120},		{661500, -110},
	{624100, -100},		{589000, -90},		{556200, -80},		{525300, -70},		{496300, -60},
	{469100, -50},		{443500, -40},		{419500, -30},		{396900, -20},		{375600, -10},
	{355600, 00},		{336800, 10},		{319100, 20},		{302400, 30},		{286700, 40},
	{271800, 50},		{257800, 60},		{244700, 70},		{232200, 80},		{220500, 90},
	{209400, 100},		{198900, 110},		{189000, 120},		{179700, 130},		{170900, 140},
	{162500, 150},		{154600, 160},		{147200, 170},		{140100, 180},		{133400, 190},
	{127000, 200},		{121000, 210},		{115400, 220},		{110000, 230},		{104800, 240},
	{100000, 250},		{95400, 260},		{91040, 270},		{86900, 280},		{82970, 290},
	{79230, 300},		{75690, 310},		{72320, 320},		{69120, 330},		{66070, 340},
	{63180, 350},		{60420, 360},		{57810, 370},		{55310, 380},		{52940, 390},
	{50680, 400},		{48530, 410},		{46490, 420},		{44530, 430},		{42670, 440},
	{40900, 450},		{39210, 460},		{37600, 470},		{36060, 480},		{34600, 490},
	{33190, 500},		{31860, 510},		{30580, 520},		{29360, 530},		{28200, 540},
	{27090, 550},		{26030, 560},		{25010, 570},		{24040, 580},		{23110, 590},
	{22220, 600},		{21370, 610},		{20560, 620},		{19780, 630},		{19040, 640},
	{18320, 650},		{17640, 660},		{16990, 670},		{16360, 680},		{15760, 690},
	{15180, 700},		{14630, 710},		{14100, 720},		{13600, 730},		{13110, 740},
	{12640, 750},		{12190, 760},		{11760, 770},		{11350, 780},		{10960, 790},
	{10580, 800},		{10210, 810},		{9859, 820},		{9522, 830},		{9198, 840},
	{8887, 850},		{8587, 860},		{8299, 870},		{8022, 880},		{7756, 890},
	{7500, 900},		{7254, 910},		{7016, 920},		{6788, 930},		{6568, 940},
	{6357, 950},		{6153, 960},		{5957, 970},		{5768, 980},		{5586, 990},
	{5410, 1000},		{5241, 1010},		{5078, 1020},		{4921, 1030},		{4769, 1040},
	{4623, 1050},		{4482, 1060},		{4346, 1070},		{4215, 1080},		{4088, 1090},
	{3966, 1100},		{3848, 1110},		{3734, 1120},		{3624, 1130},		{3518, 1140},
	{3415, 1150},		{3316, 1160},		{3220, 1170},		{3128, 1180},		{3038, 1190},
	{2952, 1200},		{2868, 1210},		{2787, 1220},		{2709, 1230},		{2634, 1240},
	{2561, 1250},
};

/* ntc2 10Kohm when 25c */
struct ntc_table ntc_table_10K[] = {
	/* {ohm, 0.1*celsius} */
	{0, 0},
};
#endif /* __OP_CHARGE_H__ */
