/* ZAudio uses minimp3 to decode files and sokol_audio to play them.
 * It inlines all necessary functions from minimp3 and minimp3_ex.h to not have as many dependencys.
 *
 * #define ZAUDIO_MAX_SOUNDS 10 - how many sounds can play at once. (default: 10)
 */

#ifndef ZAUDIO_H
#define ZAUDIO_H

#ifdef __cplusplus
    extern "C" {
#endif

void ZAudioInit(void);
void ZAudioPlay(const char* filepath);
void ZAudioTick(void);
void ZAudioDeinit(void);

#ifdef ZAUDIO_IMPLEMENTATION

/* std */
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <immintrin.h>

#ifndef ZAUDIO_MAX_SOUNDS
#define ZAUDIO_MAX_SOUNDS 10
#endif // ZAUDIO_MAX_SOUNDS

#define ZAUDIO_SAMPLE_RATE 44100
#define ZAUDIO_CHANNELS 1
#define ZAUDIO_BUFFER_FRAMES 2048

/* SIMD */
#if defined(__ARM_NEON) || defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#define HAVE_SSE 0
#define HAVE_SIMD 1
#define VSTORE vst1q_f32
#define VLD vld1q_f32
#define VSET vmovq_n_f32
#define VADD vaddq_f32
#define VSUB vsubq_f32
#define VMUL vmulq_f32
#define VMAC(a, x, y) vmlaq_f32(a, x, y)
#define VMSB(a, x, y) vmlsq_f32(a, x, y)
#define VMUL_S(x, s)  vmulq_f32(x, vmovq_n_f32(s))
#define VREV(x) vcombine_f32(vget_high_f32(vrev64q_f32(x)), vget_low_f32(vrev64q_f32(x)))
typedef float32x4_t f4;
#else
#define HAVE_SSE 0
#define HAVE_SIMD 0
#endif // _ARM_NEON

/* minimp3 */
#define MINIMP3_MAX_SAMPLES_PER_FRAME (1152*2)
#define HDR_IS_CRC(h)                 (!((h[1]) & 1))
#define HDR_SIZE                      4
#define MAX_FREE_FORMAT_FRAME_SIZE    2304    /* more than ISO spec's */
#define MINIMP3_MIN(a, b)             ((a) > (b) ? (b) : (a))
#define MINIMP3_MAX(a, b)             ((a) < (b) ? (b) : (a))
#define HDR_IS_MONO(h)                (((h[3]) & 0xC0) == 0xC0)
#define HDR_GET_LAYER(h)              (((h[1]) >> 1) & 3)
#define HDR_GET_SAMPLE_RATE(h)        (((h[2]) >> 2) & 3)
#define HDR_TEST_MPEG1(h)             ((h[1]) & 0x8)
#define HDR_TEST_NOT_MPEG25(h)        ((h[1]) & 0x10)
#define HDR_TEST_I_STEREO(h)          ((h[3]) & 0x10)
#define MAX_BITRESERVOIR_BYTES        511
#define HDR_IS_FREE_FORMAT(h)         (((h[2]) & 0xF0) == 0)
#define HDR_GET_BITRATE(h)            ((h[2]) >> 4)
#define HDR_IS_LAYER_1(h)             ((h[1] & 6) == 6)
#define HDR_IS_FRAME_576(h)           ((h[1] & 14) == 2)
#define HDR_TEST_PADDING(h)           ((h[2]) & 0x2)
#define HDR_GET_MY_SAMPLE_RATE(h)     (HDR_GET_SAMPLE_RATE(h) + (((h[1] >> 3) & 1) + ((h[1] >> 4) & 1))*3)
#define MAX_SCF                       41
#define MAX_SCFI                      (45 & ~3)

#ifndef MAX_FRAME_SYNC_MATCHES
#define MAX_FRAME_SYNC_MATCHES 10
#endif // MAX_FRAME_SYNC_MATCHES

typedef struct {
    int frame_bytes, frame_offset, channels, hz, layer, bitrate_kbps;
} mp3dec_frame_info_t;

typedef struct {
    float mdct_overlap[2][9*32], qmf_state[15*2*32];
    int reserv, free_format_bytes;
    unsigned char header[4], reserv_buf[511];
} mp3dec_t;

typedef struct {
    const uint8_t *buf;
    int pos, limit;
} bs_t;

typedef struct {
    const uint8_t *sfbtab;
    uint16_t part_23_length, big_values, scalefac_compress;
    uint8_t global_gain, block_type, mixed_block_flag, n_long_sfb, n_short_sfb;
    uint8_t table_select[3], region_count[3], subblock_gain[3];
    uint8_t preflag, scalefac_scale, count1_table, scfsi;
} L3_gr_info_t;

typedef struct {
    bs_t bs;
    uint8_t maindata[MAX_BITRESERVOIR_BYTES + MAX_FREE_FORMAT_FRAME_SIZE];
    L3_gr_info_t gr_info[4];
    float grbuf[2][576], scf[40], syn[18 + 15][2*32];
    uint8_t ist_pos[2][39];
} mp3dec_scratch_t;

static unsigned hdr_sample_rate_hz(const uint8_t *h) {
    static const unsigned g_hz[3] = { 44100, 48000, 32000 };
    return g_hz[HDR_GET_SAMPLE_RATE(h)] >> (int)!HDR_TEST_MPEG1(h) >> (int)!HDR_TEST_NOT_MPEG25(h);
}

static int hdr_valid(const uint8_t *h) {
    return h[0] == 0xff && ((h[1] & 0xF0) == 0xf0 || (h[1] & 0xFE) == 0xe2) && (HDR_GET_LAYER(h) != 0) && (HDR_GET_BITRATE(h) != 15) && (HDR_GET_SAMPLE_RATE(h) != 3);
}

static int hdr_compare(const uint8_t *h1, const uint8_t *h2) {
    return hdr_valid(h2) && ((h1[1] ^ h2[1]) & 0xFE) == 0 && ((h1[2] ^ h2[2]) & 0x0C) == 0 && !(HDR_IS_FREE_FORMAT(h1) ^ HDR_IS_FREE_FORMAT(h2));
}

static unsigned hdr_bitrate_kbps(const uint8_t *h) {
    static const uint8_t halfrate[2][3][15] = {
        { { 0,4,8,12,16,20,24,28,32,40,48,56,64,72,80 }, { 0,4,8,12,16,20,24,28,32,40,48,56,64,72,80 }, { 0,16,24,28,32,40,48,56,64,72,80,88,96,112,128 } },
        { { 0,16,20,24,28,32,40,48,56,64,80,96,112,128,160 }, { 0,16,24,28,32,40,48,56,64,80,96,112,128,160,192 }, { 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224 } },
    };
    return 2*halfrate[!!HDR_TEST_MPEG1(h)][HDR_GET_LAYER(h) - 1][HDR_GET_BITRATE(h)];
}

static unsigned hdr_frame_samples(const uint8_t *h) {
    return HDR_IS_LAYER_1(h) ? 384 : (1152 >> (int)HDR_IS_FRAME_576(h));
}

static int hdr_frame_bytes(const uint8_t *h, int free_format_size) {
    int frame_bytes = hdr_frame_samples(h)*hdr_bitrate_kbps(h)*125/hdr_sample_rate_hz(h);
    if (HDR_IS_LAYER_1(h)) { frame_bytes &= ~3; }
    return frame_bytes ? frame_bytes : free_format_size;
}

static int hdr_padding(const uint8_t *h) {
    return HDR_TEST_PADDING(h) ? (HDR_IS_LAYER_1(h) ? 4 : 1) : 0;
}

static int mp3d_match_frame(const uint8_t *hdr, int mp3_bytes, int frame_bytes) {
    int i, nmatch;
    for (i = 0, nmatch = 0; nmatch < MAX_FRAME_SYNC_MATCHES; nmatch++) {
        i += hdr_frame_bytes(hdr + i, frame_bytes) + hdr_padding(hdr + i);
        if (i + HDR_SIZE > mp3_bytes)
            return nmatch > 0;
        if (!hdr_compare(hdr, hdr + i))
            return 0;
    }
    return 1;
}

static int mp3d_find_frame(const uint8_t *mp3, int mp3_bytes, int *free_format_bytes, int *ptr_frame_bytes) {
    int i, k;
    for (i = 0; i < mp3_bytes - HDR_SIZE; i++, mp3++) {
        if (hdr_valid(mp3)) {
            int frame_bytes = hdr_frame_bytes(mp3, *free_format_bytes);
            int frame_and_padding = frame_bytes + hdr_padding(mp3);

            for (k = HDR_SIZE; !frame_bytes && k < MAX_FREE_FORMAT_FRAME_SIZE && i + 2*k < mp3_bytes - HDR_SIZE; k++) {
                if (hdr_compare(mp3, mp3 + k)) {
                    int fb = k - hdr_padding(mp3);
                    int nextfb = fb + hdr_padding(mp3 + k);
                    if (i + k + nextfb + HDR_SIZE > mp3_bytes || !hdr_compare(mp3, mp3 + k + nextfb))
                        continue;
                    frame_and_padding = k;
                    frame_bytes = fb;
                    *free_format_bytes = fb;
                }
            }
            if ((frame_bytes && i + frame_and_padding <= mp3_bytes && mp3d_match_frame(mp3, mp3_bytes - i, frame_bytes)) || (!i && frame_and_padding == mp3_bytes)) {
                *ptr_frame_bytes = frame_and_padding;
                return i;
            }
            *free_format_bytes = 0;
        }
    }
    *ptr_frame_bytes = 0;
    return mp3_bytes;
}

static uint32_t get_bits(bs_t *bs, int n) {
    uint32_t next, cache = 0, s = bs->pos & 7;
    int shl = n + s;
    const uint8_t *p = bs->buf + (bs->pos >> 3);
    if ((bs->pos += n) > bs->limit)
        return 0;
    next = *p++ & (255 >> s);
    while ((shl -= 8) > 0) {
        cache |= next << shl;
        next = *p++;
    }
    return cache | (next >> -shl);
}

static int L3_read_side_info(bs_t *bs, L3_gr_info_t *gr, const uint8_t *hdr) {
    static const uint8_t g_scf_long[8][23] = {
        { 6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54,0 },
        { 12,12,12,12,12,12,16,20,24,28,32,40,48,56,64,76,90,2,2,2,2,2,0 },
        { 6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54,0 },
        { 6,6,6,6,6,6,8,10,12,14,16,18,22,26,32,38,46,54,62,70,76,36,0 },
        { 6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54,0 },
        { 4,4,4,4,4,4,6,6,8,8,10,12,16,20,24,28,34,42,50,54,76,158,0 },
        { 4,4,4,4,4,4,6,6,6,8,10,12,16,18,22,28,34,40,46,54,54,192,0 },
        { 4,4,4,4,4,4,6,6,8,10,12,16,20,24,30,38,46,56,68,84,102,26,0 }
    };
    static const uint8_t g_scf_short[8][40] = {
        { 4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
        { 8,8,8,8,8,8,8,8,8,12,12,12,16,16,16,20,20,20,24,24,24,28,28,28,36,36,36,2,2,2,2,2,2,2,2,2,26,26,26,0 },
        { 4,4,4,4,4,4,4,4,4,6,6,6,6,6,6,8,8,8,10,10,10,14,14,14,18,18,18,26,26,26,32,32,32,42,42,42,18,18,18,0 },
        { 4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,32,32,32,44,44,44,12,12,12,0 },
        { 4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
        { 4,4,4,4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,22,22,22,30,30,30,56,56,56,0 },
        { 4,4,4,4,4,4,4,4,4,4,4,4,6,6,6,6,6,6,10,10,10,12,12,12,14,14,14,16,16,16,20,20,20,26,26,26,66,66,66,0 },
        { 4,4,4,4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,12,12,12,16,16,16,20,20,20,26,26,26,34,34,34,42,42,42,12,12,12,0 }
    };
    static const uint8_t g_scf_mixed[8][40] = {
        { 6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
        { 12,12,12,4,4,4,8,8,8,12,12,12,16,16,16,20,20,20,24,24,24,28,28,28,36,36,36,2,2,2,2,2,2,2,2,2,26,26,26,0 },
        { 6,6,6,6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,14,14,14,18,18,18,26,26,26,32,32,32,42,42,42,18,18,18,0 },
        { 6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,32,32,32,44,44,44,12,12,12,0 },
        { 6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
        { 4,4,4,4,4,4,6,6,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,22,22,22,30,30,30,56,56,56,0 },
        { 4,4,4,4,4,4,6,6,4,4,4,6,6,6,6,6,6,10,10,10,12,12,12,14,14,14,16,16,16,20,20,20,26,26,26,66,66,66,0 },
        { 4,4,4,4,4,4,6,6,4,4,4,6,6,6,8,8,8,12,12,12,16,16,16,20,20,20,26,26,26,34,34,34,42,42,42,12,12,12,0 }
    };

    unsigned tables, scfsi = 0;
    int main_data_begin, part_23_sum = 0;
    int sr_idx = HDR_GET_MY_SAMPLE_RATE(hdr); sr_idx -= (sr_idx != 0);
    int gr_count = HDR_IS_MONO(hdr) ? 1 : 2;

    if (HDR_TEST_MPEG1(hdr)) {
        gr_count *= 2;
        main_data_begin = get_bits(bs, 9);
        scfsi = get_bits(bs, 7 + gr_count);
    }
    else { main_data_begin = get_bits(bs, 8 + gr_count) >> gr_count; }

    do {
        if (HDR_IS_MONO(hdr)) { scfsi <<= 4; }
        gr->part_23_length = (uint16_t)get_bits(bs, 12);
        part_23_sum += gr->part_23_length;
        gr->big_values = (uint16_t)get_bits(bs,  9);
        if (gr->big_values > 288) { return -1; }
        gr->global_gain = (uint8_t)get_bits(bs, 8);
        gr->scalefac_compress = (uint16_t)get_bits(bs, HDR_TEST_MPEG1(hdr) ? 4 : 9);
        gr->sfbtab = g_scf_long[sr_idx];
        gr->n_long_sfb  = 22;
        gr->n_short_sfb = 0;
        if (get_bits(bs, 1)) {
            gr->block_type = (uint8_t)get_bits(bs, 2);
            if (!gr->block_type) { return -1; }
            gr->mixed_block_flag = (uint8_t)get_bits(bs, 1);
            gr->region_count[0] = 7;
            gr->region_count[1] = 255;
            if (gr->block_type == 2) {
                scfsi &= 0x0F0F;
                if (!gr->mixed_block_flag) {
                    gr->region_count[0] = 8;
                    gr->sfbtab = g_scf_short[sr_idx];
                    gr->n_long_sfb = 0;
                    gr->n_short_sfb = 39;
                }
                else {
                    gr->sfbtab = g_scf_mixed[sr_idx];
                    gr->n_long_sfb = HDR_TEST_MPEG1(hdr) ? 8 : 6;
                    gr->n_short_sfb = 30;
                }
            }
            tables = get_bits(bs, 10);
            tables <<= 5;
            gr->subblock_gain[0] = (uint8_t)get_bits(bs, 3);
            gr->subblock_gain[1] = (uint8_t)get_bits(bs, 3);
            gr->subblock_gain[2] = (uint8_t)get_bits(bs, 3);
        }
        else {
            gr->block_type = 0;
            gr->mixed_block_flag = 0;
            tables = get_bits(bs, 15);
            gr->region_count[0] = (uint8_t)get_bits(bs, 4);
            gr->region_count[1] = (uint8_t)get_bits(bs, 3);
            gr->region_count[2] = 255;
        }
        gr->table_select[0] = (uint8_t)(tables >> 10);
        gr->table_select[1] = (uint8_t)((tables >> 5) & 31);
        gr->table_select[2] = (uint8_t)((tables) & 31);
        gr->preflag = HDR_TEST_MPEG1(hdr) ? get_bits(bs, 1) : (gr->scalefac_compress >= 500);
        gr->scalefac_scale = (uint8_t)get_bits(bs, 1);
        gr->count1_table = (uint8_t)get_bits(bs, 1);
        gr->scfsi = (uint8_t)((scfsi >> 12) & 15);
        scfsi <<= 4;
        gr++;
    } while(--gr_count);

    if (part_23_sum + bs->pos > bs->limit + main_data_begin*8) { return -1; }
    return main_data_begin;
}

static int L3_restore_reservoir(mp3dec_t *h, bs_t *bs, mp3dec_scratch_t *s, int main_data_begin) {
    int frame_bytes = (bs->limit - bs->pos)/8;
    int bytes_have = MINIMP3_MIN(h->reserv, main_data_begin);
    memcpy(s->maindata, h->reserv_buf + MINIMP3_MAX(0, h->reserv - main_data_begin), MINIMP3_MIN(h->reserv, main_data_begin));
    memcpy(s->maindata + bytes_have, bs->buf + bs->pos/8, frame_bytes);
    s->bs.buf   = s->maindata;
    s->bs.pos   = 0;
    s->bs.limit = (bytes_have + frame_bytes) * 8;
    return h->reserv >= main_data_begin;
}

static void L3_read_scalefactors(uint8_t *scf, uint8_t *ist_pos, const uint8_t *scf_size, const uint8_t *scf_count, bs_t *bitbuf, int scfsi) {
    int i, k;
    for (i = 0; i < 4 && scf_count[i]; i++, scfsi *= 2) {
        int cnt = scf_count[i];
        if (scfsi & 8) {
            memcpy(scf, ist_pos, cnt);
        }
        else {
            int bits = scf_size[i];
            if (!bits) {
                memset(scf, 0, cnt);
                memset(ist_pos, 0, cnt);
            }
            else {
                int max_scf = (scfsi < 0) ? (1 << bits) - 1 : -1;
                for (k = 0; k < cnt; k++) {
                    int s = get_bits(bitbuf, bits);
                    ist_pos[k] = (s == max_scf ? -1 : s);
                    scf[k] = s;
                }
            }
        }
        ist_pos += cnt;
        scf += cnt;
    }
    scf[0] = scf[1] = scf[2] = 0;
}

static float L3_ldexp_q2(float y, int exp_q2) {
    static const float g_expfrac[4] = { 9.31322575e-10f,7.83145814e-10f,6.58544508e-10f,5.53767716e-10f };
    int e;
    do {
        e = MINIMP3_MIN(30*4, exp_q2);
        y *= g_expfrac[e & 3]*(1 << 30 >> (e >> 2));
    } while ((exp_q2 -= e) > 0);
    return y;
}

static void L3_decode_scalefactors(const uint8_t *hdr, uint8_t *ist_pos, bs_t *bs, const L3_gr_info_t *gr, float *scf, int ch) {
    static const uint8_t g_scf_partitions[3][28] = {
        { 6,5,5, 5,6,5,5,5,6,5, 7,3,11,10,0,0, 7, 7, 7,0, 6, 6,6,3, 8, 8,5,0 },
        { 8,9,6,12,6,9,9,9,6,9,12,6,15,18,0,0, 6,15,12,0, 6,12,9,6, 6,18,9,0 },
        { 9,9,6,12,9,9,9,9,9,9,12,6,18,18,0,0,12,12,12,0,12, 9,9,6,15,12,9,0 }
    };
    const uint8_t *scf_partition = g_scf_partitions[!!gr->n_short_sfb + !gr->n_long_sfb];
    uint8_t scf_size[4], iscf[40];
    int i, scf_shift = gr->scalefac_scale + 1, gain_exp, scfsi = gr->scfsi;
    float gain;

    if (HDR_TEST_MPEG1(hdr)) {
        static const uint8_t g_scfc_decode[16] = { 0,1,2,3, 12,5,6,7, 9,10,11,13, 14,15,18,19 };
        int part = g_scfc_decode[gr->scalefac_compress];
        scf_size[1] = scf_size[0] = (uint8_t)(part >> 2);
        scf_size[3] = scf_size[2] = (uint8_t)(part & 3);
    }
    else {
        static const uint8_t g_mod[6*4] = { 5,5,4,4,5,5,4,1,4,3,1,1,5,6,6,1,4,4,4,1,4,3,1,1 };
        int k, modprod, sfc, ist = HDR_TEST_I_STEREO(hdr) && ch;
        sfc = gr->scalefac_compress >> ist;
        for (k = ist*3*4; sfc >= 0; sfc -= modprod, k += 4) {
            for (modprod = 1, i = 3; i >= 0; i--) {
                scf_size[i] = (uint8_t)(sfc / modprod % g_mod[k + i]);
                modprod *= g_mod[k + i];
            }
        }
        scf_partition += k;
        scfsi = -16;
    }
    L3_read_scalefactors(iscf, ist_pos, scf_size, scf_partition, bs, scfsi);

    if (gr->n_short_sfb) {
        int sh = 3 - scf_shift;
        for (i = 0; i < gr->n_short_sfb; i += 3) {
            iscf[gr->n_long_sfb + i + 0] += gr->subblock_gain[0] << sh;
            iscf[gr->n_long_sfb + i + 1] += gr->subblock_gain[1] << sh;
            iscf[gr->n_long_sfb + i + 2] += gr->subblock_gain[2] << sh;
        }
    }
    else if (gr->preflag) {
        static const uint8_t g_preamp[10] = { 1,1,1,1,2,2,3,3,3,2 };
        for (i = 0; i < 10; i++) { iscf[11 + i] += g_preamp[i]; }
    }

    gain_exp = gr->global_gain - 214;
    gain = L3_ldexp_q2(1 << (MAX_SCFI/4),  MAX_SCFI - gain_exp);
    for (i = 0; i < (int)(gr->n_long_sfb + gr->n_short_sfb); i++) {
        scf[i] = L3_ldexp_q2(gain, iscf[i] << scf_shift);
    }
}

static const float g_pow43[129 + 16] = {
    0,-1,-2.519842f,-4.326749f,-6.349604f,-8.549880f,-10.902724f,-13.390518f,-16.000000f,-18.720754f,-21.544347f,-24.463781f,-27.473142f,-30.567351f,-33.741992f,-36.993181f,
    0,1,2.519842f,4.326749f,6.349604f,8.549880f,10.902724f,13.390518f,16.000000f,18.720754f,21.544347f,24.463781f,27.473142f,30.567351f,33.741992f,36.993181f,40.317474f,43.711787f,47.173345f,50.699631f,54.288352f,57.937408f,61.644865f,65.408941f,69.227979f,73.100443f,77.024898f,81.000000f,85.024491f,89.097188f,93.216975f,97.382800f,101.593667f,105.848633f,110.146801f,114.487321f,118.869381f,123.292209f,127.755065f,132.257246f,136.798076f,141.376907f,145.993119f,150.646117f,155.335327f,160.060199f,164.820202f,169.614826f,174.443577f,179.305980f,184.201575f,189.129918f,194.090580f,199.083145f,204.107210f,209.162385f,214.248292f,219.364564f,224.510845f,229.686789f,234.892058f,240.126328f,245.389280f,250.680604f,256.000000f,261.347174f,266.721841f,272.123723f,277.552547f,283.008049f,288.489971f,293.998060f,299.532071f,305.091761f,310.676898f,316.287249f,321.922592f,327.582707f,333.267377f,338.976394f,344.709550f,350.466646f,356.247482f,362.051866f,367.879608f,373.730522f,379.604427f,385.501143f,391.420496f,397.362314f,403.326427f,409.312672f,415.320884f,421.350905f,427.402579f,433.475750f,439.570269f,445.685987f,451.822757f,457.980436f,464.158883f,470.357960f,476.577530f,482.817459f,489.077615f,495.357868f,501.658090f,507.978156f,514.317941f,520.677324f,527.056184f,533.454404f,539.871867f,546.308458f,552.764065f,559.238575f,565.731879f,572.243870f,578.774440f,585.323483f,591.890898f,598.476581f,605.080431f,611.702349f,618.342238f,625.000000f,631.675540f,638.368763f,645.079578f
};

static float L3_pow_43(int x) {
    float frac;
    int sign, mult = 256;

    if (x < 129) { return g_pow43[16 + x]; }

    if (x < 1024) {
        mult = 16;
        x <<= 3;
    }

    sign = 2*x & 64;
    frac = (float)((x & 63) - sign) / ((x & ~63) + sign);
    return g_pow43[16 + ((x + sign) >> 6)]*(1.f + frac*((4.f/3) + frac*(2.f/9)))*mult;
}

static void L3_huffman(float *dst, bs_t *bs, const L3_gr_info_t *gr_info, const float *scf, int layer3gr_limit) {
    static const int16_t tabs[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        785,785,785,785,784,784,784,784,513,513,513,513,513,513,513,513,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,
        -255,1313,1298,1282,785,785,785,785,784,784,784,784,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,290,288,
        -255,1313,1298,1282,769,769,769,769,529,529,529,529,529,529,529,529,528,528,528,528,528,528,528,528,512,512,512,512,512,512,512,512,290,288,
        -253,-318,-351,-367,785,785,785,785,784,784,784,784,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,819,818,547,547,275,275,275,275,561,560,515,546,289,274,288,258,
        -254,-287,1329,1299,1314,1312,1057,1057,1042,1042,1026,1026,784,784,784,784,529,529,529,529,529,529,529,529,769,769,769,769,768,768,768,768,563,560,306,306,291,259,
        -252,-413,-477,-542,1298,-575,1041,1041,784,784,784,784,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,-383,-399,1107,1092,1106,1061,849,849,789,789,1104,1091,773,773,1076,1075,341,340,325,309,834,804,577,577,532,532,516,516,832,818,803,816,561,561,531,531,515,546,289,289,288,258,
        -252,-429,-493,-559,1057,1057,1042,1042,529,529,529,529,529,529,529,529,784,784,784,784,769,769,769,769,512,512,512,512,512,512,512,512,-382,1077,-415,1106,1061,1104,849,849,789,789,1091,1076,1029,1075,834,834,597,581,340,340,339,324,804,833,532,532,832,772,818,803,817,787,816,771,290,290,290,290,288,258,
        -253,-349,-414,-447,-463,1329,1299,-479,1314,1312,1057,1057,1042,1042,1026,1026,785,785,785,785,784,784,784,784,769,769,769,769,768,768,768,768,-319,851,821,-335,836,850,805,849,341,340,325,336,533,533,579,579,564,564,773,832,578,548,563,516,321,276,306,291,304,259,
        -251,-572,-733,-830,-863,-879,1041,1041,784,784,784,784,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,-511,-527,-543,1396,1351,1381,1366,1395,1335,1380,-559,1334,1138,1138,1063,1063,1350,1392,1031,1031,1062,1062,1364,1363,1120,1120,1333,1348,881,881,881,881,375,374,359,373,343,358,341,325,791,791,1123,1122,-703,1105,1045,-719,865,865,790,790,774,774,1104,1029,338,293,323,308,-799,-815,833,788,772,818,803,816,322,292,307,320,561,531,515,546,289,274,288,258,
        -251,-525,-605,-685,-765,-831,-846,1298,1057,1057,1312,1282,785,785,785,785,784,784,784,784,769,769,769,769,512,512,512,512,512,512,512,512,1399,1398,1383,1367,1382,1396,1351,-511,1381,1366,1139,1139,1079,1079,1124,1124,1364,1349,1363,1333,882,882,882,882,807,807,807,807,1094,1094,1136,1136,373,341,535,535,881,775,867,822,774,-591,324,338,-671,849,550,550,866,864,609,609,293,336,534,534,789,835,773,-751,834,804,308,307,833,788,832,772,562,562,547,547,305,275,560,515,290,290,
        -252,-397,-477,-557,-622,-653,-719,-735,-750,1329,1299,1314,1057,1057,1042,1042,1312,1282,1024,1024,785,785,785,785,784,784,784,784,769,769,769,769,-383,1127,1141,1111,1126,1140,1095,1110,869,869,883,883,1079,1109,882,882,375,374,807,868,838,881,791,-463,867,822,368,263,852,837,836,-543,610,610,550,550,352,336,534,534,865,774,851,821,850,805,593,533,579,564,773,832,578,578,548,548,577,577,307,276,306,291,516,560,259,259,
        -250,-2107,-2507,-2764,-2909,-2974,-3007,-3023,1041,1041,1040,1040,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,-767,-1052,-1213,-1277,-1358,-1405,-1469,-1535,-1550,-1582,-1614,-1647,-1662,-1694,-1726,-1759,-1774,-1807,-1822,-1854,-1886,1565,-1919,-1935,-1951,-1967,1731,1730,1580,1717,-1983,1729,1564,-1999,1548,-2015,-2031,1715,1595,-2047,1714,-2063,1610,-2079,1609,-2095,1323,1323,1457,1457,1307,1307,1712,1547,1641,1700,1699,1594,1685,1625,1442,1442,1322,1322,-780,-973,-910,1279,1278,1277,1262,1276,1261,1275,1215,1260,1229,-959,974,974,989,989,-943,735,478,478,495,463,506,414,-1039,1003,958,1017,927,942,987,957,431,476,1272,1167,1228,-1183,1256,-1199,895,895,941,941,1242,1227,1212,1135,1014,1014,490,489,503,487,910,1013,985,925,863,894,970,955,1012,847,-1343,831,755,755,984,909,428,366,754,559,-1391,752,486,457,924,997,698,698,983,893,740,740,908,877,739,739,667,667,953,938,497,287,271,271,683,606,590,712,726,574,302,302,738,736,481,286,526,725,605,711,636,724,696,651,589,681,666,710,364,467,573,695,466,466,301,465,379,379,709,604,665,679,316,316,634,633,436,436,464,269,424,394,452,332,438,363,347,408,393,448,331,422,362,407,392,421,346,406,391,376,375,359,1441,1306,-2367,1290,-2383,1337,-2399,-2415,1426,1321,-2431,1411,1336,-2447,-2463,-2479,1169,1169,1049,1049,1424,1289,1412,1352,1319,-2495,1154,1154,1064,1064,1153,1153,416,390,360,404,403,389,344,374,373,343,358,372,327,357,342,311,356,326,1395,1394,1137,1137,1047,1047,1365,1392,1287,1379,1334,1364,1349,1378,1318,1363,792,792,792,792,1152,1152,1032,1032,1121,1121,1046,1046,1120,1120,1030,1030,-2895,1106,1061,1104,849,849,789,789,1091,1076,1029,1090,1060,1075,833,833,309,324,532,532,832,772,818,803,561,561,531,560,515,546,289,274,288,258,
        -250,-1179,-1579,-1836,-1996,-2124,-2253,-2333,-2413,-2477,-2542,-2574,-2607,-2622,-2655,1314,1313,1298,1312,1282,785,785,785,785,1040,1040,1025,1025,768,768,768,768,-766,-798,-830,-862,-895,-911,-927,-943,-959,-975,-991,-1007,-1023,-1039,-1055,-1070,1724,1647,-1103,-1119,1631,1767,1662,1738,1708,1723,-1135,1780,1615,1779,1599,1677,1646,1778,1583,-1151,1777,1567,1737,1692,1765,1722,1707,1630,1751,1661,1764,1614,1736,1676,1763,1750,1645,1598,1721,1691,1762,1706,1582,1761,1566,-1167,1749,1629,767,766,751,765,494,494,735,764,719,749,734,763,447,447,748,718,477,506,431,491,446,476,461,505,415,430,475,445,504,399,460,489,414,503,383,474,429,459,502,502,746,752,488,398,501,473,413,472,486,271,480,270,-1439,-1455,1357,-1471,-1487,-1503,1341,1325,-1519,1489,1463,1403,1309,-1535,1372,1448,1418,1476,1356,1462,1387,-1551,1475,1340,1447,1402,1386,-1567,1068,1068,1474,1461,455,380,468,440,395,425,410,454,364,467,466,464,453,269,409,448,268,432,1371,1473,1432,1417,1308,1460,1355,1446,1459,1431,1083,1083,1401,1416,1458,1445,1067,1067,1370,1457,1051,1051,1291,1430,1385,1444,1354,1415,1400,1443,1082,1082,1173,1113,1186,1066,1185,1050,-1967,1158,1128,1172,1097,1171,1081,-1983,1157,1112,416,266,375,400,1170,1142,1127,1065,793,793,1169,1033,1156,1096,1141,1111,1155,1080,1126,1140,898,898,808,808,897,897,792,792,1095,1152,1032,1125,1110,1139,1079,1124,882,807,838,881,853,791,-2319,867,368,263,822,852,837,866,806,865,-2399,851,352,262,534,534,821,836,594,594,549,549,593,593,533,533,848,773,579,579,564,578,548,563,276,276,577,576,306,291,516,560,305,305,275,259,
        -251,-892,-2058,-2620,-2828,-2957,-3023,-3039,1041,1041,1040,1040,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,-511,-527,-543,-559,1530,-575,-591,1528,1527,1407,1526,1391,1023,1023,1023,1023,1525,1375,1268,1268,1103,1103,1087,1087,1039,1039,1523,-604,815,815,815,815,510,495,509,479,508,463,507,447,431,505,415,399,-734,-782,1262,-815,1259,1244,-831,1258,1228,-847,-863,1196,-879,1253,987,987,748,-767,493,493,462,477,414,414,686,669,478,446,461,445,474,429,487,458,412,471,1266,1264,1009,1009,799,799,-1019,-1276,-1452,-1581,-1677,-1757,-1821,-1886,-1933,-1997,1257,1257,1483,1468,1512,1422,1497,1406,1467,1496,1421,1510,1134,1134,1225,1225,1466,1451,1374,1405,1252,1252,1358,1480,1164,1164,1251,1251,1238,1238,1389,1465,-1407,1054,1101,-1423,1207,-1439,830,830,1248,1038,1237,1117,1223,1148,1236,1208,411,426,395,410,379,269,1193,1222,1132,1235,1221,1116,976,976,1192,1162,1177,1220,1131,1191,963,963,-1647,961,780,-1663,558,558,994,993,437,408,393,407,829,978,813,797,947,-1743,721,721,377,392,844,950,828,890,706,706,812,859,796,960,948,843,934,874,571,571,-1919,690,555,689,421,346,539,539,944,779,918,873,932,842,903,888,570,570,931,917,674,674,-2575,1562,-2591,1609,-2607,1654,1322,1322,1441,1441,1696,1546,1683,1593,1669,1624,1426,1426,1321,1321,1639,1680,1425,1425,1305,1305,1545,1668,1608,1623,1667,1592,1638,1666,1320,1320,1652,1607,1409,1409,1304,1304,1288,1288,1664,1637,1395,1395,1335,1335,1622,1636,1394,1394,1319,1319,1606,1621,1392,1392,1137,1137,1137,1137,345,390,360,375,404,373,1047,-2751,-2767,-2783,1062,1121,1046,-2799,1077,-2815,1106,1061,789,789,1105,1104,263,355,310,340,325,354,352,262,339,324,1091,1076,1029,1090,1060,1075,833,833,788,788,1088,1028,818,818,803,803,561,561,531,531,816,771,546,546,289,274,288,258,
        -253,-317,-381,-446,-478,-509,1279,1279,-811,-1179,-1451,-1756,-1900,-2028,-2189,-2253,-2333,-2414,-2445,-2511,-2526,1313,1298,-2559,1041,1041,1040,1040,1025,1025,1024,1024,1022,1007,1021,991,1020,975,1019,959,687,687,1018,1017,671,671,655,655,1016,1015,639,639,758,758,623,623,757,607,756,591,755,575,754,559,543,543,1009,783,-575,-621,-685,-749,496,-590,750,749,734,748,974,989,1003,958,988,973,1002,942,987,957,972,1001,926,986,941,971,956,1000,910,985,925,999,894,970,-1071,-1087,-1102,1390,-1135,1436,1509,1451,1374,-1151,1405,1358,1480,1420,-1167,1507,1494,1389,1342,1465,1435,1450,1326,1505,1310,1493,1373,1479,1404,1492,1464,1419,428,443,472,397,736,526,464,464,486,457,442,471,484,482,1357,1449,1434,1478,1388,1491,1341,1490,1325,1489,1463,1403,1309,1477,1372,1448,1418,1433,1476,1356,1462,1387,-1439,1475,1340,1447,1402,1474,1324,1461,1371,1473,269,448,1432,1417,1308,1460,-1711,1459,-1727,1441,1099,1099,1446,1386,1431,1401,-1743,1289,1083,1083,1160,1160,1458,1445,1067,1067,1370,1457,1307,1430,1129,1129,1098,1098,268,432,267,416,266,400,-1887,1144,1187,1082,1173,1113,1186,1066,1050,1158,1128,1143,1172,1097,1171,1081,420,391,1157,1112,1170,1142,1127,1065,1169,1049,1156,1096,1141,1111,1155,1080,1126,1154,1064,1153,1140,1095,1048,-2159,1125,1110,1137,-2175,823,823,1139,1138,807,807,384,264,368,263,868,838,853,791,867,822,852,837,866,806,865,790,-2319,851,821,836,352,262,850,805,849,-2399,533,533,835,820,336,261,578,548,563,577,532,532,832,772,562,562,547,547,305,275,560,515,290,290,288,258 };
    static const uint8_t tab32[] = { 130,162,193,209,44,28,76,140,9,9,9,9,9,9,9,9,190,254,222,238,126,94,157,157,109,61,173,205 };
    static const uint8_t tab33[] = { 252,236,220,204,188,172,156,140,124,108,92,76,60,44,28,12 };
    static const int16_t tabindex[2*16] = { 0,32,64,98,0,132,180,218,292,364,426,538,648,746,0,1126,1460,1460,1460,1460,1460,1460,1460,1460,1842,1842,1842,1842,1842,1842,1842,1842 };
    static const uint8_t g_linbits[] =  { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,6,8,10,13,4,5,6,7,8,9,11,13 };

#define PEEK_BITS(n)  (bs_cache >> (32 - n))
#define FLUSH_BITS(n) { bs_cache <<= (n); bs_sh += (n); }
#define CHECK_BITS    while (bs_sh >= 0) { bs_cache |= (uint32_t)*bs_next_ptr++ << bs_sh; bs_sh -= 8; }
#define BSPOS         ((bs_next_ptr - bs->buf)*8 - 24 + bs_sh)

    float one = 0.0f;
    int ireg = 0, big_val_cnt = gr_info->big_values;
    const uint8_t *sfb = gr_info->sfbtab;
    const uint8_t *bs_next_ptr = bs->buf + bs->pos/8;
    uint32_t bs_cache = (((bs_next_ptr[0]*256u + bs_next_ptr[1])*256u + bs_next_ptr[2])*256u + bs_next_ptr[3]) << (bs->pos & 7);
    int pairs_to_decode, np, bs_sh = (bs->pos & 7) - 8;
    bs_next_ptr += 4;

    while (big_val_cnt > 0) {
        int tab_num = gr_info->table_select[ireg];
        int sfb_cnt = gr_info->region_count[ireg++];
        const int16_t *codebook = tabs + tabindex[tab_num];
        int linbits = g_linbits[tab_num];
        if (linbits) {
            do {
                np = *sfb++ / 2;
                pairs_to_decode = MINIMP3_MIN(big_val_cnt, np);
                one = *scf++;
                do {
                    int j, w = 5;
                    int leaf = codebook[PEEK_BITS(w)];
                    while (leaf < 0) {
                        FLUSH_BITS(w);
                        w = leaf & 7;
                        leaf = codebook[PEEK_BITS(w) - (leaf >> 3)];
                    }
                    FLUSH_BITS(leaf >> 8);

                    for (j = 0; j < 2; j++, dst++, leaf >>= 4) {
                        int lsb = leaf & 0x0F;
                        if (lsb == 15) {
                            lsb += PEEK_BITS(linbits);
                            FLUSH_BITS(linbits);
                            CHECK_BITS;
                            *dst = one*L3_pow_43(lsb)*((int32_t)bs_cache < 0 ? -1: 1);
                        }
                        else {
                            *dst = g_pow43[16 + lsb - 16*(bs_cache >> 31)]*one;
                        }
                        FLUSH_BITS(lsb ? 1 : 0);
                    }
                    CHECK_BITS;
                } while (--pairs_to_decode);
            } while ((big_val_cnt -= np) > 0 && --sfb_cnt >= 0);
        }
        else {
            do {
                np = *sfb++ / 2;
                pairs_to_decode = MINIMP3_MIN(big_val_cnt, np);
                one = *scf++;
                do {
                    int j, w = 5;
                    int leaf = codebook[PEEK_BITS(w)];
                    while (leaf < 0) {
                        FLUSH_BITS(w);
                        w = leaf & 7;
                        leaf = codebook[PEEK_BITS(w) - (leaf >> 3)];
                    }
                    FLUSH_BITS(leaf >> 8);

                    for (j = 0; j < 2; j++, dst++, leaf >>= 4) {
                        int lsb = leaf & 0x0F;
                        *dst = g_pow43[16 + lsb - 16*(bs_cache >> 31)]*one;
                        FLUSH_BITS(lsb ? 1 : 0);
                    }
                    CHECK_BITS;
                } while (--pairs_to_decode);
            } while ((big_val_cnt -= np) > 0 && --sfb_cnt >= 0);
        }
    }

    for (np = 1 - big_val_cnt;; dst += 4) {
        const uint8_t *codebook_count1 = (gr_info->count1_table) ? tab33 : tab32;
        int leaf = codebook_count1[PEEK_BITS(4)];
        if (!(leaf & 8)) {
            leaf = codebook_count1[(leaf >> 3) + (bs_cache << 4 >> (32 - (leaf & 3)))];
        }
        FLUSH_BITS(leaf & 7);
        if (BSPOS > layer3gr_limit) { break; }
#define RELOAD_SCALEFACTOR  if (!--np) { np = *sfb++/2; if (!np) break; one = *scf++; }
#define DEQ_COUNT1(s) if (leaf & (128 >> s)) { dst[s] = ((int32_t)bs_cache < 0) ? -one : one; FLUSH_BITS(1) }
        RELOAD_SCALEFACTOR;
        DEQ_COUNT1(0);
        DEQ_COUNT1(1);
        RELOAD_SCALEFACTOR;
        DEQ_COUNT1(2);
        DEQ_COUNT1(3);
        CHECK_BITS;
    }

    bs->pos = layer3gr_limit;
}

static void L3_reorder(float *grbuf, float *scratch, const uint8_t *sfb) {
    int i, len;
    float *src = grbuf, *dst = scratch;

    for (;0 != (len = *sfb); sfb += 3, src += 2*len) {
        for (i = 0; i < len; i++, src++) {
            *dst++ = src[0*len];
            *dst++ = src[1*len];
            *dst++ = src[2*len];
        }
    }
    memcpy(grbuf, scratch, (dst - scratch)*sizeof(float));
}

static void L3_dct3_9(float *y) {
    float s0, s1, s2, s3, s4, s5, s6, s7, s8, t0, t2, t4;

    s0 = y[0]; s2 = y[2]; s4 = y[4]; s6 = y[6]; s8 = y[8];
    t0 = s0 + s6*0.5f;
    s0 -= s6;
    t4 = (s4 + s2)*0.93969262f;
    t2 = (s8 + s2)*0.76604444f;
    s6 = (s4 - s8)*0.17364818f;
    s4 += s8 - s2;

    s2 = s0 - s4*0.5f;
    y[4] = s4 + s0;
    s8 = t0 - t2 + s6;
    s0 = t0 - t4 + t2;
    s4 = t0 + t4 - s6;

    s1 = y[1]; s3 = y[3]; s5 = y[5]; s7 = y[7];

    s3 *= 0.86602540f;
    t0 = (s5 + s1)*0.98480775f;
    t4 = (s5 - s7)*0.34202014f;
    t2 = (s1 + s7)*0.64278761f;
    s1 = (s1 - s5 - s7)*0.86602540f;

    s5 = t0 - s3 - t2;
    s7 = t4 - s3 - t0;
    s3 = t4 + s3 - t2;

    y[0] = s4 - s7;
    y[1] = s2 + s1;
    y[2] = s0 - s3;
    y[3] = s8 + s5;
    y[5] = s8 - s5;
    y[6] = s0 + s3;
    y[7] = s2 - s1;
    y[8] = s4 + s7;
}

static void L3_imdct36(float *grbuf, float *overlap, const float *window, int nbands) {
    int i, j;
    static const float g_twid9[18] = {
        0.73727734f,0.79335334f,0.84339145f,0.88701083f,0.92387953f,0.95371695f,0.97629601f,0.99144486f,0.99904822f,0.67559021f,0.60876143f,0.53729961f,0.46174861f,0.38268343f,0.30070580f,0.21643961f,0.13052619f,0.04361938f
    };

    for (j = 0; j < nbands; j++, grbuf += 18, overlap += 9) {
        float co[9], si[9];
        co[0] = -grbuf[0];
        si[0] = grbuf[17];
        for (i = 0; i < 4; i++) {
            si[8 - 2*i] =   grbuf[4*i + 1] - grbuf[4*i + 2];
            co[1 + 2*i] =   grbuf[4*i + 1] + grbuf[4*i + 2];
            si[7 - 2*i] =   grbuf[4*i + 4] - grbuf[4*i + 3];
            co[2 + 2*i] = -(grbuf[4*i + 3] + grbuf[4*i + 4]);
        }
        L3_dct3_9(co);
        L3_dct3_9(si);

        si[1] = -si[1];
        si[3] = -si[3];
        si[5] = -si[5];
        si[7] = -si[7];

        i = 0;

#if HAVE_SIMD
        for (; i < 8; i += 4) {
            f4 vovl = VLD(overlap + i);
            f4 vc = VLD(co + i);
            f4 vs = VLD(si + i);
            f4 vr0 = VLD(g_twid9 + i);
            f4 vr1 = VLD(g_twid9 + 9 + i);
            f4 vw0 = VLD(window + i);
            f4 vw1 = VLD(window + 9 + i);
            f4 vsum = VADD(VMUL(vc, vr1), VMUL(vs, vr0));
            VSTORE(overlap + i, VSUB(VMUL(vc, vr0), VMUL(vs, vr1)));
            VSTORE(grbuf + i, VSUB(VMUL(vovl, vw0), VMUL(vsum, vw1)));
            vsum = VADD(VMUL(vovl, vw1), VMUL(vsum, vw0));
            VSTORE(grbuf + 14 - i, VREV(vsum));
        }
#endif /* HAVE_SIMD */
        for (; i < 9; i++) {
            float ovl  = overlap[i];
            float sum  = co[i]*g_twid9[9 + i] + si[i]*g_twid9[0 + i];
            overlap[i] = co[i]*g_twid9[0 + i] - si[i]*g_twid9[9 + i];
            grbuf[i]      = ovl*window[0 + i] - sum*window[9 + i];
            grbuf[17 - i] = ovl*window[9 + i] + sum*window[0 + i];
        }
    }
}

static void L3_idct3(float x0, float x1, float x2, float *dst) {
    float m1 = x1*0.86602540f;
    float a1 = x0 - x2*0.5f;
    dst[1] = x0 + x2;
    dst[0] = a1 + m1;
    dst[2] = a1 - m1;
}

static void L3_imdct12(float *x, float *dst, float *overlap) {
    static const float g_twid3[6] = { 0.79335334f,0.92387953f,0.99144486f, 0.60876143f,0.38268343f,0.13052619f };
    float co[3], si[3];
    int i;

    L3_idct3(-x[0], x[6] + x[3], x[12] + x[9], co);
    L3_idct3(x[15], x[12] - x[9], x[6] - x[3], si);
    si[1] = -si[1];

    for (i = 0; i < 3; i++) {
        float ovl  = overlap[i];
        float sum  = co[i]*g_twid3[3 + i] + si[i]*g_twid3[0 + i];
        overlap[i] = co[i]*g_twid3[0 + i] - si[i]*g_twid3[3 + i];
        dst[i]     = ovl*g_twid3[2 - i] - sum*g_twid3[5 - i];
        dst[5 - i] = ovl*g_twid3[5 - i] + sum*g_twid3[2 - i];
    }
}

static void L3_imdct_short(float *grbuf, float *overlap, int nbands) {
    for (;nbands > 0; nbands--, overlap += 9, grbuf += 18) {
        float tmp[18];
        memcpy(tmp, grbuf, sizeof(tmp));
        memcpy(grbuf, overlap, 6*sizeof(float));
        L3_imdct12(tmp, grbuf + 6, overlap + 6);
        L3_imdct12(tmp + 1, grbuf + 12, overlap + 6);
        L3_imdct12(tmp + 2, overlap, overlap + 6);
    }
}

static void L3_imdct_gr(float *grbuf, float *overlap, unsigned block_type, unsigned n_long_bands) {
    static const float g_mdct_window[2][18] = {
        { 0.99904822f,0.99144486f,0.97629601f,0.95371695f,0.92387953f,0.88701083f,0.84339145f,0.79335334f,0.73727734f,0.04361938f,0.13052619f,0.21643961f,0.30070580f,0.38268343f,0.46174861f,0.53729961f,0.60876143f,0.67559021f },
        { 1,1,1,1,1,1,0.99144486f,0.92387953f,0.79335334f,0,0,0,0,0,0,0.13052619f,0.38268343f,0.60876143f }
    };
    if (n_long_bands) {
        L3_imdct36(grbuf, overlap, g_mdct_window[0], n_long_bands);
        grbuf += 18*n_long_bands;
        overlap += 9*n_long_bands;
    }
    if (block_type == 2)
        L3_imdct_short(grbuf, overlap, 32 - n_long_bands);
    else
        L3_imdct36(grbuf, overlap, g_mdct_window[block_type == 3], 32 - n_long_bands);
}

static void L3_change_sign(float *grbuf) {
    int b, i;
    for (b = 0, grbuf += 18; b < 32; b += 2, grbuf += 36)
        for (i = 1; i < 18; i += 2)
            grbuf[i] = -grbuf[i];
}

static void L3_decode(mp3dec_t* h, mp3dec_scratch_t* s, L3_gr_info_t *gr_info, int nch) {
    int ch;

    for (ch = 0; ch < nch; ch++) {
        int layer3gr_limit = s->bs.pos + gr_info[ch].part_23_length;
        L3_decode_scalefactors(h->header, s->ist_pos[ch], &s->bs, gr_info + ch, s->scf, ch);
        L3_huffman(s->grbuf[ch], &s->bs, gr_info + ch, s->scf, layer3gr_limit);
    }

    for (ch = 0; ch < nch; ch++, gr_info++) {
        int n_long_bands = (gr_info->mixed_block_flag ? 2 : 0) << (int)(HDR_GET_MY_SAMPLE_RATE(h->header) == 2);

        if (gr_info->n_short_sfb) {
            L3_reorder(s->grbuf[ch] + n_long_bands*18, s->syn[0], gr_info->sfbtab + gr_info->n_long_sfb);
        }

        L3_imdct_gr(s->grbuf[ch], h->mdct_overlap[ch], gr_info->block_type, n_long_bands);
        L3_change_sign(s->grbuf[ch]);
    }
}

static const float g_sec[24] = {10.19000816f,0.50060302f,0.50241929f,3.40760851f,0.50547093f,0.52249861f,2.05778098f,0.51544732f,0.56694406f,1.48416460f,0.53104258f,0.64682180f,1.16943991f,0.55310392f,0.78815460f,0.97256821f,0.58293498f,1.06067765f,0.83934963f,0.62250412f,1.72244716f,0.74453628f,0.67480832f,5.10114861f};

#if HAVE_SIMD
static void mp3d_DCT_II(float* grbuf, int n) {
    int i, k = 0;
    for (; k < n; k += 4) {
        f4 t[4][8], *x;
        float *y = grbuf + k;

        for (x = t[0], i = 0; i < 8; i++, x++) {
            f4 x0 = VLD(&y[i*18]);
            f4 x1 = VLD(&y[(15 - i)*18]);
            f4 x2 = VLD(&y[(16 + i)*18]);
            f4 x3 = VLD(&y[(31 - i)*18]);
            f4 t0 = VADD(x0, x3);
            f4 t1 = VADD(x1, x2);
            f4 t2 = VMUL_S(VSUB(x1, x2), g_sec[3*i + 0]);
            f4 t3 = VMUL_S(VSUB(x0, x3), g_sec[3*i + 1]);
            x[0] = VADD(t0, t1);
            x[8] = VMUL_S(VSUB(t0, t1), g_sec[3*i + 2]);
            x[16] = VADD(t3, t2);
            x[24] = VMUL_S(VSUB(t3, t2), g_sec[3*i + 2]);
        }
        for (x = t[0], i = 0; i < 4; i++, x += 8) {
            f4 x0 = x[0], x1 = x[1], x2 = x[2], x3 = x[3], x4 = x[4], x5 = x[5], x6 = x[6], x7 = x[7], xt;
            xt = VSUB(x0, x7); x0 = VADD(x0, x7);
            x7 = VSUB(x1, x6); x1 = VADD(x1, x6);
            x6 = VSUB(x2, x5); x2 = VADD(x2, x5);
            x5 = VSUB(x3, x4); x3 = VADD(x3, x4);
            x4 = VSUB(x0, x3); x0 = VADD(x0, x3);
            x3 = VSUB(x1, x2); x1 = VADD(x1, x2);
            x[0] = VADD(x0, x1);
            x[4] = VMUL_S(VSUB(x0, x1), 0.70710677f);
            x5 = VADD(x5, x6);
            x6 = VMUL_S(VADD(x6, x7), 0.70710677f);
            x7 = VADD(x7, xt);
            x3 = VMUL_S(VADD(x3, x4), 0.70710677f);
            x5 = VSUB(x5, VMUL_S(x7, 0.198912367f)); /* rotate by PI/8 */
            x7 = VADD(x7, VMUL_S(x5, 0.382683432f));
            x5 = VSUB(x5, VMUL_S(x7, 0.198912367f));
            x0 = VSUB(xt, x6); xt = VADD(xt, x6);
            x[1] = VMUL_S(VADD(xt, x7), 0.50979561f);
            x[2] = VMUL_S(VADD(x4, x3), 0.54119611f);
            x[3] = VMUL_S(VSUB(x0, x5), 0.60134488f);
            x[5] = VMUL_S(VADD(x0, x5), 0.89997619f);
            x[6] = VMUL_S(VSUB(x4, x3), 1.30656302f);
            x[7] = VMUL_S(VSUB(xt, x7), 2.56291556f);
        }

        if (k > n - 3) {
#if HAVE_SSE
#define VSAVE2(i, v) _mm_storel_pi((__m64 *)(void*)&y[i*18], v)
#else /* HAVE_SSE */
#define VSAVE2(i, v) vst1_f32((float32_t *)&y[i*18],  vget_low_f32(v))
#endif /* HAVE_SSE */
            for (i = 0; i < 7; i++, y += 4*18) {
                f4 s = VADD(t[3][i], t[3][i + 1]);
                VSAVE2(0, t[0][i]);
                VSAVE2(1, VADD(t[2][i], s));
                VSAVE2(2, VADD(t[1][i], t[1][i + 1]));
                VSAVE2(3, VADD(t[2][1 + i], s));
            }
            VSAVE2(0, t[0][7]);
            VSAVE2(1, VADD(t[2][7], t[3][7]));
            VSAVE2(2, t[1][7]);
            VSAVE2(3, t[3][7]);
        }
        else {
#define VSAVE4(i, v) VSTORE(&y[i*18], v)
            for (i = 0; i < 7; i++, y += 4*18) {
                f4 s = VADD(t[3][i], t[3][i + 1]);
                VSAVE4(0, t[0][i]);
                VSAVE4(1, VADD(t[2][i], s));
                VSAVE4(2, VADD(t[1][i], t[1][i + 1]));
                VSAVE4(3, VADD(t[2][1 + i], s));
            }
            VSAVE4(0, t[0][7]);
            VSAVE4(1, VADD(t[2][7], t[3][7]));
            VSAVE4(2, t[1][7]);
            VSAVE4(3, t[3][7]);
        }
    }
#else
static void mp3d_DCT_II(float* grbuf, int n) {
#endif /* HAVE_SIMD */
    int i, k = 0;
    for (; k < n; k++) {
        float t[4][8], *x, *y = grbuf + k;

        for (x = t[0], i = 0; i < 8; i++, x++) {
            float x0 = y[i*18];
            float x1 = y[(15 - i)*18];
            float x2 = y[(16 + i)*18];
            float x3 = y[(31 - i)*18];
            float t0 = x0 + x3;
            float t1 = x1 + x2;
            float t2 = (x1 - x2)*g_sec[3*i + 0];
            float t3 = (x0 - x3)*g_sec[3*i + 1];
            x[0] = t0 + t1;
            x[8] = (t0 - t1)*g_sec[3*i + 2];
            x[16] = t3 + t2;
            x[24] = (t3 - t2)*g_sec[3*i + 2];
        }
        for (x = t[0], i = 0; i < 4; i++, x += 8) {
            float x0 = x[0], x1 = x[1], x2 = x[2], x3 = x[3], x4 = x[4], x5 = x[5], x6 = x[6], x7 = x[7], xt;
            xt = x0 - x7; x0 += x7;
            x7 = x1 - x6; x1 += x6;
            x6 = x2 - x5; x2 += x5;
            x5 = x3 - x4; x3 += x4;
            x4 = x0 - x3; x0 += x3;
            x3 = x1 - x2; x1 += x2;
            x[0] = x0 + x1;
            x[4] = (x0 - x1)*0.70710677f;
            x5 =  x5 + x6;
            x6 = (x6 + x7)*0.70710677f;
            x7 =  x7 + xt;
            x3 = (x3 + x4)*0.70710677f;
            x5 -= x7*0.198912367f;  /* rotate by PI/8 */
            x7 += x5*0.382683432f;
            x5 -= x7*0.198912367f;
            x0 = xt - x6; xt += x6;
            x[1] = (xt + x7)*0.50979561f;
            x[2] = (x4 + x3)*0.54119611f;
            x[3] = (x0 - x5)*0.60134488f;
            x[5] = (x0 + x5)*0.89997619f;
            x[6] = (x4 - x3)*1.30656302f;
            x[7] = (xt - x7)*2.56291556f;

        }
        for (i = 0; i < 7; i++, y += 4*18) {
            y[0*18] = t[0][i];
            y[1*18] = t[2][i] + t[3][i] + t[3][i + 1];
            y[2*18] = t[1][i] + t[1][i + 1];
            y[3*18] = t[2][i + 1] + t[3][i] + t[3][i + 1];
        }
        y[0*18] = t[0][7];
        y[1*18] = t[2][7] + t[3][7];
        y[2*18] = t[1][7];
        y[3*18] = t[3][7];
    }
}

static float mp3d_scale_pcm(float sample) { return sample*(1.f/32768.f); }

static void mp3d_synth_pair(float* pcm, int nch, const float *z) {
    float a;
    a  = (z[14*64] - z[    0]) * 29;
    a += (z[ 1*64] + z[13*64]) * 213;
    a += (z[12*64] - z[ 2*64]) * 459;
    a += (z[ 3*64] + z[11*64]) * 2037;
    a += (z[10*64] - z[ 4*64]) * 5153;
    a += (z[ 5*64] + z[ 9*64]) * 6574;
    a += (z[ 8*64] - z[ 6*64]) * 37489;
    a +=  z[ 7*64]             * 75038;
    pcm[0] = mp3d_scale_pcm(a);

    z += 2;
    a  = z[14*64] * 104;
    a += z[12*64] * 1567;
    a += z[10*64] * 9727;
    a += z[ 8*64] * 64019;
    a += z[ 6*64] * -9975;
    a += z[ 4*64] * -45;
    a += z[ 2*64] * 146;
    a += z[ 0*64] * -5;
    pcm[16*nch] = mp3d_scale_pcm(a);
}

static void mp3d_synth(float *xl, float* dstl, int nch, float *lins) {
    int i;
    float *xr = xl + 576*(nch - 1);
    float* dstr = dstl + (nch - 1);

    static const float g_win[] = {
        -1,26,-31,208,218,401,-519,2063,2000,4788,-5517,7134,5959,35640,-39336,74992,
        -1,24,-35,202,222,347,-581,2080,1952,4425,-5879,7640,5288,33791,-41176,74856,
        -1,21,-38,196,225,294,-645,2087,1893,4063,-6237,8092,4561,31947,-43006,74630,
        -1,19,-41,190,227,244,-711,2085,1822,3705,-6589,8492,3776,30112,-44821,74313,
        -1,17,-45,183,228,197,-779,2075,1739,3351,-6935,8840,2935,28289,-46617,73908,
        -1,16,-49,176,228,153,-848,2057,1644,3004,-7271,9139,2037,26482,-48390,73415,
        -2,14,-53,169,227,111,-919,2032,1535,2663,-7597,9389,1082,24694,-50137,72835,
        -2,13,-58,161,224,72,-991,2001,1414,2330,-7910,9592,70,22929,-51853,72169,
        -2,11,-63,154,221,36,-1064,1962,1280,2006,-8209,9750,-998,21189,-53534,71420,
        -2,10,-68,147,215,2,-1137,1919,1131,1692,-8491,9863,-2122,19478,-55178,70590,
        -3,9,-73,139,208,-29,-1210,1870,970,1388,-8755,9935,-3300,17799,-56778,69679,
        -3,8,-79,132,200,-57,-1283,1817,794,1095,-8998,9966,-4533,16155,-58333,68692,
        -4,7,-85,125,189,-83,-1356,1759,605,814,-9219,9959,-5818,14548,-59838,67629,
        -4,7,-91,117,177,-106,-1428,1698,402,545,-9416,9916,-7154,12980,-61289,66494,
        -5,6,-97,111,163,-127,-1498,1634,185,288,-9585,9838,-8540,11455,-62684,65290
    };
    float *zlin = lins + 15*64;
    const float *w = g_win;

    zlin[4*15]     = xl[18*16];
    zlin[4*15 + 1] = xr[18*16];
    zlin[4*15 + 2] = xl[0];
    zlin[4*15 + 3] = xr[0];

    zlin[4*31]     = xl[1 + 18*16];
    zlin[4*31 + 1] = xr[1 + 18*16];
    zlin[4*31 + 2] = xl[1];
    zlin[4*31 + 3] = xr[1];

    mp3d_synth_pair(dstr, nch, lins + 4*15 + 1);
    mp3d_synth_pair(dstr + 32*nch, nch, lins + 4*15 + 64 + 1);
    mp3d_synth_pair(dstl, nch, lins + 4*15);
    mp3d_synth_pair(dstl + 32*nch, nch, lins + 4*15 + 64);

#if HAVE_SIMD
    for (i = 14; i >= 0; i--) {
#define VLOAD(k) f4 w0 = VSET(*w++); f4 w1 = VSET(*w++); f4 vz = VLD(&zlin[4*i - 64*k]); f4 vy = VLD(&zlin[4*i - 64*(15 - k)]);
#define V0(k) { VLOAD(k) b =         VADD(VMUL(vz, w1), VMUL(vy, w0)) ; a =         VSUB(VMUL(vz, w0), VMUL(vy, w1));  }
#define V1(k) { VLOAD(k) b = VADD(b, VADD(VMUL(vz, w1), VMUL(vy, w0))); a = VADD(a, VSUB(VMUL(vz, w0), VMUL(vy, w1))); }
#define V2(k) { VLOAD(k) b = VADD(b, VADD(VMUL(vz, w1), VMUL(vy, w0))); a = VADD(a, VSUB(VMUL(vy, w1), VMUL(vz, w0))); }
        f4 a, b;
        zlin[4*i]     = xl[18*(31 - i)];
        zlin[4*i + 1] = xr[18*(31 - i)];
        zlin[4*i + 2] = xl[1 + 18*(31 - i)];
        zlin[4*i + 3] = xr[1 + 18*(31 - i)];
        zlin[4*i + 64] = xl[1 + 18*(1 + i)];
        zlin[4*i + 64 + 1] = xr[1 + 18*(1 + i)];
        zlin[4*i - 64 + 2] = xl[18*(1 + i)];
        zlin[4*i - 64 + 3] = xr[18*(1 + i)];

        V0(0) V2(1) V1(2) V2(3) V1(4) V2(5) V1(6) V2(7)

        {

        static const f4 g_scale = { 1.0f/32768.0f, 1.0f/32768.0f, 1.0f/32768.0f, 1.0f/32768.0f };
        a = VMUL(a, g_scale);
        b = VMUL(b, g_scale);
#if HAVE_SSE
        _mm_store_ss(dstr + (15 - i)*nch, _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 1, 1, 1)));
        _mm_store_ss(dstr + (17 + i)*nch, _mm_shuffle_ps(b, b, _MM_SHUFFLE(1, 1, 1, 1)));
        _mm_store_ss(dstl + (15 - i)*nch, _mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 0, 0, 0)));
        _mm_store_ss(dstl + (17 + i)*nch, _mm_shuffle_ps(b, b, _MM_SHUFFLE(0, 0, 0, 0)));
        _mm_store_ss(dstr + (47 - i)*nch, _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 3, 3, 3)));
        _mm_store_ss(dstr + (49 + i)*nch, _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 3, 3, 3)));
        _mm_store_ss(dstl + (47 - i)*nch, _mm_shuffle_ps(a, a, _MM_SHUFFLE(2, 2, 2, 2)));
        _mm_store_ss(dstl + (49 + i)*nch, _mm_shuffle_ps(b, b, _MM_SHUFFLE(2, 2, 2, 2)));
#else /* HAVE_SSE */
        vst1q_lane_f32(dstr + (15 - i)*nch, a, 1);
        vst1q_lane_f32(dstr + (17 + i)*nch, b, 1);
        vst1q_lane_f32(dstl + (15 - i)*nch, a, 0);
        vst1q_lane_f32(dstl + (17 + i)*nch, b, 0);
        vst1q_lane_f32(dstr + (47 - i)*nch, a, 3);
        vst1q_lane_f32(dstr + (49 + i)*nch, b, 3);
        vst1q_lane_f32(dstl + (47 - i)*nch, a, 2);
        vst1q_lane_f32(dstl + (49 + i)*nch, b, 2);
#endif /* HAVE_SSE */
        }
    } else
