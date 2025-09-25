#ifndef VULKAN_VIDEO_CODEC_VP9STD_H_
#define VULKAN_VIDEO_CODEC_VP9STD_H_ 1

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



// vulkan_video_codec_vp9std is a preprocessor guard. Do not pass it to API calls.
#define vulkan_video_codec_vp9std 1
#include "vulkan_video_codecs_common.h"
#define STD_VIDEO_VP9_NUM_REF_FRAMES      8
#define STD_VIDEO_VP9_REFS_PER_FRAME      3
#define STD_VIDEO_VP9_MAX_REF_FRAMES      4
#define STD_VIDEO_VP9_LOOP_FILTER_ADJUSTMENTS 2
#define STD_VIDEO_VP9_MAX_SEGMENTS        8
#define STD_VIDEO_VP9_SEG_LVL_MAX         4
#define STD_VIDEO_VP9_MAX_SEGMENTATION_TREE_PROBS 7
#define STD_VIDEO_VP9_MAX_SEGMENTATION_PRED_PROB 3

typedef enum StdVideoVP9Profile {
    STD_VIDEO_VP9_PROFILE_0 = 0,
    STD_VIDEO_VP9_PROFILE_1 = 1,
    STD_VIDEO_VP9_PROFILE_2 = 2,
    STD_VIDEO_VP9_PROFILE_3 = 3,
    STD_VIDEO_VP9_PROFILE_INVALID = 0x7FFFFFFF,
    STD_VIDEO_VP9_PROFILE_MAX_ENUM = 0x7FFFFFFF
} StdVideoVP9Profile;

typedef enum StdVideoVP9Level {
    STD_VIDEO_VP9_LEVEL_1_0 = 0,
    STD_VIDEO_VP9_LEVEL_1_1 = 1,
    STD_VIDEO_VP9_LEVEL_2_0 = 2,
    STD_VIDEO_VP9_LEVEL_2_1 = 3,
    STD_VIDEO_VP9_LEVEL_3_0 = 4,
    STD_VIDEO_VP9_LEVEL_3_1 = 5,
    STD_VIDEO_VP9_LEVEL_4_0 = 6,
    STD_VIDEO_VP9_LEVEL_4_1 = 7,
    STD_VIDEO_VP9_LEVEL_5_0 = 8,
    STD_VIDEO_VP9_LEVEL_5_1 = 9,
    STD_VIDEO_VP9_LEVEL_5_2 = 10,
    STD_VIDEO_VP9_LEVEL_6_0 = 11,
    STD_VIDEO_VP9_LEVEL_6_1 = 12,
    STD_VIDEO_VP9_LEVEL_6_2 = 13,
    STD_VIDEO_VP9_LEVEL_INVALID = 0x7FFFFFFF,
    STD_VIDEO_VP9_LEVEL_MAX_ENUM = 0x7FFFFFFF
} StdVideoVP9Level;

typedef enum StdVideoVP9FrameType {
    STD_VIDEO_VP9_FRAME_TYPE_KEY = 0,
    STD_VIDEO_VP9_FRAME_TYPE_NON_KEY = 1,
    STD_VIDEO_VP9_FRAME_TYPE_INVALID = 0x7FFFFFFF,
    STD_VIDEO_VP9_FRAME_TYPE_MAX_ENUM = 0x7FFFFFFF
} StdVideoVP9FrameType;

typedef enum StdVideoVP9ReferenceName {
    STD_VIDEO_VP9_REFERENCE_NAME_INTRA_FRAME = 0,
    STD_VIDEO_VP9_REFERENCE_NAME_LAST_FRAME = 1,
    STD_VIDEO_VP9_REFERENCE_NAME_GOLDEN_FRAME = 2,
    STD_VIDEO_VP9_REFERENCE_NAME_ALTREF_FRAME = 3,
    STD_VIDEO_VP9_REFERENCE_NAME_INVALID = 0x7FFFFFFF,
    STD_VIDEO_VP9_REFERENCE_NAME_MAX_ENUM = 0x7FFFFFFF
} StdVideoVP9ReferenceName;

