/**
 * @file neural_float.h
 * @brief Neural Float Library - Lightweight C library for neural network inference
 * @version 1.0
 * @date 2024
 * 
 * A fast, memory-efficient neural network implementation for inference using single-precision
 * floating-point operations. Supports deep recurrent neural networks with custom matrix
 * operations and model I/O capabilities.
 * 
 * @author Neural Float Library Team
 */

#ifndef _SAMPLE_A_H_
#define _SAMPLE_A_H_


/* Standard library includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

/* Library version information */
#define NEURAL_FLOAT_VERSION_MAJOR 1
#define NEURAL_FLOAT_VERSION_MINOR 0
#define NEURAL_FLOAT_VERSION_PATCH 0


/**
 * @brief Matrix structure for storing 2D floating-point data
 * 
 * This structure represents a 2D matrix with row-major storage layout.
 * The data is stored contiguously in memory for efficient access.
 */
typedef struct {
    float *data;    /**< Pointer to matrix data (row-major) */
    uint32_t size1;       /**< Number of rows */
    uint32_t size2;       /**< Number of columns */
} Matrix_float;

/**
 * @brief Opaque neural network model structure
 * 
 * This structure contains all the parameters needed for neural network inference.
 * The internal implementation is hidden from the user to maintain encapsulation
 * and allow for future optimizations without breaking the API.
 */
// 
// Define the actual neural network structure here (hidden from header)

typedef struct {
    Matrix_float **hs;
    Matrix_float **cs;
} LayerState_float;

typedef struct NeuralModel {
    int32_t num_layers;
    int32_t input_dim;
    int32_t hidden_dim;
    int32_t output_dim;

    Matrix_float **wiis;
    Matrix_float **wifs;
    Matrix_float **wigs;
    Matrix_float **wios;
    Matrix_float **biis;
    Matrix_float **bifs;
    Matrix_float **bigs;
    Matrix_float **bios;
    
    Matrix_float **whis;
    Matrix_float **whfs;
    Matrix_float **whgs;
    Matrix_float **whos;
    Matrix_float **bhis;
    Matrix_float **bhfs;
    Matrix_float **bhgs;
    Matrix_float **bhos;
    
    Matrix_float *w_proj;
    Matrix_float *b_proj;
} NeuralModel;



/*
 * ============================================================================
 * MATRIX OPERATIONS API
 * ============================================================================
 */

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

/*
 * ============================================================================
 * NEURAL NETWORK MODEL OPERATIONS API
 * ============================================================================
 */

/**
 * @brief Initialize Empty Model
 * 
 * Loads a neural network model from a directory containing model weight files.
 * The exact file format and structure is implementation-specific.
 *
 * @param num_layers Number of network layers
 * @param input_dim Input feature dimension
 * @param hidden_dim Hidden layer dimension  
 * @param output_dim Output dimension
 * @return Pointer to loaded neural model, or NULL on failure
 */
NeuralModel* load_empty_model(int32_t num_layers, int32_t input_dim, int32_t hidden_dim, int32_t output_dim);

/**
 * @brief Perform neural network forward inference
 * 
 * This is the main inference function that processes an input sequence
 * through the neural network and returns the output predictions.
 * 
 * @param model Pointer to neural network model
 * @param input Input sequence matrix [time_steps, input_features]
 * @return Output predictions matrix [output_classes, 1], or NULL on failure
 */
Matrix_float *model_inference(NeuralModel *model, Matrix_float *input);

/**
 * @brief Load neural network model from directory of binary files
 * 
 * Loads a neural network model from a directory containing model weight files.
 * The exact file format and structure is implementation-specific.
 * 
 * @param model_path Path to directory containing model files
 * @param num_layers Number of network layers
 * @param input_dim Input feature dimension
 * @param hidden_dim Hidden layer dimension  
 * @param output_dim Output dimension
 * @return Pointer to loaded neural model, or NULL on failure
 */
NeuralModel *load_model_from_directory(char* model_path, int32_t num_layers, int32_t input_dim, 
                                      int32_t hidden_dim, int32_t output_dim);

