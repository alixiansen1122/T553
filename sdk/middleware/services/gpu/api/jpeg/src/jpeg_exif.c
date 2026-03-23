/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg exif
 */

#include "jpeg_exif.h"
#include "securec.h"
#include "jpeg_stream.h"
#include "jpeg_mem.h"

/*-------------------------------------- macro define -------------------------------------*/

#define JPEG_EXIF_1BYTES            1
#define JPEG_EXIF_2BYTES            2
#define JPEG_EXIF_4BYTES            4
#define JPEG_EXIF_JPEG              6
#define JPEG_EXIF_TAG_SIZE          6
#define JPEG_EXIF_TIME_STR_LENGTH   8
#define JPEG_EXIF_DATE_STR_LENGTH   10
#define JPEG_EXIF_ENTRY_LENGTH      12
#define JPEG_EXIF_MIN_SIZE          18
#define JPEG_EXIF_ENTRY_DATA_SIZE   20

#define JPEG_EXIF_ENDIAN_LITTLE          0x4949
#define JPEG_EXIF_ENDIAN_BIG             0x4D4D
#define JPEG_EXIF_IMAGE_TITLE            0x010E
#define JPEG_EXIF_DATE_TIME              0x0132
#define JPEG_EXIF_SUB_IFD                0x8769
#define JPEG_EXIF_CUSTOM_RENDERED        0xA401
#define JPEG_EXIF_IMAGE_COMMPRESS_TYPE   0x0103
#define JPEG_EXIF_THUMBNAIL_OFFSET       0x0201
#define JPEG_EXIF_THUMBNAIL_SIZE         0x0202

/*-------------------------------------- struct info --------------------------------------*/

typedef struct {
    td_char *exif_buf;
    td_u32 exif_size;      /* exif data size */
    td_char *next_buf;
    td_u32 size_in_buf;
    td_bool is_big_endian; /* big or little endian */
    td_bool is_jpeg_compress;
    td_u32 ifd1_offset;
    td_char entry_data[JPEG_EXIF_ENTRY_DATA_SIZE];
} jpeg_exif_entry;

/*-------------------------------------- func define --------------------------------------*/

static td_void jpeg_exif_init_endian(jpeg_exif_entry *entry);
static td_bool jpeg_exif_exist(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry);
static td_bool jpeg_exif_get_data(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry);
static td_void jpeg_exif_get_endian(jpeg_exif_entry *entry);
static td_bool jpeg_exif_seek_to_ifd0_addr(jpeg_exif_entry *entry);
static td_bool jpeg_exif_seek_to_ifd1_addr(jpeg_exif_entry *entry);
static td_bool jpeg_exif_read_ifd_tree(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry);
static td_bool jpeg_exif_dispose_ifd_tree(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry, td_u16 mark);
static td_bool jpeg_exif_get_compress_type(jpeg_exif_entry *entry);
static td_bool jpeg_exif_get_thumbnail_buffer(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry);
static td_bool jpeg_exif_get_thumbnail_size(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry);
static td_bool jpeg_exif_get_thumbnail_title(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry);
static td_bool jpeg_exif_get_thumbnail_render(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry);
static td_bool jpeg_exif_get_thumbnail_time(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry);
static td_bool jpeg_exif_seek_to_subifd_addr(jpeg_exif_entry *entry);
static td_bool jpeg_exif_read_entry_info(jpeg_exif_entry *entry);
static td_bool jpeg_exif_write_entry_value(jpeg_exif_entry *entry);
static td_char jpeg_exif_read_one_byte(jpeg_exif_entry *entry);
static td_s32  jpeg_exif_read_two_byte(jpeg_exif_entry *entry);
static td_s32  jpeg_exif_read_four_byte(jpeg_exif_entry *entry);
static td_void jpeg_exif_seek_bytes(jpeg_exif_entry *entry, td_s32 bytes);
static td_void jpeg_exif_skip_bytes(jpeg_exif_entry *entry, td_s32 bytes);
static td_u16  jpeg_exif_time_read_two_byte(const jpeg_exif_entry *entry);
static td_u32  jpeg_exif_time_read_four_byte(const jpeg_exif_entry *entry);

