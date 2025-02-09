#include "MediaFile.hpp"
#include "../qt/AVPlayerCore.hpp"
#include "MediaFFmpeg.hpp"
#include <chrono>

MediaFFmpeg mffmpeg;
// MediaFFmpeg *mffmpeg = new MediaFFmpeg();


static unsigned int audioLen = 0;           // 音频缓冲区中剩余未播放的字节数
static std::uint8_t *audioChunk = nullptr;  // 音频数据缓冲区的起始指针
static std::uint8_t *audioPos = nullptr;    // 当前音频数据的读取位置

/* -------------------------------------------SDL_fill_audio callback------------------------------------------- */
/**
 * @brief 填充音频数据到 SDL 的播放缓冲区 (stream) 中。
 *        它通过处理自定义的音频缓冲区 (audioChunk, audioPos) 中的数据，将其传递给音频设备。
 *        •	udata：用户数据，这里被转换为 MediaState* 类型以存储上下文信息。
          •	stream：SDL 提供的缓冲区，用于接收音频数据。
          •	len：SDL 要求填充的音频数据长度（字节数）。
 */
void fill_audio(void * udata, Uint8 * stream, int len)
{
    SDL_memset(stream, 0, len); //清空 stream 缓冲区，填充为静音数据（0 值）。

    if(audioLen == 0) {
        qDebug() << "剩余音频数据0";
        return;
    }

    len = (len > audioLen ? audioLen : len);

    MediaState *ms = (MediaState*)udata;

    // 将 audioPos 所指向的音频数据拷贝到 stream 缓冲区，并根据音量（ms->sdl_volume）调整音频数据。
    SDL_MixAudio(stream, audioPos, len, ms->sdl_volume);

    audioPos += len;
    audioLen -= len;
}

/* -------------------------------------------Deal Play and Pause------------------------------------------- */
int av_read_play(AVFormatContext *s) {
    if (s->iformat->read_play) {  // 检查是否存在read_play函数
        return s->iformat->read_play(s);  // 启动音视频数据的读取，使其处于播放状态
    }
    if (s->pb) {  // pb 是一个指向 AVIOContext 结构体的指针，表示输入/输出的上下文
        return avio_pause(s->pb, 0);  // 暂停输入/输出的数据流。第二个参数 0 表示播放状态被设置为 0
    }
    return AVERROR(ENOSYS);  // AVERROR(ENOSYS) 表示该系统调用不受支持的错误
}

int av_read_pause(AVFormatContext *s) {
    if (s->iformat->read_pause) {
        return s->iformat->read_pause(s);
    }
    if (s->pb) {
        return avio_pause(s->pb, 1);  // 暂停输入/输出的数据流。第二个参数 1 表示暂停状态被设置为 1，即暂停状态
    }
    return AVERROR(ENOSYS);
}






/* -------------------------------------------Set MediaState Params------------------------------------------- */
void cleanUpMediaState(std::shared_ptr<MediaState> ms) {
    // Clean up all the resources
    if (ms->video_frame) {
        av_frame_free(&ms->video_frame);
        ms->video_frame = nullptr;
    }
    if (ms->audio_frame) {
        av_frame_free(&ms->audio_frame);
        ms->audio_frame = nullptr;
    }
    if (ms->pkt) {
        av_packet_free(&ms->pkt);
        ms->pkt = nullptr;
    }
    if (ms->v_codec_ctx) {
        avcodec_free_context(&ms->v_codec_ctx);
        ms->v_codec_ctx = nullptr;
    }
    if (ms->a_codec_ctx) {
        avcodec_free_context(&ms->a_codec_ctx);
        ms->a_codec_ctx = nullptr;
    }
    if (ms->sws_ctx) {
        sws_freeContext(ms->sws_ctx);
        ms->sws_ctx = nullptr;
    }
    if (ms->swr_ctx) {
        swr_free(&ms->swr_ctx);
        ms->swr_ctx = nullptr;
    }
    if (ms->fmt_ctx) {
        avformat_close_input(&ms->fmt_ctx);
        ms->fmt_ctx = nullptr;
    }
    audioLen = 0;
    audioPos = nullptr;
    SDL_CloseAudio();
    SDL_Quit();
}
int initSDLAudio(MediaInfo &mi, MediaState *ms) {

    SDL_CloseAudio();
    SDL_Quit();

    if (SDL_Init(SDL_INIT_AUDIO)) {
        SDL_Log("Fail to init sdl audio...");
        return -1;
    }

    SDL_AudioSpec audioSpec;
    audioSpec.silence = 0;
    audioSpec.freq = mi.sample_rate;
    audioSpec.format = AUDIO_FORMAT_MAP[mi.sample_fmt];
    audioSpec.samples = mi.nb_samples;
    // audioSpec.samples =FFMAX(512, 2 << av_log2(audioSpec.freq / 30));
    audioSpec.channels = mi.channels;
    audioSpec.callback = fill_audio;
    audioSpec.userdata = ms;

    int _ret = SDL_OpenAudio(&audioSpec, nullptr);
    if (_ret != 0) {
        std::cout << "Open sdl audio失败，音频出问题..." << "return" << _ret << std::endl;
            return -1;
    }
    SDL_PauseAudio(0); // Start playing audio
    return 0;
}

