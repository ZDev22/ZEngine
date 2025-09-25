#ifndef VULKAN_VIDEO_CODEC_H265STD_ENCODE_H_
#define VULKAN_VIDEO_CODEC_H265STD_ENCODE_H_ 1

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



// vulkan_video_codec_h265std_encode is a preprocessor guard. Do not pass it to API calls.
#define vulkan_video_codec_h265std_encode 1
#include "vulkan_video_codec_h265std.h"

#define VK_STD_VULKAN_VIDEO_CODEC_H265_ENCODE_API_VERSION_1_0_0 VK_MAKE_VIDEO_STD_VERSION(1, 0, 0)

#define VK_STD_VULKAN_VIDEO_CODEC_H265_ENCODE_SPEC_VERSION VK_STD_VULKAN_VIDEO_CODEC_H265_ENCODE_API_VERSION_1_0_0
#define VK_STD_VULKAN_VIDEO_CODEC_H265_ENCODE_EXTENSION_NAME "VK_STD_vulkan_video_codec_h265_encode"
typedef struct StdVideoEncodeH265WeightTableFlags {
    unsigned short    luma_weight_l0_flag;
    unsigned short    chroma_weight_l0_flag;
    unsigned short    luma_weight_l1_flag;
    unsigned short    chroma_weight_l1_flag;
} StdVideoEncodeH265WeightTableFlags;

typedef struct StdVideoEncodeH265WeightTable {
    StdVideoEncodeH265WeightTableFlags    flags;
    unsigned char                               luma_log2_weight_denom;
    signed char                                delta_chroma_log2_weight_denom;
    signed char                                delta_luma_weight_l0[STD_VIDEO_H265_MAX_NUM_LIST_REF];
    signed char                                luma_offset_l0[STD_VIDEO_H265_MAX_NUM_LIST_REF];
    signed char                                delta_chroma_weight_l0[STD_VIDEO_H265_MAX_NUM_LIST_REF][STD_VIDEO_H265_MAX_CHROMA_PLANES];
    signed char                                delta_chroma_offset_l0[STD_VIDEO_H265_MAX_NUM_LIST_REF][STD_VIDEO_H265_MAX_CHROMA_PLANES];
    signed char                                delta_luma_weight_l1[STD_VIDEO_H265_MAX_NUM_LIST_REF];
    signed char                                luma_offset_l1[STD_VIDEO_H265_MAX_NUM_LIST_REF];
    signed char                                delta_chroma_weight_l1[STD_VIDEO_H265_MAX_NUM_LIST_REF][STD_VIDEO_H265_MAX_CHROMA_PLANES];
    signed char                                delta_chroma_offset_l1[STD_VIDEO_H265_MAX_NUM_LIST_REF][STD_VIDEO_H265_MAX_CHROMA_PLANES];
} StdVideoEncodeH265WeightTable;

typedef struct StdVideoEncodeH265SliceSegmentHeaderFlags {
    unsigned int    first_slice_segment_in_pic_flag : 1;
    unsigned int    dependent_slice_segment_flag : 1;
    unsigned int    slice_sao_luma_flag : 1;
    unsigned int    slice_sao_chroma_flag : 1;
    unsigned int    num_ref_idx_active_override_flag : 1;
    unsigned int    mvd_l1_zero_flag : 1;
    unsigned int    cabac_init_flag : 1;
    unsigned int    cu_chroma_qp_offset_enabled_flag : 1;
    unsigned int    deblocking_filter_override_flag : 1;
    unsigned int    slice_deblocking_filter_disabled_flag : 1;
    unsigned int    collocated_from_l0_flag : 1;
    unsigned int    slice_loop_filter_across_slices_enabled_flag : 1;
    unsigned int    reserved : 20;
} StdVideoEncodeH265SliceSegmentHeaderFlags;

typedef struct StdVideoEncodeH265SliceSegmentHeader {
    StdVideoEncodeH265SliceSegmentHeaderFlags    flags;
    StdVideoH265SliceType                        slice_type;
    unsigned int                                     slice_segment_address;
    unsigned char                                      collocated_ref_idx;
    unsigned char                                      MaxNumMergeCand;
    signed char                                       slice_cb_qp_offset;
    signed char                                       slice_cr_qp_offset;
    signed char                                       slice_beta_offset_div2;
    signed char                                       slice_tc_offset_div2;
    signed char                                       slice_act_y_qp_offset;
    signed char                                       slice_act_cb_qp_offset;
    signed char                                       slice_act_cr_qp_offset;
    signed char                                       slice_qp_delta;
    unsigned short                                     reserved1;
    const StdVideoEncodeH265WeightTable*         pWeightTable;
} StdVideoEncodeH265SliceSegmentHeader;

