#ifndef FFMPEG_COMMON_H
#define FFMPEG_COMMON_H

extern "C" {
#include "libavdevice/avdevice.h"

#include "libavcodec/avcodec.h"

#include "libavformat/avformat.h"

#include "libavutil/avutil.h"
#include "libavutil/time.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"

#include "libavfilter/avfilter.h"

#include "libswscale/swscale.h"

#include "libswresample/swresample.h"
}

#include "SDL.h"
#include "SDL_audio.h"

#include <iostream>
#include <thread>


#endif  // FFMPEG_COMMON_H