int MediaFile::InitMediaFile(MediaState *ms) {
    // Clean up previous state
    // cleanUpMediaState(ms);
    int ret = -1;

    // Check shared pointer
    if (!ms->qVideoOutput) {
        std::cout << "MediaState->qVideoOutput isn't existing.." << std::endl;
        return -1;
    }

    // Initialize decoder parameters
    AVFormatContext *tmp_fmt_ctx = nullptr;
    ret = mffmpeg.DecoderInit(ms->filename, &tmp_fmt_ctx,
                          &ms->video_stream_idx, &ms->audio_stream_idx,
                          &ms->v_codec, &ms->a_codec,
                          &ms->v_codec_ctx, &ms->a_codec_ctx);
    if (ret < 0) {
        std::cout << "Decoder初始化失败..." << std::endl;
        av_log(nullptr, AV_LOG_FATAL, "Fail to Init Decoder...\n");
        return -1;
    }

    // Set MediaInfo
    MediaInfo mi;
    ret = mffmpeg.setMediaInfobyDecoder(&mi, ms->v_codec_ctx, tmp_fmt_ctx->streams[ms->video_stream_idx]);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_FATAL, "Fail to set video MediaInfo...\n");
        return -1;
    } else {
        av_log(nullptr, AV_LOG_INFO, "Success to set video MediaInfo...\n");
    }

    ret = mffmpeg.setMediaInfobyDecoder(&mi, ms->a_codec_ctx, tmp_fmt_ctx->streams[ms->audio_stream_idx]);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_FATAL, "Fail to set audio MediaInfo...\n");
        return -1;
    } else {
        av_log(nullptr, AV_LOG_INFO, "Success to set audio MediaInfo...\n");
    }

    ms->totalDuration = mi.duration;
    ms->totalDurationStr = std::move(mi.durationStr);
    ms->qVideoOutput->setTotalDuration(ms->totalDuration);
    ms->qVideoOutput->setTotalDurationStr(ms->totalDurationStr.c_str());

    // Initialize resampling
    ret = mffmpeg.SwsInit((void **)&ms->sws_ctx, &ms->out_buffer_video, &ms->rgb_frame,
                      ms->v_codec_ctx, tmp_fmt_ctx->streams[ms->video_stream_idx]);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_FATAL, "Fail to init sws...\n");
        return -1;
    }

    ret = mffmpeg.SwsInit((void **)&ms->swr_ctx, &ms->out_buffer_audio, &ms->audio_frame,
                      ms->a_codec_ctx, tmp_fmt_ctx->streams[ms->audio_stream_idx]);
    if (ret < 0) {
        av_log(nullptr, AV_LOG_FATAL, "Fail to init swr...\n");
        return -1;
    }

    ms->fmt_ctx = tmp_fmt_ctx;

    // Initialize video related packet and frame
    if (ms->video_stream_idx >= 0) {
        ms->video_frame = av_frame_alloc();
        ms->pkt = av_packet_alloc();
    }

    // Initialize audio related settings
    if (ms->audio_stream_idx >= 0) {
        ms->audio_frame = av_frame_alloc();
        ms->out_buffer_audio_size = mi.audio_buffer_size;

        // Set SDL audio spec
        ret = initSDLAudio(mi, ms);
        if (ret < 0) {
            return -1;
        }
        // 初始化音频相关缓冲区
        // ms->audioChunk = ms->out_buffer_audio;
        // ms->audioPos = ms->audioChunk;
        // ms->audioLen = ms->out_buffer_audio_size;

        // // 分配音频帧
        // ms->audio_frame = av_frame_alloc();
        // if (!ms->audio_frame) {
        //     av_log(nullptr, AV_LOG_FATAL, "Failed to allocate audio frame.\n");
        //     return -1;
        // }
    }

    // Create thread
    // v1
    ms->read_thread = std::thread(media_thread, ms);
    ms->read_thread.detach();

    // v2

    // ms->read_packet_thread = std::thread(read_packet_thr, ms);
    // ms->read_packet_thread.detach();
    // ms->decode_audio_thread = std::thread(decode_audio_thr, ms);
    // ms->decode_audio_thread.detach();
    // ms->decode_video_thread = std::thread(decode_video_thr, ms);
    // ms->decode_video_thread.detach();
    // ms->read_thread = std::thread(do_thread, ms);
    // ms->read_thread.detach();
    return 0;
}