#endif /* HAVE_SIMD */
    for (i = 14; i >= 0; i--) {
#define LOAD(k) float w0 = *w++; float w1 = *w++; float *vz = &zlin[4*i - k*64]; float *vy = &zlin[4*i - (15 - k)*64];
#define S0(k) { int j; LOAD(k); for (j = 0; j < 4; j++) b[j]  = vz[j]*w1 + vy[j]*w0, a[j]  = vz[j]*w0 - vy[j]*w1; }
#define S1(k) { int j; LOAD(k); for (j = 0; j < 4; j++) b[j] += vz[j]*w1 + vy[j]*w0, a[j] += vz[j]*w0 - vy[j]*w1; }
#define S2(k) { int j; LOAD(k); for (j = 0; j < 4; j++) b[j] += vz[j]*w1 + vy[j]*w0, a[j] += vy[j]*w1 - vz[j]*w0; }
        float a[4], b[4];

        zlin[4*i]     = xl[18*(31 - i)];
        zlin[4*i + 1] = xr[18*(31 - i)];
        zlin[4*i + 2] = xl[1 + 18*(31 - i)];
        zlin[4*i + 3] = xr[1 + 18*(31 - i)];
        zlin[4*(i + 16)]   = xl[1 + 18*(1 + i)];
        zlin[4*(i + 16) + 1] = xr[1 + 18*(1 + i)];
        zlin[4*(i - 16) + 2] = xl[18*(1 + i)];
        zlin[4*(i - 16) + 3] = xr[18*(1 + i)];

        S0(0) S2(1) S1(2) S2(3) S1(4) S2(5) S1(6) S2(7)

        dstr[(15 - i)*nch] = mp3d_scale_pcm(a[1]);
        dstr[(17 + i)*nch] = mp3d_scale_pcm(b[1]);
        dstl[(15 - i)*nch] = mp3d_scale_pcm(a[0]);
        dstl[(17 + i)*nch] = mp3d_scale_pcm(b[0]);
        dstr[(47 - i)*nch] = mp3d_scale_pcm(a[3]);
        dstr[(49 + i)*nch] = mp3d_scale_pcm(b[3]);
        dstl[(47 - i)*nch] = mp3d_scale_pcm(a[2]);
        dstl[(49 + i)*nch] = mp3d_scale_pcm(b[2]);
    }
}

