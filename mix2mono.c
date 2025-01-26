#include "mix2mono.h"

void set_defaults(mix2mono_config_t* mix2mono_conf) {
    strcpy(mix2mono_conf->ofile, "mix2mono.wav");
}

int get_options(int* restrict argc, char** restrict argv, mix2mono_config_t* restrict mix2mono_conf) {
    char strval[MAX_STR];

    if (*argc == 1) {
        fprintf(stdout, WELCOME_STR);

        return 1;
    }

    if (*argc == 2) {
        if (!(strcmp("--version", argv[1]))) {
            fprintf(stdout, VERSION_STR);

            return 1;
        }

        if (!(strcmp("--help", argv[1]))) {
            output_help();

            return 1;
        }
    }

    for (int i = 1; i < *argc; i++) {
        if (argv[i][0] != '-' && argv[i - 1][0] != '-') {
            CHECK_RES(sscanf(argv[i], "%s", strval));
            strcpy(mix2mono_conf->ifile, strval);
            continue;
        }

        if (!(strcmp("-o", argv[i])) || !(strcmp("--output", argv[i]))) {
            CHECK_RES(sscanf(argv[i + 1], "%s", strval));
            strcpy(mix2mono_conf->ofile, strval);
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

int open_file(SNDFILE** file, SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf) {

    *file = sf_open(mix2mono_conf->ifile, SFM_READ, sf_info);
    if(!(*file)) {
        fprintf(stderr, "\nSNDFile Error: %s.\n\n", sf_strerror(*file));

        return 1;
    };
    return 0;
}

int read_file_data(SNDFILE* file, SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf, double** x) {
    *x = calloc(sf_info->frames * sf_info->channels, sizeof(double));
    sf_count_t sf_count = sf_readf_double(file, *x, sf_info->frames);
    if (sf_count != sf_info->frames) {
        fprintf(stderr, "\nRead count not equal to requested frames, %lld != %lld.\n\n", sf_count, sf_info->frames);

        return 1;
    }

    return 0;
}

char* get_sndfile_major_format(SF_INFO* sf_info) {
    const uint32_t format_mask = 0x00FF0000;
    const uint32_t major_format = sf_info->format & format_mask;

    for (int i = 0; i < SND_MAJOR_FORMAT_NUM; i++) {
        if (major_format == snd_format_arr[i]) {
            return snd_format_arr_desc[i];
        }
    }

    return "N/A";
}

char* get_sndfile_subtype(SF_INFO* sf_info) {
    const uint16_t subtype_mask = 0x00FF;
    const uint16_t subtype = sf_info->format & subtype_mask;

    for (int i = 0; i < SND_SUBTYPE_NUM; i++) {
        if (subtype == snd_subtype_arr[i]) {
            return snd_subtype_arr_desc[i];
        }
    }

    return "N/A";
}

void output_info(SF_INFO* sf_info, mix2mono_config_t* mix2mono_conf) {
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
}

int mix2mono(SF_INFO* sf_info, double* x, double** x_mono) {

    *x_mono = calloc(sf_info->frames, sizeof(double));
    uint64_t i = 0;
    uint16_t c = 0;
    for (i = 0; i < sf_info->frames; i++) {
        for (c = 0; c < sf_info->channels; c++) {
            (*x_mono)[i] += (x[sf_info->channels * i + c]/sf_info->channels);
        }
    }

    return 0;
}

int write_file(SNDFILE** file, SF_INFO* sf_info, double* x_mono, mix2mono_config_t* mix2mono_conf) {

    SF_INFO osf_info;

    osf_info.format = sf_info->format;
    osf_info.frames = sf_info->frames;
    osf_info.channels = 1;
    osf_info.samplerate = sf_info->samplerate;
    osf_info.seekable = sf_info->seekable;
    osf_info.sections = sf_info->sections;

    *file = sf_open(mix2mono_conf->ofile, SFM_WRITE, &osf_info);
    if(!(*file)) {
        fprintf(stderr, "\nSNDFile Error: %s.\n\n", sf_strerror(*file));

        return 1;
    };

    sf_count_t sf_count = sf_write_double(*file, x_mono, sf_info->frames);
    if (sf_count != sf_info->frames) {
        fprintf(stderr, "\nRead count not equal to requested frames, %lld != %lld.\n\n", sf_count, sf_info->frames);

        return 1;
    }

    return 0;
}

void output_help() {

}

uint32_t snd_format_arr[SND_MAJOR_FORMAT_NUM] = {
    SF_FORMAT_WAV,
    SF_FORMAT_AIFF,
    SF_FORMAT_AU,
    SF_FORMAT_RAW,
    SF_FORMAT_PAF,
    SF_FORMAT_SVX,
    SF_FORMAT_NIST,
    SF_FORMAT_VOC,
    SF_FORMAT_IRCAM,
    SF_FORMAT_W64,
    SF_FORMAT_MAT4,
    SF_FORMAT_MAT5,
    SF_FORMAT_PVF,
    SF_FORMAT_XI,
    SF_FORMAT_HTK,
    SF_FORMAT_SDS,
    SF_FORMAT_AVR,
    SF_FORMAT_WAVEX,
    SF_FORMAT_SD2,
    SF_FORMAT_FLAC,
    SF_FORMAT_CAF,
    SF_FORMAT_WVE,
    SF_FORMAT_OGG,
    SF_FORMAT_MPC2K,
    SF_FORMAT_RF64,
    SF_FORMAT_MPEG,
};

char* snd_format_arr_desc[SND_MAJOR_FORMAT_NUM] = {
    "Microsoft WAV format (little endian)",
    "Apple/SGI AIFF format (big endian)",
    "Sun/NeXT AU format (big endian)",
    "RAW PCM data",
    "Ensoniq PARIS file format",
    "Amiga IFF / SVX8 / SV16 format",
    "Sphere NIST format",
    "VOC files",
    "Berkeley/IRCAM/CARL",
    "Sonic Foundry’s 64 bit RIFF/WAV",
    "Matlab (tm) V4.2 / GNU Octave 2.0",
    "Matlab (tm) V5.0 / GNU Octave 2.1",
    "Portable Voice Format",
    "Fasttracker 2 Extended Instrument",
    "HMM Tool Kit format",
    "Midi Sample Dump Standard",
    "Audio Visual Research",
    "MS WAVE with WAVEFORMATEX",
    "Sound Designer 2",
    "FLAC lossless file format",
    "Core Audio File format",
    "Psion WVE format",
    "Xiph OGG container",
    "Akai MPC 2000 sampler",
    "RF64 WAV file",
    "MPEG-1/2 audio stream",
};

uint32_t snd_subtype_arr[SND_SUBTYPE_NUM] = {
    SF_FORMAT_PCM_S8,
    SF_FORMAT_PCM_16,
    SF_FORMAT_PCM_24,
    SF_FORMAT_PCM_32,
    SF_FORMAT_PCM_U8,
    SF_FORMAT_FLOAT,
    SF_FORMAT_DOUBLE,
    SF_FORMAT_ULAW,
    SF_FORMAT_ALAW,
    SF_FORMAT_IMA_ADPCM,
    SF_FORMAT_MS_ADPCM,
    SF_FORMAT_GSM610,
    SF_FORMAT_VOX_ADPCM,
    SF_FORMAT_NMS_ADPCM_16,
    SF_FORMAT_NMS_ADPCM_24,
    SF_FORMAT_NMS_ADPCM_32,
    SF_FORMAT_G721_32,
    SF_FORMAT_G723_24,
    SF_FORMAT_G723_40,
    SF_FORMAT_DWVW_12,
    SF_FORMAT_DWVW_16,
    SF_FORMAT_DWVW_24,
    SF_FORMAT_DWVW_N,
    SF_FORMAT_DPCM_8,
    SF_FORMAT_DPCM_16,
    SF_FORMAT_VORBIS,
    SF_FORMAT_OPUS,
    SF_FORMAT_ALAC_16,
    SF_FORMAT_ALAC_20,
    SF_FORMAT_ALAC_24,
    SF_FORMAT_ALAC_32,
    SF_FORMAT_MPEG_LAYER_I,
    SF_FORMAT_MPEG_LAYER_II,
    SF_FORMAT_MPEG_LAYER_III,
};

char* snd_subtype_arr_desc[SND_SUBTYPE_NUM] = {
    "Signed 8 bit data",
    "Signed 16 bit data",
    "Signed 24 bit data",
    "Signed 32 bit data",
    "Unsigned 8 bit data (WAV and RAW only)",
    "32 bit float data",
    "64 bit float data",
    "U-Law encoded.",
    "A-Law encoded.",
    "IMA ADPCM.",
    "Microsoft ADPCM.",
    "GSM 6.10 encoding.",
    "OKI / Dialogix ADPCM",
    "16kbs NMS G721-variant encoding.",
    "24kbs NMS G721-variant encoding.",
    "32kbs NMS G721-variant encoding.",
    "32kbs G721 ADPCM encoding.",
    "24kbs G723 ADPCM encoding.",
    "40kbs G723 ADPCM encoding.",
    "12 bit Delta Width Variable Word encoding.",
    "16 bit Delta Width Variable Word encoding.",
    "24 bit Delta Width Variable Word encoding.",
    "N bit Delta Width Variable Word encoding.",
    "8 bit differential PCM (XI only)",
    "16 bit differential PCM (XI only)",
    "Xiph Vorbis encoding.",
    "Xiph/Skype Opus encoding.",
    "Apple Lossless Audio Codec (16 bit).",
    "Apple Lossless Audio Codec (20 bit).",
    "Apple Lossless Audio Codec (24 bit).",
    "Apple Lossless Audio Codec (32 bit).",
    "MPEG-1 Audio Layer I.",
    "MPEG-1 Audio Layer II.",
    "MPEG-2 Audio Layer III.",
};

