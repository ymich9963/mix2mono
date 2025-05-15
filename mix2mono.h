#include <sndfile.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MIN_STR 255
#define MAX_STR 22 + MIN_STR
#define WELCOME_STR "Mix2Mono by Yiannis Michael (ymich9963), 2025. \n\n Basic usage, `mix2mono <Audio File>`. Use the `--help` option for details on the few options in the tool, and '--version' for version information.\n\n"
#define VERSION_STR "\nMix2Mono v0.0.2, by Yiannis Michael (ymich9963), 2025.\n\n"
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

#define CHECK_STR_LEN(x, len) ({ if ((strlen(x)) >= (len)) { \
                        fprintf(stderr, "\nInput file string is too long, must be less than %d\n", MIN_STR); \
                        return 1; \
                    }\
                })

extern char* snd_subtype_arr_desc[SND_SUBTYPE_NUM];
extern uint32_t snd_subtype_arr[SND_SUBTYPE_NUM];
extern char* snd_format_arr_desc[SND_MAJOR_FORMAT_NUM];
extern uint32_t snd_format_arr[SND_MAJOR_FORMAT_NUM];

typedef struct Mix2Mono_Config mix2mono_config_t;

typedef struct Mix2Mono_Config {
    char ifile[MIN_STR];
    char ofile[MAX_STR];
    uint8_t info_flag;
} mix2mono_config_t;

int get_options(int argc, char** argv, mix2mono_config_t* mix2mono_conf);
int open_file(SNDFILE** file, SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf);
int read_file_data(SNDFILE* file, SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf, double** x);
int mix2mono(SF_INFO* sf_info, double* x, double** x_mono);
void generate_file_name(char* ofile, char* ifile);
char* get_sndfile_major_format(SF_INFO* sf_info);
char* get_sndfile_subtype(SF_INFO* sf_info);
char* get_datetime_string();
int output_file_info(SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf);
int write_file(SNDFILE** file, SF_INFO* sf_info, double* x_mono, mix2mono_config_t* mix2mono_conf);
int output_help();