static void mp3d_synth_granule(float *qmf_state, float *grbuf, int nbands, int nch, float* pcm, float *lins) {
    int i;
    for (i = 0; i < nch; i++) {
        mp3d_DCT_II(grbuf + 576*i, nbands);
    }

    memcpy(lins, qmf_state, sizeof(float)*15*64);

    for (i = 0; i < nbands; i += 2) {
        mp3d_synth(grbuf + i, pcm + 32*nch*i, nch, lins + i*64);
    }
    memcpy(qmf_state, lins + nbands*64, sizeof(float)*15*64);
}

static void L3_save_reservoir(mp3dec_t *h, mp3dec_scratch_t *s) {
    int pos = (s->bs.pos + 7)/8u;
    int remains = s->bs.limit/8u - pos;
    if (remains > MAX_BITRESERVOIR_BYTES) {
        pos += remains - MAX_BITRESERVOIR_BYTES;
        remains = MAX_BITRESERVOIR_BYTES;
    }
    if (remains > 0) {
        memmove(h->reserv_buf, s->maindata + pos, remains);
    }
    h->reserv = remains;
}

int mp3dec_decode_frame(mp3dec_t *dec, const uint8_t *mp3, int mp3_bytes, float* pcm, mp3dec_frame_info_t *info) {
    int i = 0, igr, frame_size = 0, success = 1;
    const uint8_t *hdr;
    bs_t bs_frame[1];
    mp3dec_scratch_t scratch;

    if (mp3_bytes > 4 && dec->header[0] == 0xff && hdr_compare(dec->header, mp3)) {
        frame_size = hdr_frame_bytes(mp3, dec->free_format_bytes) + hdr_padding(mp3);
        if (frame_size != mp3_bytes && (frame_size + HDR_SIZE > mp3_bytes || !hdr_compare(mp3, mp3 + frame_size))) {
            frame_size = 0;
        }
    }
    if (!frame_size) {
        memset(dec, 0, sizeof(mp3dec_t));
        i = mp3d_find_frame(mp3, mp3_bytes, &dec->free_format_bytes, &frame_size);
        if (!frame_size || i + frame_size > mp3_bytes) {
            info->frame_bytes = i;
            return 0;
        }
    }

    hdr = mp3 + i;
    memcpy(dec->header, hdr, HDR_SIZE);
    info->frame_bytes = i + frame_size;
    info->frame_offset = i;
    info->channels = HDR_IS_MONO(hdr) ? 1 : 2;
    info->hz = hdr_sample_rate_hz(hdr);
    info->layer = 4 - HDR_GET_LAYER(hdr);
    info->bitrate_kbps = hdr_bitrate_kbps(hdr);

    if (!pcm) { return hdr_frame_samples(hdr); }

    bs_frame->buf   = hdr + HDR_SIZE;
    bs_frame->pos   = 0;
    bs_frame->limit = (frame_size - HDR_SIZE) * 8;

    if (info->layer == 3) {
        int main_data_begin = L3_read_side_info(bs_frame, scratch.gr_info, hdr);
        if (main_data_begin < 0 || bs_frame->pos > bs_frame->limit) {
            dec->header[0] = 0;
            return 0;
        }
        success = L3_restore_reservoir(dec, bs_frame, &scratch, main_data_begin);
        if (success) {
            for (igr = 0; igr < (HDR_TEST_MPEG1(hdr) ? 2 : 1); igr++, pcm += 576*info->channels) {
                memset(scratch.grbuf[0], 0, 576*2*sizeof(float));
                L3_decode(dec, &scratch, scratch.gr_info + igr*info->channels, info->channels);
                mp3d_synth_granule(dec->qmf_state, scratch.grbuf[0], 18, info->channels, pcm, scratch.syn[0]);
            }
        }
        L3_save_reservoir(dec, &scratch);
    }
    else { return 0; }
    return success*hdr_frame_samples(dec->header);
}

