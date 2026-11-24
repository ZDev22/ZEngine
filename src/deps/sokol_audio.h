#if defined(SOKOL_IMPL) && !defined(SOKOL_AUDIO_IMPL)
#define SOKOL_AUDIO_IMPL
#endif
#ifndef SOKOL_AUDIO_INCLUDED
#define SOKOL_AUDIO_INCLUDED (1)
#include <stddef.h> // size_t
#include <stdint.h>

#if defined(SOKOL_API_DECL) && !defined(SOKOL_AUDIO_API_DECL)
#define SOKOL_AUDIO_API_DECL SOKOL_API_DECL
#endif
#ifndef SOKOL_AUDIO_API_DECL
#if defined(_WIN32) && defined(SOKOL_DLL) && defined(SOKOL_AUDIO_IMPL)
#define SOKOL_AUDIO_API_DECL __declspec(dllexport)
#elif defined(_WIN32) && defined(SOKOL_DLL)
#define SOKOL_AUDIO_API_DECL __declspec(dllimport)
#else
#define SOKOL_AUDIO_API_DECL extern
#endif
#endif // SOKOL_AUDIO_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef enum saudio_n3ds_ndspinterptype {
    SAUDIO_N3DS_DSP_INTERP_POLYPHASE = 0,
    SAUDIO_N3DS_DSP_INTERP_LINEAR    = 1,
    SAUDIO_N3DS_DSP_INTERP_NONE      = 2,
} saudio_n3ds_ndspinterptype;

typedef struct saudio_n3ds_desc {
    /* the 3DS requires multiple queues that it alternates between. */
    /* a single buffer will "work" but is choppy due to a slight    */
    /* delay when it changes queues.                                */
    int queue_count; /* default value = 2 */

    /* NDSP_INTERP_POLYPHASE = 0 (high quality, slower) */
    /* NDSP_INTERP_LINEAR    = 1 (med quality, medium)  */
    /* NDSP_INTERP_NONE      = 2 (low quality, fast)    */
    saudio_n3ds_ndspinterptype interpolation_type; /* default value = 0 */

    /* 3DS supports different audio channels. they can be used */
    /* in a variety of ways as independent streams etc.        */
    /* this implementation in sokol does NOT allow multiple    */
    /* due to calling the global ndspInit/ndspExit functions.  */
    /* range 0-23                                        */
    int channel_id; /* default value = 0 */
} saudio_n3ds_desc;

typedef struct saudio_win32_desc {
    unsigned char skip_coinitialize; // when 1 sokol-audio will not call CoInitializeEx/CoUninitialze
} saudio_win32_desc;

typedef struct saudio_desc {
    int sample_rate;        // requested sample rate
    int num_channels;       // number of channels, default: 1 (mono)
    int buffer_frames;      // number of frames in streaming buffer
    int packet_frames;      // number of frames in a packet
    int num_packets;        // number of packets in packet queue
} saudio_desc;

/* setup sokol-audio */
SOKOL_AUDIO_API_DECL void saudio_setup(const saudio_desc* desc);

#ifdef __cplusplus
} /* extern "C" */

/* reference-based equivalents for c++ */
inline void saudio_setup(const saudio_desc& desc) { return saudio_setup(&desc); }

#endif
#endif // SOKOL_AUDIO_INCLUDED

// >>implementation
#ifdef SOKOL_AUDIO_IMPL
#define SOKOL_AUDIO_IMPL_INCLUDED (1)

#if defined(SOKOL_MALLOC) || defined(SOKOL_CALLOC) || defined(SOKOL_FREE)
#error "SOKOL_MALLOC/CALLOC/FREE macros are no longer supported, please use saudio_desc.allocator to override memory allocation functions"
#endif

#include <stdlib.h> // alloc, free
#include <string.h> // memset, memcpy
#include <stddef.h> // size_t

#ifndef SOKOL_API_IMPL
    #define SOKOL_API_IMPL
#endif
#ifndef SOKOL_DEBUG
    #ifndef NDEBUG
        #define SOKOL_DEBUG
    #endif
#endif

#ifndef _SOKOL_PRIVATE
    #if defined(__GNUC__) || defined(__clang__)
        #define _SOKOL_PRIVATE __attribute__((unused)) static
    #else
        #define _SOKOL_PRIVATE static
    #endif
#endif

#ifndef _SOKOL_UNUSED
    #define _SOKOL_UNUSED(x) (void)(x)
#endif

// platform detection defines
#if defined(__APPLE__)
    #define _SAUDIO_APPLE (1)
    #include <TargetConditionals.h>
    #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
        #define _SAUDIO_IOS (1)
    #else
        #define _SAUDIO_MACOS (1)
    #endif
#elif defined(__EMSCRIPTEN__)
    #define _SAUDIO_EMSCRIPTEN (1)
#elif defined(_WIN32)
    #define _SAUDIO_WINDOWS (1)
    #include <winapifamily.h>
    #if (defined(WINAPI_FAMILY_PARTITION) && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP))
        #error "sokol_audio.h no longer supports UWP"
    #endif
#elif defined(__ANDROID__)
    #define _SAUDIO_ANDROID (1)
#elif defined(__linux__) || defined(__unix__)
    #define _SAUDIO_LINUX (1)
    #define _SAUDIO_PULSEAUDIO (1)
#endif

