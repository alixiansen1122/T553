#include "alipay_common.h"
#include "vendor_os.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

uint32_t alipay_get_timestamp(void) __attribute__ ((weak));
void alipay_log_ext(const char *format, ...) __attribute__ ((weak));
uint32_t alipay_get_compile_timestamp() __attribute__ ((weak));
void alipay_free(void* pt) __attribute__ ((weak));
void* alipay_malloc(uint32_t size) __attribute__ ((weak));
void* alipay_realloc(void* pt, uint32_t size) __attribute__ ((weak));
void* alipay_calloc(uint32_t nblock,uint32_t size) __attribute__ ((weak));


static uint32_t temp_system_timestamp = 0;
static uint32_t temp_time_set_flag = 0;
uint32_t alipay_get_timestamp(void) {
    #warning alipay_get_timestamp demo

    if((temp_time_set_flag == 0) || (temp_system_timestamp == 0)){
        struct timespec t;
        t.tv_sec = t.tv_nsec = 0;
        clock_gettime(CLOCK_REALTIME, &t);

        long long v = (t.tv_sec) * 1000 + t.tv_nsec / 1000000;

        uint32_t tm_ms = v / 1000;

        return tm_ms;
    } else {
        temp_time_set_flag = 0;
        return temp_system_timestamp+1;
    }
}

/*设置手表时间，设置之后要求及时生效，并且通过 alipay_get_timestamp 接口获取的是修改后的时间
 * 注:UTC时间戳，从1970开始的
 * @param [in] timestamp_s  时间戳，单位为s
 * @return 
 */
EXTERNC void alipay_set_system_time(PARAM_IN int32_t timestamp_s){
    #warning alipay_set_system_time demo
    temp_time_set_flag = 1;
    temp_system_timestamp = timestamp_s;
}

/*获取编译时候的时间戳，单位为秒
 * @return 编译时候的时间戳，单位为秒
 */
#define dec(ch) ((ch)-'0')
#define t(index, multiplier)    (dec(__TIME__[index]) * (multiplier))
/* only minutes and seconds - you get the idea */
#define mmss()  (t(3,600) + t(4,60) + t(6,10) + t(7,1))

// extracts 1..4 characters from a string and interprets it as a decimal value
#define CONV_STR2DEC_1(str, i)  (str[i]>'0'?str[i]-'0':0)
#define CONV_STR2DEC_2(str, i)  (CONV_STR2DEC_1(str, i)*10 + str[i+1]-'0')
#define CONV_STR2DEC_3(str, i)  (CONV_STR2DEC_2(str, i)*10 + str[i+2]-'0')
#define CONV_STR2DEC_4(str, i)  (CONV_STR2DEC_3(str, i)*10 + str[i+3]-'0')
// Some definitions for calculation
#define SEC_PER_MIN             60UL
#define SEC_PER_HOUR            3600UL
#define SEC_PER_DAY             86400UL
#define SEC_PER_YEAR            (SEC_PER_DAY*365)
#define UNIX_START_YEAR         1970UL
// Custom "glue logic" to convert the month name to a usable number
#define GET_MONTH(str, i)      (str[i]=='J' && str[i+1]=='a' && str[i+2]=='n' ? 1 : \
                                str[i]=='F' && str[i+1]=='e' && str[i+2]=='b' ? 2 : \
                                str[i]=='M' && str[i+1]=='a' && str[i+2]=='r' ? 3 : \
                                str[i]=='A' && str[i+1]=='p' && str[i+2]=='r' ? 4 : \
                                str[i]=='M' && str[i+1]=='a' && str[i+2]=='y' ? 5 : \
                                str[i]=='J' && str[i+1]=='u' && str[i+2]=='n' ? 6 : \
                                str[i]=='J' && str[i+1]=='u' && str[i+2]=='l' ? 7 : \
                                str[i]=='A' && str[i+1]=='u' && str[i+2]=='g' ? 8 : \
                                str[i]=='S' && str[i+1]=='e' && str[i+2]=='p' ? 9 : \
                                str[i]=='O' && str[i+1]=='c' && str[i+2]=='t' ? 10 : \
                                str[i]=='N' && str[i+1]=='o' && str[i+2]=='v' ? 11 : \
                                str[i]=='D' && str[i+1]=='e' && str[i+2]=='c' ? 12 : 0)

