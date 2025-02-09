#include "MediaFFmpeg.hpp"
#include <sstream>


/*           Decoder           */

int MediaFFmpeg::DecoderInit(std::string uri,
                AVFormatContext **ifmt_ctx,
                int *v_idx, int *a_idx,
                const AVCodec **_v_codec, const AVCodec **_a_codec,
                AVCodecContext **_v_codec_ctx, AVCodecContext **_a_codec_ctx) {

    /* 0. check uri */
    if (uri.empty()) {
        std::cout << "Uri不存在..." << std::endl;
        av_log(nullptr, AV_LOG_FATAL, "Uri is empty... \n");
        return -1;
    }

    /* 1.device register */
    avdevice_register_all();

    /* 2.demux */
    int ret = 0;

    // 1..avformatcontext
    fmt_ctx = avformat_alloc_context();
    *ifmt_ctx = fmt_ctx;  // give
    if (*ifmt_ctx == nullptr) {
        av_log(nullptr, AV_LOG_FATAL, "MediaFFmpeg : avformat_alloc_context之后ms的fmt_ctx仍不存在...");
        return -1;
    }

    // 2..avformat open input
    ret = avformat_open_input(&fmt_ctx, uri.c_str(), NULL, NULL);
    if(ret < 0){
        std::cout << "avformat_open_input失败..." << std::endl;
        av_log(NULL, AV_LOG_FATAL, "avformat_open_input error \n");
        return -1;
    }

    // 3..find stream info
    ret = avformat_find_stream_info(fmt_ctx, nullptr);
    if (ret < 0) {
        std::cout << "avformat_find_stream_info初始化失败..." << std::endl;
        av_log(nullptr, AV_LOG_FATAL, "Fail to find stream info... \n");
        return -1;
    }
    // 打印
    // std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
    // av_dump_format(fmt_ctx, 0, uri.c_str(), 0);
    // std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

    // 4..find av index
// todo : 源码没指定解码器呀？ 怎么回事？
    // 🎨从av_find_best_stream中获得AVcodec，这两个参数“0, &v_codec,”
    video_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, 0, &v_codec, 0);
    audio_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, 0, &a_codec, 0);
    if (video_index < 0 || audio_index < 0) {
        std::cout << "av_find_best_stream失败..." << std::endl;
    }
    if (!v_codec || !a_codec) {
        std::cout << "a_codec or v_codec为空指针..." << std::endl;
    }
    std::cout << "video_index : " << video_index << std::endl;
    *v_idx = video_index; *a_idx = audio_index;  // give

    // std::cout << "akfjalkjfk" << v_codec->name << std::endl;  // h264

    // 5. set video and audio codec
    if (video_index >= 0) {
        // init codec_ctx and stream
        v_codec_ctx = avcodec_alloc_context3(v_codec);
        if (!v_codec_ctx) {
            std::cout << "avcodec_alloc_context3失败..." << std::endl;
            av_log(nullptr, AV_LOG_FATAL, "Fail to alloc v_codec_ctx... \n");
            return -1;
        }
        v_st = fmt_ctx->streams[video_index];

        // copy params from codepar to codex_ctx
        ret = avcodec_parameters_to_context(v_codec_ctx, v_st->codecpar);
        if (ret < 0) {
            std::cout << "avcodec_parameters_to_context失败..." << std::endl;
            av_log(nullptr, AV_LOG_FATAL, "Fail to alloc copy v_params from codepar to codex_ctx... \n");
            return -1;
        }
        // set zerolatency ? 我记得是解码器中的
        av_opt_set(v_codec_ctx->priv_data, "tune", "zerolatency", 0);


        // open2  todo 这里就不存在
        if (avcodec_open2(v_codec_ctx, v_codec, nullptr)) {
            std::cout << "video avcodec_open2失败..." << std::endl;
            av_log(nullptr, AV_LOG_FATAL, "Fail to video avcodec_open2... \n");
            return -1;
        }
        *_v_codec = v_codec;  // give
        *_v_codec_ctx = v_codec_ctx;  // give
    }