// platform-specific headers and definitions
#if defined(_SAUDIO_WINDOWS)
    #if defined(__GNUC__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunknown-pragmas"
    #endif
    #define _SAUDIO_WINTHREADS (1)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <windows.h>
    #include <synchapi.h>
    #pragma comment (lib, "kernel32")
    #pragma comment (lib, "ole32")
    #ifndef CINTERFACE
    #define CINTERFACE
    #endif
    #ifndef COBJMACROS
    #define COBJMACROS
    #endif
    #ifndef CONST_VTABLE
    #define CONST_VTABLE
    #endif
    #include <mmdeviceapi.h>
    #include <audioclient.h>
    static const IID _saudio_IID_IAudioClient                               = { 0x1cb9ad4c, 0xdbfa, 0x4c32, {0xb1, 0x78, 0xc2, 0xf5, 0x68, 0xa7, 0x03, 0xb2} };
    static const IID _saudio_IID_IMMDeviceEnumerator                        = { 0xa95664d2, 0x9614, 0x4f35, {0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6} };
    static const CLSID _saudio_CLSID_IMMDeviceEnumerator                    = { 0xbcde0395, 0xe52f, 0x467c, {0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e} };
    static const IID _saudio_IID_IAudioRenderClient                         = { 0xf294acfc, 0x3146, 0x4483, {0xa7, 0xbf, 0xad, 0xdc, 0xa7, 0xc2, 0x60, 0xe2} };
    static const IID _saudio_IID_Devinterface_Audio_Render                  = { 0xe6327cad, 0xdcec, 0x4949, {0xae, 0x8a, 0x99, 0x1e, 0x97, 0x6a, 0x79, 0xd2} };
    static const IID _saudio_IID_IActivateAudioInterface_Completion_Handler = { 0x94ea2b94, 0xe9cc, 0x49e0, {0xc0, 0xff, 0xee, 0x64, 0xca, 0x8f, 0x5b, 0x90} };
    static const GUID _saudio_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT               = { 0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71} };
    #if defined(__cplusplus)
    #define _SOKOL_AUDIO_WIN32COM_ID(x) (x)
    #else
    #define _SOKOL_AUDIO_WIN32COM_ID(x) (&x)
    #endif
    /* fix for Visual Studio 2015 SDKs */
    #ifndef AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
    #define AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM 0x80000000
    #endif
    #ifndef AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
    #define AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY 0x08000000
    #endif
    #ifdef _MSC_VER
        #pragma warning(push)
        #pragma warning(disable:4505)   /* unreferenced local function has been removed */
    #endif
    #if defined(__GNUC__)
        #pragma GCC diagnostic pop
    #endif
#elif defined(_SAUDIO_APPLE)
    #define _SAUDIO_PTHREADS (1)
    #include <pthread.h>
    #if defined(_SAUDIO_IOS)
        #if !defined(__cplusplus)
            #if __has_feature(objc_arc) && !__has_feature(objc_arc_fields)
                #error "sokol_audio.h on iOS requires __has_feature(objc_arc_field) if ARC is enabled (use a more recent compiler version)"
            #endif
        #endif
        #include <AudioToolbox/AudioToolbox.h>
        #include <AVFoundation/AVFoundation.h>
    #else
        #include <AudioToolbox/AudioToolbox.h>
    #endif
#elif defined(_SAUDIO_ANDROID)
    #define _SAUDIO_PTHREADS (1)
    #include <pthread.h>
    #include "aaudio/AAudio.h"
#elif defined(_SAUDIO_LINUX)
    #define _SAUDIO_PTHREADS (1)
    #include <pthread.h>
    #if defined(_SAUDIO_PULSEAUDIO)
        #include <pulse/simple.h>
    #else
        #if !defined(__FreeBSD__)
            #include <alloca.h>
        #endif
        #define ALSA_PCM_NEW_HW_PARAMS_API
        #include <alsa/asoundlib.h>
    #endif
#elif defined(__EMSCRIPTEN__)
    #define _SAUDIO_NOTHREADS (1)
    #include <emscripten/emscripten.h>
#endif

#define _saudio_def(val, def) (((val) == 0) ? (def) : (val))
#define _saudio_def_flt(val, def) (((val) == 0.0f) ? (def) : (val))

#define _SAUDIO_DEFAULT_SAMPLE_RATE (44100)
#define _SAUDIO_DEFAULT_BUFFER_FRAMES (2048)
#define _SAUDIO_DEFAULT_PACKET_FRAMES (128)
#define _SAUDIO_DEFAULT_NUM_PACKETS ((_SAUDIO_DEFAULT_BUFFER_FRAMES/_SAUDIO_DEFAULT_PACKET_FRAMES)*4)

#ifndef SAUDIO_RING_MAX_SLOTS
#define SAUDIO_RING_MAX_SLOTS (1024)
#endif

// >>structs
#if defined(_SAUDIO_PTHREADS)

typedef struct {
    pthread_mutex_t mutex;
} _saudio_mutex_t;

#elif defined(_SAUDIO_WINTHREADS)

typedef struct {
    CRITICAL_SECTION critsec;
} _saudio_mutex_t;

#elif defined(_SAUDIO_NOTHREADS)

typedef struct {
    int dummy_mutex;
} _saudio_mutex_t;

#endif

#if defined(_SAUDIO_APPLE)

typedef struct {
    AudioQueueRef ca_audio_queue;
    #if defined(_SAUDIO_IOS)
    id ca_interruption_handler;
    #endif
} _saudio_apple_backend_t;

#elif defined(_SAUDIO_PULSEAUDIO)

typedef struct {
    pa_simple* device;
    float* buffer;
    int buffer_byte_size;
    int buffer_frames;
    pthread_t thread;
    unsigned char thread_stop;
} _saudio_pulseaudio_backend_t;

#elif defined(_SAUDIO_ANDROID)

typedef struct {
    AAudioStreamBuilder* builder;
    AAudioStream* stream;
    pthread_t thread;
    pthread_mutex_t mutex;
} _saudio_aaudio_backend_t;

#elif defined(_SAUDIO_WINDOWS)

typedef struct {
    HANDLE thread_handle;
    HANDLE buffer_end_event;
    unsigned char stop;
    UINT32 dst_buffer_frames;
    int src_buffer_frames;
    int src_buffer_byte_size;
    int src_buffer_pos;
    float* src_buffer;
} _saudio_wasapi_thread_data_t;

typedef struct {
    IMMDeviceEnumerator* device_enumerator;
    IMMDevice* device;
    IAudioClient* audio_client;
    IAudioRenderClient* render_client;
    _saudio_wasapi_thread_data_t thread;
} _saudio_wasapi_backend_t;

#elif defined(_SAUDIO_EMSCRIPTEN)

typedef struct {
    uint8_t* buffer;
} _saudio_web_backend_t;

#endif

#if defined(_SAUDIO_APPLE)
typedef _saudio_apple_backend_t _saudio_backend_t;
#elif defined(_SAUDIO_EMSCRIPTEN)
typedef _saudio_web_backend_t _saudio_backend_t;
#elif defined(_SAUDIO_WINDOWS)
typedef _saudio_wasapi_backend_t _saudio_backend_t;
#elif defined(_SAUDIO_ANDROID)
typedef _saudio_aaudio_backend_t _saudio_backend_t;
#elif defined(_SAUDIO_PULSEAUDIO)
typedef _saudio_pulseaudio_backend_t _saudio_backend_t;
#endif

