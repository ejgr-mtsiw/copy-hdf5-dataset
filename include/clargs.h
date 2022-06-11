/*
 ============================================================================
 Name        : clargs.h
 Author      : Eduardo Ribeiro
 Version     :
 Description : Structures and functions to manage command line arguments
 ============================================================================
 */
#ifndef CL_ARGS_H
#define CL_ARGS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Compress the dataset?
 */
#define COMPRESS_DATASET 0

/**
 * Dataset compression level
 */
#define ZLIB_COMPRESSION_LEVEL 6

/**
 * Do not edit
 */
#define READ_CL_ARGS_OK 0
#define READ_CL_ARGS_NOK 1

#define DONT_USE_COMPRESSION 0
#define USE_COMPRESSION 1

/**
 * Structure to store command line options
 */
typedef struct clargs {
	/**
	 * The name of the input file with the dataset
	 */
	char *filename;

	/**
	 * The name of the file to store the compressed dataset
	 */
	char *output_filename;

	/**
	 * The dataset identifier
	 */
	char *datasetname;

	/**
	 * Compress the dataset?
	 */
	unsigned char compress_dataset;

	/**
	 * Dataset compression level
	 */
	unsigned char compression_level;
} clargs;

/**
 * Reads the arguments from the command line and stores
 * them in the args structure
 */
int read_args(int argc, char **argv, clargs *args);

#endif
