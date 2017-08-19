
/*
 * Copyright 2007 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkPicture_DEFINED
#define SkPicture_DEFINED

#include "SkBitmap.h"
#include "SkImageDecoder.h"
#include "SkRefCnt.h"

#if SK_SUPPORT_GPU
class GrContext;
#endif

class SkBBHFactory;
class SkBBoxHierarchy;
class SkCanvas;
class SkDrawPictureCallback;
class SkData;
class SkPathHeap;
class SkPicturePlayback;
class SkPictureRecord;
class SkStream;
class SkWStream;

struct SkPictInfo;

/** \class SkPicture

    The SkPicture class records the drawing commands made to a canvas, to
    be played back at a later time.
*/
class SK_API SkPicture : public SkRefCnt {
public:
    SK_DECLARE_INST_COUNT(SkPicture)

    // AccelData provides a base class for device-specific acceleration
    // data. It is added to the picture via a call to a device's optimize
    // method.
    class AccelData : public SkRefCnt {
    public:
        typedef uint8_t Domain;
        typedef uint32_t Key;

        AccelData(Key key) : fKey(key) { }

        const Key& getKey() const { return fKey; }

        // This entry point allows user's to get a unique domain prefix
        // for their keys
        static Domain GenerateDomain();
    private:
        Key fKey;

        typedef SkRefCnt INHERITED;
    };

    SkPicture();
    /** Make a copy of the contents of src. If src records more drawing after
        this call, those elements will not appear in this picture.
    */
    SkPicture(const SkPicture& src);

    /**  PRIVATE / EXPERIMENTAL -- do not call */
    void EXPERIMENTAL_addAccelData(const AccelData* data) {
        SkRefCnt_SafeAssign(fAccelData, data);
    }
    /**  PRIVATE / EXPERIMENTAL -- do not call */
    const AccelData* EXPERIMENTAL_getAccelData(AccelData::Key key) const {
        if (NULL != fAccelData && fAccelData->getKey() == key) {
            return fAccelData;
        }
        return NULL;
    }

    /**
     *  Function signature defining a function that sets up an SkBitmap from encoded data. On
     *  success, the SkBitmap should have its Config, width, height, rowBytes and pixelref set.
     *  If the installed pixelref has decoded the data into pixels, then the src buffer need not be
     *  copied. If the pixelref defers the actual decode until its lockPixels() is called, then it
     *  must make a copy of the src buffer.
     *  @param src Encoded data.
     *  @param length Size of the encoded data, in bytes.
     *  @param dst SkBitmap to install the pixel ref on.
     *  @param bool Whether or not a pixel ref was successfully installed.
     */
    typedef bool (*InstallPixelRefProc)(const void* src, size_t length, SkBitmap* dst);

    /**
     *  Recreate a picture that was serialized into a stream.
     *  @param SkStream Serialized picture data.
     *  @param proc Function pointer for installing pixelrefs on SkBitmaps representing the
     *              encoded bitmap data from the stream.
     *  @return A new SkPicture representing the serialized data, or NULL if the stream is
     *          invalid.
     */
    static SkPicture* CreateFromStream(SkStream*,
                                       InstallPixelRefProc proc = &SkImageDecoder::DecodeMemory);

    /**
     *  Recreate a picture that was serialized into a buffer. If the creation requires bitmap
     *  decoding, the decoder must be set on the SkReadBuffer parameter by calling
     *  SkReadBuffer::setBitmapDecoder() before calling SkPicture::CreateFromBuffer().
     *  @param SkReadBuffer Serialized picture data.
     *  @return A new SkPicture representing the serialized data, or NULL if the buffer is
     *          invalid.
     */
    static SkPicture* CreateFromBuffer(SkReadBuffer&);

    virtual ~SkPicture();

    /**
     *  Swap the contents of the two pictures. Guaranteed to succeed.
     */
    void swap(SkPicture& other);

    /**
     *  Creates a thread-safe clone of the picture that is ready for playback.
     */
    SkPicture* clone() const;

    /**
     * Creates multiple thread-safe clones of this picture that are ready for
     * playback. The resulting clones are stored in the provided array of
     * SkPictures.
     */
    void clone(SkPicture* pictures, int count) const;