/*-------------------------------------- func release -------------------------------------*/

td_void jpeg_exif_init(jpeg_decompress_par_ptr par)
{
    par->exif.buffer   = TD_NULL;
    par->exif.phy_buf  = 0;
    par->exif.size     = 0;
    par->exif.base_vir_buf = TD_NULL;
    par->exif.base_phy_buf = 0;
    return;
}

td_void jpeg_exif_deinit(jpeg_decompress_par_ptr par)
{
    jpeg_mem_mgr mem = { 0 };

    if (par->thumbnail_en == TD_FALSE) {
        return;
    }

    mem.vir_buf = par->exif.base_vir_buf;
    mem.phy_buf = par->exif.base_phy_buf;
    jpeg_mem_free_large(&mem);

    par->exif.buffer   = TD_NULL;
    par->exif.phy_buf  = 0;
    par->exif.size     = 0;
    par->exif.base_vir_buf = TD_NULL;
    par->exif.base_phy_buf = 0;
    return;
}

td_void jpeg_exif_parse(jpeg_decompress_par_ptr par)
{
    td_bool ret = TD_FALSE;
    jpeg_exif_entry entry = {0};

    if (par->thumbnail_en == TD_FALSE) {
        return;
    }

    jpeg_exif_init_endian(&entry);

    ret = jpeg_exif_exist(par, &entry);
    if (ret != TD_TRUE) {
        return;
    }

    ret = jpeg_exif_get_data(par, &entry);
    if (ret != TD_TRUE) {
        return;
    }

    /* follows beging parse data for exif buffer */
    jpeg_exif_get_endian(&entry);

    ret = jpeg_exif_seek_to_ifd0_addr(&entry);
    if (ret != TD_TRUE) {
        return;
    }

    ret = jpeg_exif_read_ifd_tree(par, &entry);
    if (ret != TD_TRUE) {
        return;
    }

    ret = jpeg_exif_seek_to_ifd1_addr(&entry);
    if (ret != TD_TRUE) {
        return;
    }

    ret = jpeg_exif_read_ifd_tree(par, &entry);
    if (ret != TD_TRUE) {
        return;
    }

    return;
}

static td_void jpeg_exif_init_endian(jpeg_exif_entry *entry)
{
    entry->is_big_endian = TD_TRUE;
    return;
}

static td_bool jpeg_exif_exist(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry)
{
    td_u8 i, read_bytes;
    td_s32 code;
    td_bool eixst_exif = TD_TRUE;
    const td_u8 exif_tag[JPEG_EXIF_TAG_SIZE] = {0x45, 0x78, 0x69, 0x66, 0x0, 0x0};

    code = jpeg_stream_read_two_byte(par);
    entry->exif_size = (td_u32)((code > JPEG_EXIF_2BYTES) ? (code - JPEG_EXIF_2BYTES) : (0));
    if (entry->exif_size < JPEG_EXIF_MIN_SIZE) {
        jpeg_stream_skip_bytes(par, entry->exif_size);
        return TD_FALSE;
    }

    for (i = 0; i < JPEG_EXIF_TAG_SIZE; i++) {
        code = jpeg_stream_read_one_byte(par);
        if ((code < 0) || (code != exif_tag[i])) {
            eixst_exif = TD_FALSE;
            break;
        }
    }

    read_bytes = i + 1;
    entry->exif_size = (entry->exif_size > read_bytes) ? (entry->exif_size - read_bytes) : (0);

    if (eixst_exif != TD_TRUE) {
        jpeg_stream_skip_bytes(par, entry->exif_size);
    }
    return eixst_exif;
}

