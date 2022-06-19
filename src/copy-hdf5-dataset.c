/*
 ============================================================================
 Name        : copy-hdf5-dataset.c
 Author      : Eduardo Ribeiro
 Description : Copies and compresses a dataset created by the python script
 to be used by tghe C code
 ============================================================================
 */

#include "bit_utils.h"
#include "clargs.h"
#include "dataset.h"
#include "dataset_hdf5.h"
#include <stdio.h>
#include <stdlib.h>
#include "hdf5.h"
#include <math.h>
#include <time.h>

/**
 *
 */
int main(int argc, char **argv) {

	/**
	 * Command line arguments set by the user
	 */
	clargs args;

	/**
	 * The dataset properties
	 */
	dataset_t dataset;

	/**
	 * Parse command line arguments
	 */
	if (read_args(argc, argv, &args) == READ_CL_ARGS_NOK) {
		return EXIT_FAILURE;
	}

	/**
	 * Open input data file
	 */
	//Open the data file
	hid_t input_file_id = H5Fopen(args.filename, H5F_ACC_RDWR, H5P_DEFAULT);
	if (input_file_id < 1) {
		// Error creating file
		fprintf(stderr, "Error opening input file %s\n", args.filename);
		return EXIT_FAILURE;
	}

	hid_t input_dataset_id = H5Dopen2(input_file_id, args.datasetname,
	H5P_DEFAULT);
	if (input_dataset_id < 1) {
		// Error reading file
		fprintf(stderr, "Input dataset %s not found!\n", args.datasetname);

		// Free resources
		H5Fclose(input_file_id);
		return EXIT_FAILURE;
	}

	/**
	 * Read dataset attributes
	 */
	if (read_attributes(input_dataset_id, &dataset) != DATASET_OK) {
		fprintf(stderr, "Error readings attributes from dataset\n");

		// Free resources
		H5Dclose(input_dataset_id);
		H5Fclose(input_file_id);
		return EXIT_FAILURE;
	}

	/**
	 * Input dataset dimensions
	 * n_attributes+n_jnsqs+1 (for class)
	 * The python code reserves space for the jnsqs and only supports 2 classes
	 */
	hsize_t input_dataset_dimensions[2] = { dataset.n_observations,
			dataset.n_attributes + dataset.n_jnsqs + 1 };

	/**
	 * Create the output data file
	 */
	hid_t output_file_id = H5Fcreate(args.output_filename, H5F_ACC_EXCL,
	H5P_DEFAULT, H5P_DEFAULT);
	if (output_file_id < 1) {
		// Error creating file
		fprintf(stdout, "Error creating %s\n", args.filename);
		return EXIT_FAILURE;
	}
	fprintf(stdout, " - Empty output file created.\n");

	/**
	 * Output dataset dimensions
	 */

	/**
	 * Create a 2D dataspace
	 * Set dataspace dimension
	 * Number of lines = number of observations
	 * Number of columns = number of attributes + n bits for class
	 */
	// https://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
	unsigned long total_bits = dataset.n_attributes + dataset.n_bits_for_class;
	unsigned int n_longs = total_bits / LONG_BITS
			+ (total_bits % LONG_BITS != 0);

	hsize_t output_dataset_dimensions[2] = { dataset.n_observations, n_longs };

	/**
	 * Dataspace(s)
	 */
	hid_t output_dataset_space_id = H5Screate_simple(2,
			output_dataset_dimensions, NULL);
	fprintf(stdout, " - Output dataspace created.\n");

	hid_t input_dataset_space_id = H5Screate_simple(2, input_dataset_dimensions,
	NULL);
	fprintf(stdout, " - Input dataspace created.\n");

	// Dataset creation lists
	// Create a dataset creation property list
	hid_t output_property_list_id = H5Pcreate(H5P_DATASET_CREATE);
	H5Pset_layout(output_property_list_id, H5D_CHUNKED);

	hid_t input_property_list_id = H5Pcreate(H5P_DATASET_ACCESS);
	//H5Pset_layout(input_property_list_id, H5D_CHUNKED);

	// The choice of the chunk size affects performance!
	// for now we will choose one line
	hsize_t output_chunk_dimensions[2] = { 1, output_dataset_dimensions[1] };

	hsize_t input_chunk_dimensions[2] = { 1, input_dataset_dimensions[1] };

	H5Pset_chunk(output_property_list_id, 2, output_chunk_dimensions);
	//H5Pset_chunk(input_property_list_id, 2, input_chunk_dimensions);

	if (args.compress_dataset == USE_COMPRESSION) {
		// Set ZLIB / DEFLATE Compression.
		H5Pset_deflate(output_property_list_id, args.compression_level);
	}

	// Create the output dataset
	hid_t output_dataset_id = H5Dcreate2(output_file_id, args.datasetname,
			H5T_STD_U64LE, output_dataset_space_id, H5P_DEFAULT,
			output_property_list_id, H5P_DEFAULT);
	fprintf(stdout, " - Output dataset created.\n");

	fprintf(stdout, " - Starting filling in dataset.\n");

	// Create a memory dataspace to indicate the size of our buffer/chunk
	hid_t output_memory_space_id = H5Screate_simple(2, output_chunk_dimensions,
	NULL);

	hid_t input_memory_space_id = H5Screate_simple(2, input_chunk_dimensions,
	NULL);

	// Alocate output buffer
	unsigned long *output_buffer = calloc(output_chunk_dimensions[1],
			sizeof(uint_fast64_t));

	// Alocate input buffer
	unsigned long *input_buffer = calloc(input_chunk_dimensions[1],
			sizeof(uint_fast64_t));

	// We will read/write one line at a time
	hsize_t offset[2] = { 0, 0 };

	hsize_t in_count[2] = { 1, input_chunk_dimensions[1] };
	hsize_t out_count[2] = { 1, output_chunk_dimensions[1] };

	// Total number of longs to fill
	unsigned int n_cols = output_dataset_dimensions[1];

	// Current column
	unsigned int column = 0;
	//Class offset
	unsigned int line_class = 0;

	unsigned int class_bits_to_set = 0;

	for (unsigned int line = 0; line < dataset.n_observations; line++) {

		// Update offset
		offset[0] = line;

		// Select input hyperslab
		H5Sselect_hyperslab(input_dataset_space_id, H5S_SELECT_SET, offset,
		NULL, in_count, NULL);

		// Select output hyperslab
		H5Sselect_hyperslab(output_dataset_space_id, H5S_SELECT_SET, offset,
		NULL, out_count, NULL);

		// Read line
		H5Dread(input_dataset_id, H5T_NATIVE_ULONG, input_memory_space_id,
				input_dataset_space_id, H5P_DEFAULT, input_buffer);

		column = 0;
		line_class = input_buffer[dataset.n_attributes + dataset.n_jnsqs];
		class_bits_to_set = dataset.n_bits_for_class;

		for (unsigned int i = 0; i < n_cols; i++) {

			output_buffer[i] = 0UL;

			for (size_t bit = 0; bit < LONG_BITS; bit++) {

				output_buffer[i] <<= 1;

				if (column < dataset.n_attributes) {
					// Filling in attributes
					if (input_buffer[column] == 1) {
						output_buffer[i] |= 1;
					}
				} else {
					// Filling in the class
					if (class_bits_to_set > 0) {
						class_bits_to_set--;

						if (CHECK_BIT(line_class, class_bits_to_set)) {
							output_buffer[i] |= 1;
						}
					} else {
						// We're done here: fast forward!
						output_buffer[i] <<= (LONG_BITS - 1 - bit);
						break;
					}
				}

				column++;
			}
		}

		// Write buffer to dataset
		// mem_space and file_space should now have the same number of elements selected
		H5Dwrite(output_dataset_id, H5T_NATIVE_ULONG, output_memory_space_id,
				output_dataset_space_id, H5P_DEFAULT, output_buffer);

		if (line % 100 == 0) {
			fprintf(stdout, " - Writing [%d/%d]\n", line,
					dataset.n_observations);
		}
	}

	// Set dataset properties
	herr_t status = write_attribute(output_dataset_id, "n_classes",
	H5T_NATIVE_UINT, &dataset.n_classes);
	if (status < 0) {
		return EXIT_FAILURE;
	}

	status = write_attribute(output_dataset_id, "n_attributes",
	H5T_NATIVE_UINT, &dataset.n_attributes);
	if (status < 0) {
		return EXIT_FAILURE;
	}

	status = write_attribute(output_dataset_id, "n_observations",
	H5T_NATIVE_UINT, &dataset.n_observations);
	if (status < 0) {
		return EXIT_FAILURE;
	}

	free(output_buffer);
	free(input_buffer);

	// Close resources
	H5Pclose(output_property_list_id);
	H5Pclose(input_property_list_id);

	H5Sclose(output_memory_space_id);
	H5Sclose(input_memory_space_id);

	H5Sclose(output_dataset_space_id);
	H5Sclose(input_dataset_space_id);

	H5Dclose(output_dataset_id);
	H5Fclose(output_file_id);

	H5Dclose(input_dataset_id);
	H5Fclose(input_file_id);

	fprintf(stdout, "All done!\n");

	return EXIT_SUCCESS;
}