/* minimp3_ex */
typedef size_t (*MP3D_READ_CB)(void *buf, size_t size, void *user_data);
typedef int (*MP3D_SEEK_CB)(uint64_t position, void *user_data);
typedef int (*MP3D_ITERATE_CB)(void *user_data, const uint8_t *frame, int frame_size, size_t buf_size, uint64_t offset, mp3dec_frame_info_t *info);

typedef struct {
    const uint8_t *buffer;
    size_t size;
} mp3dec_map_info_t;

typedef struct {
    MP3D_READ_CB read;
    void *read_data;
    MP3D_SEEK_CB seek;
    void *seek_data;
} mp3dec_io_t;

typedef struct {
    uint64_t sample;
    uint64_t offset;
} mp3dec_frame_t;

typedef struct {
    mp3dec_frame_t *frames;
    size_t num_frames, capacity;
} mp3dec_index_t;

typedef struct {
    mp3dec_t mp3d;
    mp3dec_map_info_t file;
    mp3dec_io_t *io;
    mp3dec_index_t index;
    uint64_t offset, samples, start_offset, end_offset;
    mp3dec_frame_info_t info;
    float buffer[MINIMP3_MAX_SAMPLES_PER_FRAME];
    size_t input_consumed, input_filled;
    int buffer_samples, buffer_consumed;
} mp3dec_ex_t;

