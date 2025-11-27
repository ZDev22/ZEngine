#ifndef VULKAN_VIDEO_CODEC_H265STD_H_
#define VULKAN_VIDEO_CODEC_H265STD_H_ 1

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



// vulkan_video_codec_h265std is a preprocessor guard. Do not pass it to API calls.
#define vulkan_video_codec_h265std 1
#include "vulkan_video_codecs_common.h"
#define STD_VIDEO_H265_CPB_CNT_LIST_SIZE  32
#define STD_VIDEO_H265_SUBLAYERS_LIST_SIZE 7
#define STD_VIDEO_H265_SCALING_LIST_4X4_NUM_LISTS 6
#define STD_VIDEO_H265_SCALING_LIST_4X4_NUM_ELEMENTS 16
#define STD_VIDEO_H265_SCALING_LIST_8X8_NUM_LISTS 6
#define STD_VIDEO_H265_SCALING_LIST_8X8_NUM_ELEMENTS 64
#define STD_VIDEO_H265_SCALING_LIST_16X16_NUM_LISTS 6
#define STD_VIDEO_H265_SCALING_LIST_16X16_NUM_ELEMENTS 64
#define STD_VIDEO_H265_SCALING_LIST_32X32_NUM_LISTS 2
#define STD_VIDEO_H265_SCALING_LIST_32X32_NUM_ELEMENTS 64
#define STD_VIDEO_H265_CHROMA_QP_OFFSET_LIST_SIZE 6
#define STD_VIDEO_H265_CHROMA_QP_OFFSET_TILE_COLS_LIST_SIZE 19
#define STD_VIDEO_H265_CHROMA_QP_OFFSET_TILE_ROWS_LIST_SIZE 21
#define STD_VIDEO_H265_PREDICTOR_PALETTE_COMPONENTS_LIST_SIZE 3
#define STD_VIDEO_H265_PREDICTOR_PALETTE_COMP_ENTRIES_LIST_SIZE 128
#define STD_VIDEO_H265_MAX_NUM_LIST_REF   15
#define STD_VIDEO_H265_MAX_CHROMA_PLANES  2
#define STD_VIDEO_H265_MAX_SHORT_TERM_REF_PIC_SETS 64
#define STD_VIDEO_H265_MAX_DPB_SIZE       16
#define STD_VIDEO_H265_MAX_LONG_TERM_REF_PICS_SPS 32
#define STD_VIDEO_H265_MAX_LONG_TERM_PICS 16
#define STD_VIDEO_H265_MAX_DELTA_POC      48
#define STD_VIDEO_H265_NO_REFERENCE_PICTURE 0xFF

typedef enum StdVideoH265ChromaFormatIdc {
    STD_VIDEO_H265_CHROMA_FORMAT_IDC_MONOCHROME = 0,
    STD_VIDEO_H265_CHROMA_FORMAT_IDC_420 = 1,
    STD_VIDEO_H265_CHROMA_FORMAT_IDC_422 = 2,
    STD_VIDEO_H265_CHROMA_FORMAT_IDC_444 = 3,
    STD_VIDEO_H265_CHROMA_FORMAT_IDC_INVALID = 0x7FFFFFFF,
    STD_VIDEO_H265_CHROMA_FORMAT_IDC_MAX_ENUM = 0x7FFFFFFF
} StdVideoH265ChromaFormatIdc;

typedef enum StdVideoH265ProfileIdc {
    STD_VIDEO_H265_PROFILE_IDC_MAIN = 1,
    STD_VIDEO_H265_PROFILE_IDC_MAIN_10 = 2,
    STD_VIDEO_H265_PROFILE_IDC_MAIN_STILL_PICTURE = 3,
    STD_VIDEO_H265_PROFILE_IDC_FORMAT_RANGE_EXTENSIONS = 4,
    STD_VIDEO_H265_PROFILE_IDC_SCC_EXTENSIONS = 9,
    STD_VIDEO_H265_PROFILE_IDC_INVALID = 0x7FFFFFFF,
    STD_VIDEO_H265_PROFILE_IDC_MAX_ENUM = 0x7FFFFFFF
} StdVideoH265ProfileIdc;

