#ifndef VULKAN_VIDEO_CODEC_VP9STD_DECODE_H_
#define VULKAN_VIDEO_CODEC_VP9STD_DECODE_H_ 1

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



// vulkan_video_codec_vp9std_decode is a preprocessor guard. Do not pass it to API calls.
#define vulkan_video_codec_vp9std_decode 1
#include "vulkan_video_codec_vp9std.h"

#define VK_STD_VULKAN_VIDEO_CODEC_VP9_DECODE_API_VERSION_1_0_0 VK_MAKE_VIDEO_STD_VERSION(1, 0, 0)

#define VK_STD_VULKAN_VIDEO_CODEC_VP9_DECODE_SPEC_VERSION VK_STD_VULKAN_VIDEO_CODEC_VP9_DECODE_API_VERSION_1_0_0
#define VK_STD_VULKAN_VIDEO_CODEC_VP9_DECODE_EXTENSION_NAME "VK_STD_vulkan_video_codec_vp9_decode"
typedef struct StdVideoDecodeVP9PictureInfoFlags {
    unsigned int    error_resilient_mode : 1;
    unsigned int    intra_only : 1;
    unsigned int    allow_high_precision_mv : 1;
    unsigned int    refresh_frame_context : 1;
    unsigned int    frame_parallel_decoding_mode : 1;
    unsigned int    segmentation_enabled : 1;
    unsigned int    show_frame : 1;
    unsigned int    UsePrevFrameMvs : 1;
    unsigned int    reserved : 24;
} StdVideoDecodeVP9PictureInfoFlags;

typedef struct StdVideoDecodeVP9PictureInfo {
    StdVideoDecodeVP9PictureInfoFlags    flags;
    StdVideoVP9Profile                   profile;
    StdVideoVP9FrameType                 frame_type;
    unsigned char                              frame_context_idx;
    unsigned char                              reset_frame_context;
    unsigned char                              refresh_frame_flags;
    unsigned char                              ref_frame_sign_bias_mask;
    StdVideoVP9InterpolationFilter       interpolation_filter;
    unsigned char                              base_q_idx;
    signed char                               delta_q_y_dc;
    signed char                               delta_q_uv_dc;
    signed char                               delta_q_uv_ac;
    unsigned char                              tile_cols_log2;
    unsigned char                              tile_rows_log2;
    unsigned short                             reserved1[3];
    const StdVideoVP9ColorConfig*        pColorConfig;
    const StdVideoVP9LoopFilter*         pLoopFilter;
    const StdVideoVP9Segmentation*       pSegmentation;
} StdVideoDecodeVP9PictureInfo;


#ifdef __cplusplus
}
#endif

#endif
