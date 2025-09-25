#ifndef VULKAN_VIDEO_CODEC_H264STD_ENCODE_H_
#define VULKAN_VIDEO_CODEC_H264STD_ENCODE_H_ 1

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



// vulkan_video_codec_h264std_encode is a preprocessor guard. Do not pass it to API calls.
#define vulkan_video_codec_h264std_encode 1
#include "vulkan_video_codec_h264std.h"

#define VK_STD_VULKAN_VIDEO_CODEC_H264_ENCODE_API_VERSION_1_0_0 VK_MAKE_VIDEO_STD_VERSION(1, 0, 0)

#define VK_STD_VULKAN_VIDEO_CODEC_H264_ENCODE_SPEC_VERSION VK_STD_VULKAN_VIDEO_CODEC_H264_ENCODE_API_VERSION_1_0_0
#define VK_STD_VULKAN_VIDEO_CODEC_H264_ENCODE_EXTENSION_NAME "VK_STD_vulkan_video_codec_h264_encode"
typedef struct StdVideoEncodeH264WeightTableFlags {
    unsigned int    luma_weight_l0_flag;
    unsigned int    chroma_weight_l0_flag;
    unsigned int    luma_weight_l1_flag;
    unsigned int    chroma_weight_l1_flag;
} StdVideoEncodeH264WeightTableFlags;

typedef struct StdVideoEncodeH264WeightTable {
    StdVideoEncodeH264WeightTableFlags    flags;
    unsigned char                               luma_log2_weight_denom;
    unsigned char                               chroma_log2_weight_denom;
    signed char                                luma_weight_l0[STD_VIDEO_H264_MAX_NUM_LIST_REF];
    signed char                                luma_offset_l0[STD_VIDEO_H264_MAX_NUM_LIST_REF];
    signed char                                chroma_weight_l0[STD_VIDEO_H264_MAX_NUM_LIST_REF][STD_VIDEO_H264_MAX_CHROMA_PLANES];
    signed char                                chroma_offset_l0[STD_VIDEO_H264_MAX_NUM_LIST_REF][STD_VIDEO_H264_MAX_CHROMA_PLANES];
    signed char                                luma_weight_l1[STD_VIDEO_H264_MAX_NUM_LIST_REF];
    signed char                                luma_offset_l1[STD_VIDEO_H264_MAX_NUM_LIST_REF];
    signed char                                chroma_weight_l1[STD_VIDEO_H264_MAX_NUM_LIST_REF][STD_VIDEO_H264_MAX_CHROMA_PLANES];
    signed char                                chroma_offset_l1[STD_VIDEO_H264_MAX_NUM_LIST_REF][STD_VIDEO_H264_MAX_CHROMA_PLANES];
} StdVideoEncodeH264WeightTable;

typedef struct StdVideoEncodeH264SliceHeaderFlags {
    unsigned int    direct_spatial_mv_pred_flag : 1;
    unsigned int    num_ref_idx_active_override_flag : 1;
    unsigned int    reserved : 30;
} StdVideoEncodeH264SliceHeaderFlags;

typedef struct StdVideoEncodeH264PictureInfoFlags {
    unsigned int    IdrPicFlag : 1;
    unsigned int    is_reference : 1;
    unsigned int    no_output_of_prior_pics_flag : 1;
    unsigned int    long_term_reference_flag : 1;
    unsigned int    adaptive_ref_pic_marking_mode_flag : 1;
    unsigned int    reserved : 27;
} StdVideoEncodeH264PictureInfoFlags;

typedef struct StdVideoEncodeH264ReferenceInfoFlags {
    unsigned int    used_for_long_term_reference : 1;
    unsigned int    reserved : 31;
} StdVideoEncodeH264ReferenceInfoFlags;

typedef struct StdVideoEncodeH264ReferenceListsInfoFlags {
    unsigned int    ref_pic_list_modification_flag_l0 : 1;
    unsigned int    ref_pic_list_modification_flag_l1 : 1;
    unsigned int    reserved : 30;
} StdVideoEncodeH264ReferenceListsInfoFlags;

typedef struct StdVideoEncodeH264RefListModEntry {
    StdVideoH264ModificationOfPicNumsIdc    modification_of_pic_nums_idc;
    unsigned short                                abs_diff_pic_num_minus1;
    unsigned short                                long_term_pic_num;
} StdVideoEncodeH264RefListModEntry;

typedef struct StdVideoEncodeH264RefPicMarkingEntry {
    StdVideoH264MemMgmtControlOp    memory_management_control_operation;
    unsigned short                        difference_of_pic_nums_minus1;
    unsigned short                        long_term_pic_num;
    unsigned short                        long_term_frame_idx;
    unsigned short                        max_long_term_frame_idx_plus1;
} StdVideoEncodeH264RefPicMarkingEntry;

typedef struct StdVideoEncodeH264ReferenceListsInfo {
    StdVideoEncodeH264ReferenceListsInfoFlags      flags;
    unsigned char                                        num_ref_idx_l0_active_minus1;
    unsigned char                                        num_ref_idx_l1_active_minus1;
    unsigned char                                        RefPicList0[STD_VIDEO_H264_MAX_NUM_LIST_REF];
    unsigned char                                        RefPicList1[STD_VIDEO_H264_MAX_NUM_LIST_REF];
    unsigned char                                        refList0ModOpCount;
    unsigned char                                        refList1ModOpCount;
    unsigned char                                        refPicMarkingOpCount;
    unsigned char                                        reserved1[7];
    const StdVideoEncodeH264RefListModEntry*       pRefList0ModOperations;
    const StdVideoEncodeH264RefListModEntry*       pRefList1ModOperations;
    const StdVideoEncodeH264RefPicMarkingEntry*    pRefPicMarkingOperations;
} StdVideoEncodeH264ReferenceListsInfo;

typedef struct StdVideoEncodeH264PictureInfo {
    StdVideoEncodeH264PictureInfoFlags             flags;
    unsigned char                                        seq_parameter_set_id;
    unsigned char                                        pic_parameter_set_id;
    unsigned short                                       idr_pic_id;
    StdVideoH264PictureType                        primary_pic_type;
    unsigned int                                       frame_num;
    int                                        PicOrderCnt;
    unsigned char                                        temporal_id;
    unsigned char                                        reserved1[3];
    const StdVideoEncodeH264ReferenceListsInfo*    pRefLists;
} StdVideoEncodeH264PictureInfo;

typedef struct StdVideoEncodeH264ReferenceInfo {
    StdVideoEncodeH264ReferenceInfoFlags    flags;
    StdVideoH264PictureType                 primary_pic_type;
    unsigned int                                FrameNum;
    int                                 PicOrderCnt;
    unsigned short                                long_term_pic_num;
    unsigned short                                long_term_frame_idx;
    unsigned char                                 temporal_id;
} StdVideoEncodeH264ReferenceInfo;

typedef struct StdVideoEncodeH264SliceHeader {
    StdVideoEncodeH264SliceHeaderFlags        flags;
    unsigned int                                  first_mb_in_slice;
    StdVideoH264SliceType                     slice_type;
    signed char                                    slice_alpha_c0_offset_div2;
    signed char                                    slice_beta_offset_div2;
    signed char                                    slice_qp_delta;
    unsigned char                                   reserved1;
    StdVideoH264CabacInitIdc                  cabac_init_idc;
    StdVideoH264DisableDeblockingFilterIdc    disable_deblocking_filter_idc;
    const StdVideoEncodeH264WeightTable*      pWeightTable;
} StdVideoEncodeH264SliceHeader;


#ifdef __cplusplus
}
#endif

#endif