#define GET_MONTH2DAYS(month)  ((month == 1 ? 0 : 31 +                  \
                                 (month == 2 ? 0 : 28 +                 \
                                  (month == 3 ? 0 : 31 +                \
                                   (month == 4 ? 0 : 30 +               \
                                    (month == 5 ? 0 : 31 +              \
                                     (month == 6 ? 0 : 30 +             \
                                      (month == 7 ? 0 : 31 +            \
                                       (month == 8 ? 0 : 31 +           \
                                        (month == 9 ? 0 : 30 +          \
                                         (month == 10 ? 0 : 31 +        \
                                          (month == 11 ? 0 : 30)))))))))))) \


#define GET_LEAP_DAYS           ((__TIME_YEARS__-1968)/4 - (__TIME_MONTH__ <=2 ? 1 : 0))

#define __TIME_SECONDS__        CONV_STR2DEC_2(__TIME__, 6)
#define __TIME_MINUTES__        CONV_STR2DEC_2(__TIME__, 3)
#define __TIME_HOURS__          CONV_STR2DEC_2(__TIME__, 0)
#define __TIME_DAYS__           CONV_STR2DEC_2(__DATE__, 4)
#define __TIME_MONTH__          GET_MONTH(__DATE__, 0)
#define __TIME_YEARS__          CONV_STR2DEC_4(__DATE__, 7)

#define __TIME_UNIX__         ((__TIME_YEARS__-UNIX_START_YEAR)*SEC_PER_YEAR+ \
                               GET_LEAP_DAYS*SEC_PER_DAY+               \
                               GET_MONTH2DAYS(__TIME_MONTH__)*SEC_PER_DAY+ \
                               __TIME_DAYS__*SEC_PER_DAY-SEC_PER_DAY+   \
                               __TIME_HOURS__*SEC_PER_HOUR+             \
                               __TIME_MINUTES__*SEC_PER_MIN+            \
                               __TIME_SECONDS__)

uint32_t alipay_get_compile_timestamp() {
    #warning alipay_get_compile_timestamp demo
    return __TIME_UNIX__ - (8 * 60 * 60);
}

/*
 *功能同malloc函数，分配成功之后做清零操作
 */
void* alipay_malloc(uint32_t size){
    #warning alipay_malloc demo
    return malloc(size);
}

/*
 *功能同calloc函数，分配成功之后做清零操作
 */
void* alipay_calloc(uint32_t nblock,uint32_t size){
    #warning alipay_calloc demo
    return calloc(nblock, size);
}

/*
 *功能同free函数
 */
void alipay_free(void* pt){
    #warning alipay_free demo
    return free(pt);
}

void* alipay_realloc(void* pt, uint32_t size){
    #warning alipay_realloc demo
    return realloc(pt, size);
}

/**
* 获取一个非0的随机数
*/
int alipay_rand(void){
    #warning alipay_rand demo
    int ret_val = 0;
    uint32_t retry_time = 20;
    do{
        ret_val = rand();
        retry_time--;
    }while((ret_val == 0) && (retry_time > 0));
    return ret_val;
}

/**
* 设定随机数种子
*/
void alipay_srand(unsigned int seed){
    #warning alipay_srand demo
    srand(seed);
}


/////////////////////////////////////////////
////日志信息输出接口
/////////////////////////////////////////////
#ifndef ALIPAY_MACRO_IGNORE_HAL_IMPL
void alipay_log_ext(const char *format, ...) {
    #warning alipay_log_ext demo
#define LOG_MAX_SIZE    (1024*1)
    static char log_buffer[LOG_MAX_SIZE];
    memset(log_buffer, 0, LOG_MAX_SIZE);
    va_list args;
    va_start(args, format);
    vsnprintf(log_buffer, LOG_MAX_SIZE, format, args);
    printf("[VENDOR_OS]%s\n", log_buffer);
    va_end(args);
}
#endif