    enum RecordingFlags {
        /*  This flag specifies that when clipPath() is called, the path will
            be faithfully recorded, but the recording canvas' current clip will
            only see the path's bounds. This speeds up the recording process
            without compromising the fidelity of the playback. The only side-
            effect for recording is that calling getTotalClip() or related
            clip-query calls will reflect the path's bounds, not the actual
            path.
         */
        kUsePathBoundsForClip_RecordingFlag = 0x01
    };

#ifndef SK_SUPPORT_DEPRECATED_RECORD_FLAGS
    // TODO: once kOptimizeForClippedPlayback_RecordingFlag is hidden from
    // all external consumers, SkPicture::createBBoxHierarchy can also be
    // cleaned up.
private:
#endif
    enum Deprecated_RecordingFlags {
        /*  This flag causes the picture to compute bounding boxes and build
            up a spatial hierarchy (currently an R-Tree), plus a tree of Canvas'
            usually stack-based clip/etc state. This requires an increase in
            recording time (often ~2x; likely more for very complex pictures),
            but allows us to perform much faster culling at playback time, and
            completely avoid some unnecessary clips and other operations. This
            is ideal for tiled rendering, or any other situation where you're
            drawing a fraction of a large scene into a smaller viewport.

            In most cases the record cost is offset by the playback improvement
            after a frame or two of tiled rendering (and complex pictures that
            induce the worst record times will generally get the largest
            speedups at playback time).

            Note: Currently this is not serializable, the bounding data will be
            discarded if you serialize into a stream and then deserialize.
        */
        kOptimizeForClippedPlayback_RecordingFlag = 0x02,
    };
#ifndef SK_SUPPORT_DEPRECATED_RECORD_FLAGS
public:
#endif

#ifndef SK_SUPPORT_LEGACY_PICTURE_CAN_RECORD
private:
#endif

#ifdef SK_SUPPORT_LEGACY_DERIVED_PICTURE_CLASSES

    /** Returns the canvas that records the drawing commands.
        @param width the base width for the picture, as if the recording
                     canvas' bitmap had this width.
        @param height the base width for the picture, as if the recording
                     canvas' bitmap had this height.
        @param recordFlags optional flags that control recording.
        @return the picture canvas.
    */
    SkCanvas* beginRecording(int width, int height, uint32_t recordFlags = 0);
#endif

    /** Returns the recording canvas if one is active, or NULL if recording is
        not active. This does not alter the refcnt on the canvas (if present).
    */
    SkCanvas* getRecordingCanvas() const;
    /** Signal that the caller is done recording. This invalidates the canvas
        returned by beginRecording/getRecordingCanvas, and prepares the picture
        for drawing. Note: this happens implicitly the first time the picture
        is drawn.
    */
    void endRecording();

#ifndef SK_SUPPORT_LEGACY_PICTURE_CAN_RECORD
public:
#endif

    /** Replays the drawing commands on the specified canvas. This internally
        calls endRecording() if that has not already been called.
        @param canvas the canvas receiving the drawing commands.
    */
    void draw(SkCanvas* canvas, SkDrawPictureCallback* = NULL);

    /** Return the width of the picture's recording canvas. This
        value reflects what was passed to setSize(), and does not necessarily
        reflect the bounds of what has been recorded into the picture.
        @return the width of the picture's recording canvas
    */
    int width() const { return fWidth; }

    /** Return the height of the picture's recording canvas. This
        value reflects what was passed to setSize(), and does not necessarily
        reflect the bounds of what has been recorded into the picture.
        @return the height of the picture's recording canvas
    */
    int height() const { return fHeight; }

    /** Return a non-zero, unique value representing the picture. This call is
        only valid when not recording. Between a beginRecording/endRecording
        pair it will just return 0 (the invalid ID). Each beginRecording/
        endRecording pair will cause a different generation ID to be returned.
    */
    uint32_t uniqueID() const;

    /**
     *  Function to encode an SkBitmap to an SkData. A function with this
     *  signature can be passed to serialize() and SkWriteBuffer.
     *  Returning NULL will tell the SkWriteBuffer to use
     *  SkBitmap::flatten() to store the bitmap.
     *
     *  @param pixelRefOffset DEPRECATED -- caller assumes it will return 0.
     *  @return SkData If non-NULL, holds encoded data representing the passed
     *      in bitmap. The caller is responsible for calling unref().
     */
    typedef SkData* (*EncodeBitmap)(size_t* pixelRefOffset, const SkBitmap& bm);

