/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkRTreePicture_DEFINED
#define SkRTreePicture_DEFINED

#ifdef SK_SUPPORT_LEGACY_PICTURE_HEADERS
#include "SkBBHFactory.h"
#endif

#ifdef SK_SUPPORT_LEGACY_DERIVED_PICTURE_CLASSES

#include "SkPicture.h"

/**
 * Subclass of SkPicture that creates an RTree acceleration structure.
 */
class SkRTreePicture : public SkPicture {
public:
    virtual SkBBoxHierarchy* createBBoxHierarchy() const SK_OVERRIDE;

private:
    typedef SkPicture INHERITED;
};

class SkRTreePictureFactory : public SkPictureFactory {
public:
    SkRTreePictureFactory() {}

    virtual SkPicture* create(int width, int height) SK_OVERRIDE {
        return SkNEW(SkRTreePicture);
    }

private:
    typedef SkPictureFactory INHERITED;
};
#endif

#endif