/* a ringbuffer structure */
typedef struct {
    int head;  // next slot to write to
    int tail;  // next slot to read from
    int num;   // number of slots in queue
    int queue[SAUDIO_RING_MAX_SLOTS];
} _saudio_ring_t;

/* a packet FIFO structure */
typedef struct {
    int packet_size;            /* size of a single packets in bytes(!) */
    int num_packets;            /* number of packet in fifo */
    uint8_t* base_ptr;          /* packet memory chunk base pointer (dynamically allocated) */
    int cur_packet;             /* current write-packet */
    int cur_offset;             /* current byte-offset into current write packet */
    _saudio_mutex_t mutex;      /* mutex for thread-safe access */
    _saudio_ring_t read_queue;  /* buffers with data, ready to be streamed */
    _saudio_ring_t write_queue; /* empty buffers, ready to be pushed to */
} _saudio_fifo_t;

/* sokol-audio state */
typedef struct {
    int sample_rate;            /* sample rate */
    int buffer_frames;          /* number of frames in streaming buffer */
    int bytes_per_frame;        /* filled by backend */
    int packet_frames;          /* number of frames in a packet */
    int num_packets;            /* number of packets in packet queue */
    int num_channels;           /* actual number of channels */
    saudio_desc desc;
    _saudio_fifo_t fifo;
    _saudio_backend_t backend;
} _saudio_state_t;

_SOKOL_PRIVATE _saudio_state_t _saudio = {0};

// >>mutex
#if defined(_SAUDIO_NOTHREADS)

_SOKOL_PRIVATE void _saudio_mutex_init(_saudio_mutex_t* m) { (void)m; }
_SOKOL_PRIVATE void _saudio_mutex_destroy(_saudio_mutex_t* m) { (void)m; }
_SOKOL_PRIVATE void _saudio_mutex_lock(_saudio_mutex_t* m) { (void)m; }
_SOKOL_PRIVATE void _saudio_mutex_unlock(_saudio_mutex_t* m) { (void)m; }

#elif defined(_SAUDIO_PTHREADS)

_SOKOL_PRIVATE void _saudio_mutex_init(_saudio_mutex_t* m) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutex_init(&m->mutex, &attr);
}

_SOKOL_PRIVATE void _saudio_mutex_destroy(_saudio_mutex_t* m) {
    pthread_mutex_destroy(&m->mutex);
}

_SOKOL_PRIVATE void _saudio_mutex_lock(_saudio_mutex_t* m) {
    pthread_mutex_lock(&m->mutex);
}

_SOKOL_PRIVATE void _saudio_mutex_unlock(_saudio_mutex_t* m) {
    pthread_mutex_unlock(&m->mutex);
}

#elif defined(_SAUDIO_WINTHREADS)

_SOKOL_PRIVATE void _saudio_mutex_init(_saudio_mutex_t* m) {
    InitializeCriticalSection(&m->critsec);
}

_SOKOL_PRIVATE void _saudio_mutex_destroy(_saudio_mutex_t* m) {
    DeleteCriticalSection(&m->critsec);
}

_SOKOL_PRIVATE void _saudio_mutex_lock(_saudio_mutex_t* m) {
    EnterCriticalSection(&m->critsec);
}

_SOKOL_PRIVATE void _saudio_mutex_unlock(_saudio_mutex_t* m) {
    LeaveCriticalSection(&m->critsec);
}
#endif

// >>ringbuffer
_SOKOL_PRIVATE int _saudio_ring_idx(_saudio_ring_t* ring, int i) {
    return (i % ring->num);
}

_SOKOL_PRIVATE void _saudio_ring_init(_saudio_ring_t* ring, int num_slots) {
    ring->head = 0;
    ring->tail = 0;
    /* one slot reserved to detect 'full' vs 'empty' */
    ring->num = num_slots + 1;
}

_SOKOL_PRIVATE int _saudio_ring_count(_saudio_ring_t* ring) {
    if (ring->head >= ring->tail) {
        return ring->head - ring->tail;
    }
    else {
        return (ring->head + ring->num) - ring->tail;
    }
}

_SOKOL_PRIVATE void _saudio_ring_enqueue(_saudio_ring_t* ring, int val) {
    ring->queue[ring->head] = val;
    ring->head = _saudio_ring_idx(ring, ring->head + 1);
}

_SOKOL_PRIVATE int _saudio_ring_dequeue(_saudio_ring_t* ring) {
    int val = ring->queue[ring->tail];
    ring->tail = _saudio_ring_idx(ring, ring->tail + 1);
    return val;
}

// >>fifo
_SOKOL_PRIVATE int _saudio_fifo_writable_bytes(_saudio_fifo_t* fifo) {
    _saudio_mutex_lock(&fifo->mutex);
    int num_bytes = (_saudio_ring_count(&fifo->write_queue) * fifo->packet_size);
    if (fifo->cur_packet != -1) {
        num_bytes += fifo->packet_size - fifo->cur_offset;
    }
    _saudio_mutex_unlock(&fifo->mutex);
    return num_bytes;
}

/* write new data to the write queue, this is called from main thread */
_SOKOL_PRIVATE int _saudio_fifo_write(_saudio_fifo_t* fifo, const uint8_t* ptr, int num_bytes) {
    /* returns the number of bytes written, this will be smaller then requested
        if the write queue runs full
    */
    int all_to_copy = num_bytes;
    while (all_to_copy > 0) {
        /* need to grab a new packet? */
        if (fifo->cur_packet == -1) {
            _saudio_mutex_lock(&fifo->mutex);
            if (!(fifo->write_queue.head == fifo->write_queue.tail)) {
                fifo->cur_packet = _saudio_ring_dequeue(&fifo->write_queue);
            }
            _saudio_mutex_unlock(&fifo->mutex);
        }
        /* append data to current write packet */
        if (fifo->cur_packet != -1) {
            int to_copy = all_to_copy;
            const int max_copy = fifo->packet_size - fifo->cur_offset;
            if (to_copy > max_copy) {
                to_copy = max_copy;
            }
            uint8_t* dst = fifo->base_ptr + fifo->cur_packet * fifo->packet_size + fifo->cur_offset;
            memcpy(dst, ptr, (size_t)to_copy);
            ptr += to_copy;
            fifo->cur_offset += to_copy;
            all_to_copy -= to_copy;
        }
        else {
            /* early out if we're starving */
            int bytes_copied = num_bytes - all_to_copy;
            return bytes_copied;
        }
        /* if write packet is full, push to read queue */
        if (fifo->cur_offset == fifo->packet_size) {
            _saudio_mutex_lock(&fifo->mutex);
            _saudio_ring_enqueue(&fifo->read_queue, fifo->cur_packet);
            _saudio_mutex_unlock(&fifo->mutex);
            fifo->cur_packet = -1;
            fifo->cur_offset = 0;
        }
    }
    return num_bytes;
}

