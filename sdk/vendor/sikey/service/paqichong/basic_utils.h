/**
 * @file utils.h
 * @brief RTOS utils library - declaration of basic memory functions
 * @version 1.0
 * @date 2024
 * 
 * Redeclaration of elementary operations such as dymanic Memory allocation, binary file reading functions
 * @author 
 */


/* Standard library includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>


#ifndef _BASIC_UTILS_H_
#define _BASIC_UTILS_H_
/*
 * ============================================================================
 *  BASIC OPERATIONS Redefinition of memory management functions for the RTOS 
 * ============================================================================
 *
 * The function bellow contains declaration for the dynamic memory management methods in the RTOS, which includes functions
 * such as Malloc, Calloc, and Free.
*/

/**
 * @brief Implementation of dynamic memory allocation malloc function 
 * @param size is the size of the allocated space in the heap
 * @return Pointer to allocated address
 */
void* malloc(size_t size);

/**
 * @brief Implementation of dynamic memory allocation function
 * @brief The function allocates memory and initializes all bits to zero.
 * @param n is the number of elements in the allocated sequence
 * @param size is the size of each element in the allocated sequence
 * @return Pointer to allocated address
 */
void* calloc(size_t n, size_t size);

/**
 * @brief Implementation of free function
 * @brief The function frees allocated pointer space in C
 * @param ptr is the pointer to the allocated memory space that should be freed
 * @returns NULL
 */
void free(void* ptr);


#endif