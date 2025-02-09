#include "MediaCore.hpp"
#include "MediaFile.hpp"
#include "MediaCamera.hpp"
#include "../qt/AVPlayerCore.hpp"
#include <sstream>

MediaFile mf;
// MediaFile *mf = new MediaFile();
static MediaCamera mc;


MediaState::MediaState(MEDIA_TYPE media_type_, std::string filename_, void *u_video_output) {

    if (filename_.empty()) {
        std::cout << "filename is empty..." << std::endl;
    }

    media_type = media_type_;
    model_type = MODEL_TYPE_NULL;
    video_stream_idx = -1;
    audio_stream_idx = -1;
    // audioLen = 0;           // 音频buffer size 数据剩余大小
    // audioChunk = nullptr;  // 音频buffer
    // audioPos = nullptr;    // 音频buffer
    if (!filename_.empty())
        filename = filename_;

    // ----
    MediaStreamStart(media_type_, filename_, u_video_output);
    std::cout << "@@@@@@@@@@@@@@@@@MediaState Init@@@@@@@@@@@@@@@@@" << std::endl;
}



MediaState::~MediaState() {
    // destory video
    if (sws_ctx)        sws_freeContext(sws_ctx);
    if (video_frame)    av_frame_free(&video_frame);
    if (rgb_frame)      av_frame_free(&rgb_frame);

    if (fmt_ctx)        avformat_close_input(&fmt_ctx);

    // destory audio
    if (audio_frame)    av_frame_free(&audio_frame);
    SDL_Quit();
    if (swr_ctx)        swr_free(&swr_ctx);
    swr_ctx = nullptr;
    // 清空音频数据缓冲区或者重置相关状态
    out_buffer_audio = nullptr;
    // audioChunk = nullptr;
    // audioPos = nullptr;
    // audioLen = 0;

    avcodec_flush_buffers(a_codec_ctx);

    video_stream_idx = -1;
    audio_stream_idx = -1;
    filename.clear();
    std::cout << "@@@@@@@@@@@@@@@@@MediaState Destroy@@@@@@@@@@@@@@@@@" << std::endl;
}

/* ---------------------------------------MediaState Set--------------------------------------- */
// 1.Init
void MediaState::MediaStreamStart(MEDIA_TYPE type, std::string file, void *u_video_output) {

    int ret = 0;
    std::lock_guard<std::mutex> lock(this->mtx); // 加锁

    // 播放
    if (this->media_type == MEDIA_TYPE_MEDIA_PLAY) {
        if (!this->qVideoOutput) {
            this->qVideoOutput = static_cast<QVideoOutput *>(u_video_output);
            std::cout << "Success to set ms->qVideoOutput" << std::endl;
        }
        // init MediaFile 准备播放
        ret = mf.InitMediaFile(this);
        if (ret < 0) {
            std::cout << "初始化MediaFile失败..." << std::endl;
            return ;
        } else if (ret == 0) {
            std::cout << "InitMediaFile成功..." << std::endl;
        }
    }
    // 录制
    else if (this->media_type == MEDIA_TYPE_RECORD_PLAY) {
        if (!this->qVideoOutput) {
            // ms->uVideoOutput = static_cast<AVRecordPainted *>(u_video_output);
            std::cout << "录制功能并未完善..." << std::endl;
        }
        // init MediaCamera 准备录制
        // ret = mc.InitMediaCamera(ms);
    }
    std::cout << "------------Open MediaState Successful------------" << std::endl;

    if (this->qVideoOutput == nullptr) {
        std::cout << "------------this->qVideoOutput是空的------------" << std::endl;

    }
}

// 2.destory
// 智能指针版本
void MediaStreamClose(std::shared_ptr<MediaState> ms) {
    // 引用计数-1
    // while (ms) {
    // ms.reset();  // =0 调用析构
    // }
    // 返回引用计数 ： 用于调试，影响速度
    //int ref_count = ms.use_count();
    // if (ref_count > 0) {
    //     std::cout << "Close MediaState -> ref = " << ref_count << std::endl;
    // } else {
    if (!ms)
        std::cout << "------------Destory MediaState Successful------------" << std::endl;
    // }
}

























// #include "MediaCore.hpp"
// #include <thread>         // std::thread
// #include <mutex>          // std::mutex
// #include "iostream"
// #include "unistd.h"
// #include "map"
// #include "QImage"

// #include "qt/AVPlayerCore.hpp"
// // #include "qt/AVRecordPainted.hpp"
// #include "MediaCamera.hpp"
// #include "MediaFile.hpp"


// static MediaCamera mc;
// static MediaFile mf;

// void MEDIA_stream_exit(MediaState *ms)
// {
//     if(ms->img_convert_ctx != NULL)
//     {
//         sws_freeContext(ms->img_convert_ctx);
//     }

//     if(ms->pFrameRGB_video != NULL)
//     {
//         av_frame_free(&ms->pFrameRGB_video);
//     }

//     if(ms->pFrame_video != NULL)
//     {
//         av_frame_free(&ms->pFrame_video);
//     }

//     if(ms->pFrame_audio != NULL)
//     {
//         av_frame_free(&ms->pFrame_audio);
//     }

//     if(ms->ic)
//     {
//         avformat_close_input(&ms->ic);
//     }

//     // clear audio
//     SDL_Quit();
//     if(ms->audio_convert_ctx)
//     {
//         swr_free(&ms->audio_convert_ctx);
//     }
//     ms->video_stream_index = -1;
//     ms->audio_stream_index = -1;
//     ms->filename.clear();
//     std::cout << "MEDIA_stream_exit success !" << std::endl;
// }


// MediaState *MEDIA_stream_open(MEDIA_TYPE media_t,std::string filename,void *uVideoOutput)
// {
//     MediaState *ms;
//     ms = (MediaState*)av_mallocz(sizeof (MediaState));
//     if(!ms)
//     {
//         return NULL;
//     }

//     ms->media_type = media_t;
//     ms->model_type = MODEL_TYPE_NULL;

//     ms->video_stream_index = -1;
//     ms->audio_stream_index = -1;
//     ms->filename = filename;

//     if (ms->filename.empty())
//     {
//         return NULL;
//     }


//     if(ms->media_type == MEDIA_TYPE_MEDIA_PLAY)
//     {
//         if(ms->uVideoOutput == NULL)
//         {
//             ms->uVideoOutput = (QVideoOutput*)(uVideoOutput);
//         }
//         mf.InitMediaFile((void*)ms);
//     }
//     else if(ms->media_type == MEDIA_TYPE_RECORD_PLAY)
//     {
//         if(ms->uVideoOutput == NULL)
//         {
//             // ms->uVideoOutput = (AVRecordPainted*)(uVideoOutput);
//         }

//         // mc.InitCamera((void*)ms);
//     }
//     return ms;
// }










