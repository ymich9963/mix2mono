#include "mix2mono.h"

int get_options(int argc, char** restrict argv, mix2mono_config_t* restrict mix2mono_conf)
{
    if (argc == 1) {
        fprintf(stdout, WELCOME_STR);

        return 1;
    }

    if (argc == 2) {
        if (!(strcmp("--version", argv[1]))) {
            fprintf(stdout, VERSION_STR);

            return 1;
        }

        if (!(strcmp("--help", argv[1]))) {
            output_help();

            return 1;
        }
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-' && argv[i - 1][0] != '-') {
            CHECK_STR_LEN(argv[i], MIN_STR);
            strcpy(mix2mono_conf->ifile, argv[i]);
            continue;
        }

        if (!(strcmp("-i", argv[i])) || !(strcmp("--input", argv[i]))) {
            CHECK_STR_LEN(argv[i + 1], MIN_STR);
            strcpy(mix2mono_conf->ifile, argv[i + 1]);
            i++;
            continue;
        }

        if (!(strcmp("-o", argv[i])) || !(strcmp("--output", argv[i]))) {
            CHECK_STR_LEN(argv[i + 1], MAX_STR);
            strcpy(mix2mono_conf->ofile, argv[i + 1]);
            i++;
            continue;
        }

        if (!(strcmp("--info", argv[i]))) {
            mix2mono_conf->info_flag = 1;
            continue;
        }

        fprintf(stderr, "\nNo such option '%s'. Please check inputs.\n\n", argv[i]);

        return 1;
    }
    return 0;
}

int open_file(SNDFILE** restrict file, SF_INFO* restrict sf_info, mix2mono_config_t* restrict mix2mono_conf)
{
    *file = sf_open(mix2mono_conf->ifile, SFM_READ, sf_info);
    if(!(*file)) {
        fprintf(stderr, "\nError with opening input file: %s", sf_strerror(*file));

        return 1;
    };
    return 0;
}

void set_settings_uint8(mix2mono_config_t* restrict mix2mono_conf, SF_INFO* restrict sf_info)
{
    mix2mono_conf->input_file_size = sf_info->frames * sf_info->channels;
    mix2mono_conf->data_size = sizeof(uint8_t);
    mix2mono_conf->mix2mono = &mix2mono_uint8;
    fprintf(stdout,  "Detected data type 'uint8'.\n");
}

void set_settings_int8(mix2mono_config_t* restrict mix2mono_conf, SF_INFO* restrict sf_info)
{
    mix2mono_conf->input_file_size = sf_info->frames * sf_info->channels;
    mix2mono_conf->data_size = sizeof(int8_t);
    mix2mono_conf->mix2mono = &mix2mono_int8;
    fprintf(stdout,  "Detected data type 'int8'.\n");
}

void set_settings_short(mix2mono_config_t* restrict mix2mono_conf, SF_INFO* restrict sf_info)
{
    mix2mono_conf->input_file_size = sf_info->frames * sf_info->channels;
    mix2mono_conf->data_size = sizeof(short);
    mix2mono_conf->mix2mono = &mix2mono_short;
    fprintf(stdout,  "Detected data type 'short'.\n");
}

void set_settings_int(mix2mono_config_t* restrict mix2mono_conf, SF_INFO* restrict sf_info)
{
    mix2mono_conf->input_file_size = sf_info->frames * sf_info->channels;
    mix2mono_conf->data_size = sizeof(int);
    mix2mono_conf->mix2mono = &mix2mono_int;
    fprintf(stdout,  "Detected data type 'int'.\n");
}

void set_settings_float(mix2mono_config_t* restrict mix2mono_conf, SF_INFO* restrict sf_info)
{
    mix2mono_conf->input_file_size = sf_info->frames * sf_info->channels;
    mix2mono_conf->data_size = sizeof(float);
    mix2mono_conf->mix2mono = &mix2mono_float;
    fprintf(stdout,  "Detected data type 'float'.\n");

}

void set_settings_double(mix2mono_config_t* restrict mix2mono_conf, SF_INFO* restrict sf_info)
{
    mix2mono_conf->input_file_size = sf_info->frames * sf_info->channels;
    mix2mono_conf->data_size = sizeof(double);
    mix2mono_conf->mix2mono = &mix2mono_double;
    fprintf(stdout,  "Detected data type 'double'.\n");
}