typedef struct ZAudio {
    mp3dec_ex_t dec;
    unsigned char active;
} ZAudio;

struct ZAudio zaudio[ZAUDIO_MAX_SOUNDS] = {0};
float zaudio_output[ZAUDIO_BUFFER_FRAMES * ZAUDIO_CHANNELS];

/* helper funcs */
void ZAudioCloseFile(mp3dec_ex_t *dec) {
    free(dec->index.frames);
    if (dec->io && dec->file.buffer) { free((void*)dec->file.buffer); }

#if defined(__linux__)
    if (dec->file.buffer && (dec->file.buffer != MAP_FAILED)) {
        munmap((void *)dec->file.buffer, dec->file.size);
    }
#elif defined(_WIN32)
    if (dec->file.buffer) { UnmapViewOfFile(dec->file.buffer); }
#endif

    dec->file.buffer = 0;
    dec->file.size   = 0;
}

static int mp3dec_check_vbrtag(const uint8_t *frame, int frame_size, uint32_t *frames, int *delay, int *padding) {
    /* Side info offsets after header:
    /                Mono  Stereo
    /  MPEG1          17     32
    /  MPEG2 & 2.5     9     17*/

    bs_t bs[1];
    if (frame_size < (int)(HDR_SIZE + 8)) { return 0; }
    bs->buf   = frame + HDR_SIZE;
    bs->pos   = 0;
    bs->limit = frame_size - HDR_SIZE;
    size_t frame_size_u = (size_t)frame_size;
    size_t tag_off = HDR_SIZE + bs->pos/8;
    const uint8_t *tag = frame + tag_off;
    int flags = tag[7];
    tag_off += 8;
    if (frame_size_u - tag_off < 4) { return 0; }
    tag = frame + tag_off;
    *frames = (uint32_t)(tag[0] << 24) | (tag[1] << 16) | (tag[2] << 8) | tag[3];
    tag_off += 4;
    if (flags & 2) {
        if (frame_size_u - tag_off < 4) { return 0; }
        tag_off += 4;
    }
    if (flags & 4) {
        if (frame_size_u - tag_off < 100) { return 0; }
        tag_off += 100;
    }
    if (flags & 8) {
        if (frame_size_u - tag_off < 4) { return 0; }
        tag_off += 4;
    }
    *delay = *padding = 0;
    if (frame_size_u - tag_off < 1) { return 0; }
    tag = frame + tag_off;
    if (*tag) {   /* extension, LAME, Lavc, etc. Should be the same structure. */
        if (frame_size_u - tag_off <= 35) { return 0; }
        tag_off += 21;
        tag = frame + tag_off;
        *delay   = ((tag[0] << 4) | (tag[1] >> 4)) + (528 + 1);
        *padding = (((tag[1] & 0xF) << 8) | tag[2]) - (528 + 1);
    }
    return 1;
}

