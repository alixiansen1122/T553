#ifndef _VGEXT_H
#define _VGEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <VG/openvg.h>

#ifndef VG_API_ENTRYP
#define VG_API_ENTRYP VG_API_ENTRY*
#endif

/*-------------------------------------------------------------------------------
 * image extensions
 *------------------------------------------------------------------------------*/

typedef enum {
    VG_IMAGE_EXT_TYPE_NORMAL             = 0,
    VG_IMAGE_EXT_TYPE_COMPRESS           = 1,
    VG_IMAGE_EXT_TYPE_COLOR              = 2,

    VG_IMAGE_EXT_TYPE_FORCE_SIZE         = VG_MAX_ENUM
} VGImageBufferType;

typedef enum {
  VG_IMAGE_EXT_BUFFER_TYPE             = 0x1E20,
  VG_IMAGE_EXT_STRIDE                  = 0x1E21,
  VG_IMAGE_EXT_PHY_ADDR                = 0x1E22,
  VG_IMAGE_EXT_VIR_ADD                 = 0x1E23,
  VG_IMAGE_EXT_COLOR                   = 0x1E24,
  VG_IMAGE_EXT_PARAM_TYPE_FORCE_SIZE   = VG_MAX_ENUM
} VGImageParamTypeExt;

#ifdef VG_VGEXT_PROTOTYPES
VG_API_CALL void VG_API_ENTRY vgDrawImageExt(VGImage *images,
                                             VGint imageNum,
                                             VGfloat **matrix);
#else
typedef void (VG_API_ENTRYP PFNVGDRAWIMAGEEXTPROC)(VGImage *images,
                                                   VGint imageNum,
                                                   VGfloat **matrix);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _VGEXT_H */