    /**
     *  Serialize to a stream. If non NULL, encoder will be used to encode
     *  any bitmaps in the picture.
     *  encoder will never be called with a NULL pixelRefOffset.
     */
    void serialize(SkWStream*, EncodeBitmap encoder = NULL) const;

    /**
     *  Serialize to a buffer.
     */
    void flatten(SkWriteBuffer&) const;

    /**
     * Returns true if any bitmaps may be produced when this SkPicture
     * is replayed.
     * Returns false if called while still recording.
     */
    bool willPlayBackBitmaps() const;

#ifdef SK_BUILD_FOR_ANDROID
    /** Signals that the caller is prematurely done replaying the drawing
        commands. This can be called from a canvas virtual while the picture
        is drawing. Has no effect if the picture is not drawing.
        @deprecated preserving for legacy purposes
    */
    void abortPlayback();
#endif

    /** Return true if the SkStream/Buffer represents a serialized picture, and
        fills out SkPictInfo. After this function returns, the data source is not
        rewound so it will have to be manually reset before passing to
        CreateFromStream or CreateFromBuffer. Note, CreateFromStream and
        CreateFromBuffer perform this check internally so these entry points are
        intended for stand alone tools.
        If false is returned, SkPictInfo is unmodified.
    */
    static bool InternalOnly_StreamIsSKP(SkStream*, SkPictInfo*);
    static bool InternalOnly_BufferIsSKP(SkReadBuffer&, SkPictInfo*);

    /** Enable/disable all the picture recording optimizations (i.e.,
        those in SkPictureRecord). It is mainly intended for testing the
        existing optimizations (i.e., to actually have the pattern
        appear in an .skp we have to disable the optimization). Call right
        after 'beginRecording'.
    */
    void internalOnly_EnableOpts(bool enableOpts);

    /** Return true if the picture is suitable for rendering on the GPU.
     */

#if SK_SUPPORT_GPU
    bool suitableForGpuRasterization(GrContext*) const;
#endif

protected:
    // V2 : adds SkPixelRef's generation ID.
    // V3 : PictInfo tag at beginning, and EOF tag at the end
    // V4 : move SkPictInfo to be the header
    // V5 : don't read/write FunctionPtr on cross-process (we can detect that)
    // V6 : added serialization of SkPath's bounds (and packed its flags tighter)
    // V7 : changed drawBitmapRect(IRect) to drawBitmapRectToRect(Rect)
    // V8 : Add an option for encoding bitmaps
    // V9 : Allow the reader and writer of an SKP disagree on whether to support
    //      SK_SUPPORT_HINTING_SCALE_FACTOR
    // V10: add drawRRect, drawOval, clipRRect
    // V11: modify how readBitmap and writeBitmap store their info.
    // V12: add conics to SkPath, use new SkPathRef flattening
    // V13: add flag to drawBitmapRectToRect
    //      parameterize blurs by sigma rather than radius
    // V14: Add flags word to PathRef serialization
    // V15: Remove A1 bitmap config (and renumber remaining configs)
    // V16: Move SkPath's isOval flag to SkPathRef
    // V17: SkPixelRef now writes SkImageInfo
    // V18: SkBitmap now records x,y for its pixelref origin, instead of offset.
    // V19: encode matrices and regions into the ops stream
    // V20: added bool to SkPictureImageFilter's serialization (to allow SkPicture serialization)
    // V21: add pushCull, popCull
    // V22: SK_PICT_FACTORY_TAG's size is now the chunk size in bytes
    // V23: SkPaint::FilterLevel became a real enum
    // V24: SkTwoPointConicalGradient now has fFlipped flag for gradient flipping
    // V25: SkDashPathEffect now only writes phase and interval array when flattening

    // Note: If the picture version needs to be increased then please follow the
    // steps to generate new SKPs in (only accessible to Googlers): http://goo.gl/qATVcw

    // Only SKPs within the min/current picture version range (inclusive) can be read.
    static const uint32_t MIN_PICTURE_VERSION = 19;
    static const uint32_t CURRENT_PICTURE_VERSION = 25;

    mutable uint32_t      fUniqueID;

