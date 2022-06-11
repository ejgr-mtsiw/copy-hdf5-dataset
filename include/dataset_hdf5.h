/*
 ============================================================================
 Name        : dataset_hdf5.h
 Author      : Eduardo Ribeiro
 Description : Structures and functions to manage hdf5 datasets
 ============================================================================
 */

#ifndef DATASET_HDF5_H
#define DATASET_HDF5_H

#include "bit_utils.h"
#include "dataset.h"
#include "hdf5.h"
#include <math.h>
#include <malloc.h>
#include <stdint.h>

#define DATASET_OK 0

#define DATASET_INVALID_DIMENSIONS 1
#define DATASET_NOT_ENOUGH_CLASSES 2
#define DATASET_NOT_ENOUGH_ATTRIBUTES 3
#define DATASET_NOT_ENOUGH_OBSERVATIONS 4

#define ERROR_ALLOCATING_DATASET_DATA 8

/**
 * Attribute for number of classes
 */
#define N_CLASSES_ATTRIBUTE "xnc"

/**
 * Attribute for number of attributes
 */
#define N_ATTRIBUTES_ATTRIBUTE "xna"

/**
 * Attribute for number of jnsqs
 */
#define N_JNSQS_ATTRIBUTE "xnj"

/**
 * Attribute for number of observations
 */
#define N_OBSERVATIONS_ATTRIBUTE "xno"

/**
 * Number of ranks for data
 */
#define DATA_RANK 2

/**
 * Reads the dataset attributes from the hdf5 file
 */
int read_attributes(const hid_t dataset_id, DATASET *dataset);

/**
 * Reads the value of one attribute from the dataset
 */
herr_t read_attribute(hid_t dataset_id, const char *attribute, hid_t datatype,
		void *value);

/**
 * Writes an attribute to the dataset
 */
herr_t write_attribute(hid_t dataset_id, const char *attribute, hid_t datatype,
		const void *value);

/**
 * Reads chunk dimensions from dataset if chunking was enabled
 */
int get_chunk_dimensions(const hid_t dataset_id, hsize_t *chunk_dimensions);

/**
 * Returns the dataset dimensions stored in the hdf5 dataset
 */
void get_dataset_dimensions(hid_t dataset_id, hsize_t *dataset_dimensions);

/**
 * Calculates the dataset dimensions based on the number
 * of observations and attributes
 */
int calculate_dataset_dimensions(DATASET *dataset, hsize_t *dataset_dimensions);

#endif
