#ifndef MEDIACORE_HPP
#define MEDIACORE_HPP
#include <map>
#include <mutex>
#include <list>
#include <string>
#include <memory.h>
#include "PacketQueue.h"
#include "../include/ffmpeg_common.h"

#define CURR_PLAY_TIME_SIZE 30


// audio sample_format from ffmpeg map to sdl
// SDL2.0不支持plannar格式 FLTP需要重采样才能播放
static std::map<int, int> AUDIO_FORMAT_MAP {
    {AV_SAMPLE_FMT_NONE, -1},
    {AV_SAMPLE_FMT_U8, AUDIO_U8},
    {AV_SAMPLE_FMT_S16, AUDIO_S16SYS},
    {AV_SAMPLE_FMT_S32, AUDIO_S32SYS},
    {AV_SAMPLE_FMT_FLT, AUDIO_F32SYS}
};

/*Qplayer : use PakcetQueue class*/
// AVPacketQueue
// typedef struct PacketQueue {
//     std::list<AVPacket> pkt_list;
//     int nb_packets;
//     int size;
// } PacketQueue;

// MEDIA_TYPE
enum MEDIA_TYPE {
    MEDIA_TYPE_MEDIA_PLAY,
    MEDIA_TYPE_RECORD_PLAY
};

// MODEL_TYPE
enum MODEL_TYPE {
    MODEL_TYPE_NULL,
    MODEL_TYPE_RECORD,
    MPDEL_TYPE_PHOTO
};

// 前置声明
class QVideoOutput;

class MediaState {

friend std::shared_ptr<MediaState> MediaStreamOpen(std::shared_ptr<MediaState> ms,
                                                       MEDIA_TYPE type,
                                                       std::string file, void *u_video_output);
friend void MediaStreamClose(std::shared_ptr<MediaState> ms);

public:
    // construct1 : default
    MediaState() {}
    // constructor2 :Init MediaState
    MediaState(MEDIA_TYPE media_type_, std::string filename_, void *u_video_output);
    // destory
    ~MediaState();

    void MediaStreamStart(MEDIA_TYPE type, std::string file, void *u_video_output);


public:
    std::mutex              mtx;
    // 1.文件名
    std::string             filename;
    // 2.退出请求
    std::atomic<int>        exit_request;
    // 3.播放暂停请求/状态
    int                     paused;  // 后面会按位取反
    int                     last_paused;
    // 4.seek请求/时间
    int                     seek_req;
    int                     seek_time;  //seek time === totalDuration[0,N]
    // 5. TYPE
    MEDIA_TYPE              media_type;
    MODEL_TYPE              model_type;
    // 6.FFMPEG
    AVInputFormat           *iformat = nullptr;
    AVFormatContext         *fmt_ctx = nullptr;

    int                     totalDuration;
    std::string             totalDurationStr;
    char                    currPlaytime[CURR_PLAY_TIME_SIZE];

    AVStream                *video_st = nullptr;
    int                     video_stream_idx;
    PacketQueue<AVPacket *> video_pkt_que;  // todo

    AVStream                *audio_st = nullptr;
    int                     audio_stream_idx;
    PacketQueue<AVPacket *> audio_pkt_que;  // todo

    const AVCodec           *v_codec = nullptr;
    AVCodecContext          *v_codec_ctx = nullptr;

    const AVCodec           *a_codec = nullptr;
    AVCodecContext          *a_codec_ctx = nullptr;

    AVFrame                 *video_frame = nullptr;
    AVFrame                 *audio_frame = nullptr;
    AVFrame                 *rgb_frame = nullptr;

    AVPacket                *pkt = nullptr;

    uint8_t                 *out_buffer_video = nullptr;
    uint8_t                 *out_buffer_audio = nullptr;
    int                     out_buffer_audio_size;

    SwsContext              *sws_ctx = nullptr;
    SwrContext              *swr_ctx = nullptr;

    unsigned int            sdl_volume = SDL_MIX_MAXVOLUME;

    QVideoOutput            *qVideoOutput = nullptr;

    // thread
    std::thread             read_thread;
    std::thread             write_thread;


    std::thread             read_packet_thread;
    std::thread             decode_audio_thread;
    std::thread             decode_video_thread;

    bool                    mReadFileEOF    = false;
    bool                    mReadPacketThr  = false;
    bool                    mDecodeAudioThr = false;
    bool                    mDecodeVideoThr = false;


