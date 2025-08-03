#pragma once

#include <sndfile.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MIN_STR 255
#define MAX_STR 22 + MIN_STR + EXTENSION_STR
#define EXTENSION_STR 7
#define WELCOME_STR "Mix2Mono by Yiannis Michael (ymich9963), 2025. \n\n Basic usage, `mix2mono <Audio File>`. Use the `--help` option for details on the few options in the tool, and '--version' for version information.\n\n"
#define VERSION_STR "\nMix2Mono v0.1.0, by Yiannis Michael (ymich9963), 2025.\n\n"
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

typedef struct Mix2Mono_Config mix2mono_config_t;

typedef struct Mix2Mono_Config {
    char ifile[MIN_STR];
    char ofile[MAX_STR];
    uint8_t info_flag;
    size_t input_file_size;
    uint8_t data_size;

    void (*mix2mono)(size_t size, int channels, void* x, void** x_mono);
} mix2mono_config_t;

/**
 * @brief Get the options from the CLI input.
 *
 * @param argc Number of arguments.
 * @param argv Arguments.
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @return Success or failure.
 */
int get_options(int argc, char** argv, mix2mono_config_t* mix2mono_conf);

/**
 * @brief Open the audio file.
 *
 * @param file Pointer to empty SNDFILE variable.
 * @param sf_info Pointer to empty SF_INFO variable.
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @return Success or failure.
 */
int open_file(SNDFILE** file, SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf);

/**
 * @brief Read the file data in double format.
 *
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @param sndfile Pointer to SNDFILE variable pointing to an actual file stream.
 * @param sf_info Pointer to SF_INFO variable containing file information.
 * @param x Adress of data buffer.
 * @return Success or failure.
 */
int read_file_data_raw(mix2mono_config_t* mix2mono_conf, SNDFILE* sndfile, SF_INFO* sf_info, void** x);

/**
 * @brief Automatically sets the ADX configuration depending on the input file.
 *
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @param sf_info Pointer to SF_INFO variable containing file information.
 */
void set_settings_auto(mix2mono_config_t* mix2mono_conf, SF_INFO* sf_info);

/**
 * @brief Sets the ADX configuration to get the data as unsigned 8-bit integer.
 *
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @param sf_info Pointer to SF_INFO variable containing file information.
 */
void set_settings_uint8(mix2mono_config_t* mix2mono_conf, SF_INFO* sf_info);

/**
 * @brief Sets the ADX configuration to get the data as signed 8-bit integer.
 *
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @param sf_info Pointer to SF_INFO variable containing file information.
 */
void set_settings_int8(mix2mono_config_t* mix2mono_conf, SF_INFO* sf_info);

/**
 * @brief Sets the ADX configuration to get the data as signed 16-bit integer.
 *
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @param sf_info Pointer to SF_INFO variable containing file information.
 */
void set_settings_short(mix2mono_config_t* mix2mono_conf, SF_INFO* sf_info);

/**
 * @brief Sets the ADX configuration to get the data as signed 32-bit integer.
 *
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @param sf_info Pointer to SF_INFO variable containing file information.
 */
void set_settings_int(mix2mono_config_t* mix2mono_conf, SF_INFO* sf_info);

/**
 * @brief Sets the ADX configuration to get the data as a float.
 *
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @param sf_info Pointer to SF_INFO variable containing file information.
 */
void set_settings_float(mix2mono_config_t* mix2mono_conf, SF_INFO* sf_info);

/**
 * @brief Sets the ADX configuration to get the data as a double.
 *
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @param sf_info Pointer to SF_INFO variable containing file information.
 */
void set_settings_double(mix2mono_config_t* mix2mono_conf, SF_INFO* sf_info);

/**
 * @brief Mix to mono function for unsigned 8-bit integer type data.
 *
 * @param size Size of data.
 * @param channels Number of channels in the input audio file.
 * @param x Input data buffer.
 * @param x_mono Output data buffer.
 */
void mix2mono_uint8(size_t size, int channels, void* x, void** x_mono);

/**
 * @brief Mix to mono function for signed 8-bit integer type data.
 *
 * @param size Size of data.
 * @param channels Number of channels in the input audio file.
 * @param x Input data buffer.
 * @param x_mono Output data buffer.
 */
void mix2mono_int8(size_t size, int channels, void* x, void** x_mono);

/**
 * @brief Mix to mono function for signed 16-bit integer type data.
 *
 * @param size Size of data.
 * @param channels Number of channels in the input audio file.
 * @param x Input data buffer.
 * @param x_mono Output data buffer.
 */
void mix2mono_short(size_t size, int channels, void* x, void** x_mono);

/**
 * @brief Mix to mono function for signed 32-bit integer type data.
 *
 * @param size Size of data.
 * @param channels Number of channels in the input audio file.
 * @param x Input data buffer.
 * @param x_mono Output data buffer.
 */
void mix2mono_int(size_t size, int channels, void* x, void** x_mono);

/**
 * @brief Mix to mono function for float.
 *
 * @param size Size of data.
 * @param channels Number of channels in the input audio file.
 * @param x Input data buffer.
 * @param x_mono Output data buffer.
 */
void mix2mono_float(size_t size, int channels, void* x, void** x_mono);

/**
 * @brief Mix to mono function for double.
 *
 * @param size Size of data.
 * @param channels Number of channels in the input audio file.
 * @param x Input data buffer.
 * @param x_mono Output data buffer.
 */
void mix2mono_double(size_t size, int channels, void* x, void** x_mono);

/**
 * @brief Generate the output file name based on the input file name, extension and date/time.
 *
 * @param extension Extension of the input file name.
 * @param ofile Output file name string.
 * @param ifile Input file name string.
 */
void generate_file_name(const char* extension, char* ofile, char* ifile);

/**
 * @brief Get the SNDFILE major format string. Same as descriptions given in the documentation.
 *
 * @param sf_info Pointer to SF_INFO variable containing file information.
 * @return Major format string
 */
const char* get_sndfile_major_format(SF_INFO* sf_info);

/**
 * @brief Get the SNDFILE subtype string. Same as subtypes given in the documentation.
 *
 * @param sf_info Pointer to SF_INFO variable containing file information.
 * @return Subtype string.
 */
const char* get_sndfile_subtype(SF_INFO* sf_info);

/**
 * @brief Get the file extension using the libsndfile libraries.
 *
 * @param sf_info Pointer to SF_INFO variable containing file information.
 * @return Subtype string.
 */
const char* get_input_file_extension(SF_INFO* sf_info);

/**
 * @brief Get a date/time string formatted in ddmmyyHHMMSS.
 *
 * @return Date/time string.
 */
char* get_datetime_string();

/**
 * @brief Output the input file info to stdout. Specified with --info.
 *
 * @param sf_info Pointer to SF_INFO variable containing file information.
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @return Success or failure.
 */
int output_file_info(SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf);

/**
 * @brief Write the mono data to a .wav file.
 *
 * @param file Pointer to SNDFILE variable pointing to an actual file stream.
 * @param sf_info Pointer to SF_INFO variable containing file information.
 * @param x_mono Mono data pointer.
 * @param mix2mono_conf Mix2Mono configuration struct.
 * @return Success or failure.
 */
int write_file(SNDFILE** file, SF_INFO* sf_info, void* x_mono, mix2mono_config_t* mix2mono_conf);

/**
 * @brief Output the mix2mono --help option.
 *
 * @return Success or failure.
 */
int output_help();
