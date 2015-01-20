/**
 *  \file
 *  \brief  Various helper types and functions
 *
 *  \author  Pavel Nedev <pg.nedev@gmail.com>
 *
 *  \section COPYRIGHT
 *  Copyright(C) 2014 Pavel Nedev
 *
 *  \section LICENSE
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <string.h>


/**
 *  \class  CPath
 *  \brief
 */
class CPath
{
public:
    enum
    {
        MAX_LEN = MAX_PATH
    };

    CPath(const TCHAR* pathStr = NULL)
    {
        _str[0] = 0;
        if (pathStr)
            _tcscpy_s(_str, MAX_LEN, pathStr);
    }

    CPath(const char* pathStr)
    {
        _str[0] = 0;
        if (pathStr)
        {
            size_t cnt;
            mbstowcs_s(&cnt, _str, MAX_LEN, pathStr, _TRUNCATE);
        }
    }

    CPath(const CPath& path)
    {
        _tcscpy_s(_str, MAX_LEN, path._str);
    }

    ~CPath() {}

    inline const CPath& operator=(const TCHAR* pathStr)
    {
        _tcscpy_s(_str, MAX_LEN, pathStr);
        return *this;
    }

    inline const CPath& operator=(const CPath& path)
    {
        if (this != &path)
        {
            _tcscpy_s(_str, MAX_LEN, path._str);
        }
        return *this;
    }

    inline bool operator==(const TCHAR* pathStr) const
    {
        return !_tcscmp(_str, pathStr);
    }

    inline bool operator==(const CPath& path) const
    {
        return !_tcscmp(_str, path._str);
    }

    inline const CPath& operator+=(const TCHAR* str)
    {
        _tcscat_s(_str, MAX_LEN, str);
        return *this;
    }

    inline const CPath& operator+=(const CPath& path)
    {
        _tcscat_s(_str, MAX_LEN, path._str);
        return *this;
    }

    inline TCHAR* C_str() { return _str; }
    inline const TCHAR* C_str() const { return _str; }
    inline unsigned Len() const { return _tcslen(_str); }

    inline bool Exists() const
    {
        DWORD dwAttrib = GetFileAttributes(_str);
        return (bool)(dwAttrib != INVALID_FILE_ATTRIBUTES);
    }

    inline bool FileExists() const
    {
        DWORD dwAttrib = GetFileAttributes(_str);
        return (bool)(dwAttrib != INVALID_FILE_ATTRIBUTES &&
            !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    unsigned StripFilename();
    const TCHAR* GetFilename_C_str() const;
    unsigned Up();
    bool Contains(const TCHAR* pathStr) const;
    bool Contains(const CPath& path) const;
    bool IsContainedIn(const TCHAR* pathStr) const;
    bool IsContainedIn(const CPath& path) const;

private:
    TCHAR _str[MAX_LEN];
};


/**
 *  \class  CText
 *  \brief
 */
class CText
{
private:
    enum
    {
        ALLOC_CHUNK_SIZE = 2048
    };

    void resize(unsigned newLen);
    unsigned expand(unsigned newLen);

    unsigned _size;
    unsigned _len;
    TCHAR *_str;
    TCHAR _buf[ALLOC_CHUNK_SIZE];

public:
    CText() : _size(ALLOC_CHUNK_SIZE), _len(0), _str(_buf) { _buf[0] = 0; }
    CText(unsigned size);
    CText(const TCHAR* str);
    CText(const char* str);
    CText(const CText& txt);
    ~CText()
    {
        if (_str != _buf)
            delete [] _str;
    }

    const CText& operator=(const TCHAR* str);
    const CText& operator=(const char* str);
    const CText& operator=(const CText& txt);

    inline bool operator==(const TCHAR* str) const
    {
        return !_tcscmp(_str, str);
    }

    inline bool operator==(const CText& txt) const
    {
        return !_tcscmp(_str, txt._str);
    }

    const CText& operator+=(const TCHAR* str);
    const CText& operator+=(const char* str);
    const CText& operator+=(const CText& txt);
    const CText& append(const TCHAR* str, unsigned len);

    inline TCHAR* C_str() { return _str; }
    inline const TCHAR* C_str() const { return _str; }
    inline unsigned Size() const { return _size; }
    inline unsigned Len() const { return _len; }
};


/**
 *  \class  CTextA
 *  \brief
 */
class CTextA
{
private:
    enum
    {
        ALLOC_CHUNK_SIZE = 2048
    };

    void resize(unsigned newLen);
    unsigned expand(unsigned newLen);

    unsigned _size;
    unsigned _len;
    char *_str;
    char _buf[ALLOC_CHUNK_SIZE];

public:
    CTextA() : _size(ALLOC_CHUNK_SIZE), _len(0), _str(_buf) { _buf[0] = 0; }
    CTextA(unsigned size);
    CTextA(const char* str);
    CTextA(const TCHAR* str);
    CTextA(const CTextA& txt);
    ~CTextA()
    {
        if (_str != _buf)
            delete [] _str;
    }

    const CTextA& operator=(const char* str);
    const CTextA& operator=(const TCHAR* str);
    const CTextA& operator=(const CTextA& txt);

    inline bool operator==(const char* str) const
    {
        return !strcmp(_str, str);
    }

    inline bool operator==(const CTextA& txt) const
    {
        return !strcmp(_str, txt._str);
    }

    const CTextA& operator+=(const char* str);
    const CTextA& operator+=(const TCHAR* str);
    const CTextA& operator+=(const CTextA& txt);
    const CTextA& append(const char* str, unsigned len);

    inline char* C_str() { return _str; }
    inline const char* C_str() const { return _str; }
    inline unsigned Size() const { return _size; }
    inline unsigned Len() const { return _len; }
};


namespace Tools
{

void ReleaseKey(WORD virtKey);

#ifdef DEVELOPMENT
inline void Msg(const TCHAR* msg)
{
    MessageBox(NULL, msg, _T(""), MB_OK);
}

inline void MsgA(const char* msg)
{
    MessageBoxA(NULL, msg, "", MB_OK);
}
#endif

} // namespace Tools
