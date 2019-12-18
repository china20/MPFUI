
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkImageDecoder.h"
#include "SkMovie.h"
#include "SkStream.h"

extern void* SkiaIcoStaticInit();
extern void* SkiaBmpStaticInit();
extern void* SkiaJpegStaticInit();
extern void* SkiaPngStaticInit();
extern void* SkiaGifStaticInit();

extern SkImageDecoder* image_decoder_from_stream(SkStreamRewindable*);

SkImageDecoder* SkImageDecoder::Factory(SkStreamRewindable* stream) {
    SkImageDecoder* imgDecoder = image_decoder_from_stream(stream);
    if (NULL == imgDecoder)
    {
        static bool _skiaInit = false;
        if (!_skiaInit)
        {
            _skiaInit = true;

            SkiaPngStaticInit();
            SkiaBmpStaticInit();
            SkiaJpegStaticInit();
            SkiaIcoStaticInit();
            SkiaGifStaticInit();

            //imgDecoder = image_decoder_from_stream(stream);
        }
    }
    return imgDecoder;
}

/////////////////////////////////////////////////////////////////////////

typedef SkTRegistry<SkMovie*(*)(SkStreamRewindable*)> MovieReg;

SkMovie* SkMovie::DecodeStream(SkStreamRewindable* stream) {
    const MovieReg* curr = MovieReg::Head();
    while (curr) {
        SkMovie* movie = curr->factory()(stream);
        if (movie) {
            return movie;
        }
        // we must rewind only if we got NULL, since we gave the stream to the
        // movie, who may have already started reading from it
        stream->rewind();
        curr = curr->next();
    }
    return NULL;
}