/* read queued data, this is called form the stream callback (maybe separate thread) */
_SOKOL_PRIVATE int _saudio_fifo_read(_saudio_fifo_t* fifo, uint8_t* ptr, int num_bytes) {
    /* NOTE: fifo_read might be called before the fifo is properly initialized */
    _saudio_mutex_lock(&fifo->mutex);
    int num_bytes_copied = 0;
    const int num_packets_needed = num_bytes / fifo->packet_size;
    uint8_t* dst = ptr;
    /* either pull a full buffer worth of data, or nothing */
    if (_saudio_ring_count(&fifo->read_queue) >= num_packets_needed) {
        for (int i = 0; i < num_packets_needed; i++) {
            int packet_index = _saudio_ring_dequeue(&fifo->read_queue);
            _saudio_ring_enqueue(&fifo->write_queue, packet_index);
            const uint8_t* src = fifo->base_ptr + packet_index * fifo->packet_size;
            memcpy(dst, src, (size_t)fifo->packet_size);
            dst += fifo->packet_size;
            num_bytes_copied += fifo->packet_size;
        }
    }
    _saudio_mutex_unlock(&fifo->mutex);
    return num_bytes_copied;
}

#if defined(_SAUDIO_PULSEAUDIO)

// pulseaudio streaming callback running in thread
_SOKOL_PRIVATE void* _saudio_pulseaudio_cb(void*) {
    float* buf_ptr = _saudio.backend.buffer;
    const int buf_size = _saudio.backend.buffer_byte_size;
    while (!_saudio.backend.thread_stop) {
        // fill streaming buffer with new data
        if (0 == _saudio_fifo_read(&_saudio.fifo, (uint8_t*)buf_ptr, buf_size)) {
            // not enough read data available, fill the entire buffer with silence
            memset(buf_ptr, 0, (size_t)buf_size);
        }
        // pa_simple_write blocks until the server has space for the data,
        // underruns are handled by the server and an error means that the
        // connection to the server is lost
        int res = pa_simple_write(_saudio.backend.device, buf_ptr, (size_t)buf_size, 0);
        if (res < 0) {
            break;
        }
    }
    return 0;
}

_SOKOL_PRIVATE void _saudio_pulseaudio_backend_init(void) {
    pa_sample_spec spec = {0};
    spec.format = PA_SAMPLE_FLOAT32NE;
    spec.rate = (uint32_t)_saudio.sample_rate;
    spec.channels = (uint8_t)_saudio.num_channels;
    _saudio.bytes_per_frame = _saudio.num_channels * (int)sizeof(float);

    // tlength is the target latency in bytes, use the server defaults for the rest
    pa_buffer_attr attr = {0};
    attr.maxlength = (uint32_t)-1;
    attr.tlength = (uint32_t)(_saudio.buffer_frames * _saudio.bytes_per_frame);
    attr.prebuf = (uint32_t)-1;
    attr.minreq = (uint32_t)-1;
    attr.fragsize = (uint32_t)-1;

    _saudio.backend.device = pa_simple_new(0, "", PA_STREAM_PLAYBACK, 0, "", &spec, 0, &attr, 0);
    _saudio.backend.buffer_byte_size = _saudio.buffer_frames * _saudio.bytes_per_frame;
    _saudio.backend.buffer = (float*)malloc((size_t)_saudio.backend.buffer_byte_size);
    memset(&_saudio.backend.buffer, 0, (size_t)_saudio.backend.buffer_byte_size);

    pthread_create(&_saudio.backend.thread, 0, _saudio_pulseaudio_cb, 0);
}

_SOKOL_PRIVATE void _saudio_pulseaudio_backend_shutdown(void) {
    _saudio.backend.thread_stop = 1;
    pthread_join(_saudio.backend.thread, 0);
    pa_simple_drain(_saudio.backend.device, 0);
    pa_simple_free(_saudio.backend.device);
    _saudio.backend.device = 0;
    free(_saudio.backend.buffer);
    _saudio.backend.buffer = 0;
}

// >>wasapi
#elif defined(_SAUDIO_WINDOWS)

/* fill intermediate buffer with new data and reset buffer_pos */
_SOKOL_PRIVATE void _saudio_wasapi_fill_buffer(void) {
    if (0 == _saudio_fifo_read(&_saudio.fifo, (uint8_t*)_saudio.backend.thread.src_buffer, _saudio.backend.thread.src_buffer_byte_size)) {
        /* not enough read data available, fill the entire buffer with silence */
        memset(_saudio.backend.thread.src_buffer, 0, (size_t)_saudio.backend.thread.src_buffer_byte_size);
    }
}

_SOKOL_PRIVATE int _saudio_wasapi_min(int a, int b) {
    return (a < b) ? a : b;
}

_SOKOL_PRIVATE void _saudio_wasapi_submit_buffer(int num_frames) {
    BYTE* wasapi_buffer = 0;
    if (FAILED(IAudioRenderClient_GetBuffer(_saudio.backend.render_client, num_frames, &wasapi_buffer))) {
        return;
    }

    /* copy samples to WASAPI buffer, refill source buffer if needed */
    int num_remaining_samples = num_frames * _saudio.num_channels;
    int buffer_pos = _saudio.backend.thread.src_buffer_pos;
    const int buffer_size_in_samples = _saudio.backend.thread.src_buffer_byte_size / (int)sizeof(float);
    float* dst = (float*)wasapi_buffer;
    const float* dst_end = dst + num_remaining_samples;
    _SOKOL_UNUSED(dst_end); // suppress unused warning in release mode
    const float* src = _saudio.backend.thread.src_buffer;

    while (num_remaining_samples > 0) {
        if (0 == buffer_pos) {
            _saudio_wasapi_fill_buffer();
        }
        const int samples_to_copy = _saudio_wasapi_min(num_remaining_samples, buffer_size_in_samples - buffer_pos);
        memcpy(dst, &src[buffer_pos], (size_t)samples_to_copy * sizeof(float));
        num_remaining_samples -= samples_to_copy;
        buffer_pos += samples_to_copy;
        dst += samples_to_copy;

        if (buffer_pos == buffer_size_in_samples) {
            buffer_pos = 0;
        }
    }
    _saudio.backend.thread.src_buffer_pos = buffer_pos;
    IAudioRenderClient_ReleaseBuffer(_saudio.backend.render_client, num_frames, 0);
}