typedef enum StdVideoH265LevelIdc {
    STD_VIDEO_H265_LEVEL_IDC_1_0 = 0,
    STD_VIDEO_H265_LEVEL_IDC_2_0 = 1,
    STD_VIDEO_H265_LEVEL_IDC_2_1 = 2,
    STD_VIDEO_H265_LEVEL_IDC_3_0 = 3,
    STD_VIDEO_H265_LEVEL_IDC_3_1 = 4,
    STD_VIDEO_H265_LEVEL_IDC_4_0 = 5,
    STD_VIDEO_H265_LEVEL_IDC_4_1 = 6,
    STD_VIDEO_H265_LEVEL_IDC_5_0 = 7,
    STD_VIDEO_H265_LEVEL_IDC_5_1 = 8,
    STD_VIDEO_H265_LEVEL_IDC_5_2 = 9,
    STD_VIDEO_H265_LEVEL_IDC_6_0 = 10,
    STD_VIDEO_H265_LEVEL_IDC_6_1 = 11,
    STD_VIDEO_H265_LEVEL_IDC_6_2 = 12,
    STD_VIDEO_H265_LEVEL_IDC_INVALID = 0x7FFFFFFF,
    STD_VIDEO_H265_LEVEL_IDC_MAX_ENUM = 0x7FFFFFFF
} StdVideoH265LevelIdc;

typedef enum StdVideoH265SliceType {
    STD_VIDEO_H265_SLICE_TYPE_B = 0,
    STD_VIDEO_H265_SLICE_TYPE_P = 1,
    STD_VIDEO_H265_SLICE_TYPE_I = 2,
    STD_VIDEO_H265_SLICE_TYPE_INVALID = 0x7FFFFFFF,
    STD_VIDEO_H265_SLICE_TYPE_MAX_ENUM = 0x7FFFFFFF
} StdVideoH265SliceType;

typedef enum StdVideoH265PictureType {
    STD_VIDEO_H265_PICTURE_TYPE_P = 0,
    STD_VIDEO_H265_PICTURE_TYPE_B = 1,
    STD_VIDEO_H265_PICTURE_TYPE_I = 2,
    STD_VIDEO_H265_PICTURE_TYPE_IDR = 3,
    STD_VIDEO_H265_PICTURE_TYPE_INVALID = 0x7FFFFFFF,
    STD_VIDEO_H265_PICTURE_TYPE_MAX_ENUM = 0x7FFFFFFF
} StdVideoH265PictureType;

typedef enum StdVideoH265AspectRatioIdc {
    STD_VIDEO_H265_ASPECT_RATIO_IDC_UNSPECIFIED = 0,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_SQUARE = 1,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_12_11 = 2,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_10_11 = 3,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_16_11 = 4,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_40_33 = 5,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_24_11 = 6,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_20_11 = 7,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_32_11 = 8,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_80_33 = 9,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_18_11 = 10,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_15_11 = 11,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_64_33 = 12,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_160_99 = 13,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_4_3 = 14,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_3_2 = 15,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_2_1 = 16,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_EXTENDED_SAR = 255,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_INVALID = 0x7FFFFFFF,
    STD_VIDEO_H265_ASPECT_RATIO_IDC_MAX_ENUM = 0x7FFFFFFF
} StdVideoH265AspectRatioIdc;
typedef struct StdVideoH265DecPicBufMgr {
    unsigned int    max_latency_increase_plus1[STD_VIDEO_H265_SUBLAYERS_LIST_SIZE];
    unsigned char     max_dec_pic_buffering_minus1[STD_VIDEO_H265_SUBLAYERS_LIST_SIZE];
    unsigned char     max_num_reorder_pics[STD_VIDEO_H265_SUBLAYERS_LIST_SIZE];
} StdVideoH265DecPicBufMgr;

