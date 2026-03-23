/**
 * @file NN.h
 * @brief RTOS Neural Network library - declaration of Neural Network Types and functions 
 * @version 1.0
 * @date 2024
 * 
 * Definition of Neural Network Datatypes and structures on a light-weight RTOS hardware platform
 * The Neural Network is used to perform real-time action prediction using gyroscope and IMU data collected from a sensor
 * The file contains both 32 bit float implementation, quantized 16 bit int and 8 bit int implementation of the neural network
 * 
 * @author Neural Network Inference 
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include "matrix.h"
#include <string.h>
#include "historyLinkedList.h"
#include "historyListVector.h"


#ifndef _NN_H_
#define _NN_H_

/**
 * @brief Opaque neural network model structure
 * 
 * This structure contains all the parameters needed for neural network inference.
 * The internal implementation is hidden from the user to maintain encapsulation
 * and allow for future optimizations without breaking the API.
 */

/* Library version information */
#define NEURAL_FLOAT_VERSION_MAJOR 1
#define NEURAL_FLOAT_VERSION_MINOR 0
#define NEURAL_FLOAT_VERSION_PATCH 0
#define MAX_HISTORY_LEN 30
#define MAX_STATIONARY_HISTORY_LEN 1000

/*
 * ============================================================================
 * Definition of Neural Network Hyperparameters
 * ============================================================================
 */



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
 * @return the pointer to the address storing the neural network parameters
 */
void* load_empty_model(uint32_t num_layers, uint32_t input_dim, uint32_t hidden_dim, uint32_t output_dim);
void* load_empty_model_quantized(uint32_t num_layers, uint32_t input_dim, uint32_t hidden_dim, uint32_t output_dim);






/*
 * ============================================================================
 * Hyperparameter Structure
 * ============================================================================
 */                       
typedef struct NeuralNetworkInferenceContainer {
    void* predictor;
    uint32_t min_seqlen;
    uint32_t max_seqlen;
    uint32_t input_dim;
    uint32_t counter;
    int32_t latest_cnt;
    float* prev_payload; // the previous payload is used to store the previous pending input data
    float* accumulater; // the accumulater is used to store the input data for the neural network
    int32_t model_type; // the type of the current model, 0 if it's for cat, 1 if it's for dogs
} NeuralNetworkInferenceContainer;


typedef struct IntermediateOutput_external {
    float* tmp1;
    int32_t cnt;
    float* intermediate_vector1;
    float* intermediate_vector2;
    float* intermediate_vector3;
    float* intermediate_vector4;
    uint64_t final_output6;
}  IntermediateOutput_external;

NeuralNetworkInferenceContainer* init_container(FILE* model_file, bool log);

/**
 * @brief Perform neural network forward inference
 * 
 * This is the main inference function that processes an input sequence
 * through the neural network and returns the output predictions.
 * 
 * @param model Pointer to neural network model
 * @param input Input sequence matrix [time_steps, input_features]
 * @return Output the encoded (action value and timestamp) info in int64_t
 */
IntermediateOutput_external* NeuralNetwork_Container_forward(NeuralNetworkInferenceContainer* container, Matrix_float* input, uint32_t cnt, bool log, bool write_intermediate_output);


/**
 * @brief Free neural network model memory
 * 
 * Releases all memory allocated for a neural network model, including all weight
 * matrices, parameter vectors, and the model structure itself.
 * 
 * @param model Pointer to neural model to free
 */
// void free_model(NeuralNetwork *model);
// void free_model_quantized(NeuralNetworkQuant *model);
void free_container(NeuralNetworkInferenceContainer* container);


/*
 * ============================================================================
 * NEURAL NETWORK MODEL OPERATIONS New Api
 * ============================================================================
 */
/**
 * @brief Extract vector data from matrix
 * 
 * Creates a new float array containing all matrix elements in row-major order.
 * The caller is responsible for freeing the returned array.
 * 
 * @param mat Input matrix
 * @return -1 if the output is NULL, otherwise the encoded (action value and timestamp) info in int64_t
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
 * @brief Checks if the collar is static from the given timestamp cnt to the present
 * 
 * @param cnt The timestamp cnt to check if the collar is static
 * @return True if the collar is static, false otherwise
 */
bool check_stationary(NeuralNetworkInferenceContainer* container, uint32_t cnt);


/**
 * @brief Checks the version of the algorithm from the bin file provided
 * 
 * 
 * @param container is the container that contains the model parameters
 * @returns the version number of the file, as an uint32
 */

uint32_t obtain_model_version_num(NeuralNetworkInferenceContainer* container);


/**
 * @brief Free the intermediate output from the model
 * 
 * 
 * @param intermediate output contains the vectorized output from the model through each step of postprocessing
 * @returns None
 */
void free_intermediate_output(IntermediateOutput_external* output);


/**
 * @brief returns the firmware 
 * 
 * 
 * @param firmware version is the version of the model inference framework, should be compatible with the model
 * @returns the version number of the firmware calculation library
 */
float get_calculation_lib_version(void);

#endif