_SOKOL_PRIVATE DWORD WINAPI _saudio_wasapi_thread_fn(LPVOID param) {
    (void)param;
    _saudio_wasapi_submit_buffer(_saudio.backend.thread.src_buffer_frames);
    IAudioClient_Start(_saudio.backend.audio_client);
    while (!_saudio.backend.thread.stop) {
        WaitForSingleObject(_saudio.backend.thread.buffer_end_event, INFINITE);
        UINT32 padding = 0;
        if (FAILED(IAudioClient_GetCurrentPadding(_saudio.backend.audio_client, &padding))) {
            continue;
        }
        int num_frames = (int)_saudio.backend.thread.dst_buffer_frames - (int)padding;
        if (num_frames > 0) {
            _saudio_wasapi_submit_buffer(num_frames);
        }
    }
    return 0;
}

_SOKOL_PRIVATE void _saudio_wasapi_release(void) {
    if (_saudio.backend.thread.src_buffer) {
        free(_saudio.backend.thread.src_buffer);
        _saudio.backend.thread.src_buffer = 0;
    }
    if (_saudio.backend.render_client) {
        IAudioRenderClient_Release(_saudio.backend.render_client);
        _saudio.backend.render_client = 0;
    }
    if (_saudio.backend.audio_client) {
        IAudioClient_Release(_saudio.backend.audio_client);
        _saudio.backend.audio_client = 0;
    }
    if (_saudio.backend.device) {
        IMMDevice_Release(_saudio.backend.device);
        _saudio.backend.device = 0;
    }
    if (_saudio.backend.device_enumerator) {
        IMMDeviceEnumerator_Release(_saudio.backend.device_enumerator);
        _saudio.backend.device_enumerator = 0;
    }
    if (0 != _saudio.backend.thread.buffer_end_event) {
        CloseHandle(_saudio.backend.thread.buffer_end_event);
        _saudio.backend.thread.buffer_end_event = 0;
    }
}

_SOKOL_PRIVATE unsigned char _saudio_wasapi_backend_init(void) {
    REFERENCE_TIME dur;
    HRESULT hr;
    if (!_saudio.desc.win32.skip_coinitialize) {
        /* CoInitializeEx could have been called elsewhere already, in which
            case the function returns with S_0 (thus it does not make much
            sense to check the result)
        */
        hr = CoInitializeEx(0, COINIT_MULTITHREADED);
        _SOKOL_UNUSED(hr);
    }
    _saudio.backend.thread.buffer_end_event = CreateEvent(0, 0, 0, 0);
    if (0 == _saudio.backend.thread.buffer_end_event) {
        goto error;
    }
    if (FAILED(CoCreateInstance(_SOKOL_AUDIO_WIN32COM_ID(_saudio_CLSID_IMMDeviceEnumerator),
        0, CLSCTX_ALL,
        _SOKOL_AUDIO_WIN32COM_ID(_saudio_IID_IMMDeviceEnumerator),
        (void**)&_saudio.backend.device_enumerator)))
    {
        goto error;
    }
    if (FAILED(IMMDeviceEnumerator_GetDefaultAudioEndpoint(_saudio.backend.device_enumerator,
        eRender, eConsole,
        &_saudio.backend.device)))
    {
        goto error;
    }
    if (FAILED(IMMDevice_Activate(_saudio.backend.device,
        _SOKOL_AUDIO_WIN32COM_ID(_saudio_IID_IAudioClient),
        CLSCTX_ALL, 0,
        (void**)&_saudio.backend.audio_client)))
    {
        goto error;
    }

    WAVEFORMATEXTENSIBLE fmtex = {(WAVEFORMATEXTENSIBLE)0};
    fmtex.Format.nChannels = (WORD)_saudio.num_channels;
    fmtex.Format.nSamplesPerSec = (DWORD)_saudio.sample_rate;
    fmtex.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    fmtex.Format.wBitsPerSample = 32;
    fmtex.Format.nBlockAlign = (fmtex.Format.nChannels * fmtex.Format.wBitsPerSample) / 8;
    fmtex.Format.nAvgBytesPerSec = fmtex.Format.nSamplesPerSec * fmtex.Format.nBlockAlign;
    fmtex.Format.cbSize = 22;   /* WORD + DWORD + GUID */
    fmtex.Samples.wValidBitsPerSample = 32;
    if (_saudio.num_channels == 1) {
        fmtex.dwChannelMask = SPEAKER_FRONT_CENTER;
    }
    else {
        fmtex.dwChannelMask = SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT;
    }
    fmtex.SubFormat = _saudio_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
    dur = (REFERENCE_TIME)
        (((double)_saudio.buffer_frames) / (((double)_saudio.sample_rate) * (1.0/10000000.0)));
    if (FAILED(IAudioClient_Initialize(_saudio.backend.audio_client,
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK|AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM|AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
        dur, 0, (WAVEFORMATEX*)&fmtex, 0)))
    {
        goto error;
    }
    if (FAILED(IAudioClient_GetBufferSize(_saudio.backend.audio_client, &_saudio.backend.thread.dst_buffer_frames))) {
        goto error;
    }
    if (FAILED(IAudioClient_GetService(_saudio.backend.audio_client,
        _SOKOL_AUDIO_WIN32COM_ID(_saudio_IID_IAudioRenderClient),
        (void**)&_saudio.backend.render_client)))
    {
        goto error;
    }
    if (FAILED(IAudioClient_SetEventHandle(_saudio.backend.audio_client, _saudio.backend.thread.buffer_end_event))) {
        goto error;
    }
    _saudio.bytes_per_frame = _saudio.num_channels * (int)sizeof(float);
    _saudio.backend.thread.src_buffer_frames = _saudio.buffer_frames;
    _saudio.backend.thread.src_buffer_byte_size = _saudio.backend.thread.src_buffer_frames * _saudio.bytes_per_frame;

    /* allocate an intermediate buffer for sample format conversion */
    _saudio.backend.thread.src_buffer = (float*)malloc((size_t)_saudio.backend.thread.src_buffer_byte_size);

    /* create streaming thread */
    _saudio.backend.thread.thread_handle = CreateThread(NULL, 0, _saudio_wasapi_thread_fn, 0, 0, 0);
    if (0 == _saudio.backend.thread.thread_handle) {
        goto error;
    }
    return 1;
error:
    _saudio_wasapi_release();
    return 0;
}