typedef struct StdVideoH265SubLayerHrdParameters {
    unsigned int    bit_rate_value_minus1[STD_VIDEO_H265_CPB_CNT_LIST_SIZE];
    unsigned int    cpb_size_value_minus1[STD_VIDEO_H265_CPB_CNT_LIST_SIZE];
    unsigned int    cpb_size_du_value_minus1[STD_VIDEO_H265_CPB_CNT_LIST_SIZE];
    unsigned int    bit_rate_du_value_minus1[STD_VIDEO_H265_CPB_CNT_LIST_SIZE];
    unsigned int    cbr_flag;
} StdVideoH265SubLayerHrdParameters;

typedef struct StdVideoH265HrdFlags {
    unsigned int    nal_hrd_parameters_present_flag : 1;
    unsigned int    vcl_hrd_parameters_present_flag : 1;
    unsigned int    sub_pic_hrd_params_present_flag : 1;
    unsigned int    sub_pic_cpb_params_in_pic_timing_sei_flag : 1;
    unsigned int    fixed_pic_rate_general_flag : 8;
    unsigned int    fixed_pic_rate_within_cvs_flag : 8;
    unsigned int    low_delay_hrd_flag : 8;
} StdVideoH265HrdFlags;

typedef struct StdVideoH265HrdParameters {
    StdVideoH265HrdFlags                        flags;
    unsigned char                                     tick_divisor_minus2;
    unsigned char                                     du_cpb_removal_delay_increment_length_minus1;
    unsigned char                                     dpb_output_delay_du_length_minus1;
    unsigned char                                     bit_rate_scale;
    unsigned char                                     cpb_size_scale;
    unsigned char                                     cpb_size_du_scale;
    unsigned char                                     initial_cpb_removal_delay_length_minus1;
    unsigned char                                     au_cpb_removal_delay_length_minus1;
    unsigned char                                     dpb_output_delay_length_minus1;
    unsigned char                                     cpb_cnt_minus1[STD_VIDEO_H265_SUBLAYERS_LIST_SIZE];
    unsigned short                                    elemental_duration_in_tc_minus1[STD_VIDEO_H265_SUBLAYERS_LIST_SIZE];
    unsigned short                                    reserved[3];
    const StdVideoH265SubLayerHrdParameters*    pSubLayerHrdParametersNal;
    const StdVideoH265SubLayerHrdParameters*    pSubLayerHrdParametersVcl;
} StdVideoH265HrdParameters;

typedef struct StdVideoH265VpsFlags {
    unsigned int    vps_temporal_id_nesting_flag : 1;
    unsigned int    vps_sub_layer_ordering_info_present_flag : 1;
    unsigned int    vps_timing_info_present_flag : 1;
    unsigned int    vps_poc_proportional_to_timing_flag : 1;
} StdVideoH265VpsFlags;

typedef struct StdVideoH265ProfileTierLevelFlags {
    unsigned int    general_tier_flag : 1;
    unsigned int    general_progressive_source_flag : 1;
    unsigned int    general_interlaced_source_flag : 1;
    unsigned int    general_non_packed_constraint_flag : 1;
    unsigned int    general_frame_only_constraint_flag : 1;
} StdVideoH265ProfileTierLevelFlags;

typedef struct StdVideoH265ProfileTierLevel {
    StdVideoH265ProfileTierLevelFlags    flags;
    StdVideoH265ProfileIdc               general_profile_idc;
    StdVideoH265LevelIdc                 general_level_idc;
} StdVideoH265ProfileTierLevel;

typedef struct StdVideoH265VideoParameterSet {
    StdVideoH265VpsFlags                   flags;
    unsigned char                                vps_video_parameter_set_id;
    unsigned char                                vps_max_sub_layers_minus1;
    unsigned char                                reserved1;
    unsigned char                                reserved2;
    unsigned int                               vps_num_units_in_tick;
    unsigned int                               vps_time_scale;
    unsigned int                               vps_num_ticks_poc_diff_one_minus1;
    unsigned int                               reserved3;
    const StdVideoH265DecPicBufMgr*        pDecPicBufMgr;
    const StdVideoH265HrdParameters*       pHrdParameters;
    const StdVideoH265ProfileTierLevel*    pProfileTierLevel;
} StdVideoH265VideoParameterSet;

