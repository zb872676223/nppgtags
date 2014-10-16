/**
 *  \file
 *  \brief  Npp API
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
#include "Notepad_plus_msgs.h"
#include "Docking.h"
#include "PluginInterface.h"
#include "Common.h"


/**
 *  \class  INpp
 *  \brief
 */
class INpp
{
public:
    static inline INpp& Get() { return Instance; }

    inline HWND ReadHandle()
    {
        int currentEdit;

        SendMessage(_nppData._nppHandle, NPPM_GETCURRENTSCINTILLA,
                0, (LPARAM)&currentEdit);

        _hSC = currentEdit ? _nppData._scintillaSecondHandle :
                _nppData._scintillaMainHandle;

        return _hSC;
    }

    inline void SetData(NppData nppData)
    {
        _nppData = nppData;
        ReadHandle();
    }

    inline HWND GetMainHandle() { return _nppData._nppHandle; }
    inline HWND GetHandle() { return _hSC; }

    inline void SetPluginMenuFlag(int cmdID, bool enable) const
    {
        SendMessage(_nppData._nppHandle, NPPM_SETMENUITEMCHECK,
                (WPARAM)cmdID, (LPARAM)enable);
    }

    inline void RegisterWin(HWND hwnd) const
    {
        SendMessage(_nppData._nppHandle, NPPM_MODELESSDIALOG,
                MODELESSDIALOGADD, (LPARAM)hwnd);
    }

    inline void UnregisterWin(HWND hwnd) const
    {
        SendMessage(_nppData._nppHandle, NPPM_MODELESSDIALOG,
                MODELESSDIALOGREMOVE, (LPARAM)hwnd);
    }

    inline void RegisterDockingWin(tTbData &data) const
    {
        SendMessage(_nppData._nppHandle, NPPM_DMMREGASDCKDLG,
                0, (LPARAM)&data);
    }

    inline void ShowDockingWin(HWND hwnd) const
    {
        SendMessage(_nppData._nppHandle, NPPM_DMMSHOW, 0, (LPARAM)hwnd);
    }

    inline void HideDockingWin(HWND hwnd) const
    {
        SendMessage(_nppData._nppHandle, NPPM_DMMHIDE, 0, (LPARAM)hwnd);
    }

    inline void UpdateDockingWin(HWND hwnd) const
    {
        SendMessage(_nppData._nppHandle, NPPM_DMMUPDATEDISPINFO,
                0, (LPARAM)hwnd);
    }

    inline void GetMainDir(CPath& mainPath) const
    {
        SendMessage(_nppData._nppHandle, NPPM_GETNPPDIRECTORY,
                0, (LPARAM)mainPath.C_str());
    }

    inline void GetPluginsConfDir(CPath& confPath) const
    {
        SendMessage(_nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR,
                0, (LPARAM)confPath.C_str());
    }

    inline void GetFilePath(CPath& filePath) const
    {
        SendMessage(_nppData._nppHandle, NPPM_GETFULLCURRENTPATH,
                0, (LPARAM)filePath.C_str());
    }

    inline void GetFileNamePart(CPath& fileName) const
    {
        SendMessage(_nppData._nppHandle, NPPM_GETNAMEPART,
                0, (LPARAM)fileName.C_str());
    }

    inline int OpenFile(const CPath& filePath) const
    {
        if (!SendMessage(_nppData._nppHandle, NPPM_DOOPEN,
                0, (LPARAM)filePath.C_str()))
            return -1;
        return 0;
    }

    inline void GetFontName(TCHAR* fontName, int len) const
    {
        char fontNameA[32];
        size_t cnt;

        SendMessage(_hSC, SCI_STYLEGETFONT, (WPARAM)STYLE_DEFAULT,
                (LPARAM)fontNameA);
        mbstowcs_s(&cnt, fontName, len, fontNameA, _TRUNCATE);
    }

    inline int GetFontSize() const
    {
        return SendMessage(_hSC, SCI_STYLEGETSIZE, (WPARAM)STYLE_DEFAULT, 0);
    }

