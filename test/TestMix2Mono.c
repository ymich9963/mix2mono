#include "unity/unity.h"
#include "unity/unity_internals.h"
#include "../mix2mono.h"


void setUp() {}

void tearDown() {
    fflush(stdout);
}

void test_output_help() {
    TEST_ASSERT_EQUAL_INT(0, output_help());
}

void test_output_file_info() {
    SF_INFO sf_info = {
        .samplerate = 1,
        .frames = 1,
        .channels = 1,
        .sections = 1,
        .seekable = 1,
        .format = SF_FORMAT_WAV | SF_FORMAT_FLOAT,
    };

    mix2mono_config_t mix2mono_conf = {
        .ifile = "test.wav",
        .info_flag = 1,
    };

    TEST_ASSERT_EQUAL_INT(0, output_file_info(&sf_info, &mix2mono_conf));
}

void split(char* cmd, char** argv, int* argc) {
    char* token = strtok(cmd," "); 
    *argc = 0;
    while(token != NULL) {
        argv[*argc] = token; 
        token = strtok(NULL," "); 
        *argc = *argc + 1;
    }
}

void reset(char** argv, mix2mono_config_t* mix2mono_conf, int size) {
    for(int i = 0; i < size; i++) {
        argv[i] = "\0";
    }
    mix2mono_config_t x = { 0 };
    *mix2mono_conf = x;
}

void test_get_options() {
    mix2mono_config_t mix2mono_conf;
    int argc;
    char* argv[9];

    char cmd0[] = "first";
    split(cmd0, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(argc, argv, &mix2mono_conf));
    reset(argv, &mix2mono_conf, 10);

    char cmd1[] = "first --version";
    split(cmd1, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(argc, argv, &mix2mono_conf));
    reset(argv, &mix2mono_conf, 10);

    char cmd2[] = "first --help";
    split(cmd2, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(argc, argv, &mix2mono_conf));
    reset(argv, &mix2mono_conf, 10);

    char cmd3[] = "first --test";
    split(cmd3, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(argc, argv, &mix2mono_conf));

    reset(argv, &mix2mono_conf, 10);
    char cmd4[] = "first test.wav -i test.wav -o test.wav --info";
    split(cmd4, argv, &argc);
    TEST_ASSERT_EQUAL_INT(0, get_options(argc, argv, &mix2mono_conf));
    reset(argv, &mix2mono_conf, 10);

    char cmd5[] = "first --balls balls";
    split(cmd5, argv, &argc);
    TEST_ASSERT_EQUAL_INT(1, get_options(argc, argv, &mix2mono_conf));
    reset(argv, &mix2mono_conf, 10);
}

void test_open_file() {
    SNDFILE *sndfile;
    SF_INFO sf_info;
    mix2mono_config_t mix2mono_conf = {
        .ifile = "test.wav",
    };

    TEST_ASSERT_EQUAL_INT(0, open_file(&sndfile, &sf_info, &mix2mono_conf));

    mix2mono_conf.ifile[0] = 'b';
    TEST_ASSERT_EQUAL_INT(1, open_file(&sndfile, &sf_info, &mix2mono_conf));
}