typedef struct StdVideoH265ScalingLists {
    unsigned char    ScalingList4x4[STD_VIDEO_H265_SCALING_LIST_4X4_NUM_LISTS][STD_VIDEO_H265_SCALING_LIST_4X4_NUM_ELEMENTS];
    unsigned char    ScalingList8x8[STD_VIDEO_H265_SCALING_LIST_8X8_NUM_LISTS][STD_VIDEO_H265_SCALING_LIST_8X8_NUM_ELEMENTS];
    unsigned char    ScalingList16x16[STD_VIDEO_H265_SCALING_LIST_16X16_NUM_LISTS][STD_VIDEO_H265_SCALING_LIST_16X16_NUM_ELEMENTS];
    unsigned char    ScalingList32x32[STD_VIDEO_H265_SCALING_LIST_32X32_NUM_LISTS][STD_VIDEO_H265_SCALING_LIST_32X32_NUM_ELEMENTS];
    unsigned char    ScalingListDCCoef16x16[STD_VIDEO_H265_SCALING_LIST_16X16_NUM_LISTS];
    unsigned char    ScalingListDCCoef32x32[STD_VIDEO_H265_SCALING_LIST_32X32_NUM_LISTS];
} StdVideoH265ScalingLists;

typedef struct StdVideoH265SpsVuiFlags {
    unsigned int    aspect_ratio_info_present_flag : 1;
    unsigned int    overscan_info_present_flag : 1;
    unsigned int    overscan_appropriate_flag : 1;
    unsigned int    video_signal_type_present_flag : 1;
    unsigned int    video_full_range_flag : 1;
    unsigned int    colour_description_present_flag : 1;
    unsigned int    chroma_loc_info_present_flag : 1;
    unsigned int    neutral_chroma_indication_flag : 1;
    unsigned int    field_seq_flag : 1;
    unsigned int    frame_field_info_present_flag : 1;
    unsigned int    default_display_window_flag : 1;
    unsigned int    vui_timing_info_present_flag : 1;
    unsigned int    vui_poc_proportional_to_timing_flag : 1;
    unsigned int    vui_hrd_parameters_present_flag : 1;
    unsigned int    bitstream_restriction_flag : 1;
    unsigned int    tiles_fixed_structure_flag : 1;
    unsigned int    motion_vectors_over_pic_boundaries_flag : 1;
    unsigned int    restricted_ref_pic_lists_flag : 1;
} StdVideoH265SpsVuiFlags;

typedef struct StdVideoH265SequenceParameterSetVui {
    StdVideoH265SpsVuiFlags             flags;
    StdVideoH265AspectRatioIdc          aspect_ratio_idc;
    unsigned short                            sar_width;
    unsigned short                            sar_height;
    unsigned char                             video_format;
    unsigned char                             colour_primaries;
    unsigned char                             transfer_characteristics;
    unsigned char                             matrix_coeffs;
    unsigned char                             chroma_sample_loc_type_top_field;
    unsigned char                             chroma_sample_loc_type_bottom_field;
    unsigned char                             reserved1;
    unsigned char                             reserved2;
    unsigned short                            def_disp_win_left_offset;
    unsigned short                            def_disp_win_right_offset;
    unsigned short                            def_disp_win_top_offset;
    unsigned short                            def_disp_win_bottom_offset;
    unsigned int                            vui_num_units_in_tick;
    unsigned int                            vui_time_scale;
    unsigned int                            vui_num_ticks_poc_diff_one_minus1;
    unsigned short                            min_spatial_segmentation_idc;
    unsigned short                            reserved3;
    unsigned char                             max_bytes_per_pic_denom;
    unsigned char                             max_bits_per_min_cu_denom;
    unsigned char                             log2_max_mv_length_horizontal;
    unsigned char                             log2_max_mv_length_vertical;
    const StdVideoH265HrdParameters*    pHrdParameters;
} StdVideoH265SequenceParameterSetVui;