    inline int GetTextHeight() const
    {
        long currPos = SendMessage(_hSC, SCI_GETCURRENTPOS, 0, 0);
        currPos = SendMessage(_hSC, SCI_LINEFROMPOSITION, (WPARAM)currPos, 0);
        return SendMessage(_hSC, SCI_TEXTHEIGHT, (WPARAM)currPos, 0);
    }

    inline void GetPointPos(int* x, int* y) const
    {
        long currPos = SendMessage(_hSC, SCI_GETCURRENTPOS, 0, 0);
        *x = SendMessage(_hSC, SCI_POINTXFROMPOSITION, 0, (LPARAM)currPos) + 2;
        *y = SendMessage(_hSC, SCI_POINTYFROMPOSITION, 0, (LPARAM)currPos) + 2;
    }

    inline void GetView(long* firstVisibleLine, long* pos) const
    {
        *firstVisibleLine = SendMessage(_hSC, SCI_GETFIRSTVISIBLELINE, 0, 0);
        *pos = SendMessage(_hSC, SCI_GETCURRENTPOS, 0, 0);
    }

    inline void SetView(long firstVisibleLine, long pos) const
    {
        long curLine = SendMessage(_hSC, SCI_GETFIRSTVISIBLELINE, 0, 0);
        curLine = firstVisibleLine - curLine;
        SendMessage(_hSC, SCI_LINESCROLL, 0, (LPARAM)curLine);
        SendMessage(_hSC, SCI_SETSEL, (WPARAM)pos, (LPARAM)pos);
    }

    inline void GoToLine(long line) const
    {
        long curLine = SendMessage(_hSC, SCI_GETFIRSTVISIBLELINE, 0, 0);
        curLine = line - curLine - 1;
        SendMessage(_hSC, SCI_LINESCROLL, 0, (LPARAM)curLine);
    }

    inline long PositionFromLine(long line) const
    {
        return SendMessage(_hSC, SCI_POSITIONFROMLINE, (WPARAM)line - 1, 0);
    }

    inline long LineEndPosition(long line) const
    {
        return SendMessage(_hSC, SCI_GETLINEENDPOSITION, (WPARAM)line, 0);
    }

    inline int IsSelectionVertical() const
    {
        return SendMessage(_hSC, SCI_SELECTIONISRECTANGLE, 0, 0);
    }

    inline long GetSelectionSize() const
    {
        return SendMessage(_hSC, SCI_GETSELTEXT, 0, 0) - 1;
    }

    inline long GetSelection(char* buf, int bufSize) const
    {
        long selLen = GetSelectionSize();
        if (selLen != 0 && bufSize > selLen)
            SendMessage(_hSC, SCI_GETSELTEXT, 0, (LPARAM)buf);
        return selLen;
    }

    inline void ClearSelection() const
    {
        long currPos = SendMessage(_hSC, SCI_GETCURRENTPOS, 0, 0);
        SendMessage(_hSC, SCI_SETSEL, (WPARAM)currPos, (LPARAM)currPos);
    }

    inline long GetWordSize() const
    {
        long currPos = SendMessage(_hSC, SCI_GETCURRENTPOS, 0, 0);
        long wordStart = SendMessage(_hSC, SCI_WORDSTARTPOSITION,
                (WPARAM)currPos, (LPARAM)true);
        long wordEnd = SendMessage(_hSC, SCI_WORDENDPOSITION,
                (WPARAM)currPos, (LPARAM)true);

        return wordEnd - wordStart;
    }

    long GetWord(char* buf, int bufSize, bool select) const;
    void ReplaceWord(const char* replText) const;
    bool SearchText(const char* text, bool matchCase, bool wholeWord,
            long startPos, long endPos) const;

    inline void Backspace() const
    {
        SendMessage(_hSC, SCI_DELETEBACK, 0, 0);
    }

    inline void AddText(char* txt, int len) const
    {
        SendMessage(_hSC, SCI_ADDTEXT, (WPARAM)len, (LPARAM)txt);
    }

private:
    static INpp Instance;

    INpp() : _hSC(NULL) {}
    INpp(const INpp&);
    ~INpp() {}

    NppData _nppData;
    HWND _hSC;
};