/* ---------------------------------------thread--------------------------------------- */
/* 播放暂停 & seek & decode */
/*********************************Version 2*********************************/
// 1.入队线程
void stdThrSleep (int ms) {
    std::chrono::milliseconds sleepTime(ms);
    std::this_thread::sleep_for(sleepTime);
}
/*av_read_frame*/
void read_frame (std::shared_ptr<MediaState> ms) {
    AVPacket *packet = av_packet_alloc();
    int ret = av_read_frame(ms->fmt_ctx, packet);
    if (ret == 0) {
        if (packet->stream_index == ms->video_stream_idx) {
            // video packet enqueue
            AVPacket *ref_packet = av_packet_alloc();
            if (av_packet_ref(ref_packet, packet) == 0) {
                ms->video_pkt_que.enque(ref_packet);
            } else {
                av_packet_free(&ref_packet);
            }
        }
        // audio packet enqueue
        if (packet->stream_index == ms->audio_stream_idx) {
            AVPacket *ref_packet = av_packet_alloc();
            if (av_packet_ref(ref_packet, packet) == 0) {
                ms->audio_pkt_que.enque(ref_packet);
            } else {
                av_packet_free(&ref_packet);
            }
        }
    }
    else if (ret < 0) {
        if (!ms->mReadFileEOF && ret == AVERROR_EOF) {
            ms->mReadFileEOF = true;
        } else {
            av_log(nullptr, AV_LOG_FATAL, "Fail to av_read_frame...\n");
            return ;
        }
    }
    return ;
}
int read_packet_thr(std::shared_ptr<MediaState> ms) {
    std::cout << "aaa..." << std::endl;
    // 不退出就一直进行，EOF了就一直睡
    while (ms->exit_request == 0) {
        if (!ms->mReadPacketThr) {
            ms->mReadPacketThr = true;
        }
        // 考虑暂停情况
        if (ms->paused) {
            stdThrSleep(10);
            SDL_Delay(10);
            continue;
        }
        // check queue is full ?
        if (ms->v_codec_ctx && ms->a_codec_ctx) {
            // 存在音频和视频
            if (ms->video_pkt_que.size() > 600 && ms->audio_pkt_que.size() > 1200) {
                stdThrSleep(10);
                continue;
            }
            else if (ms->v_codec_ctx) {
                // 仅存在视频
                // avg_frame_rate平均帧率
                float sleepTime = ms->fmt_ctx->streams[ms->video_stream_idx]->avg_frame_rate.num /
                                  ms->fmt_ctx->streams[ms->video_stream_idx]->avg_frame_rate.den;
                stdThrSleep(static_cast<int>(sleepTime));
                continue;
            }
            else {
                // 仅存在音频
                stdThrSleep(10);
                continue;
            }
        }
        if (!ms->mReadFileEOF) {
            read_frame(ms);  // 没读完继续读
        } else {
            stdThrSleep(10);
        }
    }
    std::cout << "Exit read_packet_thr..." << std::endl;
    ms->mReadPacketThr = false;
    return 0;
}
// 2.出队解码线程
// audio
int decode_audio_thr (std::shared_ptr<MediaState> ms) {
    int ret = 0;
    if (!ms->audio_frame)
        ms->audio_frame = av_frame_alloc();
    while (!ms->exit_request) {
//
        if (!ms->mDecodeAudioThr)
            ms->mDecodeAudioThr = true;
//
        if (ms->paused) {
            stdThrSleep(10);
            SDL_Delay(10);
            continue;
        }
// check que
        if (ms->audio_pkt_que.isEmpty()) {
            stdThrSleep(1);
            continue;
        }
// deque
        AVPacket *de_packet =  ms->audio_pkt_que.deque();
        if (!de_packet) {
            std::cout << "Wrong..." << std::endl;
            break;
        }
// decode
        ret = avcodec_send_packet(ms->a_codec_ctx, de_packet);
        if (ret < 0) {
            av_packet_free(&de_packet);
            continue;
        }
        while (ret == 0) {
            ret = avcodec_receive_frame(ms->a_codec_ctx, ms->audio_frame);
            if (ret == 0) {
                // swr
                unsigned long long a_frame_bytes_size = ms->audio_frame->sample_rate * (ms->audio_frame->nb_samples / 8);
                ret = swr_convert(ms->swr_ctx,
                                  &ms->out_buffer_audio, a_frame_bytes_size,  // out
                                  (const uint8_t **)ms->audio_frame->data, ms->audio_frame->nb_samples);  // in
                if (ret < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Fail to swr...\n");
                    MediaStreamClose(ms);
                }
                // todo : 我觉得这里是音视频同步！！！

                // 4 SDL delay audioLen表示剩余的音频数据长度，当它大于零时，程序需要等待，直到音频数据被完全消费。通过 SDL_Delay(2)，程序会暂停 2 毫秒，从而避免占用 CPU 资源
                while (audioLen > 0) {
                    // std::cout << "等2ms" << std::endl;
                    SDL_Delay(2);  // 2ms等待目前audio data倍消费完毕
                }
                // std::cout << "消费完了 ：" << ms->audioLen << std::endl;
                // 5 更新音频Pos{buffer}以及音频剩余大小{buffer_size}
                audioChunk = ms->out_buffer_audio;
                audioPos   = audioChunk;
                audioLen   = ms->out_buffer_audio_size;

                av_frame_unref(ms->audio_frame);
            }
            else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                std::cout << "vWrong..." << std::endl;
                break;  // 跳过该帧 其他处理？？ sleep？？
            }
            else if (ret < 0) {
                std::cout << "vvvWrong..." << std::endl;
                break;
            }
        }
        av_packet_free(&de_packet);
    }
    std::cout << "Exit decode audio thread..." << std::endl;
    ms->mDecodeAudioThr = false;
    return 0;
}
// video
int decode_video_thr (std::shared_ptr<MediaState> ms) {
    int ret = 0;
    if (!ms->video_frame)
        ms->video_frame = av_frame_alloc();
    while (!ms->exit_request) {
//
        if (!ms->mDecodeVideoThr)
            ms->mDecodeVideoThr = true;
//
        if (ms->paused) {
            stdThrSleep(10);
            SDL_Delay(10);
            continue;
        }
// check que
        if (ms->video_pkt_que.isEmpty()) {
            stdThrSleep(1);
            std::cout << "check que..." << std::endl;
            continue;
        }
// deque
        AVPacket *de_packet =  ms->video_pkt_que.deque();
        if (!de_packet) {
            std::cout << "Wrong..." << std::endl;
            break;
        }
// decode
        ret = avcodec_send_packet(ms->v_codec_ctx, de_packet);
        if (ret < 0) {
            av_packet_free(&de_packet);
            std::cout << "Wrong..." << std::endl;
            continue;
        }

        AVStream *stream = ms->fmt_ctx->streams[ms->video_stream_idx];
        uint32_t second = ms->pkt->pts * av_q2d(stream->time_base);  // pts : pkt to stream

        std::stringstream ss;
        ss << second / 3600 << ":" << (second % 3600) / 60 << ":" <<
            (second % 60);
        std::string currPlaytime_tmp = ss.str();
        // 将字符串拷贝到 currPlaytime 中
        std::strncpy(ms->currPlaytime, currPlaytime_tmp.c_str(), sizeof(ms->currPlaytime) - 1);
        ms->currPlaytime[sizeof(ms->currPlaytime) - 1] = '\0'; // 确保字符串以 null

        ms->qVideoOutput->setCurrPlayTimeStr(ms->currPlaytime);
        ms->qVideoOutput->setCurrPlayTime(second);

        while (ret == 0) {
            ret = avcodec_receive_frame(ms->v_codec_ctx, ms->video_frame);
            if (ret == 0) {
                std::cout << "vvvRright..." << std::endl;
                // sws
                // 3 yuv->rgb : 获取到 video_frame 解码数据，在这里进行格式转换，然后进行渲染
                sws_scale(ms->sws_ctx, (const uint8_t * const*)ms->video_frame->data, ms->video_frame->linesize,
                          0, ms->v_codec_ctx->height,
                          ms->rgb_frame->data, ms->rgb_frame->linesize);
                // 4 rgb to QImage
                QImage img((uint8_t *)ms->rgb_frame->data[0],
                           ms->video_frame->width,
                           ms->video_frame->height,
                           QImage::Format_RGB32);
                // 5 传给qVideoOutput接口 : 播放每一帧视频图像
                ms->qVideoOutput->setImage(img);
                // 6 unref frame
                av_frame_unref(ms->video_frame);
            }
            else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                std::cout << "vWrong..." << std::endl;
                break;  // 跳过该帧 其他处理？？ sleep？？
            }
            else if (ret < 0) {
                std::cout << "vvvWrong..." << std::endl;
                break;
            }
        }
        av_packet_free(&de_packet);
    }
    std::cout << "Exit decode video thread..." << std::endl;
    ms->mDecodeVideoThr = false;
    return 0;
}