typedef enum StdVideoVP9InterpolationFilter {
    STD_VIDEO_VP9_INTERPOLATION_FILTER_EIGHTTAP = 0,
    STD_VIDEO_VP9_INTERPOLATION_FILTER_EIGHTTAP_SMOOTH = 1,
    STD_VIDEO_VP9_INTERPOLATION_FILTER_EIGHTTAP_SHARP = 2,
    STD_VIDEO_VP9_INTERPOLATION_FILTER_BILINEAR = 3,
    STD_VIDEO_VP9_INTERPOLATION_FILTER_SWITCHABLE = 4,
    STD_VIDEO_VP9_INTERPOLATION_FILTER_INVALID = 0x7FFFFFFF,
    STD_VIDEO_VP9_INTERPOLATION_FILTER_MAX_ENUM = 0x7FFFFFFF
} StdVideoVP9InterpolationFilter;

typedef enum StdVideoVP9ColorSpace {
    STD_VIDEO_VP9_COLOR_SPACE_UNKNOWN = 0,
    STD_VIDEO_VP9_COLOR_SPACE_BT_601 = 1,
    STD_VIDEO_VP9_COLOR_SPACE_BT_709 = 2,
    STD_VIDEO_VP9_COLOR_SPACE_SMPTE_170 = 3,
    STD_VIDEO_VP9_COLOR_SPACE_SMPTE_240 = 4,
    STD_VIDEO_VP9_COLOR_SPACE_BT_2020 = 5,
    STD_VIDEO_VP9_COLOR_SPACE_RESERVED = 6,
    STD_VIDEO_VP9_COLOR_SPACE_RGB = 7,
    STD_VIDEO_VP9_COLOR_SPACE_INVALID = 0x7FFFFFFF,
    STD_VIDEO_VP9_COLOR_SPACE_MAX_ENUM = 0x7FFFFFFF
} StdVideoVP9ColorSpace;
typedef struct StdVideoVP9ColorConfigFlags {
    unsigned int    color_range : 1;
    unsigned int    reserved : 31;
} StdVideoVP9ColorConfigFlags;

typedef struct StdVideoVP9ColorConfig {
    StdVideoVP9ColorConfigFlags    flags;
    unsigned char                        BitDepth;
    unsigned char                        subsampling_x;
    unsigned char                        subsampling_y;
    unsigned char                        reserved1;
    StdVideoVP9ColorSpace          color_space;
} StdVideoVP9ColorConfig;

typedef struct StdVideoVP9LoopFilterFlags {
    unsigned int    loop_filter_delta_enabled : 1;
    unsigned int    loop_filter_delta_update : 1;
    unsigned int    reserved : 30;
} StdVideoVP9LoopFilterFlags;

typedef struct StdVideoVP9LoopFilter {
    StdVideoVP9LoopFilterFlags    flags;
    unsigned char                       loop_filter_level;
    unsigned char                       loop_filter_sharpness;
    unsigned char                       update_ref_delta;
    signed char                        loop_filter_ref_deltas[STD_VIDEO_VP9_MAX_REF_FRAMES];
    unsigned char                       update_mode_delta;
    signed char                        loop_filter_mode_deltas[STD_VIDEO_VP9_LOOP_FILTER_ADJUSTMENTS];
} StdVideoVP9LoopFilter;

typedef struct StdVideoVP9SegmentationFlags {
    unsigned int    segmentation_update_map : 1;
    unsigned int    segmentation_temporal_update : 1;
    unsigned int    segmentation_update_data : 1;
    unsigned int    segmentation_abs_or_delta_update : 1;
    unsigned int    reserved : 28;
} StdVideoVP9SegmentationFlags;

typedef struct StdVideoVP9Segmentation {
    StdVideoVP9SegmentationFlags    flags;
    unsigned char                         segmentation_tree_probs[STD_VIDEO_VP9_MAX_SEGMENTATION_TREE_PROBS];
    unsigned char                         segmentation_pred_prob[STD_VIDEO_VP9_MAX_SEGMENTATION_PRED_PROB];
    unsigned char                         FeatureEnabled[STD_VIDEO_VP9_MAX_SEGMENTS];
    short                         FeatureData[STD_VIDEO_VP9_MAX_SEGMENTS][STD_VIDEO_VP9_SEG_LVL_MAX];
} StdVideoVP9Segmentation;


#ifdef __cplusplus
}
#endif

#endif
