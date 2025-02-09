#ifndef MEDIAFILE_HPP
#define MEDIAFILE_HPP
#include <memory.h>
#include "MediaCore.hpp"
#include "../include/ffmpeg_common.h"
#include <sstream>

class MediaFile
{
public:
    MediaFile() = default;

    int InitMediaFile(MediaState *ms);
    // int InitMediaFile(MediaState *ms);
};

// SDL播放
static void fill_audio(void *udata, Uint8 *stream, int len);
// 播放暂停
int av_read_play(AVFormatContext *s);
int av_read_pause(AVFormatContext *s);

// 播放读取线程
// void media_play_read_thread(void *arg);

// 3.thread
//智能指针
int media_play_thread(std::shared_ptr<MediaState> ms);
int media_seek_thread(std::shared_ptr<MediaState> ms);
int media_thread(MediaState *ms);



// 1
int read_packet_thr(std::shared_ptr<MediaState> ms);
// 2
int decode_video_thr(std::shared_ptr<MediaState> ms);
// 3
int decode_audio_thr(std::shared_ptr<MediaState> ms);

int do_thread (std::shared_ptr<MediaState> ms);



// int media_play_thread(MediaState *ms);
// int media_seek_thread(MediaState *ms);
// int media_thread(MediaState *ms);

#endif // MEDIAFILE_HPP



// #ifndef MEDIAFILE_HPP
// #define MEDIAFILE_HPP


// class MediaFile
// {
// public:
//     MediaFile();

//     int InitMediaFile(void *_ms);
// };

// #endif // MEDIAFILE_HPP
