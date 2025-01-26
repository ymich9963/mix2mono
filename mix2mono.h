#include <sndfile.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_STR 30
#define WELCOME_STR ""
#define VERSION_STR ""
#define SND_MAJOR_FORMAT_NUM 27
#define SND_SUBTYPE_NUM 36

/* Check macros */
/* Check response from sscanf */
#define	CHECK_RES(x) ({ if (!(x)) { \
							fprintf(stderr, "Argument entered was wrong...\n"); \
							return 1; \
						} \
					  })

/* Check if an error occured to exit program */
#define	CHECK_ERR(x) ({ if ((x)) { \
							exit(EXIT_FAILURE); \
						} \
					  })

/* Check if a function returns failure */
#define	CHECK_RET(x) ({ if ((x)) { \
							return 1; \
						} \
					  })

extern char* snd_subtype_arr_desc[SND_SUBTYPE_NUM];
extern uint32_t snd_subtype_arr[SND_SUBTYPE_NUM];
extern char* snd_format_arr_desc[SND_MAJOR_FORMAT_NUM];
extern uint32_t snd_format_arr[SND_MAJOR_FORMAT_NUM];

typedef struct Mix2Mono_Config mix2mono_config_t;

typedef struct Mix2Mono_Config {
    char ifile[MAX_STR];
    char ofile[MAX_STR];
    uint8_t info_flag;
} mix2mono_config_t;

void set_defaults(mix2mono_config_t* mix2mono_conf);
int get_options(int* argc, char** argv, mix2mono_config_t* mix2mono_conf);
int open_file(SNDFILE** file, SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf);
int read_file_data(SNDFILE* file, SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf, double** x);
int mix2mono(SF_INFO* sf_info, double* x, double** x_mono);
char* get_sndfile_major_format(SF_INFO* sf_info);
char* get_sndfile_subtype(SF_INFO* sf_info);
void output_info(SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf);
int write_file(SNDFILE** file, SF_INFO* sf_info, double* x_mono, mix2mono_config_t* mix2mono_conf);
void output_help();
