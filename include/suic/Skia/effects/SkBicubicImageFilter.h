/*
 * Copyright 2013 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkBicubicImageFilter_DEFINED
#define SkBicubicImageFilter_DEFINED

#include "SkImageFilter.h"
#include "SkScalar.h"
#include "SkSize.h"
#include "SkPoint.h"

/*! \class SkBicubicImageFilter
    Bicubic resampling image filter.  This filter does a 16-tap bicubic
    filter using the given matrix.
 */

class SK_API SkBicubicImageFilter : public SkImageFilter {
public:
    virtual ~SkBicubicImageFilter();

    static SkBicubicImageFilter* CreateMitchell(const SkSize& scale, SkImageFilter* input = NULL);

    SK_DECLARE_PUBLIC_FLATTENABLE_DESERIALIZATION_PROCS(SkBicubicImageFilter)

protected:
    /** Construct a (scaling-only) bicubic resampling image filter.
        @param scale        How much to scale the image.
        @param coefficients The 16 coefficients of the bicubic matrix.
        @param input        The input image filter.  If NULL, the src bitmap
                            passed to filterImage() is used instead.
    */
    SkBicubicImageFilter(const SkSize& scale, const SkScalar coefficients[16],
                         SkImageFilter* input = NULL);
    SkBicubicImageFilter(SkReadBuffer& buffer);
    virtual void flatten(SkWriteBuffer&) const SK_OVERRIDE;

    virtual bool onFilterImage(Proxy*, const SkBitmap& src, const Context&,
                               SkBitmap* result, SkIPoint* loc) const SK_OVERRIDE;

#if SK_SUPPORT_GPU
    virtual bool canFilterImageGPU() const SK_OVERRIDE { return true; }
    virtual bool filterImageGPU(Proxy* proxy, const SkBitmap& src, const Context& ctx,
                                SkBitmap* result, SkIPoint* offset) const SK_OVERRIDE;
#endif

private:
    SkSize    fScale;
    SkScalar  fCoefficients[16];
    typedef SkImageFilter INHERITED;
};

#endif