// audio
    if (*a_idx >= 0) {
        // init codec_ctx and stream
        a_codec_ctx = avcodec_alloc_context3(a_codec);
        if (!a_codec_ctx) {
            av_log(nullptr, AV_LOG_FATAL, "Fail to alloc a_codec_ctx... \n");
            return -1;
        }
        a_st = fmt_ctx->streams[audio_index];
        // copy params from codepar to codex_ctx
        ret = avcodec_parameters_to_context(a_codec_ctx, a_st->codecpar);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_FATAL, "Fail to alloc copy a_params from codepar to codex_ctx... \n");
            return -1;
        }
        // open2
        if (avcodec_open2(a_codec_ctx, a_codec, nullptr)) {
            std::cout << "audio avcodec_open2失败..." << std::endl;
            av_log(nullptr, AV_LOG_FATAL, "Fail to audio avcodec_open2... \n");
            return -1;
        }
        *_a_codec = a_codec;  // give
        *_a_codec_ctx = a_codec_ctx;  // give
    }
    return 0;
}


int MediaFFmpeg::setMediaInfobyDecoder(MediaInfo *mi, AVCodecContext *codec_ctx, AVStream *st) {
// todo : audio setMediaInfobyDecoder params 'codec_ctx' is nullptr...
    if(!mi){
        av_log(nullptr, AV_LOG_FATAL, "setMediaInfobyDecoder params 'mi' is nullptr...\n");
        return -1;
    }
    if(!st){
        av_log(nullptr, AV_LOG_FATAL, "setMediaInfobyDecoder params 'st' is nullptr...\n");
        return -1;
    }
    if(!codec_ctx){
        av_log(nullptr, AV_LOG_FATAL, "setMediaInfobyDecoder params 'codec_ctx' is nullptr...\n");
        return -1;
    }


    // video
    if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
        if (st->duration != AV_NOPTS_VALUE) {
            mi->duration = st->duration * av_q2d(st->time_base);
            std::cout << (mi->duration / 3600)<< ":" << (mi->duration % 3600) / 60 << ":" << (mi->duration % 60) << std::endl;
            std::stringstream ss;
            ss << (mi->duration / 3600) << ":" << (mi->duration % 3600) / 60 << ":" << (mi->duration % 60);
            mi->durationStr = ss.str();
        } else{
            std::cout<< "video duration unknown ! \n" << std::endl;
        }
        mi->width = codec_ctx->width;
        mi->height = codec_ctx->height;
    }
    // audio
    if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
        mi->sample_fmt        =     AV_SAMPLE_FMT_S16;
        mi->sample_rate       =     codec_ctx->sample_rate;// defaule : 48000
        mi->channels          =     codec_ctx->channels;   // defaule : 1
        mi->nb_samples        =     codec_ctx->frame_size; // defaule : 1024
        mi->audio_buffer_size =     av_samples_get_buffer_size(nullptr, mi->channels, mi->nb_samples, (enum AVSampleFormat)mi->sample_fmt, 1);
    }
    return 0;
}



/*           Encoder           */



