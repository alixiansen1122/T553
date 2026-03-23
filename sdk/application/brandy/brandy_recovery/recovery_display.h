 /*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description:  Application Core OS Initialize Interface for Standard.
 * Author:
 * Create: 2023-12-13
 */

#ifndef RECOVERY_DISPLAY_H
#define RECOVERY_DISPLAY_H

#define DPU_HEADER_LENTH      8
#define recovery_align_byte(byte, align) (((byte) + (align) - 1) & (~((align) -1)))

void recovery_display_init(void);
void recovery_display_sample(void);

#endif