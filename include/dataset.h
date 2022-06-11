/*
 ============================================================================
 Name        : dataset.h
 Author      : Eduardo Ribeiro
 Description : Structures and functions to manage datasets
 ============================================================================
 */
#ifndef DATASET_H
#define DATASET_H

typedef struct dataset {
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

} DATASET;

#endif