void set_settings_auto(mix2mono_config_t* restrict mix2mono_conf, SF_INFO* restrict sf_info) 
{
    const uint16_t subtype_mask = 0x00FF;
    const uint16_t subtype = sf_info->format & subtype_mask;

    switch (subtype) {
        case SF_FORMAT_PCM_U8:
        case SF_FORMAT_ULAW:
        case SF_FORMAT_ALAW:
            set_settings_uint8(mix2mono_conf, sf_info);
            break;
        case SF_FORMAT_PCM_S8:
            set_settings_int8(mix2mono_conf, sf_info);
            break;
        case SF_FORMAT_PCM_16:
            set_settings_short(mix2mono_conf, sf_info);
            break;
        case SF_FORMAT_PCM_32:
            set_settings_int(mix2mono_conf, sf_info);
            break;
        case SF_FORMAT_FLOAT:
            set_settings_float(mix2mono_conf, sf_info);
            break;
        case SF_FORMAT_DOUBLE:
            set_settings_double(mix2mono_conf, sf_info);
            break;
        default:
            fprintf(stderr, "Auto-format detection for the detected format is not implemented. Resorting to reading as double.\n");
            set_settings_double(mix2mono_conf, sf_info);
            break;
    }
}

int read_file_data_raw(mix2mono_config_t* restrict mix2mono_conf, SNDFILE* restrict sndfile, SF_INFO* restrict sf_info, void** x)
{
    *x = malloc(mix2mono_conf->input_file_size * mix2mono_conf->data_size);
    sf_count_t sf_count = sf_read_raw(sndfile, *x, mix2mono_conf->input_file_size * mix2mono_conf->data_size);
    if (sf_count != mix2mono_conf->input_file_size * mix2mono_conf->data_size) {
        fprintf(stderr, "\nRead count not equal to requested frames, %lld != %lld.\n", sf_count, mix2mono_conf->input_file_size * mix2mono_conf->data_size);

        return 1;
    }

    return 0;
}

const char* get_sndfile_major_format(SF_INFO* restrict sf_info)
{
    SF_FORMAT_INFO format_info ;
    int k, count;
    const uint32_t format_mask = 0x00FF0000;
    const uint32_t major_format = sf_info->format & format_mask;

    sf_command(NULL, SFC_GET_FORMAT_MAJOR_COUNT, &count, sizeof (int));

    for (k = 0 ; k < count ; k++) {
        format_info.format = k;
        sf_command(NULL, SFC_GET_FORMAT_MAJOR, &format_info, sizeof(format_info));
        if (major_format == format_info.format) {
            return format_info.name;
        }
    }

    return "N/A";
}

const char* get_sndfile_subtype(SF_INFO* restrict sf_info)
{
    SF_FORMAT_INFO format_info ;
    int k, count;
    const uint16_t subtype_mask = 0x00FF;
    const uint16_t subtype = sf_info->format & subtype_mask;

    sf_command(NULL, SFC_GET_FORMAT_SUBTYPE_COUNT, &count, sizeof (int));

    for (k = 0 ; k < count ; k++) {
        format_info.format = k;
        sf_command(NULL, SFC_GET_FORMAT_SUBTYPE, &format_info, sizeof(format_info));
        if (subtype == format_info.format) {
            return format_info.name;
        }
    }

    return "N/A";
}

const char* get_input_file_extension(SF_INFO* restrict sf_info)
{
    SF_FORMAT_INFO format_info ;
    int k, count;
    const uint32_t format_mask = 0x00FF0000;
    const uint32_t major_format = sf_info->format & format_mask;

    sf_command(NULL, SFC_GET_FORMAT_MAJOR_COUNT, &count, sizeof (int));

    for (k = 0 ; k < count ; k++) {
        format_info.format = k;
        sf_command(NULL, SFC_GET_FORMAT_MAJOR, &format_info, sizeof(format_info));
        if (major_format == format_info.format) {
            return format_info.extension;
        }
    }

    return "wav";
}

char* get_datetime_string()
{
    time_t time_since_epoch = time(NULL);
    struct tm* tm = localtime(&time_since_epoch);
    static char s[13];
    strftime(s, sizeof(s), "%d%m%y%H%M%S", tm);

    return s;
}

void generate_file_name(const char* restrict extension, char* restrict ofile, char* restrict ifile)
{
    if (ofile[0] != '\0' ) {

        return;
    }

    char ifile_no_extension[MIN_STR];
    size_t file_extension_size = 1 + strlen(extension);

    /* Remove the extension in the input file name */
    strncpy(ifile_no_extension, ifile, strlen(ifile) - file_extension_size);

    /* Fix an issue when the copied string is not terminated correctly */
    ifile_no_extension[strlen(ifile) - file_extension_size] = '\0';

    /* Remove the path specifier */
    if (ifile_no_extension[0] == '.' && ifile_no_extension[1] == '\\') {
        memmove(ifile_no_extension, ifile_no_extension + 2, MIN_STR - 2);
    }

    sprintf(ofile, "mix2mono-%s-%s.%s", ifile_no_extension, get_datetime_string(), extension); 
}