/*           Sws Swr           */
// 任意
int MediaFFmpeg::SwsInit(void **sw_ctx, std::uint8_t **out_buffer,
                         AVFrame **out_frame, AVCodecContext *codec_ctx, AVStream *st) {
    if (!st) {
        av_log(nullptr, AV_LOG_FATAL, "SwsInit失败，st不存在...\n");
        return -1;
    }
    if (!codec_ctx) {
        av_log(nullptr, AV_LOG_FATAL, "SwsInit失败，codec_ctx不存在...\n");
        return -1;
    }
    if (!out_buffer && !out_frame) {
        av_log(nullptr, AV_LOG_FATAL, "SwsInit失败，out_buffer,out_frame都不存在...\n");
        return -1;
    }
    if (!out_buffer) {
        av_log(nullptr, AV_LOG_FATAL, "SwsInit失败，out_buffer不存在...\n");
        return -1;
    }
    if (!out_frame) {
        av_log(nullptr, AV_LOG_FATAL, "SwsInit失败，out_frame不存在...\n");
        return -1;
    }
    if (!sw_ctx) {
        av_log(nullptr, AV_LOG_FATAL, "SwsInit失败，sw_ctx不存在...\n");
        return -1;
    }


    if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
        size_t out_buf_size = av_image_get_buffer_size(AV_PIX_FMT_RGB32, codec_ctx->width, codec_ctx->height, 1);
        *out_buffer = static_cast<uint8_t *>(av_malloc(out_buf_size));
        if (!*out_buffer) {
            av_log(nullptr, AV_LOG_FATAL, "Fail to allocate out_buffer in SwsInit...");
            return -1;
        }

        AVFrame *tmp_frame = av_frame_alloc();
        if (!tmp_frame) {
            av_log(nullptr, AV_LOG_FATAL, "Fail to allocate AVFrame in SwsInit...");
            av_free(*out_buffer);
            *out_buffer = nullptr;
            return -1;
        }

        int ret = av_image_fill_arrays(tmp_frame->data, tmp_frame->linesize, *out_buffer,
                                       AV_PIX_FMT_RGB32, codec_ctx->width, codec_ctx->height, 1);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_FATAL, "Fail to av_image_fill_arrays in SwsInit...");
            av_frame_free(&tmp_frame);
            av_free(*out_buffer);
            *out_buffer = nullptr;
            return -1;
        }

        SwsContext *img_convert_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
                                                     codec_ctx->width, codec_ctx->height, AV_PIX_FMT_RGB32,
                                                     SWS_BICUBIC, nullptr, nullptr, nullptr);
        if (!img_convert_ctx) {
            av_log(nullptr, AV_LOG_FATAL, "SwsContext yuv->rgb 失败...\n");
            av_frame_free(&tmp_frame);
            av_free(*out_buffer);
            *out_buffer = nullptr;
            return -1;
        }

        *sw_ctx = reinterpret_cast<void *>(img_convert_ctx);
        *out_frame = tmp_frame;
    }

    else if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
        AVFrame *tmp_frame = av_frame_alloc();
        if (!tmp_frame) {
            av_log(nullptr, AV_LOG_FATAL, "Fail to allocate AVFrame in SwsInit...");
            return -1;
        }

        enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
        int out_sample_rate = codec_ctx->sample_rate;
        int out_channels = codec_ctx->channels;
        int out_nb_sample = codec_ctx->frame_size;
        uint64_t out_channal_layout = av_get_default_channel_layout(out_channels);
        size_t out_buffer_size = av_samples_get_buffer_size(nullptr, out_channels, out_nb_sample, out_sample_fmt, 1);
        uint8_t *audio_out_buffer = static_cast<std::uint8_t *>(av_malloc(out_buffer_size));
        if (!audio_out_buffer) {
            av_log(nullptr, AV_LOG_FATAL, "Fail to allocate audio_out_buffer in SwsInit...");
            av_frame_free(&tmp_frame);
            return -1;
        }

        SwrContext *audio_convert_swr = reinterpret_cast<SwrContext *>(swr_alloc_set_opts(nullptr,
                                                                                          out_channal_layout, out_sample_fmt, out_sample_rate,
                                                                                          codec_ctx->channel_layout, codec_ctx->sample_fmt, codec_ctx->sample_rate,
                                                                                          0, nullptr));
        if (!audio_convert_swr) {
            av_log(nullptr, AV_LOG_FATAL, "SwrContext 失败...\n");
            av_frame_free(&tmp_frame);
            av_free(audio_out_buffer);
            return -1;
        }

        int ret = swr_init(audio_convert_swr);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_FATAL, "SwrContext init 失败...\n");
            swr_free(&audio_convert_swr);
            av_frame_free(&tmp_frame);
            av_free(audio_out_buffer);
            return -1;
        }

        *out_frame = tmp_frame;
        *out_buffer = audio_out_buffer;
        *sw_ctx = reinterpret_cast<void *>(audio_convert_swr);
    }
    return 0;
}
// int MediaFFmpeg::SwsInit(void **sw_ctx, std::uint8_t **out_buffer,
//             AVFrame **out_frame,
//                          AVCodecContext *codec_ctx, AVStream *st) {
//     if (!st || !codec_ctx) return -1;