static td_bool jpeg_exif_get_data(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry)
{
    td_u32 i;
    td_s32 ret, code;
    jpeg_mem_mgr mem = { 0 };

    if (entry->exif_size == 0) {
        return TD_FALSE;
    }

    mem.size  = entry->exif_size;
    mem.align = JPEG_MEM_STREAM_ALIGN;
    ret = jpeg_mem_alloc_large(&mem);
    if (ret != EXT_SUCCESS) {
        jpeg_stream_skip_bytes(par, entry->exif_size);
        return TD_FALSE;
    }

    entry->exif_buf = mem.vir_buf;
    for (i = 0; i < entry->exif_size; i++) {
        code = jpeg_stream_read_one_byte(par);
        if (code < 0) {
            jpeg_mem_free_large(&mem);
            entry->exif_buf = TD_NULL;
            jpeg_stream_skip_bytes(par, entry->exif_size);
            return TD_FALSE;
        }
        *(entry->exif_buf + i) = (td_char)code;
    }

    entry->next_buf    = entry->exif_buf;
    entry->size_in_buf = entry->exif_size;
    par->exif.base_vir_buf = mem.vir_buf;
    par->exif.base_phy_buf = mem.phy_buf;

    return TD_TRUE;
}

static td_void jpeg_exif_get_endian(jpeg_exif_entry *entry)
{
    td_s32 code = jpeg_exif_read_two_byte(entry);
    if (code == JPEG_EXIF_ENDIAN_LITTLE) {
        entry->is_big_endian = TD_FALSE;
    }
}

/* ifd:
 * image file directory
 * ifd0: main pic
 */
static td_bool jpeg_exif_seek_to_ifd0_addr(jpeg_exif_entry *entry)
{
    td_s32 ifd0_offset;

    jpeg_exif_skip_bytes(entry, JPEG_EXIF_2BYTES);

    ifd0_offset = jpeg_exif_read_four_byte(entry);
    if ((ifd0_offset < 0) || (ifd0_offset > (td_s32)entry->exif_size)) {
        return TD_FALSE;
    }

    entry->next_buf    = entry->exif_buf + ifd0_offset;
    entry->size_in_buf = entry->exif_size - (td_u32)ifd0_offset;

    return TD_TRUE;
}

/* ifd:
 * image file directory
 * ifd1: thumbnail
 */
static td_bool jpeg_exif_seek_to_ifd1_addr(jpeg_exif_entry *entry)
{
    if ((entry->ifd1_offset <= 0) || (entry->ifd1_offset > entry->exif_size)) {
        return TD_FALSE;
    }
    entry->next_buf    = entry->exif_buf + entry->ifd1_offset;
    entry->size_in_buf = entry->exif_size - entry->ifd1_offset;
    return TD_TRUE;
}

static td_bool jpeg_exif_read_ifd_tree(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry)
{
    td_s32 code;
    td_s32 ifd1_offset;

    code = jpeg_exif_read_two_byte(entry);
    if (code < 0) {
        return TD_FALSE;
    } else if (code == 0) {
        return TD_TRUE;
    } else {
        ifd1_offset = code * 12; /* 12 for ifd1 offset */
    }

    jpeg_exif_seek_bytes(entry, ifd1_offset);
    entry->ifd1_offset = (td_u32)jpeg_exif_read_four_byte(entry);
    jpeg_exif_seek_bytes(entry, -(ifd1_offset + JPEG_EXIF_4BYTES));

    if (entry->exif_size < entry->ifd1_offset) {
        return TD_FALSE;
    }

    return jpeg_exif_dispose_ifd_tree(par, entry, (td_u16)code);
}