void test_set_settings() {
    SF_INFO sf_info = {
        .frames = 1,
        .channels = 1,
    };
    mix2mono_config_t mix2mono_conf;

    sf_info.format = SF_FORMAT_PCM_U8;
    set_settings_auto(&mix2mono_conf, &sf_info);
    TEST_ASSERT_EQUAL_INT(1, mix2mono_conf.data_size);

    sf_info.format = SF_FORMAT_ULAW;
    set_settings_auto(&mix2mono_conf, &sf_info);
    TEST_ASSERT_EQUAL_INT(1, mix2mono_conf.data_size);

    sf_info.format = SF_FORMAT_ALAW;
    set_settings_auto(&mix2mono_conf, &sf_info);
    TEST_ASSERT_EQUAL_INT(1, mix2mono_conf.data_size);

    sf_info.format = SF_FORMAT_PCM_S8;
    set_settings_auto(&mix2mono_conf, &sf_info);
    TEST_ASSERT_EQUAL_INT(1, mix2mono_conf.data_size);

    sf_info.format = SF_FORMAT_PCM_16;
    set_settings_auto(&mix2mono_conf, &sf_info);
    TEST_ASSERT_EQUAL_INT(2, mix2mono_conf.data_size);

    sf_info.format = SF_FORMAT_PCM_32;
    set_settings_auto(&mix2mono_conf, &sf_info);
    TEST_ASSERT_EQUAL_INT(4, mix2mono_conf.data_size);

    sf_info.format = SF_FORMAT_FLOAT;
    set_settings_auto(&mix2mono_conf, &sf_info);
    TEST_ASSERT_EQUAL_INT(4, mix2mono_conf.data_size);

    sf_info.format = SF_FORMAT_DOUBLE;
    set_settings_auto(&mix2mono_conf, &sf_info);
    TEST_ASSERT_EQUAL_INT(8, mix2mono_conf.data_size);

    // random format that mix2mono doesn't support
    sf_info.format = SF_FORMAT_AIFF;
    set_settings_auto(&mix2mono_conf, &sf_info);
    TEST_ASSERT_EQUAL_INT(8, mix2mono_conf.data_size);
}

void test_read_file_data_raw() {
    SNDFILE *sndfile;
    SF_INFO sf_info;
    mix2mono_config_t mix2mono_conf = {
        .ifile = "test.wav",
    };
    void* x;
    open_file(&sndfile, &sf_info, &mix2mono_conf);

    TEST_ASSERT_EQUAL_INT(0, read_file_data_raw(&mix2mono_conf, sndfile, &sf_info, &x));

    mix2mono_conf.input_file_size = 1;
    mix2mono_conf.data_size = 1;
    TEST_ASSERT_EQUAL_INT(1, read_file_data_raw(&mix2mono_conf, sndfile, &sf_info, &x));
}

void test_get_sndfile_major_format() {
    SF_INFO sf_info = {
        .format = SF_FORMAT_WAV | SF_FORMAT_PCM_S8,
    };

    TEST_ASSERT_EQUAL_STRING("WAV (Microsoft)", get_sndfile_major_format(&sf_info));

    sf_info.format = 0xFFFFFF;
    TEST_ASSERT_EQUAL_STRING("N/A", get_sndfile_major_format(&sf_info));
}

void test_get_sndfile_subtype() {
    SF_INFO sf_info = {
        .format = SF_FORMAT_WAV | SF_FORMAT_PCM_S8,
    };

    TEST_ASSERT_EQUAL_STRING("Signed 8 bit PCM", get_sndfile_subtype(&sf_info));

    sf_info.format = 0xFFFFFF;
    TEST_ASSERT_EQUAL_STRING("N/A", get_sndfile_subtype(&sf_info));
}

void test_get_input_file_extension() {
    SF_INFO sf_info = {
        .format = SF_FORMAT_WAV,
    };

    TEST_ASSERT_EQUAL_STRING("wav", get_input_file_extension(&sf_info));

    sf_info.format = 999;

    TEST_ASSERT_EQUAL_STRING("wav", get_input_file_extension(&sf_info));
}

void test_get_datetime_string() {
    TEST_ASSERT_EQUAL_INT(12, strlen(get_datetime_string()));
}

void test_generate_file_name() {
    char ofile[MAX_STR];
    char ifile[MIN_STR] = ".\\test.wav";

    memset(ofile, '\0', MAX_STR);
    generate_file_name("wav", ofile, ifile);

    TEST_ASSERT_EQUAL_INT(0, strncmp(ofile, "mix2mono-test-", 14));
    TEST_ASSERT_EQUAL_INT(0, strncmp(strrev(ofile), "vaw.", 4));
    TEST_ASSERT_EQUAL_INT(strlen("mix2mono-test-ddmmyyHHMMSS.wav"), strlen(ofile));

    strcpy(ofile, "test");
    generate_file_name("wav", ofile, ifile);
    TEST_ASSERT_EQUAL_STRING("test", ofile);
}

