#ifndef VULKAN_VIDEO_CODEC_H264STD_DECODE_H_
#define VULKAN_VIDEO_CODEC_H264STD_DECODE_H_ 1

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



// vulkan_video_codec_h264std_decode is a preprocessor guard. Do not pass it to API calls.
#define vulkan_video_codec_h264std_decode 1
#include "vulkan_video_codec_h264std.h"

#define VK_STD_VULKAN_VIDEO_CODEC_H264_DECODE_API_VERSION_1_0_0 VK_MAKE_VIDEO_STD_VERSION(1, 0, 0)

#define VK_STD_VULKAN_VIDEO_CODEC_H264_DECODE_SPEC_VERSION VK_STD_VULKAN_VIDEO_CODEC_H264_DECODE_API_VERSION_1_0_0
#define VK_STD_VULKAN_VIDEO_CODEC_H264_DECODE_EXTENSION_NAME "VK_STD_vulkan_video_codec_h264_decode"
#define STD_VIDEO_DECODE_H264_FIELD_ORDER_COUNT_LIST_SIZE 2

typedef enum StdVideoDecodeH264FieldOrderCount {
    STD_VIDEO_DECODE_H264_FIELD_ORDER_COUNT_TOP = 0,
    STD_VIDEO_DECODE_H264_FIELD_ORDER_COUNT_BOTTOM = 1,
    STD_VIDEO_DECODE_H264_FIELD_ORDER_COUNT_INVALID = 0x7FFFFFFF,
    STD_VIDEO_DECODE_H264_FIELD_ORDER_COUNT_MAX_ENUM = 0x7FFFFFFF
} StdVideoDecodeH264FieldOrderCount;
typedef struct StdVideoDecodeH264PictureInfoFlags {
    unsigned int    field_pic_flag : 1;
    unsigned int    is_intra : 1;
    unsigned int    IdrPicFlag : 1;
    unsigned int    bottom_field_flag : 1;
    unsigned int    is_reference : 1;
    unsigned int    complementary_field_pair : 1;
} StdVideoDecodeH264PictureInfoFlags;

typedef struct StdVideoDecodeH264PictureInfo {
    StdVideoDecodeH264PictureInfoFlags    flags;
    unsigned char                               seq_parameter_set_id;
    unsigned char                               pic_parameter_set_id;
    unsigned char                               reserved1;
    unsigned char                               reserved2;
    unsigned short                              frame_num;
    unsigned short                              idr_pic_id;
    int                               PicOrderCnt[STD_VIDEO_DECODE_H264_FIELD_ORDER_COUNT_LIST_SIZE];
} StdVideoDecodeH264PictureInfo;

typedef struct StdVideoDecodeH264ReferenceInfoFlags {
    unsigned int    top_field_flag : 1;
    unsigned int    bottom_field_flag : 1;
    unsigned int    used_for_long_term_reference : 1;
    unsigned int    is_non_existing : 1;
} StdVideoDecodeH264ReferenceInfoFlags;

typedef struct StdVideoDecodeH264ReferenceInfo {
    StdVideoDecodeH264ReferenceInfoFlags    flags;
    unsigned short                                FrameNum;
    unsigned short                                reserved;
    int                                 PicOrderCnt[STD_VIDEO_DECODE_H264_FIELD_ORDER_COUNT_LIST_SIZE];
} StdVideoDecodeH264ReferenceInfo;


#ifdef __cplusplus
}
#endif

#endif