int do_thread (std::shared_ptr<MediaState> ms) {
    if (ms == nullptr) {
        av_log(nullptr, AV_LOG_FATAL, "media_thread参数ms为空...\n");
        return -1;
    }
    // std::cout << "@@@@@@@@@@@@@@进入do_thread@@@@@@@@@@@@@@" << std::endl;
    while (1) {
        if(ms->exit_request) {
            std::cout << "exit media_play_read_thread()" << std::endl;
            // delete mffmpeg;
            MediaStreamClose(ms);
            // SDL_Quit();
            ms.reset();
            // cleanUpMediaState(ms);
            std::cout << "MediaState reference : " << ms.use_count() << std::endl;
            return 0;
            // break;
        }

        if(ms->last_paused != ms->paused)
        {
            ms->last_paused = ms->paused;
            if(ms->paused)
            {
                cout << "paused~~~~" <<endl;
                continue;
            }

            if (ms->paused)
                av_read_pause(ms->fmt_ctx);
            else
                av_read_play(ms->fmt_ctx);
        }

        if(ms->paused)
        {
            //10 ms
            SDL_Delay(10);
            continue;
        }

        // std::cout << "----------进入seek部分----------" << std::endl;

                    int64_t ts = 0;        // 目标seek
        int64_t min_seek = INT_MIN;  // seek最小时间 向后seek
        int64_t max_seek = INT_MAX;  // seek最大时间 向前seek

        if (ms->seek_req) {

            // 1.计算目标seek位置
            if (ms->seek_req == 1) {  // <1>拖动进度条方式seek

                ts = static_cast<int64_t>(ms->seek_time * AV_TIME_BASE);  // 此时是目标时间
                min_seek = ts - ms->qVideoOutput->_currPlayTime > 0 ? ms->qVideoOutput->getCurrPlayTime() + 2 : INT64_MIN;
                max_seek = ts - ms->qVideoOutput->_currPlayTime < 0 ? ms->seek_time - 2 : INT64_MAX;
            } else if (ms->seek_req == 2) {  // 快进快退
                int64_t new_time = ms->qVideoOutput->_currPlayTime + ms->seek_time;
                ts = static_cast<int64_t>(new_time * AV_TIME_BASE);
                min_seek = new_time > ms->qVideoOutput->_currPlayTime ? ms->qVideoOutput->_currPlayTime + 2 : INT64_MIN;
                max_seek = new_time < ms->qVideoOutput->_currPlayTime ? ms->qVideoOutput->_currPlayTime - 2 : INT64_MAX;
            }
            else if (ms->seek_req == 3) {
                int64_t new_time = ms->qVideoOutput->getCurrPlayTime() - ms->seek_time;
                ts = static_cast<int64_t>((new_time > 0 ? new_time : 0) * AV_TIME_BASE);
                min_seek = new_time > ms->qVideoOutput->_currPlayTime ? ms->qVideoOutput->_currPlayTime + 2 : INT64_MIN;
                max_seek = new_time < ms->qVideoOutput->_currPlayTime ? ms->qVideoOutput->_currPlayTime - 2 : INT64_MAX;
            }

            // set stream_idx and min_seek max_seek
            //stream_index->流索引，但是只有在 flags 包含 AVSEEK_FLAG_FRAME 的时候才是 设置某个流的读取位置。其他情况都只是把这个流的 time_base （时间基）作为参考。
            int stream_idx  = -1;
            ms->seek_req = 0;
            ms->seek_time = 0;
            int ret = avformat_seek_file(ms->fmt_ctx,  stream_idx,  min_seek,  ts,  max_seek, AVSEEK_FLAG_BACKWARD);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR,
                       "%s: error while seeking\n", ms->fmt_ctx->url);
            }
            ms->seek_req = 0;
            ms->seek_time = 0;
            std::cout << "tststststststststststststststtsts  " << ts / AV_TIME_BASE << std::endl;
            // update curr time
            ms->qVideoOutput->_currPlayTime = ts / AV_TIME_BASE;

            // 6.刷新解码器
            avcodec_flush_buffers(ms->v_codec_ctx);
            avcodec_flush_buffers(ms->a_codec_ctx);
        }
    }
}


