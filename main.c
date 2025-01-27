#include "mix2mono.h"

//TODO: Test different formats and encodings. Works with WAVE.

int main (int argc, char** argv) {
    printf("\n\t\t-Mix2Mono-\n");
    SF_INFO sf_info;        // File info

    mix2mono_config_t mix2mono_conf; // Tool config

    /* Initialise the struct */
    memset(&sf_info, 0, sizeof(SF_INFO));

    /* Set defaults to ensure certain behabiour */
    set_defaults(&mix2mono_conf);

    /* Get the options from the CLI */
    CHECK_ERR(get_options(&argc, argv, &mix2mono_conf));

    SNDFILE* file = NULL;          // Pointer to file
    CHECK_ERR(open_file(&file, &sf_info, &mix2mono_conf));

    /* Output info on the inputted file */
    output_file_info(&sf_info, &mix2mono_conf);

    double* x = NULL;              // Data from audio file, considered the input signal
    CHECK_ERR(read_file_data(file, &sf_info, &mix2mono_conf, &x));
    printf("\tRead file data succesfully.\n");

    double* x_mono = NULL;         // Var to store the mono data
    mix2mono(&sf_info, x, &x_mono);
    printf("\tMixed file to mono.\n");

    SNDFILE* ofile = NULL;        // Output file pointer
    CHECK_ERR(write_file(&ofile, &sf_info, x_mono, &mix2mono_conf));
    printf("\tOutputted data to '%s'!\n\n", mix2mono_conf.ofile);

    sf_close(file);
    sf_close(ofile);
    free(x);
    free(x_mono);
    return 0;
}