static int mp3dec_load_index(void *user_data, const uint8_t *frame, int frame_size, size_t buf_size, uint64_t offset, mp3dec_frame_info_t *info) {
    mp3dec_frame_t *idx_frame;
    mp3dec_ex_t *dec = (mp3dec_ex_t *)user_data;
    if (!dec->index.frames && !dec->start_offset) { /* detect VBR tag and try to avoid full scan */
        uint32_t frames;
        int delay, padding;
        dec->info = *info;
        dec->start_offset = dec->offset = offset;
        dec->end_offset   = offset + buf_size;
        if (3 == dec->info.layer) {
            int ret = mp3dec_check_vbrtag(frame, frame_size, &frames, &delay, &padding);
            if (ret) { dec->start_offset = dec->offset = offset + frame_size; }
            if (ret > 0) {
                padding *= info->channels;
                dec->samples = HDR_IS_LAYER_1(frame) ? 384 : (1152 >> (int)(HDR_IS_FRAME_576(frame)*info->channels*(uint64_t)frames));
                if (padding > 0 && dec->samples >= (uint64_t)padding) { dec->samples -= padding; }
                return 0;
            }
            else if (ret < 0) { return 0; }
        }
    }
    if (dec->index.num_frames + 1 > dec->index.capacity) {
        if (!dec->index.capacity) { dec->index.capacity = 4096; }
        else { dec->index.capacity *= 2; }
        mp3dec_frame_t *alloc_buf = (mp3dec_frame_t *)realloc((void*)dec->index.frames, sizeof(mp3dec_frame_t)*dec->index.capacity);
        if (!alloc_buf) { return 0; }
        dec->index.frames = alloc_buf;
    }
    idx_frame = &dec->index.frames[dec->index.num_frames++];
    idx_frame->offset = offset;
    idx_frame->sample = dec->samples;
    if (!dec->buffer_samples && dec->index.num_frames < 256) {
        /* for some cutted mp3 frames, bit-reservoir not filled and decoding can't be started from first frames */
        /* try to decode up to 255 first frames till samples starts to decode */
        dec->buffer_samples = mp3dec_decode_frame(&dec->mp3d, frame, MINIMP3_MIN(buf_size, (size_t)INT_MAX), dec->buffer, info);
        dec->samples += dec->buffer_samples*info->channels;
    }
    else { dec->samples += HDR_IS_LAYER_1(frame) ? 384 : (1152 >> (int)(HDR_IS_FRAME_576(frame)*info->channels)); }
    return 0;
}