//     /* video : sws 创建RGB格式输出frame，开辟out_buffer，init sws_context */
//     if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//         // 1. alloc out_buffer
//         size_t out_buf_size = av_image_get_buffer_size(AV_PIX_FMT_RGB32, codec_ctx->width, codec_ctx->height, 1);
//         *out_buffer = static_cast<std::uint8_t *>(av_malloc(out_buf_size));
//         // 2. alloc AVFrame->data_buffer
//         AVFrame *tmp_frame = av_frame_alloc();
//         int ret = av_image_fill_arrays(tmp_frame->data, tmp_frame->linesize, *out_buffer,
//                              (enum AVPixelFormat)AV_PIX_FMT_RGB32, codec_ctx->width, codec_ctx->height, 1);
//         if (ret < 0) {
//             av_log(nullptr, AV_LOG_FATAL, "Fail to av_image_fill_arrays in SwsInit1...");
//         }
//         // 3. init sws   yuv->rgb
//         SwsContext *img_convert_ctx = sws_getContext(codec_ctx->width, codec_ctx->height,
//                                                      codec_ctx->pix_fmt,
//                                                      codec_ctx->width, codec_ctx->height,
//                                                      AV_PIX_FMT_RGB32,
//                                                      SWS_BICUBIC, nullptr, nullptr, nullptr);
//         // if (!tmp_frame) {
//         //     av_log(nullptr, AV_LOG_FATAL, "SwsContext tmp_frame为空 失败...\n");
//         //     return -1;
//         // }
//         if (!img_convert_ctx) {
//             av_log(nullptr, AV_LOG_FATAL, "SwsContext yuv->rgb 失败...\n");
//             return -1;
//         }
//         // 4. 给形参赋值
// // todo : 不是空的...
//         *sw_ctx     = (void *)img_convert_ctx;
//         *out_frame  = tmp_frame;
//         // if (!out_frame) {
//         //     av_log(nullptr, AV_LOG_FATAL, "SwsContext tmp_frame为空 失败...\n");
//         //     return -1;
//         // }
//         // if (!sw_ctx ) {
//         //     av_log(nullptr, AV_LOG_FATAL, "SwsContext yuv->rgb 失败...\n");
//         //     return -1;
//         // }
//     }
//     /* audio : swr_alloc_set_opts() + swr_init() */
//     if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
//         AVFrame *tmp_frame_ = av_frame_alloc();
//         // 1. 重采样输出参数
//         enum AVSampleFormat out_sample_fmt      = AV_SAMPLE_FMT_S16;
//         int                 out_sample_rate     = codec_ctx->sample_rate;
//         int                 out_channels        = codec_ctx->channels;
//         int                 out_nb_sample       = codec_ctx->frame_size;
//         std::uint64_t       out_channal_layout  = av_get_default_channel_layout(out_channels);  //AV_CH_LAYOUT_STEREO;  //通道布局 输出双声道
//         size_t              out_buffer_size     = av_samples_get_buffer_size(nullptr, out_channels, out_nb_sample, out_sample_fmt, 1);
//         std::uint8_t        *audio_out_buffer   = static_cast<std::uint8_t *>(av_malloc(out_buffer_size));


