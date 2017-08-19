// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����StreamDef.h
// ��  �ܣ�������Ŀ�Stream�ӿں�Ĭ��ʵ�֡�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UISTREAMDEF_H_
#define _UISTREAMDEF_H_

#include <System/Windows/Object.h>

namespace suic
{

class SUICORE_API ISStream
{
public:

    virtual ~ISStream() {}

    virtual void Clear() = 0;
    virtual int Seek(int pos) = 0;
    virtual int GetSize() const = 0;
    virtual int Read(Byte* data, int bytes) = 0;
    virtual int Write(Byte* data, int bytes) = 0;
};

class SUICORE_API StreamOp
{
public:

    static void WriteToStream(ISStream* dest, ISStream* src);
};

class SUICORE_API ByteStream : public ISStream
{
public:

    ByteStream();
    virtual ~ByteStream();

    int Read(Byte* data, int bytes);
    int Write(Byte* data, int bytes);

    int GetSize() const;
    Byte* GetBuffer() const;

    int Seek(int pos);
    void Clear();

    void Resize(int size);

private:

    Byte* AllocSize(int size);

private:

    int _currPos;
    int _size;
    int _capacity;
    Byte* _bytes;
};

class SUICORE_API FileStream : public ISStream
{
public:

    FileStream();
    virtual ~FileStream();

    bool Open(const String& filename, bool bNoExistCreate);

    int Read(Byte* data, int bytes);
    int Write(Byte* data, int bytes);
    int Seek(int pos);

    int GetSize() const;
    void Clear();
    void Close();

private:

    FILE* _file;
    Mulstr _path;
};

}

#endif