/**
 * @brief Load neural network model from a single binary file (with manual hyperparameters)
 * 
 * Loads a neural network model from a single binary file containing all weights
 * and parameters in a compact format. Requires manual specification of hyperparameters.
 * 
 * @param file Open file pointer positioned at start of model data
 * @param num_layers Number of network layers
 * @param input_dim Input feature dimension
 * @param hidden_dim Hidden layer dimension
 * @param output_dim Output dimension
 * @return Pointer to loaded neural model, or NULL on failure
 */
NeuralModel* load_model_from_file(FILE* file, int32_t num_layers, 
                                 int32_t input_dim, int32_t hidden_dim, int32_t output_dim);

/**
 * @brief Load neural network model from a single binary file (with embedded hyperparameters)
 * 
 * Loads a neural network model from a single binary file that contains both
 * hyperparameters and model weights. The hyperparameters are automatically
 * read from the file header.
 * 
 * @param file Open file pointer positioned at start of model data
 * @return Pointer to loaded neural model, or NULL on failure
 */
NeuralModel* load_model_from_file_with_params(FILE* file);

/**
 * @brief Load neural network model from file path (with embedded hyperparameters)
 * 
 * Convenience function that opens a file and loads a neural network model
 * with embedded hyperparameters.
 * 
 * @param filepath Path to the model binary file
 * @return Pointer to loaded neural model, or NULL on failure
 */
NeuralModel* load_model_from_filepath(const char* filepath);

/**
 * @brief Save neural network model to a single binary file (without hyperparameters)
 * 
 * Saves all model weights and parameters to a single binary file in a compact format
 * that can be loaded later using load_model_from_file(). Does not include hyperparameters.
 * 
 * @param model Pointer to neural model to save
 * @param file Open file pointer for writing
 */
void save_model_to_file(NeuralModel* model, FILE* file);

/**
 * @brief Save neural network model to a single binary file (with embedded hyperparameters)
 * 
 * Saves all model weights, parameters, and hyperparameters to a single binary file
 * in a compact format that can be loaded later using load_model_from_file_with_params().
 * 
 * @param model Pointer to neural model to save
 * @param file Open file pointer for writing
 */
void save_model_to_file_with_params(NeuralModel* model, FILE* file);

/**
 * @brief Compare two neural network models for equality
 * 
 * Performs element-wise comparison of all weights and parameters to determine
 * if two neural models are identical.
 * 
 * @param model1 First neural model
 * @param model2 Second neural model  
 * @return true if models are identical, false otherwise
 */
bool compare_models(NeuralModel* model1, NeuralModel* model2);

/**
 * @brief Free neural network model memory
 * 
 * Releases all memory allocated for a neural network model, including all weight
 * matrices, parameter vectors, and the model structure itself.
 * 
 * @param model Pointer to neural model to free
 */
void free_model(NeuralModel *model);

/**
 * @brief Get model information
 * 
 * Retrieves basic information about the loaded model dimensions.
 * 
 * @param model Pointer to neural model
 * @param num_layers Pointer to store number of layers (can be NULL)
 * @param input_dim Pointer to store input dimension (can be NULL)
 * @param hidden_dim Pointer to store hidden dimension (can be NULL)
 * @param output_dim Pointer to store output dimension (can be NULL)
 * @return 0 on success, -1 on error
 */
int32_t get_model_info(NeuralModel *model, int32_t *num_layers, int32_t *input_dim, 
                   int32_t *hidden_dim, int32_t *output_dim);

/*
 * ============================================================================
 * UTILITY FUNCTIONS API
 * ============================================================================
 */

/**
 * @brief Extract vector data from matrix
 * 
 * Creates a new float array containing all matrix elements in row-major order.
 * The caller is responsible for freeing the returned array.
 * 
 * @param mat Input matrix
 * @return Pointer to float array, or NULL on failure
 */
float* get_vector_output(Matrix_float *mat);

/**
 * @brief Get predicted class label from output matrix
 * 
 * Finds the index of the maximum value in the output matrix, which
 * corresponds to the predicted class in classification tasks.
 * 
 * @param predictions Output matrix from neural network inference
 * @return Index of predicted class, or -1 on error
 */
int32_t get_prediction_label(Matrix_float* predictions);

/**
 * @brief Print matrix contents to stdout
 * 
 * Prints a formatted representation of the matrix for debugging purposes.
 * 
 * @param m Matrix to print
 */
void print_matrix(Matrix_float* m);


#endif /* NEURAL_FLOAT_H */ 