static td_bool jpeg_exif_dispose_ifd_tree(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry, td_u16 mark)
{
    td_s32 entry_tag;
    td_s32 code = mark;
    td_bool ret = TD_TRUE;

    while ((code > 0) && (ret == TD_TRUE)) {
        entry_tag = jpeg_exif_read_two_byte(entry);
        switch (entry_tag) {
            case JPEG_EXIF_IMAGE_COMMPRESS_TYPE:
                ret = jpeg_exif_get_compress_type(entry);
                break;
            case JPEG_EXIF_THUMBNAIL_OFFSET:
                ret = jpeg_exif_get_thumbnail_buffer(par, entry);
                break;
            case JPEG_EXIF_THUMBNAIL_SIZE:
                ret = jpeg_exif_get_thumbnail_size(par, entry);
                break;
            case JPEG_EXIF_IMAGE_TITLE:
                ret = jpeg_exif_get_thumbnail_title(par, entry);
                break;
            case JPEG_EXIF_CUSTOM_RENDERED:
                ret = jpeg_exif_get_thumbnail_render(par, entry);
                break;
            case JPEG_EXIF_DATE_TIME:
                ret = jpeg_exif_get_thumbnail_time(par, entry);
                break;
            case JPEG_EXIF_SUB_IFD:
                ret = jpeg_exif_seek_to_subifd_addr(entry);
                code = jpeg_exif_read_two_byte(entry);
                continue;
            default:
                jpeg_exif_skip_bytes(entry, JPEG_EXIF_ENTRY_LENGTH - JPEG_EXIF_2BYTES);
                break;
        }
        code--;
    }
    return ret;
}

static td_bool jpeg_exif_get_compress_type(jpeg_exif_entry *entry)
{
    td_u16 compress_type;
    td_bool ret = TD_FALSE;

    ret = jpeg_exif_read_entry_info(entry);
    if (ret != TD_TRUE) {
        return TD_FALSE;
    }

    compress_type = jpeg_exif_time_read_two_byte(entry);
    if (compress_type == JPEG_EXIF_JPEG) {
        entry->is_jpeg_compress = TD_TRUE;
    }

    return TD_TRUE;
}

static td_bool jpeg_exif_get_thumbnail_buffer(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry)
{
    td_bool ret = TD_FALSE;
    td_u32 offset;

    if (entry->is_jpeg_compress != TD_TRUE) {
        jpeg_exif_skip_bytes(entry, JPEG_EXIF_ENTRY_LENGTH - JPEG_EXIF_2BYTES);
        return TD_TRUE;
    }

    ret = jpeg_exif_read_entry_info(entry);
    if (ret != TD_TRUE) {
        return TD_FALSE;
    }

    offset = jpeg_exif_time_read_four_byte(entry);
    par->exif.buffer  = (td_char*)(par->exif.base_vir_buf + offset);
    par->exif.phy_buf = (td_u32)(par->exif.base_phy_buf + offset);
    par->exif.size    = (entry->exif_size > offset) ? (entry->exif_size - offset) : (0);

    return TD_TRUE;
}

static td_bool jpeg_exif_get_thumbnail_size(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry)
{
    td_bool ret = TD_FALSE;
    td_u32 size;

    if (entry->is_jpeg_compress != TD_TRUE) {
        jpeg_exif_skip_bytes(entry, JPEG_EXIF_ENTRY_LENGTH - JPEG_EXIF_2BYTES);
        return TD_TRUE;
    }

    ret = jpeg_exif_read_entry_info(entry);
    if (ret != TD_TRUE) {
        return TD_FALSE;
    }

    size = jpeg_exif_time_read_four_byte(entry);
    if ((par->exif.buffer - entry->exif_buf + size) > entry->exif_size) {
        return TD_TRUE;
    }

    par->exif.size = size;
    return TD_TRUE;
}

static td_bool jpeg_exif_get_thumbnail_title(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry)
{
    td_bool ret = TD_FALSE;

    ret = jpeg_exif_read_entry_info(entry);
    if (ret != TD_TRUE) {
        return TD_FALSE;
    }

    if ((entry->entry_data[0] == 'h') &&
        (entry->entry_data[1] == 'd') &&
        (entry->entry_data[2] == 'r')) { /* 0, 1, 2 is hdr tag data */
        par->is_hdr = TD_TRUE;
    }

    return TD_TRUE;
}