typedef struct StdVideoH265PredictorPaletteEntries {
    unsigned short    PredictorPaletteEntries[STD_VIDEO_H265_PREDICTOR_PALETTE_COMPONENTS_LIST_SIZE][STD_VIDEO_H265_PREDICTOR_PALETTE_COMP_ENTRIES_LIST_SIZE];
} StdVideoH265PredictorPaletteEntries;

typedef struct StdVideoH265SpsFlags {
    unsigned int    sps_temporal_id_nesting_flag : 1;
    unsigned int    separate_colour_plane_flag : 1;
    unsigned int    conformance_window_flag : 1;
    unsigned int    sps_sub_layer_ordering_info_present_flag : 1;
    unsigned int    scaling_list_enabled_flag : 1;
    unsigned int    sps_scaling_list_data_present_flag : 1;
    unsigned int    amp_enabled_flag : 1;
    unsigned int    sample_adaptive_offset_enabled_flag : 1;
    unsigned int    pcm_enabled_flag : 1;
    unsigned int    pcm_loop_filter_disabled_flag : 1;
    unsigned int    long_term_ref_pics_present_flag : 1;
    unsigned int    sps_temporal_mvp_enabled_flag : 1;
    unsigned int    strong_intra_smoothing_enabled_flag : 1;
    unsigned int    vui_parameters_present_flag : 1;
    unsigned int    sps_extension_present_flag : 1;
    unsigned int    sps_range_extension_flag : 1;
    unsigned int    transform_skip_rotation_enabled_flag : 1;
    unsigned int    transform_skip_context_enabled_flag : 1;
    unsigned int    implicit_rdpcm_enabled_flag : 1;
    unsigned int    explicit_rdpcm_enabled_flag : 1;
    unsigned int    extended_precision_processing_flag : 1;
    unsigned int    intra_smoothing_disabled_flag : 1;
    unsigned int    high_precision_offsets_enabled_flag : 1;
    unsigned int    persistent_rice_adaptation_enabled_flag : 1;
    unsigned int    cabac_bypass_alignment_enabled_flag : 1;
    unsigned int    sps_scc_extension_flag : 1;
    unsigned int    sps_curr_pic_ref_enabled_flag : 1;
    unsigned int    palette_mode_enabled_flag : 1;
    unsigned int    sps_palette_predictor_initializers_present_flag : 1;
    unsigned int    intra_boundary_filtering_disabled_flag : 1;
} StdVideoH265SpsFlags;

typedef struct StdVideoH265ShortTermRefPicSetFlags {
    unsigned int    inter_ref_pic_set_prediction_flag : 1;
    unsigned int    delta_rps_sign : 1;
} StdVideoH265ShortTermRefPicSetFlags;

typedef struct StdVideoH265ShortTermRefPicSet {
    StdVideoH265ShortTermRefPicSetFlags    flags;
    unsigned int                               delta_idx_minus1;
    unsigned short                               use_delta_flag;
    unsigned short                               abs_delta_rps_minus1;
    unsigned short                               used_by_curr_pic_flag;
    unsigned short                               used_by_curr_pic_s0_flag;
    unsigned short                               used_by_curr_pic_s1_flag;
    unsigned short                               reserved1;
    unsigned char                                reserved2;
    unsigned char                                reserved3;
    unsigned char                                num_negative_pics;
    unsigned char                                num_positive_pics;
    unsigned short                               delta_poc_s0_minus1[STD_VIDEO_H265_MAX_DPB_SIZE];
    unsigned short                               delta_poc_s1_minus1[STD_VIDEO_H265_MAX_DPB_SIZE];
} StdVideoH265ShortTermRefPicSet;