//         SwrContext *audio_convert_swr = (SwrContext *)swr_alloc_set_opts(nullptr,
//                                                                            out_channal_layout, out_sample_fmt, out_sample_rate,
//                                                                            codec_ctx->channel_layout, codec_ctx->sample_fmt, codec_ctx->sample_rate,
//                                                                            0, nullptr);
//         if (!audio_convert_swr) {
//             av_log(nullptr, AV_LOG_FATAL, "SwrContext 失败...\n");
//             return -1;
//         }
//         int ret = swr_init(audio_convert_swr);
//         if (ret < 0) {
//             av_log(nullptr, AV_LOG_FATAL, "SwrContext init 失败...\n");
//             return -1;
//         }
//         // 2. 传给形参
//         *out_frame  = tmp_frame_;
//         *out_buffer = audio_out_buffer;
//         *sw_ctx     = (void *)audio_convert_swr;
//     }
//     return 0;
// }


// 音视频都
int MediaFFmpeg::SwsInit(SwsContext **sws_ctx, SwrContext **swr_ctx,
            std::uint8_t **out_buffer_video, std::uint8_t **out_buffer_audio,
            AVFrame **out_frame, AVCodecContext *codec_ctx, AVStream *st) {

    if (!st || !codec_ctx) return -1;

    /* video : sws 创建RGB格式输出frame，开辟out_buffer，init sws_context */
    if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
        // 1. alloc out_buffer
        size_t out_buf_size = av_image_get_buffer_size(AV_PIX_FMT_RGB32, codec_ctx->width, codec_ctx->height, 1);
        *out_buffer_video = static_cast<std::uint8_t *>(av_malloc(out_buf_size));
        // 2. alloc AVFrame->data_buffer
        AVFrame *tmp_frame = av_frame_alloc();
        int ret = av_image_fill_arrays(tmp_frame->data, tmp_frame->linesize, *out_buffer_video,
                                       (enum AVPixelFormat)AV_PIX_FMT_RGB32, codec_ctx->width, codec_ctx->height, 1);
        if (ret < 0) {
            av_log(nullptr, AV_LOG_FATAL, "Fail to av_image_fill_arrays in SwsInit1...");
        }
        // 3. init sws   yuv->rgb
        SwsContext *img_convert_ctx = sws_getContext(codec_ctx->width, codec_ctx->height,
                                                     codec_ctx->pix_fmt,
                                                     codec_ctx->width, codec_ctx->height,
                                                     AV_PIX_FMT_RGB32,
                                                     SWS_BICUBIC, nullptr, nullptr, nullptr);
        // 4. 给形参赋值
        *sws_ctx     = img_convert_ctx;
        *out_frame  = tmp_frame;
    }
    /* audio : swr_alloc_set_opts() + swr_init() */
    else if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
        AVFrame *tmp_frame = av_frame_alloc();
        // 1. 重采样输出参数
        enum AVSampleFormat out_sample_fmt      = AV_SAMPLE_FMT_S16;
        int                 out_sample_rate     = codec_ctx->sample_rate;
        int                 out_channels        = codec_ctx->channels;
        int                 out_nb_sample       = codec_ctx->frame_size;
        std::uint64_t       out_channal_layout  = av_get_default_channel_layout(out_channels);  //AV_CH_LAYOUT_STEREO;  //通道布局 输出双声道
        size_t              out_buffer_size     = av_samples_get_buffer_size(nullptr, out_channels, out_nb_sample, out_sample_fmt, 1);
        std::uint8_t        *audio_out_buffer   = static_cast<std::uint8_t *>(av_malloc(out_buffer_size));


        SwrContext *audio_convert_swr = (SwrContext *)swr_alloc_set_opts(nullptr,
                                                                          out_channal_layout, out_sample_fmt, out_sample_rate,
                                                                          codec_ctx->channel_layout, codec_ctx->sample_fmt, codec_ctx->sample_rate,
                                                                          0, nullptr);
        swr_init(audio_convert_swr);

        // 2. 传给形参
        *out_frame          = tmp_frame;
        *out_buffer_audio   = audio_out_buffer;
        *swr_ctx            = audio_convert_swr;
    }
    return 0;

}