static td_bool jpeg_exif_get_thumbnail_render(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry)
{
    td_bool ret = TD_FALSE;
    td_u16 code;

    ret = jpeg_exif_read_entry_info(entry);
    if (ret != TD_TRUE) {
        return TD_FALSE;
    }

    code = jpeg_exif_time_read_two_byte(entry);
    if (code == 3) { /* 3 is hdr tag data */
        par->is_hdr = TD_TRUE;
    }

    return TD_TRUE;
}

static td_bool jpeg_exif_get_thumbnail_time(jpeg_decompress_par_ptr par, jpeg_exif_entry *entry)
{
    td_bool ret = TD_FALSE;
    errno_t err_ret;
    td_char data[JPEG_EXIF_DATE_STR_LENGTH + 1] = {'\0'};
    td_char time[JPEG_EXIF_TIME_STR_LENGTH + 1] = {'\0'};

    ret = jpeg_exif_read_entry_info(entry);
    if (ret != TD_TRUE) {
        return TD_FALSE;
    }

    err_ret = strncpy_s(data, sizeof(data), entry->entry_data, JPEG_EXIF_DATE_STR_LENGTH);
    if (err_ret != EOK) {
        return TD_FALSE;
    }

    err_ret = strncpy_s(time, sizeof(time), entry->entry_data + sizeof(data), JPEG_EXIF_TIME_STR_LENGTH);
    if (err_ret != EOK) {
        return TD_FALSE;
    }

    data[4] = 0; /* 4 for calc year month day */
    data[7] = 0; /* 7 for calc year month day */
    par->time.year = (td_u16)strtoul(&data[0], NULL, 0);
    par->time.month = (td_u8)strtoul(&data[5], NULL, 0); /* 5 for calc year month day */
    par->time.day = (td_u8)strtoul(&data[8], NULL, 0); /* 8 for calc year month day */

    time[2] = 0; /* 2 for calc hour minute second */
    time[5] = 0; /* 5 for calc hour minute second */
    par->time.hour = (td_u8)strtoul(&time[0], NULL, 0);
    par->time.minute = (td_u8)strtoul(&time[3], NULL, 0); /* 3 for calc hour minute second */
    par->time.second = (td_u8)strtoul(&time[6], NULL, 0); /* 6 for calc hour minute second */

    return TD_TRUE;
}

static td_bool jpeg_exif_seek_to_subifd_addr(jpeg_exif_entry *entry)
{
    td_u32 ifd_offset;
    td_bool ret = TD_FALSE;

    ret = jpeg_exif_read_entry_info(entry);
    if (ret != TD_TRUE) {
        return TD_FALSE;
    }

    ifd_offset = jpeg_exif_time_read_four_byte(entry);
    if ((ifd_offset < 0) || (ifd_offset > entry->exif_size)) {
        return TD_FALSE;
    }
    entry->next_buf    = entry->exif_buf + ifd_offset;
    entry->size_in_buf = (entry->exif_size > ifd_offset) ? (entry->exif_size - ifd_offset) : (0);

    return TD_TRUE;
}

