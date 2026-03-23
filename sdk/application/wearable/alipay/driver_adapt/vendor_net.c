/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: alipay vendor net adapt.
 * Author:
 * Create:
 */

#include "alipay_common.h"
#include "alipay_net_kal.h"
#include "time.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include "vendor_net.h"
#include "securec.h"

#ifdef SUPPORT_OHOSFWK
#include "alipay_app.h"
#endif

void alipay_iot_gettimeofday(alipay_iot_timeval *now)
{
    now->tv_sec = time(NULL);
    now->tv_usec = 0;
}

void alipay_iot_get_local_time(alipay_iot_local_time *ltime)
{
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    ltime->year = timeinfo->tm_year + 1900;
    ltime->month = timeinfo->tm_mon + 1;
    ltime->day = timeinfo->tm_mday;
    ltime->hour = timeinfo->tm_hour;
    ltime->minute = timeinfo->tm_min;
    ltime->second = timeinfo->tm_sec;
    ltime->isdst = 0;
}

void alipay_task_sleep(uint32_t ms)
{
    osDelay(ms);
}

#ifdef SUPPORT_OHOSFWK
#define DEFAULT_SOCKET_FD 1
int alipay_iot_dns(const char *name, unsigned char ip[4])
{
    int ret;
    if (!msg_center_alipay_get_network_status()) {
        return -1;
    }
    ret = msg_center_alipay_hostent_request((void*)name, strlen(name));
    ret |= msg_center_alipay_hostent_reponse(ip);
    return ret;
}

int alipay_iot_socket_create(int domain, alipay_socket_type_enum type, int protocol)
{
    unused(domain);
    unused(type);
    unused(protocol);
    if (!msg_center_alipay_get_network_status()) {
        return -1;
    }
    int fd = DEFAULT_SOCKET_FD;
    return fd;
}

int alipay_iot_socket_connect(int s, const struct alipay_iot_sockaddr *name,
                              unsigned int namelen)
{
    unused(s);
    unused(namelen);

    int ret;
    alipay_connect_request request;

    request.dsp_port = name->data.sin_data.port;
    memcpy(request.dst_ip, name->data.sin_data.ip, 4);

    if (!msg_center_alipay_get_network_status()) {
        return -1;
    }
    ret = msg_center_alipay_connect_request(&request, sizeof(alipay_connect_request));
    ret |= msg_center_alipay_connect_reponse();
    return ret;
}

int alipay_iot_socket_write(int s, const void *dataptr, int len)
{
    unused(s);

    int ret;
    if (!msg_center_alipay_get_network_status()) {
        return -1;
    }
    ret = msg_center_alipay_tcpdata_request((void *)dataptr, len);
    return ret;
}

int alipay_iot_fd_isset(int fd, alipay_iot_fd_set* fdset)
{
    if (fdset->fds_bits[0] == fd) {
        return 1;
    } else {
        return 0;
    }
}

void alipay_iot_fd_setbit(int fd, alipay_iot_fd_set* fdset)
{
    fdset->fds_bits[0] = fd;
    return;
}

void alipay_iot_fd_zero(alipay_iot_fd_set* fdset)
{
    (void)memset_s(fdset, sizeof(alipay_iot_fd_set), 0, sizeof(alipay_iot_fd_set));
    return;
}

int alipay_iot_select(int maxfdp1,
                      alipay_iot_fd_set* readset,
                      alipay_iot_fd_set* writeset,
                      alipay_iot_fd_set* exceptset,
                      alipay_iot_timeval* timeout)
{
    unused(maxfdp1);
    unused(timeout);

    int ret;
    if (!msg_center_alipay_get_network_status()) {
        return -1;
    }
    if (readset->fds_bits[0] == DEFAULT_SOCKET_FD) {
        ret = msg_center_alipay_tcpdata_wait(timeout->tv_sec * 1000 + timeout->tv_usec / 1000);
        if (ret == 0) {
            readset->fds_bits[0] = 0;
        } else {
            readset->fds_bits[0] = DEFAULT_SOCKET_FD;
        }        
    } else {
        readset->fds_bits[0] = 0;
    }

    writeset->fds_bits[0] = DEFAULT_SOCKET_FD;
    exceptset->fds_bits[0] = 0;

    return 1;
}

