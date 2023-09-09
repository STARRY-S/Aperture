#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include "ap_utils.h"
#include "ap_cvector.h"
#include "ap_audio.h"

/**
 * Check codec format is supported or not
 */
static inline bool ap_decode_support_codec(int codec_id)
{
        switch (codec_id)
        {
        case AV_CODEC_ID_MP2:
        case AV_CODEC_ID_MP3:
                return true;
        }

        return false;
}

/**
 * @brief Check error message from libavcodec
 *
 * @param name function name
 * @param ret error code
 */
static inline int ap_decode_check_avcodec(const char *name, int ret)
{
        if (ret < 0) {
                if (name) {
                        LOGE("%s: %s", name, av_err2str(ret));
                } else {
                        LOGE("%s", av_err2str(ret));
                }
        }
        return ret;
}

static inline int ap_decode_get_fmt_from_sample_fmt(
        const char **fmt, enum AVSampleFormat sample_fmt)
{
        struct sample_fmt_entry {
                enum AVSampleFormat sample_fmt;
                const char *fmt_be, *fmt_le;
        } sample_fmt_entries[] = {
                { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
                { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
                { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
                { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
                { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
        };

        *fmt = NULL;
        for (int i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); ++i) {
                struct sample_fmt_entry *entry = &sample_fmt_entries[i];
                if (sample_fmt == entry->sample_fmt) {
                        *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
                        return 0;
                }
        }

        LOGE("ap_decode_get_fmt_from_sample_fmt: format %s does not supported",
                av_get_sample_fmt_name(sample_fmt));
        return AP_ERROR_DECODE_FMT_NSUPPORT;
}

static inline int ap_decode_frame_packet(
        AVCodecContext *cdc_ctx,
        AVFrame *frame,
        AVPacket *packet,
        FILE *fp_out,
        struct AP_Vector *out_vec)
{
        int ret = 0;
        int data_size = 0;
        int i, ch;

        if ((ret = avcodec_send_packet(cdc_ctx, packet)) < 0) {
                // LOGW("avcodec_send_packet: %s", av_err2str(ret));
        }
        data_size = av_get_bytes_per_sample(cdc_ctx->sample_fmt);

        while ((ret = avcodec_receive_frame(cdc_ctx, frame)) >= 0) {
                for (i = 0; i < frame->nb_samples; i++) {
                        for (ch = 0; ch < cdc_ctx->ch_layout.nb_channels; ch++) {
                                uint8_t *ptr = frame->data[ch] + data_size * i;
                                if (fp_out) {
                                        fwrite(ptr, 1, data_size, fp_out);
                                }
                                if (out_vec) {
                                        ap_vector_insert_back(out_vec,
                                                (char*) ptr, data_size);
                                }
                        }
                }
        }

        if ((ret != AVERROR(EAGAIN)) && (ret != AVERROR_EOF)) {
                LOGE("avcodec_receive_packet failed");
                return AP_ERROR_DECODE_FAILED;
        }
        return 0;
}

static inline void ap_decode_audio_exit(
        AVFrame *frame,
        AVPacket *packet,
        AVCodecContext *cdc_ctx,
        AVFormatContext *fmt_ctx)
{
        if (frame) {
                av_frame_free(&frame);
        }
        if (packet) {
                av_packet_free(&packet);
        }
        if (cdc_ctx) {
                avcodec_close(cdc_ctx);
                avcodec_free_context(&cdc_ctx);
        }
        if (fmt_ctx) {
                avformat_close_input(&fmt_ctx);
        }
}

static int ap_decode_audio(
        const char *input_file,         // [in] input file name
        const char *output_file,        // [in] output file name
        struct AP_Vector *out_vec,      // [out] vector stores audio PCM data
        int *format,                    // [out] format in AP_Audio_FMT
        float *frequency,               // [out] frequency
        int *channels
)
{
        if (!format || !frequency || !channels) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        AVCodec 	*codec   = NULL;
        AVCodecContext  *cdc_ctx = NULL;
        AVFormatContext *fmt_ctx = NULL;
        AVPacket 	*packet  = NULL;
        AVFrame 	*frame   = NULL;
        FILE 		*fp_out  = NULL;

        *format = 0;
        *frequency = 0.0f;
        *channels = 0;

#if AP_PLATFORM_ANDROID
        // TODO: add custom IO for ffmpeg on android platform
        LOGW("ap_decode_audio: not supported on android yet");
        return AP_ERROR_DECODE_FAILED;
#endif

        int ret = avformat_open_input(&fmt_ctx, input_file, NULL, NULL);
        if (ap_decode_check_avcodec("avformat_open_input", ret) < 0) {
                return AP_ERROR_DECODE_FAILED;
        }

        ret = avformat_find_stream_info(fmt_ctx, NULL);
        if (ap_decode_check_avcodec("avformat_find_stream_info", ret) < 0) {
                ap_decode_audio_exit(frame, packet, cdc_ctx, fmt_ctx);
                return AP_ERROR_DECODE_FAILED;
        }

        ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
        if (ap_decode_check_avcodec("av_find_best_stream", ret) < 0) {
                ap_decode_audio_exit(frame, packet, cdc_ctx, fmt_ctx);
                return AP_ERROR_DECODE_FAILED;
        }
        int codec_id = fmt_ctx->streams[ret]->codecpar->codec_id;
        if (!ap_decode_support_codec(codec_id)) {
                LOGE("ap_decode codec ID %d does not supported yet", codec_id);
                return AP_ERROR_DECODE_FMT_NSUPPORT;
        }

        codec = (AVCodec*) avcodec_find_decoder(codec_id);
        if (codec == NULL) {
                LOGE("avcodec_find_decoder failed");
                ap_decode_audio_exit(frame, packet, cdc_ctx, fmt_ctx);
                return AP_ERROR_DECODE_FAILED;
        }

        if ((cdc_ctx = avcodec_alloc_context3(codec)) == NULL) {
                LOGE("avcodec_alloc_context3 failed");
                ap_decode_audio_exit(frame, packet, cdc_ctx, fmt_ctx);
                return AP_ERROR_DECODE_FAILED;
        }

        ret = avcodec_open2(cdc_ctx, codec, NULL);
        if (ap_decode_check_avcodec("avcodec_open2", ret) < 0) {
                ap_decode_audio_exit(frame, packet, cdc_ctx, fmt_ctx);
                return AP_ERROR_DECODE_FAILED;
        }

        enum AVSampleFormat sfmt = cdc_ctx->sample_fmt;
        const char *fmt = NULL;

        if (av_sample_fmt_is_planar(sfmt)) {
                // const char *packed = av_get_sample_fmt_name(sfmt);
                // LOGW("the sample format of the decoder produced is planar %s.\n"
                //         "\tThis decoder will output the first channel only.",
                //         packed ? packed : "?");
                sfmt = av_get_packed_sample_fmt(sfmt);
        }

        ret = ap_decode_get_fmt_from_sample_fmt(&fmt, sfmt);
        AP_CHECK(ret);
        if (ret != 0) {
                ap_decode_audio_exit(frame, packet, cdc_ctx, fmt_ctx);
                return AP_ERROR_DECODE_FAILED;
        }
        if ((packet = av_packet_alloc()) == NULL) {
                LOGE("av_packet_alloc failed");
                ap_decode_audio_exit(frame, packet, cdc_ctx, fmt_ctx);
                return AP_ERROR_DECODE_FAILED;
        }

        if ((frame = av_frame_alloc()) == NULL) {
                LOGE("av_frame_alloc failed");
                ap_decode_audio_exit(frame, packet, cdc_ctx, fmt_ctx);
                return AP_ERROR_DECODE_FAILED;
        }

        if (output_file != NULL) {
                if ((fp_out = fopen(output_file, "wb")) == NULL) {
                        LOGE("fopen %s failed", output_file);
                        ap_decode_audio_exit(frame, packet, cdc_ctx, fmt_ctx);
                        return AP_ERROR_DECODE_FAILED;
                }
        }

        while ((ret = av_read_frame(fmt_ctx, packet)) == 0) {
                if (packet->size > 0) {
                        ap_decode_frame_packet(
                                cdc_ctx, frame, packet, fp_out, out_vec);
                }
        }
        ap_decode_frame_packet(cdc_ctx, frame, NULL, fp_out, out_vec);

        *format = ap_audio_fmt_av_2_ap(cdc_ctx->sample_fmt, fmt);
        *frequency = (float) cdc_ctx->sample_rate;
        *channels = cdc_ctx->ch_layout.nb_channels;
        LOGD("decoded audio: %s\n\tsamplerate: %d, channel %d, size: %.2lfM",
                input_file, cdc_ctx->sample_rate,
                cdc_ctx->ch_layout.nb_channels,
                (out_vec) ? (double) out_vec->length / 1024 / 1024 : 0.0
        );

        if (fp_out) {
                LOGI("decoded file to %s", output_file);
                LOGI("play it by using: ffplay -f %s -ac %d -ar %d %s",
                        fmt, cdc_ctx->ch_layout.nb_channels,
                        cdc_ctx->sample_rate, output_file);
                fclose(fp_out);
        }
        ap_decode_audio_exit(frame, packet, cdc_ctx, fmt_ctx);
        return 0;
}

int ap_decode_to_file(const char* filename, const char *out_name)
{
#if !AP_PLATFORM_ANDROID
        int format = 0, channels = 0;
        float frequency = 0;
        return ap_decode_audio(
                filename, out_name, NULL, &format, &frequency, &channels
        );
#else	// __ANDROID__
        LOGW("ap_decode_to_file: does not support android yet");
        return AP_ERROR_DECODE_FAILED;
#endif	// __ANDROID__
}

int ap_decode_to_memory(
        const char* filename,
        struct AP_Vector **out_vec_p,
        int *ap_format,
        float *frequency,
        int *channels)
{
        if (!out_vec_p || !ap_format || !frequency || !channels) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        struct AP_Vector *out_vec = *out_vec_p;
        if (out_vec == NULL) {
                *out_vec_p = out_vec = AP_MALLOC(sizeof(struct AP_Vector));
                if (out_vec == NULL) {
                        return AP_ERROR_MALLOC_FAILED;
                }
                memset(out_vec, 0, sizeof(struct AP_Vector));
        }

        int ret = ap_vector_init(out_vec, AP_VECTOR_CHAR);
        AP_CHECK(ret);
        ret = ap_decode_audio(
                filename, NULL, out_vec, ap_format, frequency, channels
        );
        AP_CHECK(ret);

        return ret;
}