/*********************************Version 1*********************************/
int media_thread(MediaState *ms) {

    // std::lock_guard<std::mutex> lock(ms->mtx); // 加锁

    if (ms == nullptr) {
        av_log(nullptr, AV_LOG_FATAL, "media_thread参数ms为空...\n");
        return -1;
    }
    std::cout << "@@@@@@@@@@@@@@进入media_thread@@@@@@@@@@@@@@" << std::endl;


    while (1) {
        /* 播放暂停线程 */
        // 1.check exit request
        // std::cout << "----------进入play&pause部分----------" << std::endl;
        if(ms->exit_request) {
            std::cout << "播放退出: exit_request = 1, 退出media_thread线程，停止SDL并析构MediaState" << std::endl;
            SDL_Quit();
            delete ms;
            return 0;
        }

        if(ms->last_paused != ms->paused)
        {
            ms->last_paused = ms->paused;
            if (ms->paused) {
                std::cout << "播放暂停..." << std::endl;
                av_read_pause(ms->fmt_ctx);
            } else {
                std::cout << "恢复播放..." << std::endl;
                av_read_play(ms->fmt_ctx);
            }
        }
        // 阻塞
        if(ms->paused)
        {
            //10 ms
            SDL_Delay(10);
            continue;
        }

        /* seek线程 */
        std::cout << "----------进入seek部分----------" << std::endl;
        int64_t ts = 0;              // 目标seek
        int64_t min_seek = INT_MIN;  // seek最小时间 向后seek
        int64_t max_seek = INT_MAX;  // seek最大时间 向前seek

        if (ms->seek_req) {

            // 1.计算目标seek位置
            if (ms->seek_req == 1) {  // <1>拖动进度条方式seek

                ts = static_cast<int64_t>(ms->seek_time * AV_TIME_BASE);  // 此时是目标时间
                min_seek = ts - ms->qVideoOutput->_currPlayTime > 0 ? ms->qVideoOutput->getCurrPlayTime() + 2 : INT64_MIN;
                max_seek = ts - ms->qVideoOutput->_currPlayTime < 0 ? ms->seek_time - 2 : INT64_MAX;
            } else if (ms->seek_req == 2) {  // 快进快退
                int64_t new_time = ms->qVideoOutput->_currPlayTime + ms->seek_time;
                ts = static_cast<int64_t>(new_time * AV_TIME_BASE);
                min_seek = new_time > ms->qVideoOutput->_currPlayTime ? ms->qVideoOutput->_currPlayTime + 2 : INT64_MIN;
                max_seek = new_time < ms->qVideoOutput->_currPlayTime ? ms->qVideoOutput->_currPlayTime - 2 : INT64_MAX;
            }
            else if (ms->seek_req == 3) {
                int64_t new_time = ms->qVideoOutput->getCurrPlayTime() - ms->seek_time;
                ts = static_cast<int64_t>((new_time > 0 ? new_time : 0) * AV_TIME_BASE);
                min_seek = new_time > ms->qVideoOutput->_currPlayTime ? ms->qVideoOutput->_currPlayTime + 2 : INT64_MIN;
                max_seek = new_time < ms->qVideoOutput->_currPlayTime ? ms->qVideoOutput->_currPlayTime - 2 : INT64_MAX;
            }

            // set stream_idx and min_seek max_seek
            // stream_index->流索引，但是只有在 flags 包含 AVSEEK_FLAG_FRAME 的时候才是 设置某个流的读取位置。
            // 其他情况都只是把这个流的 time_base （时间基）作为参考。
            int stream_idx  = -1;
            int ret = avformat_seek_file(ms->fmt_ctx,  stream_idx,  min_seek,  ts,  max_seek, AVSEEK_FLAG_BACKWARD);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR,
                       "%s: error while seeking\n", ms->fmt_ctx->url);
            }
            ms->seek_req = 0;
            ms->seek_time = 0;
            // update curr time
            ms->qVideoOutput->_currPlayTime = ts / AV_TIME_BASE;
            std::stringstream ss_seek;
            int64_t seek_second = ts / AV_TIME_BASE;
            ss_seek << seek_second / 3600 << ":" << (seek_second % 3600) / 60 << ":" <<
                (seek_second % 60);
            std::cout << "目标时间: " << ss_seek.str() << "s" << std::endl;

            // 6.刷新解码器
            avcodec_flush_buffers(ms->v_codec_ctx);
            avcodec_flush_buffers(ms->a_codec_ctx);
        }

        /* 解码 */
        // 条件 ： 解复用正确、处于播放状态
        int ret = -1;
        while ((av_read_frame(ms->fmt_ctx, ms->pkt)>=0)
               && ms->exit_request == 0
               && ms->paused == 0
               && (ms->seek_req != 1 && ms->seek_req != 2 && ms->seek_req != 3))
            {
            // std::cout << "----------进入decode部分----------" << std::endl;
                // 1.video
                if (ms->pkt->stream_index == ms->video_stream_idx) {
                    // <1> 解码一个AVPacket{含有一帧/多帧}
                    ret = avcodec_send_packet(ms->v_codec_ctx, ms->pkt);
                    if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
                        std::cout << "Send video packet return EOF or EAGAIN..." << std::endl;
                        delete ms;
                        // MediaStreamClose(ms);  // close
                        break;
                    } else if (ret < 0) {
                        std::cout << "Fail to send video packet..." << std::endl;
                        delete ms;
                        // MediaStreamClose(ms);  // close
                        break;
                    }
                    // <2> 获取当前播放时间 set qVideoOutPut(pkt) curr_play_time
                    // ⚠️send解码出的pkt就是stream的时间基
                    AVStream *stream = ms->fmt_ctx->streams[ms->video_stream_idx];
                    uint32_t second = ms->pkt->pts * av_q2d(stream->time_base);  // pts : pkt to stream

                    std::stringstream ss;
                    ss << second / 3600 << ":" << (second % 3600) / 60 << ":" <<
                        (second % 60);
                    std::string currPlaytime_tmp = ss.str();
                    // 将字符串拷贝到 currPlaytime 中
                    std::strncpy(ms->currPlaytime, currPlaytime_tmp.c_str(), sizeof(ms->currPlaytime) - 1);
                    ms->currPlaytime[sizeof(ms->currPlaytime) - 1] = '\0'; // 确保字符串以 null

                    ms->qVideoOutput->setCurrPlayTimeStr(ms->currPlaytime);
                    ms->qVideoOutput->setCurrPlayTime(second);

                    // <3> while解码收帧 & yuv->rgb
                    while (1) {
                        // 1 收帧
                        ret = avcodec_receive_frame(ms->v_codec_ctx, ms->video_frame);
                        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                            break;
                        } else if (ret < 0) {
                            av_log(nullptr, AV_LOG_ERROR, "Fail to receive video frame...\n");
                            continue;
                        }
                        // 3 yuv->rgb : 获取到 video_frame 解码数据，在这里进行格式转换，然后进行渲染
                        sws_scale(ms->sws_ctx,
                                  (const uint8_t * const*)ms->video_frame->data, ms->video_frame->linesize,// src
                                  0, ms->v_codec_ctx->height,
                                  ms->rgb_frame->data, ms->rgb_frame->linesize);                           // dst
                        // 4 rgb to QImage
                        QImage img((uint8_t *)ms->rgb_frame->data[0],
                                   ms->video_frame->width,
                                   ms->video_frame->height,
                                   QImage::Format_RGB32);
                        // 5 传给qVideoOutput接口 : 播放每一帧视频图像
                        ms->qVideoOutput->setImage(img);
                        // 6 unref frame
                        av_frame_unref(ms->video_frame);
                    }
                }
                // 2. audio
                else if (ms->pkt->stream_index == ms->audio_stream_idx) {
                    // <1>解码一个AVPacket
                    ret = avcodec_send_packet(ms->a_codec_ctx, ms->pkt);
                    // <2> 解码收帧 & Planar->S16(SDL)
                    while (ret >= 0) {
                        // 1 收帧
                        ret = avcodec_receive_frame(ms->a_codec_ctx, ms->audio_frame);
                        // 2 check
                        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                            // std::cout << "Receive audio frame return EOF or EAGAIN..." << std::endl;
                            break;
                        } else if (ret < 0) {
                            av_log(nullptr, AV_LOG_ERROR, "Fail to receive audio frame...\n");
                            continue;
                        }
                        // 3 重采样
                        unsigned long long a_frame_bytes_size = ms->audio_frame->sample_rate * (ms->audio_frame->nb_samples / 8);
                        ret = swr_convert(ms->swr_ctx,
                                          &ms->out_buffer_audio, a_frame_bytes_size,  // out
                                          (const uint8_t **)ms->audio_frame->data, ms->audio_frame->nb_samples);  // in
                        if (ret < 0) {
                            av_log(NULL, AV_LOG_ERROR, "Fail to swr...\n");
                            delete ms;
                            return -1;
                            // MediaStreamClose(ms);
                        }
                        // todo : 我觉得这里是音视频同步！！！
                        // 4 SDL delay audioLen表示剩余的音频数据长度，当它大于零时，程序需要等待，直到音频数据被完全消费。通过 SDL_Delay(2)，程序会暂停 2 毫秒，从而避免占用 CPU 资源
                        while (audioLen > 0) {
                            // std::cout << "等2ms" << std::endl;
                            SDL_Delay(2);  // 2ms等待目前audio data倍消费完毕
                        }
                        // std::cout << "消费完了 ：" << ms->audioLen << std::endl;
                        // 5 更新音频Pos{buffer}以及音频剩余大小{buffer_size}
                        audioChunk = ms->out_buffer_audio;
                        audioPos   = audioChunk;
                        audioLen   = ms->out_buffer_audio_size;

                        // 6 unref
                        av_frame_unref(ms->audio_frame);
                    }
                }
                av_packet_unref(ms->pkt);
            }

        if (ret < 0) {
            std::cout << "Media Thread return : " << ret << std::endl;
            // MediaStreamClose(ms);
            continue;
            // goto end;
        }
    }
}

























