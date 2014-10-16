// this file is part of notepad++
// Copyright (C)2003 Don HO <donho@altern.org>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include "PluginInterface.h"
#include "Notepad_plus_msgs.h"
#include "menuCmdID.h"
#include "INpp.h"
#include "GTags.h"
#include "TreeViewUI.h"


namespace
{

const int cFuncCount = 15;
FuncItem InterfaceFunc[cFuncCount];


/**
 *  \brief
 */
void addMenuItem(const TCHAR* itemName = NULL,
        PFUNCPLUGINCMD cmdFunc = NULL, bool initCheckMark = false)
{
    static int i = 0;

    if (itemName != NULL)
        _tcscpy_s(InterfaceFunc[i]._itemName, PLUGIN_ITEM_SIZE, itemName);
    InterfaceFunc[i]._pFunc = cmdFunc;
    InterfaceFunc[i++]._init2Check = initCheckMark;
}


/**
 *  \brief
 */
void autoUpdate()
{
    int NppCmdID = -1;

    GTags::AutoUpdate = !GTags::AutoUpdate;

    for (int i = 0; i < cFuncCount; i++)
        if (InterfaceFunc[i]._pFunc == autoUpdate)
        {
            NppCmdID = InterfaceFunc[i]._cmdID;
            break;
        }

    if (NppCmdID != -1)
        INpp::Get().SetPluginMenuFlag(NppCmdID, GTags::AutoUpdate);
}

}


BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD reasonForCall,
        LPVOID lpReserved)
{
    switch (reasonForCall)
    {
        case DLL_PROCESS_ATTACH:
        {
            GetModuleFileName((HMODULE)hModule,
                    GTags::DllPath.C_str(), GTags::DllPath.Size());

            CPath gtags(GTags::DllPath);
            gtags.StripFilename();
            gtags += GTags::cPluginName;
            gtags += _T("\\global.exe");

            bool gtagsBinsFound = gtags.FileExists();
            if (gtagsBinsFound)
            {
                gtags.StripFilename();
                gtags += _T("gtags.exe");
                gtagsBinsFound = gtags.FileExists();
            }

            if (!gtagsBinsFound)
            {
                gtags.StripFilename();
                TCHAR msg[512];
                _sntprintf_s(msg, 512, _TRUNCATE,
                        _T("GTags binaries not found in\n\"%s\"\n")
                        _T("%s plugin will not be loaded!"),
                        gtags.C_str(), GTags::cPluginName);
                MessageBox(NULL, msg, GTags::cPluginName,
                        MB_OK | MB_ICONERROR);
                return FALSE;
            }

            GTags::HInst = hModule;
            break;
        }
        case DLL_PROCESS_DETACH:
            GTags::HInst = NULL;
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}


extern "C" __declspec(dllexport) void setInfo(NppData nppData)
{
    INpp& npp = INpp::Get();
    npp.SetData(nppData);
    npp.ReadHandle();
    npp.GetFontName(GTags::UIFontName, 32);
    GTags::UIFontSize = (unsigned)npp.GetFontSize() - 1;

    TreeViewUI::Init();

    ZeroMemory(InterfaceFunc, sizeof(InterfaceFunc));

    addMenuItem(GTags::cAutoCompl, GTags::AutoComplete);
    addMenuItem(GTags::cAutoComplFile, GTags::AutoCompleteFile);
    addMenuItem(GTags::cFindFile, GTags::FindFile);
    addMenuItem(GTags::cFindDefinition, GTags::FindDefinition);
    addMenuItem(GTags::cFindReference, GTags::FindReference);
    addMenuItem(GTags::cGrep, GTags::Grep);
    addMenuItem(GTags::cFindLiteral, GTags::FindLiteral);
    addMenuItem(_T("Go Back"), GTags::GoBack);
    addMenuItem(); // separator
    addMenuItem(GTags::cCreateDatabase, GTags::CreateDatabase);
    addMenuItem(_T("Delete Database"), GTags::DeleteDatabase);
    addMenuItem(); // separator
    addMenuItem(_T("Auto-update on file save"), autoUpdate, GTags::AutoUpdate);
    addMenuItem(); // separator
    addMenuItem(GTags::cVersion, GTags::About);
}


extern "C" __declspec(dllexport) const TCHAR* getName()
{
    return GTags::cPluginName;
}


extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int* nbF)
{
    *nbF = cFuncCount;
    return InterfaceFunc;
}


extern "C" __declspec(dllexport) void beNotified(SCNotification* notifyCode)
{
    if (notifyCode->nmhdr.code == NPPN_FILESAVED)
    {
        if (GTags::AutoUpdate)
            GTags::UpdateSingleFile();
    }
}


extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message,
        WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}


#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
    return TRUE;
}
#endif //UNICODE