_SOKOL_PRIVATE void _saudio_wasapi_backend_shutdown(void) {
    if (_saudio.backend.thread.thread_handle) {
        _saudio.backend.thread.stop = 1;
        SetEvent(_saudio.backend.thread.buffer_end_event);
        WaitForSingleObject(_saudio.backend.thread.thread_handle, INFINITE);
        CloseHandle(_saudio.backend.thread.thread_handle);
        _saudio.backend.thread.thread_handle = 0;
    }
    if (_saudio.backend.audio_client) {
        IAudioClient_Stop(_saudio.backend.audio_client);
    }
    _saudio_wasapi_release();
    if (!_saudio.desc.win32.skip_coinitialize) {
        CoUninitialize();
    }
}

// >>webaudio
#elif defined(_SAUDIO_EMSCRIPTEN)

#ifdef __cplusplus
extern "C" {
#endif

EMSCRIPTEN_KEEPALIVE int _saudio_emsc_pull(int num_frames) {
    if (num_frames == _saudio.buffer_frames) {
        const int num_bytes = num_frames * _saudio.bytes_per_frame;
        if (0 == _saudio_fifo_read(&_saudio.fifo, _saudio.backend.buffer, num_bytes)) {
            /* not enough read data available, fill the entire buffer with silence */
            memset(_saudio.backend.buffer, 0, (size_t)num_bytes);
        }
        int res = (int) _saudio.backend.buffer;
        return res;
    }
    else {
        return 0;
    }
}

#ifdef __cplusplus
} /* extern "C" */
#endif

/* setup the WebAudio context and attach a ScriptProcessorNode */
EM_JS(int, saudio_js_init, (int sample_rate, int num_channels, int buffer_size), {
    Module._saudio_context = null;
    Module._saudio_node = null;
    if (typeof AudioContext !== 'undefined') {
        Module._saudio_context = new AudioContext({
            sampleRate: sample_rate,
            latencyHint: 'interactive',
        });
    }
    else {
        Module._saudio_context = null;
    }
    if (Module._saudio_context) {
        Module._saudio_node = Module._saudio_context.createScriptProcessor(buffer_size, 0, num_channels);
        Module._saudio_node.onaudioprocess = (event) => {
            const num_frames = event.outputBuffer.length;
            const ptr = __saudio_emsc_pull(num_frames);
            if (ptr) {
                const num_channels = event.outputBuffer.numberOfChannels;
                for (let chn = 0; chn < num_channels; chn++) {
                    const chan = event.outputBuffer.getChannelData(chn);
                    for (let i = 0; i < num_frames; i++) {
                        chan[i] = HEAPF32[(ptr>>2) + ((num_channels*i)+chn)]
                    }
                }
            }
        };
        Module._saudio_node.connect(Module._saudio_context.destination);

        // in some browsers, WebAudio needs to be activated on a user action
        const resume_webaudio = () => {
            if (Module._saudio_context) {
                const state = Module._saudio_context.state;
                if ((state === 'suspended') || (state === 'interrupted')) {
                    Module._saudio_context.resume().catch((err) => {
                        console.warn('sokol_audio.h: webaudio resume failed');
                    });
                }
            }
        };
        Module._saudio_context.onstatechange = resume_webaudio;
        document.addEventListener('visibilitychange', () => {
            if (document.visibilityState === 'visible') {
                resume_webaudio();
            }
        });
        document.addEventListener('click', resume_webaudio, {once:1});
        document.addEventListener('touchend', resume_webaudio, {once:1});
        document.addEventListener('keydown', resume_webaudio, {once:1});
        return 1;
    }
    else {
        return 0;
    }
})

/* shutdown the WebAudioContext and ScriptProcessorNode */
EM_JS(void, saudio_js_shutdown, (void), {
    \x2F\x2A\x2A @suppress {missingProperties} \x2A\x2F
    const ctx = Module._saudio_context;
    if (ctx !== null) {
        if (Module._saudio_node) {
            Module._saudio_node.disconnect();
        }
        ctx.close();
        Module._saudio_context = null;
        Module._saudio_node = null;
    }
})

/* get the actual sample rate back from the WebAudio context */
EM_JS(int, saudio_js_sample_rate, (void), {
    if (Module._saudio_context) {
        return Module._saudio_context.sampleRate;
    }
    else {
        return 0;
    }
})

/* get the actual buffer size in number of frames */
EM_JS(int, saudio_js_buffer_frames, (void), {
    if (Module._saudio_node) {
        return Module._saudio_node.bufferSize;
    }
    else {
        return 0;
    }
})

/* return 1 if the WebAudio context is currently suspended (or interrupted), else 0 */
EM_JS(int, saudio_js_suspended, (void), {
    if (Module._saudio_context) {
        const state = Module._saudio_context.state;
        if ((state === 'suspended') || (state === 'interrupted')) {
            return 1;
        } else {
            return 0;
        }
    } else {
        // shouldn't actually happen since caller makes sure that setup has been called
        return 0;
    }
})

_SOKOL_PRIVATE unsigned char _saudio_webaudio_backend_init(void) {
    if (saudio_js_init(_saudio.sample_rate, _saudio.num_channels, _saudio.buffer_frames)) {
        _saudio.bytes_per_frame = (int)sizeof(float) * _saudio.num_channels;
        _saudio.sample_rate = saudio_js_sample_rate();
        _saudio.buffer_frames = saudio_js_buffer_frames();
        const size_t buf_size = (size_t) (_saudio.buffer_frames * _saudio.bytes_per_frame);
        _saudio.backend.buffer = (uint8_t*)malloc(buf_size);
        return 1;
    }
    else {
        return 0;
    }
}