// #include "MediaFFmpeg.hpp"
// #include <iostream>
// #include <stdio.h>

// MediaFFmpeg::MediaFFmpeg()
// {

// }

// int MediaFFmpeg::DecoderInit(std::string url,
//                              AVFormatContext **ictx,
//                              int *v_index,
//                              int *a_index,
//                              const AVCodec **_v_codec,
//                              const AVCodec **_a_codec,
//                              AVCodecContext **_v_decode_ctx,
//                              AVCodecContext **_a_decode_ctx){

//     AVFormatContext *ic = NULL;
//     const AVCodec *v_codec = NULL;
//     const AVCodec *a_codec = NULL;
//     AVCodecContext *v_decode_ctx = NULL;
//     AVCodecContext *a_decode_ctx = NULL;

//     //init devices
//     avdevice_register_all();

//     int ret = 0;
//     //1.创建解复用上下文
//     ic = avformat_alloc_context();
//     if (!ic) {
//         av_log(NULL, AV_LOG_FATAL, "Could not allocate context.\n");
//         return -1;
//     }
//     *ictx = ic;

//     //2.open input url
//     ret = avformat_open_input(&ic, url.c_str(), NULL, NULL);
//     if(ret < 0){
//         av_log(NULL, AV_LOG_FATAL, "avformat_open_input error \n");
//         return -1;
//     }

//     ret = avformat_find_stream_info(ic, NULL);
//     if(ret < 0){
//         av_log(NULL, AV_LOG_FATAL, "avformat_find_stream_info error \n");
//         return -1;
//     }

//     av_dump_format(ic, 0, url.c_str(), 0);

//     *v_index = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1,0,&v_codec,0);
//     *a_index = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1,0,&a_codec,0);

//     if(*v_index >= 0){
//         //alloc video decode ctx
//         v_decode_ctx = avcodec_alloc_context3(v_codec);
//         if(!v_decode_ctx){
//             av_log(NULL, AV_LOG_FATAL, "avcodec_alloc_context3 error \n");
//             return -1;
//         }

//         AVStream *v_stream = ic->streams[*v_index];

//         //copy video parameters
//         ret = avcodec_parameters_to_context(v_decode_ctx, v_stream->codecpar);
//         if(ret < 0){
//             av_log(NULL, AV_LOG_FATAL, "avcodec_alloc_context3 error \n");
//             return -1;
//         }

//         av_opt_set(v_decode_ctx->priv_data, "tune", "zerolatency", 0);

//         ret = avcodec_open2(v_decode_ctx, v_codec, NULL);
//         if(ret < 0){
//             av_log(NULL, AV_LOG_FATAL, "avcodec_open2 error \n");
//             return -1;
//         }

//         *_v_codec = v_codec;
//         *_v_decode_ctx = v_decode_ctx;
//     }

//     if(*a_index >= 0){
//         //alloc audio decode ctx
//         a_decode_ctx = avcodec_alloc_context3(a_codec);
//         if(!a_decode_ctx){
//             av_log(NULL, AV_LOG_FATAL, "avcodec_alloc_context3 error \n");
//             return -1;
//         }

//         AVStream *a_stream = ic->streams[*a_index];
//         //copy audio parameters
//         ret = avcodec_parameters_to_context(a_decode_ctx, a_stream->codecpar);
//         if(ret < 0){
//             av_log(NULL, AV_LOG_FATAL, "avcodec_alloc_context3 error \n");
//             return -1;
//         }


//         ret = avcodec_open2(a_decode_ctx, a_codec, NULL);
//         if(ret < 0){
//             av_log(NULL, AV_LOG_FATAL, "avcodec_open2 error \n");
//             return -1;
//         }

//         *_a_codec = a_codec;
//         *_a_decode_ctx = a_decode_ctx;
//     }
//     return 0;
// }

