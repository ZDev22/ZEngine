#ifndef VULKAN_VIDEO_CODEC_AV1STD_ENCODE_H_
#define VULKAN_VIDEO_CODEC_AV1STD_ENCODE_H_ 1

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



// vulkan_video_codec_av1std_encode is a preprocessor guard. Do not pass it to API calls.
#define vulkan_video_codec_av1std_encode 1
#include "vulkan_video_codec_av1std.h"

#define VK_STD_VULKAN_VIDEO_CODEC_AV1_ENCODE_API_VERSION_1_0_0 VK_MAKE_VIDEO_STD_VERSION(1, 0, 0)

#define VK_STD_VULKAN_VIDEO_CODEC_AV1_ENCODE_SPEC_VERSION VK_STD_VULKAN_VIDEO_CODEC_AV1_ENCODE_API_VERSION_1_0_0
#define VK_STD_VULKAN_VIDEO_CODEC_AV1_ENCODE_EXTENSION_NAME "VK_STD_vulkan_video_codec_av1_encode"
typedef struct StdVideoEncodeAV1DecoderModelInfo {
    unsigned char     buffer_delay_length_minus_1;
    unsigned char     buffer_removal_time_length_minus_1;
    unsigned char     frame_presentation_time_length_minus_1;
    unsigned char     reserved1;
    unsigned int    num_units_in_decoding_tick;
} StdVideoEncodeAV1DecoderModelInfo;

typedef struct StdVideoEncodeAV1ExtensionHeader {
    unsigned char    temporal_id;
    unsigned char    spatial_id;
} StdVideoEncodeAV1ExtensionHeader;

typedef struct StdVideoEncodeAV1OperatingPointInfoFlags {
    unsigned int    decoder_model_present_for_this_op : 1;
    unsigned int    low_delay_mode_flag : 1;
    unsigned int    initial_display_delay_present_for_this_op : 1;
    unsigned int    reserved : 29;
} StdVideoEncodeAV1OperatingPointInfoFlags;

typedef struct StdVideoEncodeAV1OperatingPointInfo {
    StdVideoEncodeAV1OperatingPointInfoFlags    flags;
    unsigned short                                    operating_point_idc;
    unsigned char                                     seq_level_idx;
    unsigned char                                     seq_tier;
    unsigned int                                    decoder_buffer_delay;
    unsigned int                                    encoder_buffer_delay;
    unsigned char                                     initial_display_delay_minus_1;
} StdVideoEncodeAV1OperatingPointInfo;

typedef struct StdVideoEncodeAV1PictureInfoFlags {
    unsigned int    error_resilient_mode : 1;
    unsigned int    disable_cdf_update : 1;
    unsigned int    use_superres : 1;
    unsigned int    render_and_frame_size_different : 1;
    unsigned int    allow_screen_content_tools : 1;
    unsigned int    is_filter_switchable : 1;
    unsigned int    force_integer_mv : 1;
    unsigned int    frame_size_override_flag : 1;
    unsigned int    buffer_removal_time_present_flag : 1;
    unsigned int    allow_intrabc : 1;
    unsigned int    frame_refs_short_signaling : 1;
    unsigned int    allow_high_precision_mv : 1;
    unsigned int    is_motion_mode_switchable : 1;
    unsigned int    use_ref_frame_mvs : 1;
    unsigned int    disable_frame_end_update_cdf : 1;
    unsigned int    allow_warped_motion : 1;
    unsigned int    reduced_tx_set : 1;
    unsigned int    skip_mode_present : 1;
    unsigned int    delta_q_present : 1;
    unsigned int    delta_lf_present : 1;
    unsigned int    delta_lf_multi : 1;
    unsigned int    segmentation_enabled : 1;
    unsigned int    segmentation_update_map : 1;
    unsigned int    segmentation_temporal_update : 1;
    unsigned int    segmentation_update_data : 1;
    unsigned int    UsesLr : 1;
    unsigned int    usesChromaLr : 1;
    unsigned int    show_frame : 1;
    unsigned int    showable_frame : 1;
    unsigned int    reserved : 3;
} StdVideoEncodeAV1PictureInfoFlags;

typedef struct StdVideoEncodeAV1PictureInfo {
    StdVideoEncodeAV1PictureInfoFlags          flags;
    StdVideoAV1FrameType                       frame_type;
    unsigned int                                   frame_presentation_time;
    unsigned int                                   current_frame_id;
    unsigned char                                    order_hint;
    unsigned char                                    primary_ref_frame;
    unsigned char                                    refresh_frame_flags;
    unsigned char                                    coded_denom;
    unsigned short                                   render_width_minus_1;
    unsigned short                                   render_height_minus_1;
    StdVideoAV1InterpolationFilter             interpolation_filter;
    StdVideoAV1TxMode                          TxMode;
    unsigned char                                    delta_q_res;
    unsigned char                                    delta_lf_res;
    unsigned char                                    ref_order_hint[STD_VIDEO_AV1_NUM_REF_FRAMES];
    signed char                                     ref_frame_idx[STD_VIDEO_AV1_REFS_PER_FRAME];
    unsigned char                                    reserved1[3];
    unsigned int                                   delta_frame_id_minus_1[STD_VIDEO_AV1_REFS_PER_FRAME];
    const StdVideoAV1TileInfo*                 pTileInfo;
    const StdVideoAV1Quantization*             pQuantization;
    const StdVideoAV1Segmentation*             pSegmentation;
    const StdVideoAV1LoopFilter*               pLoopFilter;
    const StdVideoAV1CDEF*                     pCDEF;
    const StdVideoAV1LoopRestoration*          pLoopRestoration;
    const StdVideoAV1GlobalMotion*             pGlobalMotion;
    const StdVideoEncodeAV1ExtensionHeader*    pExtensionHeader;
    const unsigned int*                            pBufferRemovalTimes;
} StdVideoEncodeAV1PictureInfo;

typedef struct StdVideoEncodeAV1ReferenceInfoFlags {
    unsigned int    disable_frame_end_update_cdf : 1;
    unsigned int    segmentation_enabled : 1;
    unsigned int    reserved : 30;
} StdVideoEncodeAV1ReferenceInfoFlags;

typedef struct StdVideoEncodeAV1ReferenceInfo {
    StdVideoEncodeAV1ReferenceInfoFlags        flags;
    unsigned int                                   RefFrameId;
    StdVideoAV1FrameType                       frame_type;
    unsigned char                                    OrderHint;
    unsigned char                                    reserved1[3];
    const StdVideoEncodeAV1ExtensionHeader*    pExtensionHeader;
} StdVideoEncodeAV1ReferenceInfo;


#ifdef __cplusplus
}
#endif

#endif