typedef struct StdVideoH265LongTermRefPicsSps {
    unsigned int    used_by_curr_pic_lt_sps_flag;
    unsigned int    lt_ref_pic_poc_lsb_sps[STD_VIDEO_H265_MAX_LONG_TERM_REF_PICS_SPS];
} StdVideoH265LongTermRefPicsSps;

typedef struct StdVideoH265SequenceParameterSet {
    StdVideoH265SpsFlags                          flags;
    StdVideoH265ChromaFormatIdc                   chroma_format_idc;
    unsigned int                                      pic_width_in_luma_samples;
    unsigned int                                      pic_height_in_luma_samples;
    unsigned char                                       sps_video_parameter_set_id;
    unsigned char                                       sps_max_sub_layers_minus1;
    unsigned char                                       sps_seq_parameter_set_id;
    unsigned char                                       bit_depth_luma_minus8;
    unsigned char                                       bit_depth_chroma_minus8;
    unsigned char                                       log2_max_pic_order_cnt_lsb_minus4;
    unsigned char                                       log2_min_luma_coding_block_size_minus3;
    unsigned char                                       log2_diff_max_min_luma_coding_block_size;
    unsigned char                                       log2_min_luma_transform_block_size_minus2;
    unsigned char                                       log2_diff_max_min_luma_transform_block_size;
    unsigned char                                       max_transform_hierarchy_depth_inter;
    unsigned char                                       max_transform_hierarchy_depth_intra;
    unsigned char                                       num_short_term_ref_pic_sets;
    unsigned char                                       num_long_term_ref_pics_sps;
    unsigned char                                       pcm_sample_bit_depth_luma_minus1;
    unsigned char                                       pcm_sample_bit_depth_chroma_minus1;
    unsigned char                                       log2_min_pcm_luma_coding_block_size_minus3;
    unsigned char                                       log2_diff_max_min_pcm_luma_coding_block_size;
    unsigned char                                       reserved1;
    unsigned char                                       reserved2;
    unsigned char                                       palette_max_size;
    unsigned char                                       delta_palette_max_predictor_size;
    unsigned char                                       motion_vector_resolution_control_idc;
    unsigned char                                       sps_num_palette_predictor_initializers_minus1;
    unsigned int                                      conf_win_left_offset;
    unsigned int                                      conf_win_right_offset;
    unsigned int                                      conf_win_top_offset;
    unsigned int                                      conf_win_bottom_offset;
    const StdVideoH265ProfileTierLevel*           pProfileTierLevel;
    const StdVideoH265DecPicBufMgr*               pDecPicBufMgr;
    const StdVideoH265ScalingLists*               pScalingLists;
    const StdVideoH265ShortTermRefPicSet*         pShortTermRefPicSet;
    const StdVideoH265LongTermRefPicsSps*         pLongTermRefPicsSps;
    const StdVideoH265SequenceParameterSetVui*    pSequenceParameterSetVui;
    const StdVideoH265PredictorPaletteEntries*    pPredictorPaletteEntries;
} StdVideoH265SequenceParameterSet;

