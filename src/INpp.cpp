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


#include "INpp.h"


INpp INpp::Instance;


/**
 *  \brief
 */
long INpp::GetWord(char* buf, int bufSize, bool select) const
{
    long currPos = SendMessage(_hSC, SCI_GETCURRENTPOS, 0, 0);
    long wordStart = SendMessage(_hSC, SCI_WORDSTARTPOSITION,
            (WPARAM)currPos, (LPARAM)true);
    long wordEnd = SendMessage(_hSC, SCI_WORDENDPOSITION,
            (WPARAM)currPos, (LPARAM)true);

    long len = wordEnd - wordStart;
    if (len != 0)
    {
        if (select)
            SendMessage(_hSC, SCI_SETSEL, (WPARAM)wordStart, (LPARAM)wordEnd);
        else
            SendMessage(_hSC, SCI_SETSEL, (WPARAM)wordEnd, (LPARAM)wordEnd);

        if (bufSize > len)
        {
            struct TextRange tr = { { wordStart, wordEnd }, buf };
            SendMessage(_hSC, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);
        }
    }

    return len;
}


/**
 *  \brief
 */
void INpp::ReplaceWord(const char* replText) const
{
    long currPos = SendMessage(_hSC, SCI_GETCURRENTPOS, 0, 0);
    long wordStart = SendMessage(_hSC, SCI_WORDSTARTPOSITION,
            (WPARAM)currPos, (LPARAM)true);
    long wordEnd = SendMessage(_hSC, SCI_WORDENDPOSITION,
            (WPARAM)currPos, (LPARAM)true);

    SendMessage(_hSC, SCI_SETTARGETSTART, (WPARAM)wordStart, 0);
    SendMessage(_hSC, SCI_SETTARGETEND, (WPARAM)wordEnd, 0);

    SendMessage(_hSC, SCI_REPLACETARGET, (WPARAM)-1, (LPARAM)replText);
    wordEnd = wordStart + strlen(replText);
    SendMessage(_hSC, SCI_SETSEL, (WPARAM)wordEnd, (LPARAM)wordEnd);
}


/**
 *  \brief
 */
bool INpp::SearchText(const char* text, bool matchCase, bool wholeWord,
        long startPos = 0, long endPos = 0) const
{
    struct TextToFind ttf = {0};
    int searchFlags = 0;

    ttf.lpstrText = const_cast<char*>(text);

    if (startPos < 0)
        startPos = 0;
    ttf.chrg.cpMin = startPos;

    if (endPos <= 0)
        endPos = SendMessage(_hSC, SCI_GETLENGTH, 0, 0);
    ttf.chrg.cpMax = endPos;

    if (matchCase)
        searchFlags |= SCFIND_MATCHCASE;
    if (wholeWord)
        searchFlags |= SCFIND_WHOLEWORD;

    SendMessage(_hSC, SCI_SETSEL, (WPARAM)startPos, (LPARAM)endPos);
    if (SendMessage(_hSC, SCI_FINDTEXT,
            (WPARAM)searchFlags, (LPARAM)&ttf) == -1)
        return false;

    startPos = ttf.chrgText.cpMin;
    endPos = ttf.chrgText.cpMax;

    long lineNum =
        SendMessage(_hSC, SCI_LINEFROMPOSITION, (WPARAM)startPos, 0);
    long firstVisibleLine =
        SendMessage(_hSC, SCI_GETFIRSTVISIBLELINE, 0, 0);
    if (lineNum > 4)
        lineNum -= 5;
    lineNum -= firstVisibleLine;
    SendMessage(_hSC, SCI_LINESCROLL, 0, (LPARAM)lineNum);
    SendMessage(_hSC, SCI_SETSEL, (WPARAM)startPos, (LPARAM)endPos);

    return true;
}