int alipay_iot_socket_read(int s, void *mem, int len)
{
    unused(s);
    int ret;
    if (!msg_center_alipay_get_network_status()) {
        return -1;
    }
    ret = msg_center_alipay_tcpdata_read(mem, len);
    return ret;
}

int alipay_iot_socket_close(int s)
{
    unused(s);

    int ret;
    ret = msg_center_alipay_close_request();
    return ret;
}
#else
int alipay_iot_dns(const char *name, unsigned char ip[4])
{
    unused(name);
    unused(ip);
    return -1;
}

int alipay_iot_socket_create(int domain, alipay_socket_type_enum type, int protocol)
{
    unused(domain);
    unused(type);
    unused(protocol);

    return -1;
}

int alipay_iot_socket_connect(int s, const struct alipay_iot_sockaddr *name,
                              unsigned int namelen)
{
    unused(s);
    unused(name);
    unused(namelen);
    return -1;
}

int alipay_iot_socket_write(int s, const void *dataptr, int len)
{
    unused(s);
    unused(dataptr);
    unused(len);
    return -1;
}

int alipay_iot_fd_isset(int fd, alipay_iot_fd_set* fdset)
{
    unused(fd);
    unused(fdset);
    return -1;
}

void alipay_iot_fd_setbit(int fd, alipay_iot_fd_set* fdset)
{
    unused(fd);
    unused(fdset);
    return;
}

void alipay_iot_fd_zero(alipay_iot_fd_set* fdset)
{
    unused(fdset);
    return;
}

int alipay_iot_select(int maxfdp1,
                      alipay_iot_fd_set* readset,
                      alipay_iot_fd_set* writeset,
                      alipay_iot_fd_set* exceptset,
                      alipay_iot_timeval* timeout)
{
    unused(maxfdp1);
    unused(readset);
    unused(writeset);
    unused(exceptset);
    unused(timeout);
    return -1;
}

int alipay_iot_socket_read(int s, void *mem, int len)
{
    unused(s);
    unused(mem);
    unused(len);
    return -1;
}

int alipay_iot_socket_close(int s)
{
    unused(s);
    return -1;
}
#endif

int alipay_iot_socket_bind(int s,
                           const struct alipay_iot_sockaddr *name,
                           unsigned int namelen)
{
    unused(s);
    unused(name);
    unused(namelen);
    return -1;
}

int alipay_iot_socket_sendto(int s,
                             const void *dataptr,
                             int size,
                             int flags,
                             const struct alipay_iot_sockaddr *to,
                             unsigned int tolen)
{
    unused(s);
    unused(dataptr);
    unused(size);
    unused(flags);
    unused(to);
    unused(tolen);
    return -1;
}

int alipay_iot_socket_recvfrom(int s,
                               void *mem,
                               int len,
                               int flags,
                               struct alipay_iot_sockaddr *from,
                               unsigned int *fromlen)
{
    unused(s);
    unused(mem);
    unused(len);
    unused(flags);
    unused(from);
    unused(fromlen);
    return -1;
}

int alipay_iot_socket_setsockopt(int s,
                                 int level,
                                 int optname,
                                 const void *opval,
                                 unsigned int optlen)
{
    unused(s);
    unused(level);
    unused(optname);
    unused(opval);
    unused(optlen);
    return 0;
}

int alipay_iot_socket_getsockopt(int s,
                                 int level,
                                 int optname,
                                 void *opval,
                                 unsigned int *optlen)
{
    unused(s);
    unused(level);
    unused(optname);
    unused(opval);
    unused(optlen);
    return 0;
}