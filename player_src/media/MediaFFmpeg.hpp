#ifndef MEDIAFFMPEG_H
#define MEDIAFFMPEG_H
#include "../include/ffmpeg_common.h"

// 字节数
#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio    //48000 * (32/8)

typedef struct MediaInfo {
    /*video*/
    int width;
    int height;
    int duration;
    std::string durationStr;

    /*audio*/
    AVSampleFormat sample_fmt;
    int sample_rate;
    int channels;
    int nb_samples;
    unsigned int audio_buffer_size = 0;  // 输出buffer

} MediaInfo;

class MediaFFmpeg
{
public:
    MediaFFmpeg() = default;
    ~MediaFFmpeg() {
        // 关闭视频解码器和释放相关资源
        if (v_codec_ctx) {
            avcodec_close(v_codec_ctx);
            avcodec_free_context(&v_codec_ctx);
            v_codec_ctx = nullptr;
        }

        // 关闭音频解码器和释放相关资源
        if (a_codec_ctx) {
            avcodec_close(a_codec_ctx);
            avcodec_free_context(&a_codec_ctx);
            a_codec_ctx = nullptr;
        }

        // 关闭格式上下文并释放相关资源
        if (fmt_ctx) {
            avformat_close_input(&fmt_ctx);
            fmt_ctx = nullptr;
        }

        // 其他需要释放的资源，比如解码器指针和流指针
        v_codec = nullptr;
        a_codec = nullptr;
        v_st = nullptr;
        a_st = nullptr;

        // 释放完毕后，确保所有成员变量被置为 nullptr 或者合适的空值
    }
    /*           Decoder           */
    int DecoderInit(std::string uri,
                    AVFormatContext **ifmt_ctx,
                    int *v_idx, int *a_idx,
                    const AVCodec **_v_codec, const AVCodec **_a_codec,
                    AVCodecContext **_v_codec_ctx, AVCodecContext **_a_codec_ctx);  // 通过 MediaState *ms输入参数
    int setMediaInfobyDecoder(MediaInfo *mi, AVCodecContext *codec_ctx, AVStream *st);

    int DecoderData();

    /*           Encoder           */
    int EncoderInit();
    int setMediaInfobyEncoder();

    /*           Sws Swr           */
    // 任意 ： 根据音视频流类型自动判断
    int SwsInit(void **sw_ctx, std::uint8_t **out_buffer,
                AVFrame **out_frame,
                AVCodecContext *codec_ctx, AVStream *st);
    // 音视频都
    int SwsInit(SwsContext **sws_ctx, SwrContext **swr_ctx,
                std::uint8_t **out_buffer_video, std::uint8_t **out_buffer_audio,
                AVFrame **out_frame,
                AVCodecContext *codec_ctx, AVStream *st);
public:
    int             video_index     = -1;
    int             audio_index     = -1;
    AVFormatContext *fmt_ctx        = nullptr;
    const AVCodec   *v_codec        = nullptr;
    const AVCodec   *a_codec        = nullptr;
    AVCodecContext  *v_codec_ctx    = nullptr;
    AVCodecContext  *a_codec_ctx    = nullptr;
    AVStream        *v_st           = nullptr;
    AVStream        *a_st           = nullptr;

};

#endif // MEDIAFFMPEG_H



// #ifndef MEDIAFFMPEG_H
// #define MEDIAFFMPEG_H

// #include "ffmpeg_common.h"
// #include <string>

// #define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio    //48000 * (32/8)


// typedef struct MediaInfo
// {
//     int width;
//     int height;
//     int duration;
//     std::string durationFormatStr;

//     int sample_fmt;
//     int sample_rate;  //采样率/*48000; */
//     int channels;     //通道数/*1;  */
//     int nb_samples;/*1024;  */
//     int audio_buffer_size;
// }MediaInfo;

// class MediaFFmpeg
// {
// public:
//     MediaFFmpeg();

//     /*   Decoder  */
//     int DecoderInit(std::string url,
//                     AVFormatContext **ictx,
//                     int *v_index,
//                     int *a_index,
//                     const AVCodec **v_codec,
//                     const AVCodec **a_codec,
//                     AVCodecContext **v_decode_ctx,
//                     AVCodecContext **a_decode_ctx);

//     int DecoderGetMediaInfo(MediaInfo *mi,AVCodecContext *decodec_ctx, AVStream *stream);


//     int DecoderData();


//     /*   Encoder  */


//     /*   sws  */
//     int SWSInit(struct SwsContext **swsCtx,
//                 struct SwrContext **swrCtx,
//                 unsigned char **out_buffer_video,
//                 unsigned char **out_buffer_audio,
//                 AVFrame **outFrame,
//                 AVCodecContext *v_decode_ctx,
//                 AVStream *stream);

//     int SWSInit2(void **swCtx,
//                  unsigned char **out_buffer,
//                  AVFrame **outFrame,
//                  AVCodecContext *decode_ctx,
//                  AVStream *stream);

// };

// #endif // MEDIAFFMPEG_H
