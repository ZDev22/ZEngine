#ifndef VULKAN_VIDEO_CODEC_H265STD_DECODE_H_
#define VULKAN_VIDEO_CODEC_H265STD_DECODE_H_ 1

/*
** Copyright 2015-2025 The Khronos Group Inc.
**
** SPDX-License-Identifier: Apache-2.0
*/

/*
** This header is generated from the Khronos Vulkan XML API Registry.
**
*/


#ifdef __cplusplus
extern "C" {
#endif



// vulkan_video_codec_h265std_decode is a preprocessor guard. Do not pass it to API calls.
#define vulkan_video_codec_h265std_decode 1
#include "vulkan_video_codec_h265std.h"

#define VK_STD_VULKAN_VIDEO_CODEC_H265_DECODE_API_VERSION_1_0_0 VK_MAKE_VIDEO_STD_VERSION(1, 0, 0)

#define VK_STD_VULKAN_VIDEO_CODEC_H265_DECODE_SPEC_VERSION VK_STD_VULKAN_VIDEO_CODEC_H265_DECODE_API_VERSION_1_0_0
#define VK_STD_VULKAN_VIDEO_CODEC_H265_DECODE_EXTENSION_NAME "VK_STD_vulkan_video_codec_h265_decode"
#define STD_VIDEO_DECODE_H265_REF_PIC_SET_LIST_SIZE 8
typedef struct StdVideoDecodeH265PictureInfoFlags {
    unsigned int    IrapPicFlag : 1;
    unsigned int    IdrPicFlag  : 1;
    unsigned int    IsReference : 1;
    unsigned int    short_term_ref_pic_set_sps_flag : 1;
} StdVideoDecodeH265PictureInfoFlags;

typedef struct StdVideoDecodeH265PictureInfo {
    StdVideoDecodeH265PictureInfoFlags    flags;
    unsigned char                               sps_video_parameter_set_id;
    unsigned char                               pps_seq_parameter_set_id;
    unsigned char                               pps_pic_parameter_set_id;
    unsigned char                               NumDeltaPocsOfRefRpsIdx;
    int                               PicOrderCntVal;
    unsigned short                              NumBitsForSTRefPicSetInSlice;
    unsigned short                              reserved;
    unsigned char                               RefPicSetStCurrBefore[STD_VIDEO_DECODE_H265_REF_PIC_SET_LIST_SIZE];
    unsigned char                               RefPicSetStCurrAfter[STD_VIDEO_DECODE_H265_REF_PIC_SET_LIST_SIZE];
    unsigned char                               RefPicSetLtCurr[STD_VIDEO_DECODE_H265_REF_PIC_SET_LIST_SIZE];
} StdVideoDecodeH265PictureInfo;

typedef struct StdVideoDecodeH265ReferenceInfoFlags {
    unsigned int    used_for_long_term_reference : 1;
    unsigned int    unused_for_reference : 1;
} StdVideoDecodeH265ReferenceInfoFlags;

typedef struct StdVideoDecodeH265ReferenceInfo {
    StdVideoDecodeH265ReferenceInfoFlags    flags;
    int                                 PicOrderCntVal;
} StdVideoDecodeH265ReferenceInfo;


#ifdef __cplusplus
}
#endif

#endif
