#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 

typedef struct HistoryVectorNode {
    int cnt;
    float* output_vector;
    struct HistoryVectorNode* prev;
    struct HistoryVectorNode* next;
    int max_prob_action;
} HistoryVectorNode;

typedef struct History_Vector_Elem_T {
    int cnt;
    float* output_vector;
} History_Vector_Elem_T;

typedef struct History_Vector_subarray_T {
    int total_labels;
    int start_cnt;
    int end_cnt;
    int* element_counter;
    HistoryVectorNode* start;
    HistoryVectorNode* end;
    int length;
} History_Vector_subarray_T;

typedef struct HistoryVectorList {
    int max_history_len;
    int history_len;
    int total_labels;
    int* label_count;
    HistoryVectorNode* head;
    HistoryVectorNode* tail;
    int first_cnt;
    int last_cnt;
} HistoryVectorList;



History_Vector_Elem_T get_midway_element_vector(History_Vector_Elem_T start, History_Vector_Elem_T end, int midway_cnt, int vector_len);


HistoryVectorList* init_vector_history_list(int label_num, int max_history_len);
int get_vector_history_len(HistoryVectorList* list);
void add_pred_to_vector_history(HistoryVectorList* list, History_Vector_Elem_T elem);
History_Vector_Elem_T pop_pred_from_vector_history(HistoryVectorList* list);
void clear_vector_history(HistoryVectorList* list);
void free_vector_history_list(HistoryVectorList* list);
void print_vector_history(HistoryVectorList* list);
bool check_vector_history_correctness(HistoryVectorList* list);
int* generate_last_N_vector_history_intarray(HistoryVectorList* list, int cnt_start, int cnt_end);
int get_last_vector_history_prediction(HistoryVectorList* list);
float* get_last_history_vector(HistoryVectorList* list);

//shifting subarray window
History_Vector_subarray_T* create_new_vector_subarray(int num_labels);
bool reset_vector_subarray(History_Vector_subarray_T* subarr, HistoryVectorNode* start_elem, HistoryVectorNode* end_elem);
bool shift_vector_subarr_end_to_right(History_Vector_subarray_T* subarr);
bool shift_vector_subarr_start_to_right(History_Vector_subarray_T* subarr);
bool shift_vector_subarr_whole_window_right(History_Vector_subarray_T* subarr);
void free_vector_subarray(History_Vector_subarray_T* subarr);
void print_vector_subarray(History_Vector_subarray_T* subarr);
bool vector_subarray_valid(History_Vector_subarray_T* subarr);
float* calculate_ema_output(History_Vector_subarray_T* subarr, float alpha);


//functional methods for label selection
int search_vector_majority(HistoryVectorList* list);


