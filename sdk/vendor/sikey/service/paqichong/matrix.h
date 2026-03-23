/**
 * @file matrix.h
 * @brief RTOS machine learning library - Implementation of matrix operations
 * @version 1.0
 * @date 2024
 * 
 * Definition of different matrix on a light-weight RTOS hardware platform
 * The file includes matrix tensor operators of different datatypes (32 bit float, 16 bit int and 8 bit int) implemented in a Naive manner, 
 * 
 * @author Neural Network Inference Team
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#ifndef _MATRIX_H_
#define _MATRIX_H_

/**
 * @brief Matrix structure for storing 2D floating-point data
 * 
 * This structure represents a 2D matrix with row-major storage layout.
 * The data is stored contiguously in memory for efficient access.
 */
typedef struct Matrix_float{
    float *data;    /**< Pointer to matrix data (row-major) */
    uint32_t size1;       /**< Number of rows */
    uint32_t size2;       /**< Number of columns */
} Matrix_float;


typedef struct Matrix_int8{
    int8_t *data;    /**< Pointer to matrix data (row-major) */
    uint32_t size1;       /**< Number of rows */
    uint32_t size2;       /**< Number of columns */
} Matrix_int8;


typedef struct Matrix_int32{
    int32_t *data;
    uint32_t size1;
    uint32_t size2;
} Matrix_int32;







/*
 * ============================================================================
 * MATRIX OPERATIONS API FLOAT
 * ============================================================================
 */

/**
 * @brief Read Matrix from a file pointer 
 * @param rows Number of rows
 * @param cols Number of columns
 * @return Pointer to allocated matrix, or NULL on failure
 */
Matrix_float *read_tensor_from_file(FILE* file, int32_t rows, int32_t cols);

/**
 * @brief Allocate a new matrix with uninitialized data
 * @param rows Number of rows
 * @param cols Number of columns
 * @return Pointer to allocated matrix, or NULL on failure
 */
Matrix_float *matrix_alloc(uint32_t rows, uint32_t cols);

/**
 * @brief Allocate a new matrix with zero-initialized data
 * @param rows Number of rows  
 * @param cols Number of columns
 * @return Pointer to allocated matrix, or NULL on failure
 */
Matrix_float *matrix_calloc(uint32_t rows, uint32_t cols);

/**
 * @brief Free matrix memory
 * @param mat Pointer to matrix to free
 */
void matrix_free(Matrix_float *mat);

/**
 * @brief Set a matrix element value
 * @param mat Pointer to matrix
 * @param i Row index (0-based)
 * @param j Column index (0-based)
 * @param val Value to set
 */
void matrix_set(Matrix_float *mat, uint32_t i, uint32_t j, float val);

/**
 * @brief Get a matrix element value
 * @param mat Pointer to matrix
 * @param i Row index (0-based)
 * @param j Column index (0-based)
 * @return Matrix element value
 */
float matrix_get(Matrix_float *mat, uint32_t i, uint32_t j);
float matrix_get_float(Matrix_float *mat, uint32_t row, uint32_t col);
/**
 * @brief Multiply two matrices
 * @param A First matrix
 * @param B Second matrix
 * @return Result matrix, or NULL on failure
 */
Matrix_float *matrix_mult(Matrix_float *A, Matrix_float *B);

/**
 * @brief Add two matrices
 * @param A First matrix
 * @param B Second matrix
 * @return Result matrix, or NULL on failure
 */
Matrix_float *matrix_sum(Matrix_float *A, Matrix_float *B);




/**
 * @brief element wise multiplication of two matrices of the same shape
 * @param A First matrix
 * @param B Second matrix
 */
Matrix_float *matrix_hadamard(Matrix_float *A, Matrix_float *B);


/**
 * @brief Append Matrix B to Matrix A and store it in Matrix A
 * @param A the target matrix to append to 
 * @param B the matrix we want to append 
 * @return true if the Matrix B could be appended to Matrix A and false otherwise
 */
// bool matrix_append(Matrix_float* A, Matrix_float* B);


/**
 * @brief Fill in random numbers in a matrix
 * @param mat the matrix to be filled, which should not be NULL
 */
void fill_matrix_random(Matrix_float *mat);

/**
 * @brief Fill in all zero in a matrix
 * @param mat the matrix to be filled, @param mat the matrix to be filled, which should not be NULL
 */
void fill_matrix_zero(Matrix_float *mat);
/*
 * ============================================================================
 * MATRIX OPERATIONS API INT32_T
 * ============================================================================
 */

/**
 * @brief Allocate a new matrix with uninitialized data
 * @param rows Number of rows
 * @param cols Number of columns
 * @return Pointer to allocated matrix, or NULL on failure
 */
Matrix_int8 *matrix_alloc_int8(uint32_t rows, uint32_t cols);

/**
 * @brief Allocate a new matrix with uninitialized data
 * @param mat Pointer to matrix to free
 */
void matrix_free_int8(Matrix_int8* mat);


/**
 * @brief Print matrix with float elements
 * @param mat Point to the matrix to be printed
 */
void print_matrix(Matrix_float *m);

/*
 * ============================================================================
 * MATRIX OPERATIONS API INT8_T
 * ============================================================================
 */

/**
 * @brief Allocate a new matrix with uninitialized data
 * @param rows Number of rows
 * @param cols Number of columns
 * @return Pointer to allocated matrix, or NULL on failure
 */
Matrix_int32 *matrix_alloc_int32(uint32_t rows, uint32_t cols);

/**
 * @brief Allocate a new matrix with uninitialized data
 * @param mat Pointer to matrix to free
 */
void matrix_free_int32(Matrix_int32* mat);


#endif