// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// StreamDef.cpp

#include <System/Types/StreamDef.h>
#include <System/Tools/Memory.h>

#include <io.h>
#include <stdio.h>

namespace suic
{

ByteStream::ByteStream()
{
    _currPos = 0;
    _size = 0;
    _capacity = 16;
    _bytes = (Byte*)AllocMem(16);
}

ByteStream::~ByteStream()
{
    if (_bytes)
    {
        FreeMem(_bytes);
    }
}

Byte* ByteStream::AllocSize(int size)
{
    if (size >= _size)
    {
        // 容量不能小于2
        if (size < 2)
        {
            size = 2;
        }

        // 这里直接拷贝，不调用虚构和构造函数
        return (Byte*)AllocMem(size);
    }
    else
    {
        return 0;
    }
}

void ByteStream::Resize(int size)
{
    if (size > _capacity)
    {
        Byte* tmp(AllocSize(size));
        if (tmp)
        {
            MemCopy(tmp, _bytes, _size);
            FreeMem(_bytes);
            _capacity = size;
            _bytes = tmp;
        }
    }
}

void StreamOp::WriteToStream(ISStream* dest, ISStream* src)
{
    const int BUFFERLEN = 1024;
    suic::Byte buff[BUFFERLEN];
    int readBytes = src->Read(buff, BUFFERLEN);
    while (readBytes > 0)
    {
        dest->Write(buff, readBytes);
        readBytes = src->Read(buff, BUFFERLEN);
    }
}

int ByteStream::Read(Byte* data, int bytes)
{
    bytes = min(_size - _currPos, bytes);

    if (NULL == _bytes || bytes <= 0)
    {
        return 0;
    }
    else
    {
        MemCopy(data, _bytes + _currPos, bytes);
        _currPos += bytes;

        return bytes;
    }
}

int ByteStream::Write(Byte* data, int bytes)
{
    if (_capacity <= (bytes + _size))
    {
        Resize((bytes + _size) * 2.5);
    }

    MemCopy(_bytes + _size, data, bytes);
    _size += bytes;

    return bytes;
}

int ByteStream::GetSize() const
{
    return _size;
}

Byte* ByteStream::GetBuffer() const
{
    return _bytes;
}

int ByteStream::Seek(int pos)
{
    int iCurr = _currPos;
    _currPos = pos;
    return iCurr;
}

void ByteStream::Clear()
{
    if (_bytes)
    {
        FreeMem(_bytes);
        _bytes = NULL;
    }
    _currPos = 0;
}

void ByteStream::Detach(ByteStream* Other)
{
    Other->Clear();
    Other->_bytes = _bytes;
    Other->_size = _size;
    Other->_capacity = _capacity;
    Other->_currPos = 0;

    _size = 0;
    _currPos = 0;
    _capacity = 0;
    _bytes = NULL;
}

FileStream::FileStream()
    : _file(NULL)
{

}

FileStream::~FileStream()
{
    Close();
}

bool FileStream::Open(const String& filename, bool bNoExistCreate)
{
    if (_file == NULL)
    {
        _path = filename.c_str();
        if (bNoExistCreate)
        {
            _file = ::fopen(_path.c_str(), "ab+");
        }
        else
        {
            _file = ::fopen(_path.c_str(), "rb");
        }
    }

    return (_file != NULL);
}

int FileStream::Read(Byte* buffer, int byteCount)
{
    return (int)::fread(buffer, 1, byteCount, _file);
}

int FileStream::Write(Byte* buffer, int byteCount)
{
    return (int)::fwrite(buffer, 1, byteCount, _file);
}

int FileStream::Seek(int pos)
{
    int iCurr = 0;
    if (NULL != _file)
    {
        iCurr = ::fseek(_file, (long)pos, SEEK_SET);
    }
    return iCurr;
}

int FileStream::GetSize() const
{
    int iSize = 0;
    if (NULL != _file)
    {
        long iCurr = ::fseek(_file, 0, SEEK_END);
        iSize = (int)::ftell(_file);
        ::fseek(_file, iCurr, SEEK_SET);
    }
    return iSize;
}

void FileStream::Clear()
{
    if (_file != NULL)
    {
        Close();
        _file = ::fopen(_path.c_str(), "w");
        ::fclose(_file);
        _file = ::fopen(_path.c_str(), "ab+");
    }
}

void FileStream::Close()
{
    if (NULL != _file)
    {
        ::fclose(_file);
        _file = NULL;
    }
}

}
