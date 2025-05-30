#include "mix2mono.h"

//TODO: Test different formats and encodings. Works with WAVE.
//FIX: Does not like paths as file inputs

int main (int argc, char** argv) {
    SF_INFO sf_info;        // File info
    SNDFILE* file;          // Pointer to file
    void* x;                // Data from audio file, considered the input signal
    void* x_mono;         // Var to store the mono data
    SNDFILE* ofile;         // Output file pointer
    mix2mono_config_t mix2mono_conf; // Tool config

    /* Initialise the output file name */
    memset(mix2mono_conf.ofile, '\0', MAX_STR);

    /* Initialise the struct */
    memset(&sf_info, 0, sizeof(SF_INFO));

    /* Get the options from the CLI */
    CHECK_ERR(get_options(argc, argv, &mix2mono_conf));

    /* Initialise input audio file buffer and open file */
    file = NULL;
    CHECK_ERR(open_file(&file, &sf_info, &mix2mono_conf));

    /* Auto detect the input file format/encoding and match that */
    set_settings_auto(&mix2mono_conf, &sf_info);

    /* Output info on the inputted file */
    output_file_info(&sf_info, &mix2mono_conf);

    /* Initialise input data array and read the file data */
    x = NULL;
    CHECK_ERR(read_file_data_raw(&mix2mono_conf, file, &sf_info, &x));
    printf("Read file data succesfully.\n");

    /* Initialise mono array and convert file data to mono */
    x_mono = NULL;
    mix2mono_conf.mix2mono(mix2mono_conf.file_size, sf_info.channels, x, &x_mono);
    printf("Mixed file to mono.\n");

    const char* extension = get_input_file_extension(&sf_info);
    generate_file_name(extension, mix2mono_conf.ofile, mix2mono_conf.ifile);

    /* Initialise output file buffer and write to file */ 
    ofile = NULL;
    CHECK_ERR(write_file(&ofile, &sf_info, x_mono, &mix2mono_conf));
    printf("Outputted data to '%s'!\n", mix2mono_conf.ofile);

    sf_close(file);
    sf_close(ofile);
    free(x);
    free(x_mono);

    return 0;
}
