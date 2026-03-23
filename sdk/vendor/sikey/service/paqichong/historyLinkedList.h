#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 

typedef struct HistoryNode {
    int cnt;
    int val;
    struct HistoryNode* prev;
    struct HistoryNode* next;
} HistoryNode;

typedef struct History_Elem_T {
    int cnt;
    int val;
} History_Elem_T;

typedef struct History_Elem_subarray_T {
    int total_labels;
    int start_cnt;
    int end_cnt;
    int* element_counter;
    HistoryNode* start;
    HistoryNode* end;
    int length;
} History_Elem_subarray_T;

typedef struct HistoryList {
    int max_history_len;
    int history_len;
    int total_labels;
    int* label_count;
    HistoryNode* head;
    HistoryNode* tail;
    int first_cnt;
    int last_cnt;
} HistoryList;

HistoryList* init_history_list(int label_num, int max_history_len);
int get_history_len(HistoryList* list);
void add_pred_to_history(HistoryList* list, History_Elem_T elem);
History_Elem_T pop_pred_from_history(HistoryList* list);
void clear_history(HistoryList* list);
void free_history_list(HistoryList* list);
void print_history(HistoryList* list);
bool check_history_correctness(HistoryList* list);
int* generate_last_N_history_intarray(HistoryList* list, int cnt_start, int cnt_end);
int get_last_history_elem(HistoryList* list);

//shifting subarray window
History_Elem_subarray_T* create_new_subarray(int num_labels);
bool reset_subarray(History_Elem_subarray_T* subarr, HistoryNode* start_elem, HistoryNode* end_elem);
bool shift_end_to_right(History_Elem_subarray_T* subarr);
bool shift_start_to_right(History_Elem_subarray_T* subarr);
bool shift_whole_window_right(History_Elem_subarray_T* subarr);
void free_subarray(History_Elem_subarray_T* subarr);
//functional methods for label selection
int search_majority(HistoryList* list);