static td_bool jpeg_exif_read_entry_info(jpeg_exif_entry *entry)
{
    td_s32 code1, code2;
    errno_t err_ret;
    td_u32 i, len, size_in_buf, ifd_offset_addr;
    td_char *cur_addr = TD_NULL;
    /* 1 2 4 8 12 are entry info */
    td_u32 size_of_data[JPEG_EXIF_ENTRY_DATA_SIZE] = {1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};

    code1 = jpeg_exif_read_two_byte(entry);
    code2 = jpeg_exif_read_four_byte(entry);
    /* sizeof not layer to 12 and not leaset 1 */
    if ((code1 > 12) || (code1 < 1) || (code2 > JPEG_EXIF_ENTRY_DATA_SIZE)) {
        return TD_FALSE;
    }
    len = (td_u32)((td_s32)(size_of_data[code1 - 1]) * code2); /* 1 get len pos */
    len = (len > JPEG_EXIF_ENTRY_DATA_SIZE) ? (JPEG_EXIF_ENTRY_DATA_SIZE) : len;

    err_ret = memset_s(entry->entry_data, sizeof(entry->entry_data), 0x0, sizeof(entry->entry_data));
    if (err_ret != EOK) { return TD_FALSE; }

    if (len <= JPEG_EXIF_4BYTES) {
        return jpeg_exif_write_entry_value(entry);
    }

    ifd_offset_addr = (td_u32)jpeg_exif_read_four_byte(entry);
    cur_addr = entry->next_buf;
    entry->next_buf = (td_char *)(entry->exif_buf + ifd_offset_addr);
    size_in_buf = (entry->exif_size > ifd_offset_addr) ? (entry->exif_size - ifd_offset_addr) : (0);
    if ((size_in_buf < len) || (size_in_buf < JPEG_EXIF_ENTRY_DATA_SIZE)) {
        return TD_FALSE;
    }
    for (i = 0; (i < len) && (i < JPEG_EXIF_ENTRY_DATA_SIZE); i++) {
        entry->entry_data[i] = *(entry->next_buf + i);
    }
    entry->next_buf = cur_addr;

    return TD_TRUE;
}

static td_bool jpeg_exif_write_entry_value(jpeg_exif_entry *entry)
{
    for (td_s32 i = 0; i < JPEG_EXIF_4BYTES; i++) {
        entry->entry_data[i] = jpeg_exif_read_one_byte(entry);
        if ((((td_u32)entry->entry_data[i]) & 0xff) < 0) {
            return TD_FALSE;
        }
    }
    return TD_TRUE;
}

static td_char jpeg_exif_read_one_byte(jpeg_exif_entry *entry)
{
    td_char code;
    if (entry->size_in_buf < JPEG_EXIF_1BYTES) {
        return -1;
    }
    code = (td_char)(((td_uchar)*(entry->next_buf)) & 0xff);
    entry->next_buf += JPEG_EXIF_1BYTES;
    entry->size_in_buf = (entry->size_in_buf > JPEG_EXIF_1BYTES) ? (entry->size_in_buf - JPEG_EXIF_1BYTES) : (0);
    return code;
}

static td_s32 jpeg_exif_read_two_byte(jpeg_exif_entry *entry)
{
    td_u32 code;
    if (entry->size_in_buf < JPEG_EXIF_2BYTES) {
        return -1;
    }
    if (entry->is_big_endian == TD_TRUE) {
        code = (((((td_u32) *(entry->next_buf)) << 8) & 0xff00) | /* 8:shift val */
               ((((td_u32) *((entry->next_buf) + 1))) & 0xff));
    } else {
        code = (((td_u32) *(entry->next_buf)) | (((td_u32) *((entry->next_buf) + 1)) << 8)); /* 8:shift val */
    }
    entry->next_buf += JPEG_EXIF_2BYTES;
    entry->size_in_buf = (entry->size_in_buf > JPEG_EXIF_2BYTES) ? (entry->size_in_buf - JPEG_EXIF_2BYTES) : (0);
    return (td_s32)code;
}

