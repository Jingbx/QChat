#ifndef MEDIACAMERA_H
#define MEDIACAMERA_H
#include "MediaCore.hpp"

class MediaCamera
{
public:
    MediaCamera();

    int InitMediaCamera(std::shared_ptr<MediaState> ms) {}
    // int InitMediaCamera(MediaState *ms) {}
};

#endif // MEDIACAMERA_H