    // AVRecord                *avRecord;
};



// 1.Init
std::shared_ptr<MediaState> MediaStreamOpen(std::shared_ptr<MediaState> ms, MEDIA_TYPE type, std::string file, void *u_video_output);
// 2.destory
void MediaStreamClose(std::shared_ptr<MediaState> ms);


// // 1.Init
// MediaState *MediaStreamOpen(MEDIA_TYPE type, std::string file, void *u_video_output);
// // 2.destory
// void MediaStreamClose(MediaState *ms);
// // 3.thread
// int media_play_thread(MediaState *ms);
// int media_seek_thread(MediaState *ms);
// int media_thread(MediaState *ms);

#endif // MEDIACORE_HPP


// #ifndef MEDIACORE_H_
// #define MEDIACORE_H_

// #include "string"
// #include "SDL.h"
// #include "SDL_audio.h"
// #include "list"
// #include "thread"
// #include "map"
// #include "queue"
// #include "ffmpeg_common.h"
// // #include "MediaRecord.hpp"

// //  audio sample rates map from FFMPEG to SDL (only non planar)
// static std::map<int,int> AUDIO_FORMAT_MAP = {
//     // AV_SAMPLE_FMT_NONE = -1,
//     {AV_SAMPLE_FMT_U8,  AUDIO_U8    },
//     {AV_SAMPLE_FMT_S16, AUDIO_S16SYS},
//     {AV_SAMPLE_FMT_S32, AUDIO_S32SYS},
//     {AV_SAMPLE_FMT_FLT, AUDIO_F32SYS}
// };


// typedef struct PacketQueue {
//     std::list<AVPacket> pkt_list;
//     int nb_packets;
//     int size;
// } PacketQueue;


// enum MEDIA_TYPE{
//     MEDIA_TYPE_MEDIA_PLAY,
//     MEDIA_TYPE_RECORD_PLAY,
// };

// enum MODEL_TYPE{
//     MODEL_TYPE_NULL,
//     MODEL_TYPE_RECORD,
//     MODEL_TYPE_PHOTO,
// };

// typedef struct MediaState{
//     std::string filename; //文件名

//     int exit_request;//1:exit

//     //暂停状态：0:未暂停，1：暂停中
//     int paused; //1:暂停，:0播放
//     int last_paused;    //存储last paused status (“暂停”或“播放”状态)
//     int seek_req; //标识一次seek请求
//     int seek_time; //seek time === totalDuration[0,N]

//     // media type:media play,record play
//     MEDIA_TYPE media_type;

//     MODEL_TYPE model_type;

//     const AVInputFormat *iformat;   //指向demuxer
//     AVFormatContext *ic; //iformat的上下文

//     //总时长
//     int totalDuration;
//     std::string totalDurationStr;
//     char currPlayTime[30];


//     int video_stream_index;
//     AVStream *video_st;
//     PacketQueue videoq;//video队列

//     int audio_stream_index;
//     AVStream *audio_st;
//     PacketQueue audioq;//audio队列

//     std::thread read_thr;//read thread
//     std::thread write_thr;//write thread

//     //data queue
//     std::queue<AVPacket> dataQueue;


//     const AVCodec *codec_video = NULL;
//     const AVCodec *codec_audio = NULL;
//     //decode
//     AVCodecContext *decode_v_ctx = NULL;
//     AVCodecContext *decode_a_ctx = NULL;
//     AVFrame    *pFrame_video = NULL;
//     AVFrame    *pFrame_audio = NULL;
//     AVFrame    *pFrameRGB_video = NULL;
//     AVPacket *packet  = NULL;
//     unsigned char *out_buffer_video ;
//     unsigned char *out_buffer_audio ;
//     int out_buffer_audio_size;
//     struct SwsContext *img_convert_ctx = NULL;
//     struct SwrContext *audio_convert_ctx  = NULL;

//     unsigned int sdl_volume = SDL_MIX_MAXVOLUME;

//     void* uVideoOutput;

//     // AVRecord *avRecord;

// } MediaState;


// //open stream
// MediaState *MEDIA_stream_open(MEDIA_TYPE media_t,std::string filename, void *uVideoOutput);

// void MEDIA_stream_exit(MediaState *ms);

// #endif // MEDIA_H