int output_file_info(SF_INFO* restrict sf_info, mix2mono_config_t* restrict mix2mono_conf)
{
    if (mix2mono_conf->info_flag) {
        fprintf(stdout, "\n\t\t---FILE INFO---\n");
        fprintf(stdout, "\tFile Name: %s\n", mix2mono_conf->ifile);
        fprintf(stdout, "\tSample Rate: %d\n", sf_info->samplerate);
        fprintf(stdout, "\tSamples: %lld\n", sf_info->frames);
        fprintf(stdout, "\tChannels: %d\n", sf_info->channels);
        fprintf(stdout, "\tSections: %d\n", sf_info->sections);
        fprintf(stdout, "\tSeekable: ");
        fprintf(stdout, sf_info->seekable ? "Yes\n" : "No\n");
        fprintf(stdout, "\tFormat: %s\n", get_sndfile_major_format(sf_info));
        fprintf(stdout, "\tSubtype: %s\n", get_sndfile_subtype(sf_info));
        fprintf(stdout, "\t\t---------------\n\n");
    }

    return 0;
}

void mix2mono_uint8(size_t size, int channels, void* x, void** x_mono)
{
    *x_mono = calloc(size/channels, sizeof(uint8_t));
    for (uint64_t i = 0; i < size/channels; i++) {
        for (uint16_t c = 0; c < channels; c++) {
            ((uint8_t*)*x_mono)[i] += (((uint8_t*)x)[i * channels + c]/channels);
        }
    }
}

void mix2mono_int8(size_t size, int channels, void* x, void** x_mono)
{
    *x_mono = calloc(size/channels, sizeof(int8_t));
    for (uint64_t i = 0; i < size/channels; i++) {
        for (uint16_t c = 0; c < channels; c++) {
            ((int8_t*)*x_mono)[i] += (((int8_t*)x)[i * channels + c]/channels);
        }
    }
}

void mix2mono_short(size_t size, int channels, void* x, void** x_mono)
{
    *x_mono = calloc(size/channels, sizeof(int16_t));
    for (uint64_t i = 0; i < size/channels; i++) {
        for (uint16_t c = 0; c < channels; c++) {
            ((int16_t*)*x_mono)[i] += (((int16_t*)x)[i * channels + c]/channels);
        }
    }
}

void mix2mono_int(size_t size, int channels, void* x, void** x_mono)
{
    *x_mono = calloc(size/channels, sizeof(int32_t));
    for (uint64_t i = 0; i < size/channels; i++) {
        for (uint16_t c = 0; c < channels; c++) {
            ((int32_t*)*x_mono)[i] += (((int32_t*)x)[i * channels + c]/channels);
        }
    }
}

void mix2mono_float(size_t size, int channels, void* x, void** x_mono)
{
    *x_mono = calloc(size/channels, sizeof(float));
    for (uint64_t i = 0; i < size/channels; i++) {
        for (uint16_t c = 0; c < channels; c++) {
            ((float*)*x_mono)[i] += (((float*)x)[i * channels + c]/channels);
        }
    }
}

void mix2mono_double(size_t size, int channels, void* x, void** x_mono)
{
    *x_mono = calloc(size/channels, sizeof(double));
    for (uint64_t i = 0; i < size/channels; i++) {
        for (uint16_t c = 0; c < channels; c++) {
            ((double*)*x_mono)[i] += (((double*)x)[i * channels + c]/channels);
        }
    }
}


int write_file(SNDFILE** restrict file, SF_INFO* restrict sf_info, void* restrict x_mono, mix2mono_config_t* restrict mix2mono_conf)
{
    SF_INFO osf_info = *sf_info;
    osf_info.channels = 1;

    *file = sf_open(mix2mono_conf->ofile, SFM_WRITE, &osf_info);
    if(!(*file)) {
        fprintf(stderr, "\nError with writing to file: %s\n", sf_strerror(*file));

        return 1;
    };

    osf_info.frames = sf_info->frames;
    sf_write_raw(*file, x_mono, osf_info.frames * mix2mono_conf->data_size);

    return 0;
}

int output_help()
{
    printf( 
            "Mix2Mono options,\n\n"
            "Basic usage, `mix2mono <audio file>`.\n"
            "\n\t-i,\t--input <file>\t= Path or name of the input file."
            "\n\t-o,\t--output <file>\t= Path or name of the output file."
            "\n\t\t--info\t\t= Output to stdout some info about the input file."
            "\n\t\t--version\t= Output version number."
            "\n"
          );

    return 0;
}