typedef struct StdVideoEncodeH265ReferenceListsInfoFlags {
    unsigned int    ref_pic_list_modification_flag_l0 : 1;
    unsigned int    ref_pic_list_modification_flag_l1 : 1;
    unsigned int    reserved : 30;
} StdVideoEncodeH265ReferenceListsInfoFlags;

typedef struct StdVideoEncodeH265ReferenceListsInfo {
    StdVideoEncodeH265ReferenceListsInfoFlags    flags;
    unsigned char                                      num_ref_idx_l0_active_minus1;
    unsigned char                                      num_ref_idx_l1_active_minus1;
    unsigned char                                      RefPicList0[STD_VIDEO_H265_MAX_NUM_LIST_REF];
    unsigned char                                      RefPicList1[STD_VIDEO_H265_MAX_NUM_LIST_REF];
    unsigned char                                      list_entry_l0[STD_VIDEO_H265_MAX_NUM_LIST_REF];
    unsigned char                                      list_entry_l1[STD_VIDEO_H265_MAX_NUM_LIST_REF];
} StdVideoEncodeH265ReferenceListsInfo;

typedef struct StdVideoEncodeH265PictureInfoFlags {
    unsigned int    is_reference : 1;
    unsigned int    IrapPicFlag : 1;
    unsigned int    used_for_long_term_reference : 1;
    unsigned int    discardable_flag : 1;
    unsigned int    cross_layer_bla_flag : 1;
    unsigned int    pic_output_flag : 1;
    unsigned int    no_output_of_prior_pics_flag : 1;
    unsigned int    short_term_ref_pic_set_sps_flag : 1;
    unsigned int    slice_temporal_mvp_enabled_flag : 1;
    unsigned int    reserved : 23;
} StdVideoEncodeH265PictureInfoFlags;

typedef struct StdVideoEncodeH265LongTermRefPics {
    unsigned char     num_long_term_sps;
    unsigned char     num_long_term_pics;
    unsigned char     lt_idx_sps[STD_VIDEO_H265_MAX_LONG_TERM_REF_PICS_SPS];
    unsigned char     poc_lsb_lt[STD_VIDEO_H265_MAX_LONG_TERM_PICS];
    unsigned short    used_by_curr_pic_lt_flag;
    unsigned char     delta_poc_msb_present_flag[STD_VIDEO_H265_MAX_DELTA_POC];
    unsigned char     delta_poc_msb_cycle_lt[STD_VIDEO_H265_MAX_DELTA_POC];
} StdVideoEncodeH265LongTermRefPics;

typedef struct StdVideoEncodeH265PictureInfo {
    StdVideoEncodeH265PictureInfoFlags             flags;
    StdVideoH265PictureType                        pic_type;
    unsigned char                                        sps_video_parameter_set_id;
    unsigned char                                        pps_seq_parameter_set_id;
    unsigned char                                        pps_pic_parameter_set_id;
    unsigned char                                        short_term_ref_pic_set_idx;
    int                                        PicOrderCntVal;
    unsigned char                                        TemporalId;
    unsigned char                                        reserved1[7];
    const StdVideoEncodeH265ReferenceListsInfo*    pRefLists;
    const StdVideoH265ShortTermRefPicSet*          pShortTermRefPicSet;
    const StdVideoEncodeH265LongTermRefPics*       pLongTermRefPics;
} StdVideoEncodeH265PictureInfo;

typedef struct StdVideoEncodeH265ReferenceInfoFlags {
    unsigned int    used_for_long_term_reference : 1;
    unsigned int    unused_for_reference : 1;
    unsigned int    reserved : 30;
} StdVideoEncodeH265ReferenceInfoFlags;

typedef struct StdVideoEncodeH265ReferenceInfo {
    StdVideoEncodeH265ReferenceInfoFlags    flags;
    StdVideoH265PictureType                 pic_type;
    int                                 PicOrderCntVal;
    unsigned char                                 TemporalId;
} StdVideoEncodeH265ReferenceInfo;


#ifdef __cplusplus
}
#endif

#endif
