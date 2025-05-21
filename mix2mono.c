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

int open_file(SNDFILE** file, SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf)
{
    *file = sf_open(mix2mono_conf->ifile, SFM_READ, sf_info);
    if(!(*file)) {
        fprintf(stderr, "\nError with opening input file: %s", sf_strerror(*file));

        return 1;
    };
    return 0;
}

int read_file_data(SNDFILE* file, SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf, double** x)
{
    *x = calloc(sf_info->frames * sf_info->channels, sizeof(double));
    sf_count_t sf_count = sf_readf_double(file, *x, sf_info->frames);
    if (sf_count != sf_info->frames) {
        fprintf(stderr, "\nRead count not equal to requested frames, %lld != %lld.\n", sf_count, sf_info->frames);

        return 1;
    }

    return 0;
}

const char* get_sndfile_major_format(SF_INFO* sf_info)
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

const char* get_sndfile_subtype(SF_INFO* sf_info)
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

char* get_datetime_string()
{
    time_t time_since_epoch = time(NULL);
    struct tm* tm = localtime(&time_since_epoch);
    static char s[13];
    strftime(s, sizeof(s), "%d%m%y%H%M%S", tm);

    return s;
}


void generate_file_name(char* ofile, char* ifile)
{
    if (ofile[0] != '\0' ) {

        return;
    }

    char ifile_no_extension[MIN_STR];

    /* Remove the extension in the input file name */
    strncpy(ifile_no_extension, ifile, strlen(ifile) - 4);

    /* Fix an issue when the copied string is not terminated correctly */
    ifile_no_extension[strlen(ifile) - 4] = '\0';

    /* Remove the path specifier */
    if (ifile_no_extension[0] == '.' && ifile_no_extension[1] == '\\') {
        memmove(ifile_no_extension, ifile_no_extension + 2, MIN_STR - 2);
    }

    sprintf(ofile, "mix2mono-%s-%s.wav", ifile_no_extension, get_datetime_string()); 
}

int output_file_info(SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf)
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

int mix2mono(SF_INFO* sf_info, double* x, double** x_mono)
{
    *x_mono = calloc(sf_info->frames, sizeof(double));
    for (uint64_t i = 0; i < sf_info->frames; i++) {
        for (uint16_t c = 0; c < sf_info->channels; c++) {
            (*x_mono)[i] += (x[sf_info->channels * i + c]/sf_info->channels);
        }
    }

    return 0;
}

int write_file(SNDFILE** file, SF_INFO* sf_info, double* x_mono, mix2mono_config_t* mix2mono_conf)
{
    SF_INFO osf_info = *sf_info;
    osf_info.channels = 1;

    *file = sf_open(mix2mono_conf->ofile, SFM_WRITE, &osf_info);
    if(!(*file)) {
        fprintf(stderr, "\nError with writing to file: %s\n", sf_strerror(*file));

        return 1;
    };

    osf_info.frames = sf_info->frames;
    sf_write_double(*file, x_mono, osf_info.frames);

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