typedef struct StdVideoH265PpsFlags {
    unsigned int    dependent_slice_segments_enabled_flag : 1;
    unsigned int    output_flag_present_flag : 1;
    unsigned int    sign_data_hiding_enabled_flag : 1;
    unsigned int    cabac_init_present_flag : 1;
    unsigned int    constrained_intra_pred_flag : 1;
    unsigned int    transform_skip_enabled_flag : 1;
    unsigned int    cu_qp_delta_enabled_flag : 1;
    unsigned int    pps_slice_chroma_qp_offsets_present_flag : 1;
    unsigned int    weighted_pred_flag : 1;
    unsigned int    weighted_bipred_flag : 1;
    unsigned int    transquant_bypass_enabled_flag : 1;
    unsigned int    tiles_enabled_flag : 1;
    unsigned int    entropy_coding_sync_enabled_flag : 1;
    unsigned int    uniform_spacing_flag : 1;
    unsigned int    loop_filter_across_tiles_enabled_flag : 1;
    unsigned int    pps_loop_filter_across_slices_enabled_flag : 1;
    unsigned int    deblocking_filter_control_present_flag : 1;
    unsigned int    deblocking_filter_override_enabled_flag : 1;
    unsigned int    pps_deblocking_filter_disabled_flag : 1;
    unsigned int    pps_scaling_list_data_present_flag : 1;
    unsigned int    lists_modification_present_flag : 1;
    unsigned int    slice_segment_header_extension_present_flag : 1;
    unsigned int    pps_extension_present_flag : 1;
    unsigned int    cross_component_prediction_enabled_flag : 1;
    unsigned int    chroma_qp_offset_list_enabled_flag : 1;
    unsigned int    pps_curr_pic_ref_enabled_flag : 1;
    unsigned int    residual_adaptive_colour_transform_enabled_flag : 1;
    unsigned int    pps_slice_act_qp_offsets_present_flag : 1;
    unsigned int    pps_palette_predictor_initializers_present_flag : 1;
    unsigned int    monochrome_palette_flag : 1;
    unsigned int    pps_range_extension_flag : 1;
} StdVideoH265PpsFlags;

typedef struct StdVideoH265PictureParameterSet {
    StdVideoH265PpsFlags                          flags;
    unsigned char                                       pps_pic_parameter_set_id;
    unsigned char                                       pps_seq_parameter_set_id;
    unsigned char                                       sps_video_parameter_set_id;
    unsigned char                                       num_extra_slice_header_bits;
    unsigned char                                       num_ref_idx_l0_default_active_minus1;
    unsigned char                                       num_ref_idx_l1_default_active_minus1;
    signed char                                        init_qp_minus26;
    unsigned char                                       diff_cu_qp_delta_depth;
    signed char                                        pps_cb_qp_offset;
    signed char                                        pps_cr_qp_offset;
    signed char                                        pps_beta_offset_div2;
    signed char                                        pps_tc_offset_div2;
    unsigned char                                       log2_parallel_merge_level_minus2;
    unsigned char                                       log2_max_transform_skip_block_size_minus2;
    unsigned char                                       diff_cu_chroma_qp_offset_depth;
    unsigned char                                       chroma_qp_offset_list_len_minus1;
    signed char                                        cb_qp_offset_list[STD_VIDEO_H265_CHROMA_QP_OFFSET_LIST_SIZE];
    signed char                                        cr_qp_offset_list[STD_VIDEO_H265_CHROMA_QP_OFFSET_LIST_SIZE];
    unsigned char                                       log2_sao_offset_scale_luma;
    unsigned char                                       log2_sao_offset_scale_chroma;
    signed char                                        pps_act_y_qp_offset_plus5;
    signed char                                        pps_act_cb_qp_offset_plus5;
    signed char                                        pps_act_cr_qp_offset_plus3;
    unsigned char                                       pps_num_palette_predictor_initializers;
    unsigned char                                       luma_bit_depth_entry_minus8;
    unsigned char                                       chroma_bit_depth_entry_minus8;
    unsigned char                                       num_tile_columns_minus1;
    unsigned char                                       num_tile_rows_minus1;
    unsigned char                                       reserved1;
    unsigned char                                       reserved2;
    unsigned short                                      column_width_minus1[STD_VIDEO_H265_CHROMA_QP_OFFSET_TILE_COLS_LIST_SIZE];
    unsigned short                                      row_height_minus1[STD_VIDEO_H265_CHROMA_QP_OFFSET_TILE_ROWS_LIST_SIZE];
    unsigned int                                      reserved3;
    const StdVideoH265ScalingLists*               pScalingLists;
    const StdVideoH265PredictorPaletteEntries*    pPredictorPaletteEntries;
} StdVideoH265PictureParameterSet;


#ifdef __cplusplus
}
#endif

#endif