static size_t mp3dec_skip_id3v2(const uint8_t *buf, size_t buf_size) {
    if (buf_size >= 10 && !memcmp(buf, "ID3", 3) && !((buf[5] & 15) || (buf[6] & 0x80) || (buf[7] & 0x80) || (buf[8] & 0x80) || (buf[9] & 0x80))) {
        size_t id3v2size = (((buf[6] & 0x7f) << 21) | ((buf[7] & 0x7f) << 14) | ((buf[8] & 0x7f) << 7) | (buf[9] & 0x7f)) + 10;
        if ((buf[5] & 16)) { id3v2size += 10; /* footer */ }
        return id3v2size;
    }
    return 0;
}

static void mp3dec_skip_id3v1(const uint8_t *buf, size_t *pbuf_size) {
    size_t buf_size = *pbuf_size;
    if (buf_size >= 128 && !memcmp(buf + buf_size - 128, "TAG", 3)) {
        buf_size -= 128;
        if (buf_size >= 227 && !memcmp(buf + buf_size - 227, "TAG+", 4)) { buf_size -= 227; }
    }
    if (buf_size > 32 && !memcmp(buf + buf_size - 32, "APETAGEX", 8)) {
        buf_size -= 32;
        const uint8_t *tag = buf + buf_size + 8 + 4;
        uint32_t tag_size = (uint32_t)(tag[3] << 24) | (tag[2] << 16) | (tag[1] << 8) | tag[0];
        if (buf_size >= tag_size) { buf_size -= tag_size; }
    }
    *pbuf_size = buf_size;
}