void test_mix2mono() {
    // INFO: Just want to say that during testing these functions I forgot how integer division works,
    // because I was using all 1s instead of 2s as the test data and then executing the function.

    SF_INFO sf_info = {
        .frames = 4,
        .channels = 2,
    };

    // uint8
    uint8_t x_uint8[] = {2,2,2,2,2,2,2,2};
    void* x_mono_uint8;
    mix2mono_uint8(8, 2, x_uint8, &x_mono_uint8);
    uint8_t result_uint8[] = {2,2,2,2};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_UINT8(result_uint8[i], ((uint8_t*)x_mono_uint8)[i]);
    }
    TEST_ASSERT_NOT_EQUAL_UINT8(1, ((uint8_t*)x_mono_uint8)[5]);

    // int8
    int8_t x_int8[] = {2,2,2,2,2,2,2,2};
    void* x_mono_int8;
    mix2mono_int8(8, 2, x_int8, &x_mono_int8);
    int8_t result_int8[] = {2,2,2,2};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_INT8(result_int8[i], ((int8_t*)x_mono_int8)[i]);
    }
    TEST_ASSERT_NOT_EQUAL_INT8(1, ((int8_t*)x_mono_int8)[5]);

    // short/int16
    int16_t x_int16[] = {2,2,2,2,2,2,2,2};
    void* x_mono_int16;
    mix2mono_short(8, 2, x_int16, &x_mono_int16);
    int16_t result_int16[] = {2,2,2,2};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_INT16(result_int16[i], ((int16_t*)x_mono_int16)[i]);
    }
    TEST_ASSERT_NOT_EQUAL_INT16(1, ((int16_t*)x_mono_int16)[5]);

    // int/int32
    int32_t x_int32[] = {2,2,2,2,2,2,2,2};
    void* x_mono_int32;
    mix2mono_int(8, 2, x_int32, &x_mono_int32);
    int result_int32[] = {2,2,2,2};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_INT(result_int32[i], ((int32_t*)x_mono_int32)[i]);
    }
    TEST_ASSERT_NOT_EQUAL_INT(1, ((int32_t*)x_mono_int32)[5]);

    // float
    float x_float[] = {1,1,1,1,1,1,1,1};
    void* x_mono_float;
    mix2mono_float(8, 2, x_float, &x_mono_float);
    float result_float[] = {1,1,1,1};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_FLOAT(result_float[i], ((float*)x_mono_float)[i]);
    }
    TEST_ASSERT_NOT_EQUAL_FLOAT(1, ((float*)x_mono_float)[5]);

    // double
    double x_double[] = {1,1,1,1,1,1,1,1};
    void* x_mono_double;
    mix2mono_double(8, 2, x_double, &x_mono_double);
    double result_double[] = {1,1,1,1};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_FLOAT(result_double[i], ((double*)x_mono_double)[i]);
    }
    TEST_ASSERT_NOT_EQUAL_FLOAT(1, ((double*)x_mono_double)[5]);
}

void test_write_file() {
    SNDFILE *sndfile;
    SF_INFO sf_info;
    double x_mono[] = {1,1,1,1};
    mix2mono_config_t mix2mono_conf = {
        .ofile = "otest.wav",
    };

    memset(&sf_info, 0, sizeof(SF_INFO));
    sf_info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    sf_info.frames = 4;
    sf_info.samplerate = 48000;

    TEST_ASSERT_EQUAL_INT(0, write_file(&sndfile, &sf_info, x_mono, &mix2mono_conf));

    mix2mono_conf.ofile[0] = '\0';
    TEST_ASSERT_EQUAL_INT(1, write_file(&sndfile, &sf_info, x_mono, &mix2mono_conf));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_output_help);
    RUN_TEST(test_output_file_info);
    RUN_TEST(test_get_options);
    RUN_TEST(test_open_file);
    RUN_TEST(test_set_settings);
    RUN_TEST(test_read_file_data_raw);
    RUN_TEST(test_get_sndfile_major_format);
    RUN_TEST(test_get_sndfile_subtype);
    RUN_TEST(test_get_input_file_extension);
    RUN_TEST(test_get_datetime_string);
    RUN_TEST(test_generate_file_name);
    RUN_TEST(test_mix2mono);
    RUN_TEST(test_write_file);
    return UNITY_END();
}
