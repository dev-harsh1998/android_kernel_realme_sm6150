#ifndef _OPPO_PROJECT_H_
#define _OPPO_PROJECT_H_

enum OPPO_REGION {
	REGION_UNKNOWN = 0,
	REGION_CHINA = 1,
	REGION_OVERSEA = 2,
};

enum OPPO_PROJECT {
	OPPO_UNKNOWN = 0,
	OPPO_19031 = 19031,
	OPPO_19331 = 19331,
	OPPO_19111 = 19111,
	OPPO_19513 = 19513,
    OPPO_19771 = 19771,
};

typedef enum OPPO_PROJECT OPPO_PROJECT;

#define MAX_OCP 4
#define MAX_LEN 8

typedef struct
{
	uint16_t		nProject;
	uint8_t			nModem;
	uint8_t			nOperator;
	uint8_t			nRegion;
	uint8_t			nReserve;
	char			nPCB[MAX_LEN];
	uint8_t			nOppoBootMode;
	uint8_t			nPmicOcp[MAX_OCP];
	uint8_t			reserved[16];
	int				nEngVersion;
	unsigned int	nEngVerConfidential;
} ProjectInfoCDTType;

/*#ifdef VENDOR_EDIT*/
/*Cong.Dai@psw.bsp.tp, 2019.06.27, Add for sw_version*/
#define OPPO_ENG_VERSION_NOT_INIT      -1
enum OPPO_ENG_VERSION {
    RELEASE                 = 0x00,
    AGING                   = 0x01,
    CTA                     = 0x02,
    PERFORMANCE             = 0x03,
    PREVERSION              = 0x04,
    ALL_NET_CMCC_TEST       = 0x05,
    ALL_NET_CMCC_FIELD      = 0x06,
    ALL_NET_CU_TEST         = 0x07,
    ALL_NET_CU_FIELD        = 0x08,
    ALL_NET_CT_TEST         = 0x09,
    ALL_NET_CT_FIELD        = 0x0A,
};
/*#endif VENDOR_EDIT*/

#define GET_PCB_VERSION() (get_PCB_Version())
#define GET_PCB_VERSION_STRING() (get_PCB_Version_String())
#define GET_MODEM_VERSION() (get_Modem_Version())
#define GET_OPERATOR_VERSION() (get_Operator_Version())

#ifdef CONFIG_OPPO_COMMON_SOFT
unsigned int get_project(void);
unsigned int is_project(OPPO_PROJECT project);
unsigned const char * get_PCB_Version(void);
int cmp_pcb(const char *pcb);
unsigned char get_Modem_Version(void);
unsigned char get_Operator_Version(void);
unsigned char get_oppo_region(void);
int get_eng_version(void);
bool is_confidential(void);
bool oppo_daily_build(void);
#else
unsigned int get_project(void) { return 0;}
unsigned int is_project(OPPO_PROJECT project) { return 0;}
unsigned const char * get_PCB_Version(void) { return NULL;}
int cmp_pcb(const char *pcb) { return 0; }
unsigned char get_Modem_Version(void) { return 0;}
unsigned char get_Operator_Version(void) { return 0;}
unsigned char get_oppo_region(void) { return 0;}
int get_eng_version(void) { return 0;}
bool is_confidential(void) { return true;}
bool oppo_daily_build(void) { return false;}
#endif

#endif