static td_s32 jpeg_exif_read_four_byte(jpeg_exif_entry *entry)
{
    td_s32 code;
    if (entry->size_in_buf < JPEG_EXIF_4BYTES) {
        return -1;
    }
    if (entry->is_big_endian == TD_TRUE) {
        code = (td_s32)((((((td_u32)*(entry->next_buf)) << 24) & 0xff000000) |        /* 24:shift val */
               ((((td_u32)*((entry->next_buf) + 1)) << 16) & 0xff0000) |     /* 16:shift val */
               ((((td_u32)*((entry->next_buf) + 2)) << 8) & 0xff00) |        /* 2:buf index, 8:shift val */
               ((((td_u32)*((entry->next_buf) + 3)) & 0xff))));               /* 3:buf index */
    } else {
        code = (td_s32)((((((td_u32)*(entry->next_buf)) & 0xff)) |
                ((((td_u32)*((entry->next_buf) + 1)) << 8) & 0xff00) |        /* 1:buf index, 8:shift val */
                ((((td_u32)*((entry->next_buf) + 2)) << 16) & 0xff0000) |     /* 2:buf index, 16:shift val */
                ((((td_u32)*((entry->next_buf) + 3)) << 24) & 0xff000000)));   /* 3:buf index, 24:shift val */
    }
    entry->next_buf += JPEG_EXIF_4BYTES;
    entry->size_in_buf = (entry->size_in_buf > JPEG_EXIF_4BYTES) ? (entry->size_in_buf - JPEG_EXIF_4BYTES) : (0);
    return code;
}

static td_void jpeg_exif_seek_bytes(jpeg_exif_entry *entry, td_s32 bytes)
{
    if (bytes < 0) {
        if ((td_s32)(entry->exif_size - entry->size_in_buf) < (-bytes)) {
            entry->next_buf = entry->exif_buf;
            entry->size_in_buf = entry->exif_size;
        } else {
            entry->next_buf -= bytes;
            entry->size_in_buf += (td_u32)bytes;
        }
    } else {
        if ((td_s32)entry->size_in_buf < bytes) {
            entry->next_buf += entry->size_in_buf;
            entry->size_in_buf = 0;
        } else {
            entry->next_buf += bytes;
            entry->size_in_buf = entry->size_in_buf - (td_u32)bytes;
        }
    }
    return;
}

static td_void jpeg_exif_skip_bytes(jpeg_exif_entry *entry, td_s32 bytes)
{
    if ((td_s32)entry->size_in_buf < bytes) {
        entry->next_buf += entry->size_in_buf;
        entry->size_in_buf = 0;
    } else {
        entry->next_buf += bytes;
        entry->size_in_buf = (td_u32)((td_s32)entry->size_in_buf - bytes);
    }
    return;
}


static td_u16 jpeg_exif_time_read_two_byte(const jpeg_exif_entry *entry)
{
    if (entry->is_big_endian == TD_TRUE) {
        return ((((td_u16) *(entry->entry_data)) << 8) | (((td_u16) *((entry->entry_data) + 1)))); /* 8:shift val */
    } else {
        return (((td_u16) *(entry->entry_data)) | (((td_u16) *((entry->entry_data) + 1)) << 8));   /* 8:shift val */
    }
}

static td_u32 jpeg_exif_time_read_four_byte(const jpeg_exif_entry *entry)
{
    if (entry->is_big_endian == TD_TRUE) {
        return (((((td_u32)*(entry->entry_data)) << 24) & 0xff000000) |      /* 24:shift val */
                ((((td_u32)*((entry->entry_data) + 1)) << 16) & 0xff0000) |  /* 16:shift val */
                ((((td_u32)*((entry->entry_data) + 2)) << 8) & 0xff00) |     /* 2:buf index, 8:shift val */
                (((td_u32)*((entry->entry_data) + 3)) & 0xff));              /* 3:buf index */
    } else {
        return ((((td_u32)*(entry->entry_data)) & 0xff) |
                ((((td_u32)*((entry->entry_data) + 1)) << 8) & 0xff00) |      /* 1:buf index, 8:shift val */
                ((((td_u32)*((entry->entry_data) + 2)) << 16) & 0xff0000) |   /* 2:buf index, 16:shift val */
                ((((td_u32)*((entry->entry_data) + 3)) << 24) & 0xff000000)); /* 3:buf index, 24:shift val */
    }
}