    // fPlayback, fRecord, fWidth & fHeight are protected to allow derived classes to
    // install their own SkPicturePlayback-derived players,SkPictureRecord-derived
    // recorders and set the picture size
    SkPicturePlayback*    fPlayback;
    SkPictureRecord*      fRecord;
    int                   fWidth, fHeight;
    const AccelData*      fAccelData;

    void needsNewGenID() { fUniqueID = SK_InvalidGenID; }

    // Create a new SkPicture from an existing SkPicturePlayback. Ref count of
    // playback is unchanged.
    SkPicture(SkPicturePlayback*, int width, int height);

#ifdef SK_SUPPORT_LEGACY_DERIVED_PICTURE_CLASSES
    // For testing. Derived classes may instantiate an alternate
    // SkBBoxHierarchy implementation
    virtual SkBBoxHierarchy* createBBoxHierarchy() const;
#endif

    SkCanvas* beginRecording(int width, int height, SkBBHFactory* factory, uint32_t recordFlags);

private:
    friend class SkPictureRecord;
    friend class SkPictureTester;   // for unit testing

    SkAutoTUnref<SkPathHeap> fPathHeap;  // reference counted

    const SkPath& getPath(int index) const;
    int addPathToHeap(const SkPath& path);

    void flattenToBuffer(SkWriteBuffer& buffer) const;
    bool parseBufferTag(SkReadBuffer& buffer, uint32_t tag, uint32_t size);

    static void WriteTagSize(SkWriteBuffer& buffer, uint32_t tag, size_t size);
    static void WriteTagSize(SkWStream* stream, uint32_t tag, size_t size);

    void initForPlayback() const;
    void dumpSize() const;

    // An OperationList encapsulates a set of operation offsets into the picture byte
    // stream along with the CTMs needed for those operation.
    class OperationList : ::SkNoncopyable {
    public:
        virtual ~OperationList() {}

        // If valid returns false then there is no optimization data
        // present. All the draw operations need to be issued.
        virtual bool valid() const { return false; }

        // The following three entry points should only be accessed if
        // 'valid' returns true.
        virtual int numOps() const { SkASSERT(false); return 0; };
        // The offset in the picture of the operation to execute.
        virtual uint32_t offset(int index) const { SkASSERT(false); return 0; };
        // The CTM that must be installed for the operation to behave correctly
        virtual const SkMatrix& matrix(int index) const { SkASSERT(false); return SkMatrix::I(); }

        static const OperationList& InvalidList();
    };

    /** PRIVATE / EXPERIMENTAL -- do not call
        Return the operations required to render the content inside 'queryRect'.
    */
    const OperationList& EXPERIMENTAL_getActiveOps(const SkIRect& queryRect);

    /** PRIVATE / EXPERIMENTAL -- do not call
        Return the ID of the operation currently being executed when playing
        back. 0 indicates no call is active.
    */
    size_t EXPERIMENTAL_curOpID() const;

    void createHeader(SkPictInfo* info) const;
    static bool IsValidPictInfo(const SkPictInfo& info);

    friend class SkFlatPicture;
    friend class SkPicturePlayback;
    friend class SkPictureRecorder;
    friend class SkGpuDevice;
    friend class GrGatherDevice;
    friend class SkDebugCanvas;

    typedef SkRefCnt INHERITED;
};

/**
 *  Subclasses of this can be passed to canvas.drawPicture. During the drawing
 *  of the picture, this callback will periodically be invoked. If its
 *  abortDrawing() returns true, then picture playback will be interrupted.
 *
 *  The resulting drawing is undefined, as there is no guarantee how often the
 *  callback will be invoked. If the abort happens inside some level of nested
 *  calls to save(), restore will automatically be called to return the state
 *  to the same level it was before the drawPicture call was made.
 */
class SK_API SkDrawPictureCallback {
public:
    SkDrawPictureCallback() {}
    virtual ~SkDrawPictureCallback() {}

    virtual bool abortDrawing() = 0;
};

#ifdef SK_SUPPORT_LEGACY_DERIVED_PICTURE_CLASSES

class SkPictureFactory : public SkRefCnt {
public:
    /**
     *  Allocate a new SkPicture. Return NULL on failure.
     */
    virtual SkPicture* create(int width, int height) = 0;

private:
    typedef SkRefCnt INHERITED;
};

#endif

#ifdef SK_SUPPORT_LEGACY_PICTURE_HEADERS
#include "SkPictureRecorder.h"
#endif

#endif