_SOKOL_PRIVATE void _saudio_webaudio_backend_shutdown(void) {
    saudio_js_shutdown();
    if (_saudio.backend.buffer) {
        free(_saudio.backend.buffer);
        _saudio.backend.buffer = 0;
    }
}

// >>aaudio
#elif defined(_SAUDIO_ANDROID)

_SOKOL_PRIVATE aaudio_data_callback_result_t _saudio_aaudio_data_callback(AAudioStream* stream, void* user_data, void* audio_data, int32_t num_frames) {
    _SOKOL_UNUSED(user_data);
    _SOKOL_UNUSED(stream);
    uint8_t* ptr = (uint8_t*)audio_data;
    int num_bytes = _saudio.bytes_per_frame * num_frames;
    if (0 == _saudio_fifo_read(&_saudio.fifo, ptr, num_bytes)) {
        // not enough read data available, fill the entire buffer with silence
        memset(ptr, 0, (size_t)num_bytes);
    }
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

_SOKOL_PRIVATE unsigned char _saudio_aaudio_start_stream(void) {
    if (AAudioStreamBuilder_openStream(_saudio.backend.builder, &_saudio.backend.stream) != AAUDIO_OK) {
        return 0;
    }
    AAudioStream_requestStart(_saudio.backend.stream);
    return 1;
}

_SOKOL_PRIVATE void _saudio_aaudio_stop_stream(void) {
    if (_saudio.backend.stream) {
        AAudioStream_requestStop(_saudio.backend.stream);
        AAudioStream_close(_saudio.backend.stream);
        _saudio.backend.stream = 0;
    }
}

_SOKOL_PRIVATE void* _saudio_aaudio_restart_stream_thread_fn(void* param) {
    _SOKOL_UNUSED(param);
    _SAUDIO_WARN(AAUDIO_RESTARTING_STREAM_AFTER_ERROR);
    pthread_mutex_lock(&_saudio.backend.mutex);
    _saudio_aaudio_stop_stream();
    _saudio_aaudio_start_stream();
    pthread_mutex_unlock(&_saudio.backend.mutex);
    return 0;
}

_SOKOL_PRIVATE void _saudio_aaudio_backend_shutdown(void) {
    pthread_mutex_lock(&_saudio.backend.mutex);
    _saudio_aaudio_stop_stream();
    pthread_mutex_unlock(&_saudio.backend.mutex);
    if (_saudio.backend.builder) {
        AAudioStreamBuilder_delete(_saudio.backend.builder);
        _saudio.backend.builder = 0;
    }
    pthread_mutex_destroy(&_saudio.backend.mutex);
}

_SOKOL_PRIVATE unsigned char _saudio_aaudio_backend_init(void) {
    _SAUDIO_INFO(USING_AAUDIO_BACKEND);

    _saudio.bytes_per_frame = _saudio.num_channels * (int)sizeof(float);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutex_init(&_saudio.backend.mutex, &attr);

    if (AAudio_createStreamBuilder(&_saudio.backend.builder) != AAUDIO_OK) {
        _saudio_aaudio_backend_shutdown();
        return 0;
    }

    AAudioStreamBuilder_setFormat(_saudio.backend.builder, AAUDIO_FORMAT_PCM_FLOAT);
    AAudioStreamBuilder_setSampleRate(_saudio.backend.builder, _saudio.sample_rate);
    AAudioStreamBuilder_setChannelCount(_saudio.backend.builder, _saudio.num_channels);
    AAudioStreamBuilder_setBufferCapacityInFrames(_saudio.backend.builder, _saudio.buffer_frames * 2);
    AAudioStreamBuilder_setFramesPerDataCallback(_saudio.backend.builder, _saudio.buffer_frames);
    AAudioStreamBuilder_setDataCallback(_saudio.backend.builder, _saudio_aaudio_data_callback, 0);

    if (!_saudio_aaudio_start_stream()) {
        _saudio_aaudio_backend_shutdown();
        return 0;
    }

    return 1;
}

// >>coreaudio
#elif defined(_SAUDIO_APPLE)

#if defined(_SAUDIO_IOS)
// silence iOS AVAudioSessionInterruptionTypeBegan deprecation warning
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#if __has_feature(objc_arc)
#define _SAUDIO_OBJC_RELEASE(obj) { obj = nil; }
#else
#define _SAUDIO_OBJC_RELEASE(obj) { [obj release]; obj = nil; }
#endif

@interface _saudio_interruption_handler : NSObject { }
@end

@implementation _saudio_interruption_handler
-(id)init {
    self = [super init];
    AVAudioSession* session = [AVAudioSession sharedInstance];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handle_interruption:) name:AVAudioSessionInterruptionNotification object:session];
    return self;
}

-(void)dealloc {
    [self remove_handler];
    #if !__has_feature(objc_arc)
    [super dealloc];
    #endif
}

-(void)remove_handler {
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AVAudioSessionInterruptionNotification" object:nil];
}

-(void)handle_interruption:(NSNotification*)notification {
    AVAudioSession* session = [AVAudioSession sharedInstance];
    NSDictionary* dict = notification.userInfo;
    NSInteger type = [[dict valueForKey:AVAudioSessionInterruptionTypeKey] integerValue];
    switch (type) {
        case AVAudioSessionInterruptionTypeBegan:
            if (_saudio.backend.ca_audio_queue) {
                AudioQueuePause(_saudio.backend.ca_audio_queue);
            }
            [session setActive:0 error:nil];
            break;
        case AVAudioSessionInterruptionTypeEnded:
            [session setActive:1 error:nil];
            if (_saudio.backend.ca_audio_queue) {
                AudioQueueStart(_saudio.backend.ca_audio_queue, NULL);
            }
            break;
        default:
            break;
    }
}
@end
#pragma clang diagnostic pop
#endif // _SAUDIO_IOS

/* NOTE: the buffer data callback is called on a separate thread! */
_SOKOL_PRIVATE void _saudio_coreaudio_callback(void* user_data, AudioQueueRef queue, AudioQueueBufferRef buffer) {
    _SOKOL_UNUSED(user_data);
    const int num_frames = (int)buffer->mAudioDataByteSize / _saudio.bytes_per_frame;
    const int num_channels = _saudio.num_channels;
    AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);
}