// int MediaFFmpeg::DecoderGetMediaInfo(MediaInfo *mi,AVCodecContext *decodec_ctx,AVStream *stream){
//     if(!mi || !stream || !decodec_ctx){
//         return -1;

//     }

//     //video
//     if(stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
//         //获取视频总时长
//         if(AV_NOPTS_VALUE != stream->duration)
//         {
//             mi->duration =  stream->duration * av_q2d(stream->time_base);
//             std::cout << "video_time : " <<
//                 (mi->duration / 3600)<< ":" <<
//                 (mi->duration % 3600) / 60<< ":" <<
//                 (mi->duration % 60) << std::endl;

//             char formatStr[128] = {0,};
//             sprintf(formatStr, "%02d:%02d:%02d",
//                     (mi->duration / 3600),
//                     ((mi->duration % 3600) / 60),
//                     (mi->duration % 60));
//             mi->durationFormatStr = formatStr;
//         }
//         else{
//             printf("audio duration unknown ! \n");
//         }

//         mi->width = stream->codecpar->width;
//         mi->height = stream->codecpar->height;
//     }
//     //audio
//     else if(stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
//         mi->sample_fmt                  =  AV_SAMPLE_FMT_S16;
//         mi->sample_rate                 =   decodec_ctx->sample_rate;  //采样率/*48000; */
//         mi->channels                    =   decodec_ctx->channels;     //通道数/*1;  */
//         mi->nb_samples                  =   decodec_ctx->frame_size;/*1024;  */
//         mi->audio_buffer_size           =   av_samples_get_buffer_size(NULL, mi->channels, mi->nb_samples, (enum AVSampleFormat)mi->sample_fmt, 1);   //输出buff
//     }

//     return 0;
// }


// /*   sws  */
// int MediaFFmpeg::SWSInit(struct SwsContext **swsCtx,
//                          struct SwrContext **swrCtx,
//                          unsigned char **out_buffer_video,
//                          unsigned char **out_buffer_audio,
//                          AVFrame **outFrame,
//                          AVCodecContext *decode_ctx,
//                          AVStream *stream){


//     if(!decode_ctx || !stream){
//         return -1;
//     }

//     if(stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
//         *out_buffer_video = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32, decode_ctx->width, decode_ctx->height, 1));

//         AVFrame *oFrame = av_frame_alloc();
//         av_image_fill_arrays(
//             oFrame->data,
//             oFrame->linesize,
//             *out_buffer_video,
//             AV_PIX_FMT_RGB32,
//             decode_ctx->width,
//             decode_ctx->height,
//             1);


//         //初始化img_convert_ctx结构 对H264做图像转换
//         struct SwsContext *img_convert_ctx = sws_getContext(decode_ctx->width,
//                                                             decode_ctx->height,
//                                                             decode_ctx->pix_fmt,//AVPixelFormat
//                                                             decode_ctx->width,
//                                                             decode_ctx->height,
//                                                             AV_PIX_FMT_RGB32,
//                                                             SWS_BICUBIC,
//                                                             NULL, NULL, NULL);



//         *outFrame = oFrame;
//         *swsCtx = img_convert_ctx;
//     }
//     else if(stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
//         // 重采样contex
//         enum AVSampleFormat out_sample_fmt  =   AV_SAMPLE_FMT_S16;   //声音格式  SDL仅支持部分音频格式
//         int out_sample_rate                 =   decode_ctx->sample_rate;  //采样率/*48000; */
//         int out_channels                    =   decode_ctx->channels;     //通道数/*1;  */
//         int out_nb_samples                  =   decode_ctx->frame_size;/*1024;  */
//         int out_buffer_audio_size           =   av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);   //输出buff
//         unsigned char *_out_buffer_audio     =   (unsigned char *)av_malloc(MAX_AUDIO_FRAME_SIZE * out_channels);
//         uint64_t out_chn_layout             =   av_get_default_channel_layout(decode_ctx->channels); //AV_CH_LAYOUT_STEREO;  //通道布局 输出双声道

