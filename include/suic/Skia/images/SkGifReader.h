
/*
 * Copyright 2008 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkGIFMovie_H
#define SkGIFMovie_H

#include "SkCanvas.h"
#include "SkStream.h"

class SkStreamRewindable;

class SkGifReader {
public:

    SkGifReader();
    ~SkGifReader();

    int ReadFile(const char path[]);
    int ReadMemory(const void* data, size_t length);
    int ReadStream(SkStream* stream);

    int duration();
    int width();
    int height();
    int isOpaque();

    int GetDelay();
    int GetImage(SkBitmap* imgage);

    void Reset();
    int MoveNext();

private:

    int onGetBitmap(SkBitmap* bm);

private:

    int fCurrIndex;
    int fLastDrawIndex;
    SkBitmap fBackup;
    void* fGIF;
};

#endif
