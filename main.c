#include "mix2mono.h"

int main (int argc, char** argv) {
    SF_INFO sf_info;        // File info
    SNDFILE* file  = NULL;  // Pointer to file
    double* x      = NULL;  // Data from audio file, considered the input signal
    double* x_mono = NULL;  // Var to store the mono data
    SNDFILE* ofile = NULL;  // Output file pointer
    mix2mono_config_t mix2mono_conf; // Tool config

    /* Initialise the output file name */
    memset(mix2mono_conf.ofile, '\0', MAX_STR);

    /* Initialise the struct */
    memset(&sf_info, 0, sizeof(SF_INFO));

    /* Get the options from the CLI */
    CHECK_ERR(get_options(argc, argv, &mix2mono_conf));

    /* Open audio file */
    CHECK_ERR(open_file(&file, &sf_info, &mix2mono_conf));

    /* Output info on the inputted file */
    output_file_info(&sf_info, &mix2mono_conf);

    /* Initialise input data array and read the file data */
    CHECK_ERR(read_file_data_double(&mix2mono_conf, file, &sf_info, &x));
    printf("Read file data succesfully.\n");

    /* Initialise mono array and convert file data to mono */
    mix2mono(sf_info.frames, sf_info.channels, x, &x_mono);
    printf("Mixed file to mono.\n");

    /* Get extension and generate the output filename */
    const char* extension = get_input_file_extension(&sf_info);
    generate_file_name(extension, mix2mono_conf.ofile, mix2mono_conf.ifile);

    /* Initialise output file buffer and write to file */ 
    CHECK_ERR(write_file(&ofile, &sf_info, x_mono, &mix2mono_conf));
    printf("Outputted data to '%s'!\n", mix2mono_conf.ofile);

    sf_close(file);
    sf_close(ofile);
    free(x);
    free(x_mono);

    return 0;
}