//         SwrContext *audio_convert_ctx = (struct SwrContext *)swr_alloc_set_opts(NULL,
//                                                                                  out_chn_layout,                    /*out*/
//                                                                                  out_sample_fmt,                    /*out*/
//                                                                                  out_sample_rate,                   /*out*/
//                                                                                  decode_ctx->channel_layout,           /*in*/
//                                                                                  decode_ctx->sample_fmt,               /*in*/
//                                                                                  decode_ctx->sample_rate,              /*in*/
//                                                                                  0,
//                                                                                  NULL);

//         swr_init(audio_convert_ctx);

//         *out_buffer_audio = _out_buffer_audio;
//         *swrCtx =audio_convert_ctx;
//     }

//     return 0;
// }


// int MediaFFmpeg::SWSInit2(void **swCtx,
//                           unsigned char **out_buffer,
//                           AVFrame **outFrame,
//                           AVCodecContext *decode_ctx,
//                           AVStream *stream){


//     if(!decode_ctx || !stream){
//         return -1;
//     }
//     if (swCtx == nullptr) {
//         av_log(nullptr, AV_LOG_FATAL, "fhlahsflkjasklfjklajiwfioajf");
//     }

//     if(stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
//         *out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32, decode_ctx->width, decode_ctx->height, 1));

//         AVFrame *oFrame = av_frame_alloc();
//         av_image_fill_arrays(
//             oFrame->data,
//             oFrame->linesize,
//             *out_buffer,
//             AV_PIX_FMT_RGB32,
//             decode_ctx->width,
//             decode_ctx->height,
//             1);


//         //初始化img_convert_ctx结构 对H264做图像转换
//         struct SwsContext *img_convert_ctx = sws_getContext(decode_ctx->width,
//                                                             decode_ctx->height,
//                                                             decode_ctx->pix_fmt,//AVPixelFormat
//                                                             decode_ctx->width,
//                                                             decode_ctx->height,
//                                                             AV_PIX_FMT_RGB32,
//                                                             SWS_BICUBIC,
//                                                             NULL, NULL, NULL);



//         *outFrame = oFrame;
//         *swCtx = (void*)img_convert_ctx;
//     }
//     else if(stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
//         AVFrame *oFrame = av_frame_alloc();

//         // 重采样contex
//         enum AVSampleFormat out_sample_fmt  =   AV_SAMPLE_FMT_S16;   //声音格式  SDL仅支持部分音频格式
//         int out_sample_rate                 =   decode_ctx->sample_rate;  //采样率/*48000; */
//         int out_channels                    =   decode_ctx->channels;     //通道数/*1;  */
//         int out_nb_samples                  =   decode_ctx->frame_size;/*1024;  */
//         int out_buffer_audio_size           =   av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);   //输出buff
//         unsigned char *_out_buffer_audio     =   (unsigned char *)av_malloc(MAX_AUDIO_FRAME_SIZE * out_channels);
//         uint64_t out_chn_layout             =   av_get_default_channel_layout(decode_ctx->channels); //AV_CH_LAYOUT_STEREO;  //通道布局 输出双声道


//         SwrContext *audio_convert_ctx = (struct SwrContext *)swr_alloc_set_opts(NULL,
//                                                                                  out_chn_layout,                    /*out*/
//                                                                                  out_sample_fmt,                    /*out*/
//                                                                                  out_sample_rate,                   /*out*/
//                                                                                  decode_ctx->channel_layout,           /*in*/
//                                                                                  decode_ctx->sample_fmt,               /*in*/
//                                                                                  decode_ctx->sample_rate,              /*in*/
//                                                                                  0,
//                                                                                  NULL);

//         swr_init(audio_convert_ctx);

//         *outFrame = oFrame;
//         *out_buffer = _out_buffer_audio;
//         *swCtx = (void*)audio_convert_ctx;
//     }


//     return 0;
// }