_SOKOL_PRIVATE void _saudio_coreaudio_backend_shutdown(void) {
    if (_saudio.backend.ca_audio_queue) {
        AudioQueueStop(_saudio.backend.ca_audio_queue, 1);
        AudioQueueDispose(_saudio.backend.ca_audio_queue, 0);
        _saudio.backend.ca_audio_queue = 0;
    }
    #if defined(_SAUDIO_IOS)
        /* remove interruption handler */
        if (_saudio.backend.ca_interruption_handler != nil) {
            [_saudio.backend.ca_interruption_handler remove_handler];
            _SAUDIO_OBJC_RELEASE(_saudio.backend.ca_interruption_handler);
        }
        /* deactivate audio session */
        AVAudioSession* session = [AVAudioSession sharedInstance];
        [session setActive:0 error:nil];;
    #endif // _SAUDIO_IOS
}

_SOKOL_PRIVATE unsigned char _saudio_coreaudio_backend_init(void) {

    #if defined(_SAUDIO_IOS)
        /* activate audio session */
        AVAudioSession* session = [AVAudioSession sharedInstance];
        [session setCategory: AVAudioSessionCategoryPlayback error:nil];
        [session setActive:1 error:nil];

        /* create interruption handler */
        _saudio.backend.ca_interruption_handler = [[_saudio_interruption_handler alloc] init];
    #endif

    /* create an audio queue with fp32 samples */
    AudioStreamBasicDescription fmt = {(AudioStreamBasicDescription)0};
    fmt.mSampleRate = (double) _saudio.sample_rate;
    fmt.mFormatID = kAudioFormatLinearPCM;
    fmt.mFormatFlags = kLinearPCMFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    fmt.mFramesPerPacket = 1;
    fmt.mChannelsPerFrame = (uint32_t) _saudio.num_channels;
    fmt.mBytesPerFrame = (uint32_t)sizeof(float) * (uint32_t)_saudio.num_channels;
    fmt.mBytesPerPacket = fmt.mBytesPerFrame;
    fmt.mBitsPerChannel = 32;
    OSStatus res = AudioQueueNewOutput(&fmt, _saudio_coreaudio_callback, 0, NULL, NULL, 0, &_saudio.backend.ca_audio_queue);
    if (0 != res) {
        return 0;
    }

    /* create 2 audio buffers */
    for (int i = 0; i < 2; i++) {
        AudioQueueBufferRef buf = NULL;
        const uint32_t buf_byte_size = (uint32_t)_saudio.buffer_frames * fmt.mBytesPerFrame;
        res = AudioQueueAllocateBuffer(_saudio.backend.ca_audio_queue, buf_byte_size, &buf);
        if (0 != res) {
            _saudio_coreaudio_backend_shutdown();
            return 0;
        }
        buf->mAudioDataByteSize = buf_byte_size;
        memset(buf->mAudioData, 0, buf->mAudioDataByteSize);
        AudioQueueEnqueueBuffer(_saudio.backend.ca_audio_queue, buf, 0, NULL);
    }

    /* init or modify actual playback parameters */
    _saudio.bytes_per_frame = (int)fmt.mBytesPerFrame;

    /* ...and start playback */
    res = AudioQueueStart(_saudio.backend.ca_audio_queue, NULL);
    if (0 != res) {
        _saudio_coreaudio_backend_shutdown();
        return 0;
    }
    return 1;
}

#endif

// >>public
SOKOL_API_IMPL void saudio_setup(const saudio_desc* desc) {
    _saudio.desc = *desc;
    _saudio.sample_rate = _saudio_def(_saudio.desc.sample_rate, _SAUDIO_DEFAULT_SAMPLE_RATE);
    _saudio.buffer_frames = _saudio_def(_saudio.desc.buffer_frames, _SAUDIO_DEFAULT_BUFFER_FRAMES);
    _saudio.packet_frames = _saudio_def(_saudio.desc.packet_frames, _SAUDIO_DEFAULT_PACKET_FRAMES);
    _saudio.num_packets = _saudio_def(_saudio.desc.num_packets, _SAUDIO_DEFAULT_NUM_PACKETS);
    _saudio.num_channels = _saudio_def(_saudio.desc.num_channels, 1);
    _saudio_mutex_init(&_saudio.fifo.mutex);

    #if defined(_SAUDIO_PULSEAUDIO)
        _saudio_pulseaudio_backend_init();
    #elif defined(_SAUDIO_WINDOWS)
        _saudio_wasapi_backend_init();
    #elif defined(_SAUDIO_EMSCRIPTEN)
        _saudio_webaudio_backend_init();
    #elif defined(_SAUDIO_ANDROID)
        _saudio_aaudio_backend_init();
    #elif defined(_SAUDIO_APPLE)
        _saudio_coreaudio_backend_init();
    #endif

    _saudio_mutex_lock(&_saudio.fifo.mutex);
    _saudio.fifo.packet_size = _saudio.packet_frames * _saudio.bytes_per_frame;
    _saudio.fifo.num_packets = _saudio.num_packets;
    _saudio.fifo.base_ptr = (uint8_t*)malloc((size_t)(_saudio.packet_frames * _saudio.bytes_per_frame * _saudio.num_packets));
    _saudio.fifo.cur_packet = -1;
    _saudio.fifo.cur_offset = 0;
    _saudio_ring_init(&_saudio.fifo.read_queue, _saudio.num_packets);
    _saudio_ring_init(&_saudio.fifo.write_queue, _saudio.num_packets);
    for (unsigned short  i = 0; i < _saudio.num_packets; i++) {
        _saudio_ring_enqueue(&_saudio.fifo.write_queue, i);
    }
    _saudio_mutex_unlock(&_saudio.fifo.mutex);
}

SOKOL_API_IMPL unsigned char saudio_suspended(void) {
    #if defined(_SAUDIO_EMSCRIPTEN)
        return 1 == saudio_js_suspended();
    #else
        return 0;
    #endif
}

#undef _saudio_def
#undef _saudio_def_flt

#if defined(_SAUDIO_WINDOWS)
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif

#endif /* SOKOL_AUDIO_IMPL */
