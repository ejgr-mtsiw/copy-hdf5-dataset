#include "clargs.h"

/**
 * Reads the arguments from the command line and stores
 * them in the args structure
 */
int read_args(int argc, char **argv, clargs *args) {
	int c = 0, idata = 0;

	opterr = 0;

	// Set defaults
	args->filename = NULL;
	args->output_filename = NULL;
	args->datasetname = NULL;
	args->compress_dataset = COMPRESS_DATASET;
	args->compression_level = ZLIB_COMPRESSION_LEVEL;

	while ((c = getopt(argc, argv, "f:o:d:z:")) != -1) {
		switch (c) {
		case 'f':
			args->filename = optarg;
			break;
		case 'o':
			args->output_filename = optarg;
			break;
		case 'd':
			args->datasetname = optarg;
			break;
		case 'z':
			idata = atoi(optarg);
			if (idata > 0 && idata < 10) {
				args->compress_dataset = USE_COMPRESSION;
				args->compression_level = idata;
			} else {
				args->compress_dataset = DONT_USE_COMPRESSION;
			}
			break;
		case '?':
			if (optopt == 'f') {
				fprintf(stderr, "Must set input filename (-f).\n");
				return READ_CL_ARGS_NOK;
			}

			if (optopt == 'o') {
				fprintf(stderr, "Must set output filename (-f).\n");
				return READ_CL_ARGS_NOK;
			}

			if (optopt == 'd') {
				fprintf(stderr, "Must set dataset identifier (-d).\n");
				return READ_CL_ARGS_NOK;
			}

			if (optopt == 'z') {
				// Compress using default compression level
				args->compress_dataset = USE_COMPRESSION;
			}
			break;

		default:
			abort();
		}
	}

	if (args->filename == NULL || args->datasetname == NULL) {
		fprintf(stdout,
				"Usage:\n %s -f <filename> -o <output_filename> -d <dataset>\n",
				argv[0]);
		fprintf(stdout, "Optional parameters:\n");
		fprintf(stdout,
				"-z compress_dataset_level Default: 6, 0 disables compression\n");
		return READ_CL_ARGS_NOK;
	}

	return READ_CL_ARGS_OK;
}