static void mp3dec_skip_id3(const uint8_t **pbuf, size_t *pbuf_size) {
    uint8_t *buf = (uint8_t *)(*pbuf);
    size_t buf_size = *pbuf_size;
    size_t id3v2size = mp3dec_skip_id3v2(buf, buf_size);
    if (id3v2size) {
        if (id3v2size >= buf_size) { id3v2size = buf_size; }
        buf      += id3v2size;
        buf_size -= id3v2size;
    }
    mp3dec_skip_id3v1(buf, &buf_size);
    *pbuf = (const uint8_t *)buf;
    *pbuf_size = buf_size;
}

void mp3dec_iterate_buf(const uint8_t *buf, size_t buf_size, MP3D_ITERATE_CB callback, void *user_data) {
    const uint8_t *orig_buf = buf;
    if (!buf || (size_t)-1 == buf_size || !callback) { return; }
    mp3dec_skip_id3(&buf, &buf_size);
    if (!buf_size) { return; }
    mp3dec_frame_info_t frame_info;
    memset(&frame_info, 0, sizeof(frame_info));
    do {
        int free_format_bytes = 0, frame_size = 0;
        int i = mp3d_find_frame(buf, buf_size, &free_format_bytes, &frame_size);
        buf      += i;
        buf_size -= i;
        if (i && !frame_size) { continue; }
        if (!frame_size) { break; }
        const uint8_t *hdr = buf;
        frame_info.channels = HDR_IS_MONO(hdr) ? 1 : 2;
        frame_info.hz = hdr_sample_rate_hz(hdr);
        frame_info.layer = 4 - HDR_GET_LAYER(hdr);
        frame_info.bitrate_kbps = hdr_bitrate_kbps(hdr);
        frame_info.frame_bytes = frame_size;

        if (callback) {
            if (callback(user_data, hdr, frame_size, buf_size, hdr - orig_buf, &frame_info)) { return; }
        }
        buf      += frame_size;
        buf_size -= frame_size;
    } while (1);
    return;
}

size_t mp3dec_ex_read_frame(mp3dec_ex_t *dec, float **buf, mp3dec_frame_info_t *frame_info, size_t max_samples) {
    *buf = NULL;
    uint64_t end_offset = dec->end_offset ? dec->end_offset : dec->file.size;
    while (dec->buffer_consumed == dec->buffer_samples) {
        const uint8_t *dec_buf;
        if (dec->io) {
            dec_buf = dec->file.buffer + dec->input_consumed;
            if (!(dec->input_filled - dec->input_consumed)) { return 0; }
            dec->buffer_samples = mp3dec_decode_frame(&dec->mp3d, dec_buf, dec->input_filled - dec->input_consumed, dec->buffer, frame_info);
            dec->input_consumed += frame_info->frame_bytes;
        }
        else {
            dec_buf = dec->file.buffer + dec->offset;
            uint64_t buf_size = end_offset - dec->offset;
            if (!buf_size) { return 0; }
            dec->buffer_samples = mp3dec_decode_frame(&dec->mp3d, dec_buf, MINIMP3_MIN(buf_size, (uint64_t)INT_MAX), dec->buffer, frame_info);
        }
        dec->buffer_consumed = 0;
        if (dec->buffer_samples) {
            dec->buffer_samples *= frame_info->channels;
            if (dec->buffer_consumed != dec->buffer_samples && dec->info.channels != frame_info->channels) {
                return 0;
            }
        }
        dec->offset += frame_info->frame_bytes;
    }
    size_t out_samples = MINIMP3_MIN((size_t)(dec->buffer_samples - dec->buffer_consumed), max_samples);
    *buf = dec->buffer + dec->buffer_consumed;
    dec->buffer_consumed += out_samples;
    return out_samples;
}

/* ZAUDIO */
void ZAudioInit(void) {
    saudio_desc saudio_desc = {0};
    saudio_desc.sample_rate = ZAUDIO_SAMPLE_RATE;
    saudio_desc.num_channels = ZAUDIO_CHANNELS;
    saudio_desc.buffer_frames = ZAUDIO_BUFFER_FRAMES;
    saudio_setup(&saudio_desc);
}

void ZAudioPlay(const char* filepath) {
    if (!filepath) { return; }

    for (unsigned char i = 0; i < ZAUDIO_MAX_SOUNDS; i++) {
        if (zaudio[i].active) { continue; }

        memset(&zaudio[i].dec, 0, sizeof(zaudio[i].dec));

#if defined(__linux__)
        int file;
        struct stat st;

        file = open(filepath, O_RDONLY);
        if (file < 0 || fstat(file, &st) < 0) {
            close(file);
            return;
        }

        zaudio[i].dec.file.size = st.st_size;

        zaudio[i].dec.file.buffer = (const uint8_t *)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, file, 0);
        close(file);
        if (MAP_FAILED == zaudio[i].dec.file.buffer) { return; }

#elif defined(_WIN32)
    HANDLE file = CreateFileA(filepath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    HANDLE mapping = NULL;
    LARGE_INTEGER s;
    s.LowPart = GetFileSize(file, (DWORD*)&s.HighPart);
    if (s.LowPart == INVALID_FILE_SIZE && GetLastError() != NO_ERROR)
        goto error;
    zaudio[i].dec.size = s.QuadPart;

    mapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!mapping)
        goto error;
    zaudio[i].dec.buffer = (const uint8_t*)MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, s.QuadPart);
    CloseHandle(mapping);
    if (!zaudio[i].dec.buffer)
        goto error;

    CloseHandle(file);
error:
    ZAudioClose(zaudio[i].dec);
    CloseHandle(file);
    return;
}
#endif
        memset(&zaudio[i].dec.mp3d.header, 0, 4);
        mp3dec_iterate_buf(zaudio[i].dec.file.buffer, zaudio[i].dec.file.size, mp3dec_load_index, &zaudio[i].dec);
        zaudio[i].dec.buffer_samples = 0;
        zaudio[i].active = 1;
        return;
    }
}

void ZAudioTick(void) {
    unsigned short frames = _saudio_fifo_writable_bytes(&_saudio.fifo) / _saudio.bytes_per_frame;
    if (frames == 0) { return; }
    else if (frames > ZAUDIO_BUFFER_FRAMES) { frames = ZAUDIO_BUFFER_FRAMES; }
    size_t samples = frames * ZAUDIO_CHANNELS;
    memset(zaudio_output, 0, ZAUDIO_BUFFER_FRAMES * ZAUDIO_CHANNELS * 4);

    for (unsigned char i = 0; i < ZAUDIO_MAX_SOUNDS; i++) {
        if (zaudio[i].active) {
            mp3dec_frame_info_t frame_info = {0};
            size_t newSamples = samples;
            size_t offset = 0;
            while (newSamples) {
                float* buf_frame = NULL;
                size_t read_samples = mp3dec_ex_read_frame(&zaudio[i].dec, &buf_frame, &frame_info, newSamples);
                if (!read_samples) { break; }
                for (size_t j = 0; j < read_samples; j++) { zaudio_output[offset + j] += buf_frame[j]; }
                offset += read_samples;
                newSamples -= read_samples;
            }

            if (offset == 0) {
                ZAudioCloseFile(&zaudio[i].dec);
                zaudio[i].active = 0;
                continue;
            }
        }
    }

    _saudio_fifo_write(&_saudio.fifo, (const uint8_t*)zaudio_output, frames * _saudio.bytes_per_frame);
}

void ZAudioDeinit(void) {
    for (unsigned char i = 0; i < ZAUDIO_MAX_SOUNDS; i++) {
        if (zaudio[i].active) {
            ZAudioCloseFile(&zaudio[i].dec);
        }
    }

    #if defined(_SAUDIO_PULSEAUDIO)
        _saudio_pulseaudio_backend_shutdown();
    #elif defined(_SAUDIO_WINDOWS)
        _saudio_wasapi_backend_shutdown();
    #elif defined(_SAUDIO_EMSCRIPTEN)
        _saudio_webaudio_backend_shutdown();
    #elif defined(_SAUDIO_ANDROID)
        _saudio_aaudio_backend_shutdown();
    #elif defined(_SAUDIO_APPLE)
        _saudio_coreaudio_backend_shutdown();
    #endif

    _saudio_mutex_destroy(&_saudio.fifo.mutex);
}

#endif // ZAUDIO_IMPLEMENTATION
#ifdef __cplusplus
    }
#endif // __cplusplus
#endif // ZAUDIO_H
