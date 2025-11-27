#ifndef VULKAN_VIDEO_CODEC_AV1STD_DECODE_H_
#define VULKAN_VIDEO_CODEC_AV1STD_DECODE_H_ 1
/*
** Copyright 2015-2025 The Khronos Group Inc.
**
** SPDX-License-Identifier: Apache-2.0
*/
#ifdef __cplusplus
extern "C" {
#endif
#define vulkan_video_codec_av1std_decode 1
#include "vulkan_video_codec_av1std.h"
#define VK_STD_VULKAN_VIDEO_CODEC_AV1_DECODE_API_VERSION_1_0_0 VK_MAKE_VIDEO_STD_VERSION(1, 0, 0)
#define VK_STD_VULKAN_VIDEO_CODEC_AV1_DECODE_SPEC_VERSION VK_STD_VULKAN_VIDEO_CODEC_AV1_DECODE_API_VERSION_1_0_0
#define VK_STD_VULKAN_VIDEO_CODEC_AV1_DECODE_EXTENSION_NAME "VK_STD_vulkan_video_codec_av1_decode"
typedef struct StdVideoDecodeAV1PictureInfoFlags {
    unsigned int error_resilient_mode : 1;
    unsigned int disable_cdf_update : 1;
    unsigned int use_superres : 1;
    unsigned int render_and_frame_size_different : 1;
    unsigned int allow_screen_content_tools : 1;
    unsigned int is_filter_switchable : 1;
    unsigned int force_integer_mv : 1;
    unsigned int frame_size_override_flag : 1;
    unsigned int buffer_removal_time_present_flag : 1;
    unsigned int allow_intrabc : 1;
    unsigned int frame_refs_short_signaling : 1;
    unsigned int allow_high_precision_mv : 1;
    unsigned int is_motion_mode_switchable : 1;
    unsigned int use_ref_frame_mvs : 1;
    unsigned int disable_frame_end_update_cdf : 1;
    unsigned int allow_warped_motion : 1;
    unsigned int reduced_tx_set : 1;
    unsigned int reference_select : 1;
    unsigned int skip_mode_present : 1;
    unsigned int delta_q_present : 1;
    unsigned int delta_lf_present : 1;
    unsigned int delta_lf_multi : 1;
    unsigned int segmentation_enabled : 1;
    unsigned int segmentation_update_map : 1;
    unsigned int segmentation_temporal_update : 1;
    unsigned int segmentation_update_data : 1;
    unsigned int UsesLr : 1;
    unsigned int usesChromaLr : 1;
    unsigned int apply_grain : 1;
    unsigned int reserved : 3;
} StdVideoDecodeAV1PictureInfoFlags;
typedef struct StdVideoDecodeAV1PictureInfo {
    StdVideoDecodeAV1PictureInfoFlags flags;
    StdVideoAV1FrameType frame_type;
    unsigned int current_frame_id;
    unsigned char OrderHint;
    unsigned char primary_ref_frame;
    unsigned char refresh_frame_flags;
    unsigned char reserved1;
    StdVideoAV1InterpolationFilter interpolation_filter;
    StdVideoAV1TxMode TxMode;
    unsigned char delta_q_res;
    unsigned char delta_lf_res;
    unsigned char SkipModeFrame[STD_VIDEO_AV1_SKIP_MODE_FRAMES];
    unsigned char coded_denom;
    unsigned char reserved2[3];
    unsigned char OrderHints[STD_VIDEO_AV1_NUM_REF_FRAMES];
    unsigned int expectedFrameId[STD_VIDEO_AV1_NUM_REF_FRAMES];
    const StdVideoAV1TileInfo* pTileInfo;
    const StdVideoAV1Quantization* pQuantization;
    const StdVideoAV1Segmentation* pSegmentation;
    const StdVideoAV1LoopFilter* pLoopFilter;
    const StdVideoAV1CDEF* pCDEF;
    const StdVideoAV1LoopRestoration* pLoopRestoration;
    const StdVideoAV1GlobalMotion* pGlobalMotion;
    const StdVideoAV1FilmGrain* pFilmGrain;
} StdVideoDecodeAV1PictureInfo;
typedef struct StdVideoDecodeAV1ReferenceInfoFlags {
    unsigned int disable_frame_end_update_cdf : 1;
    unsigned int segmentation_enabled : 1;
    unsigned int reserved : 30;
} StdVideoDecodeAV1ReferenceInfoFlags;
typedef struct StdVideoDecodeAV1ReferenceInfo {
    StdVideoDecodeAV1ReferenceInfoFlags    flags;
    unsigned char frame_type;
    unsigned char RefFrameSignBias;
    unsigned char OrderHint;
    unsigned char SavedOrderHints[STD_VIDEO_AV1_NUM_REF_FRAMES];
} StdVideoDecodeAV1ReferenceInfo;
#ifdef __cplusplus
}
#endif
#endif