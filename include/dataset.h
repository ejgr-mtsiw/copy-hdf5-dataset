/*
 ============================================================================
 Name        : dataset.h
 Author      : Eduardo Ribeiro
 Description : Structures and functions to manage datasets
 ============================================================================
 */
#ifndef DATASET_H
#define DATASET_H

#define DATASET_OK 0

#define DATASET_INVALID_DIMENSIONS 1
#define DATASET_NOT_ENOUGH_CLASSES 2
#define DATASET_NOT_ENOUGH_ATTRIBUTES 4
#define DATASET_NOT_ENOUGH_OBSERVATIONS 8
#define DATASET_ERROR_ALLOCATING_DATA 16

typedef struct dataset_t {
	/**
	 * Number of attributes
	 */
	unsigned int n_attributes;

	/**
	 * Number of jnsqs (because original python script adds space for them
	 * before writing the class, we need to know how many bytes to skip)
	 */
	unsigned int n_jnsqs;

	/**
	 * Number of observations
	 */
	unsigned int n_observations;

	/**
	 * Number of classes
	 */
	unsigned int n_classes;

	/**
	 * Number of bits used to store the class
	 */
	unsigned int n_bits_for_class;

} dataset_t;

#endif
