/**
 *  \file
 *  \brief  GTags config class
 *
 *  \author  Pavel Nedev <pg.nedev@gmail.com>
 *
 *  \section COPYRIGHT
 *  Copyright(C) 2015-2016 Pavel Nedev
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


#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "INpp.h"
#include "Common.h"
#include "Config.h"
#include "GTags.h"


namespace GTags
{

const TCHAR Settings::cInfo[] =
        _T("# Configuration file for Notepad++ ") PLUGIN_NAME _T(" plugin\n")
        _T("# This file is automatically generated and will be overwritten")
        _T(" on next ") PLUGIN_NAME _T(" config\n");

const TCHAR Settings::cUseDefDbKey[]     = _T("UseDefaultDB = ");
const TCHAR Settings::cDefDbPathKey[]    = _T("DefaultDBPath = ");
const TCHAR Settings::cREOptionKey[]     = _T("RegExpOptionOn = ");
const TCHAR Settings::cMCOptionKey[]     = _T("MatchCaseOptionOn = ");

const TCHAR DbConfig::cInfo[] =
        _T("# ") PLUGIN_NAME _T(" database config\n");

const TCHAR DbConfig::cParserKey[]       = _T("Parser = ");
const TCHAR DbConfig::cAutoUpdateKey[]   = _T("AutoUpdate = ");
const TCHAR DbConfig::cUseLibDbKey[]     = _T("UseLibraryDBs = ");
const TCHAR DbConfig::cLibDbPathsKey[]   = _T("LibraryDBPaths = ");

const TCHAR DbConfig::cDefaultParser[]   = _T("default");
const TCHAR DbConfig::cCtagsParser[]     = _T("ctags");
const TCHAR DbConfig::cPygmentsParser[]  = _T("pygments");

const TCHAR* DbConfig::cParsers[DbConfig::PARSER_LIST_END] = {
    DbConfig::cDefaultParser,
    DbConfig::cCtagsParser,
    DbConfig::cPygmentsParser
};


/**
 *  \brief
 */
DbConfig::DbConfig()
{
    SetDefaults();
}


/**
 *  \brief
 */
bool DbConfig::IsOptionRecognized(const TCHAR* option)
{
    if (!_tcsncmp(option, cParserKey, _countof(cParserKey) - 1) ||
        !_tcsncmp(option, cAutoUpdateKey, _countof(cAutoUpdateKey) - 1) ||
        !_tcsncmp(option, cUseLibDbKey, _countof(cUseLibDbKey) - 1) ||
        !_tcsncmp(option, cLibDbPathsKey, _countof(cLibDbPathsKey) - 1))
        return true;

    return false;
}


/**
 *  \brief
 */
void DbConfig::SetDefaults()
{
    _parserIdx = DEFAULT_PARSER;
    _autoUpdate = true;
    _useLibDb = false;
    _libDbPaths.clear();
}


/**
 *  \brief
 */
bool DbConfig::LoadFromFolder(const CPath& cfgFileFolder)
{
    SetDefaults();

    CPath cfgFile(cfgFileFolder);
    cfgFile += cPluginCfgFileName;

    if (!cfgFile.FileExists())
        return false;

    FILE* fp;
    _tfopen_s(&fp, cfgFile.C_str(), _T("rt"));
    if (fp == NULL)
        return false;

    TCHAR line[8192];
    while (_fgetts(line, _countof(line), fp))
    {
        // Comment or empty line
        if (line[0] == _T('#') || line[0] == _T('\n'))
            continue;

        // Strip newline from the end of the line
        line[_tcslen(line) - 1] = 0;

        if (Settings::IsOptionRecognized(line))
            continue;

        if (!_tcsncmp(line, cParserKey, _countof(cParserKey) - 1))
        {
            unsigned pos = _countof(cParserKey) - 1;
            if (!_tcsncmp(&line[pos], cCtagsParser, _countof(cCtagsParser) - 1))
                _parserIdx = CTAGS_PARSER;
            else if (!_tcsncmp(&line[pos], cPygmentsParser, _countof(cPygmentsParser) - 1))
                _parserIdx = PYGMENTS_PARSER;
            else
                _parserIdx = DEFAULT_PARSER;
        }
        else if (!_tcsncmp(line, cAutoUpdateKey, _countof(cAutoUpdateKey) - 1))
        {
            unsigned pos = _countof(cAutoUpdateKey) - 1;
            if (!_tcsncmp(&line[pos], _T("yes"), _countof(_T("yes")) - 1))
                _autoUpdate = true;
            else
                _autoUpdate = false;
        }
        else if (!_tcsncmp(line, cUseLibDbKey, _countof(cUseLibDbKey) - 1))
        {
            unsigned pos = _countof(cUseLibDbKey) - 1;
            if (!_tcsncmp(&line[pos], _T("yes"), _countof(_T("yes")) - 1))
                _useLibDb = true;
            else
                _useLibDb = false;
        }
        else if (!_tcsncmp(line, cLibDbPathsKey, _countof(cLibDbPathsKey) - 1))
        {
            unsigned pos = _countof(cLibDbPathsKey) - 1;
            DbPathsFromBuf(&line[pos], _T(";"));
        }
        else
        {
            SetDefaults();
            fclose(fp);
            return false;
        }
    }

    fclose(fp);

    return true;
}


/**
 *  \brief
 */
bool DbConfig::Write(FILE* fp) const
{
    bool success = false;

    CText libDbPaths;
    DbPathsToBuf(libDbPaths, _T(';'));

    if (_ftprintf_s(fp, _T("%s\n"), cInfo) > 0)
    if (_ftprintf_s(fp, _T("%s%s\n"), cParserKey, Parser()) > 0)
    if (_ftprintf_s(fp, _T("%s%s\n"), cAutoUpdateKey, (_autoUpdate ? _T("yes") : _T("no"))) > 0)
    if (_ftprintf_s(fp, _T("%s%s\n"), cUseLibDbKey, (_useLibDb ? _T("yes") : _T("no"))) > 0)
    if (_ftprintf_s(fp, _T("%s%s\n"), cLibDbPathsKey, libDbPaths.C_str()) > 0)
        success = true;

    return success;
}


/**
 *  \brief
 */
bool DbConfig::SaveToFolder(const CPath& cfgFileFolder) const
{
    CPath cfgFile(cfgFileFolder);
    cfgFile += cPluginCfgFileName;

    FILE* fp;
    _tfopen_s(&fp, cfgFile.C_str(), _T("wt"));
    if (fp == NULL)
        return false;

    bool success = Write(fp);
    fclose(fp);

    return success;
}


/**
 *  \brief
 */
void DbConfig::DbPathsFromBuf(TCHAR* buf, const TCHAR* separators)
{
    TCHAR* pTmp = NULL;
    for (TCHAR* ptr = _tcstok_s(buf, separators, &pTmp); ptr; ptr = _tcstok_s(NULL, separators, &pTmp))
    {
        CPath db(ptr);
        db.StripTrailingSpaces();
        if (db.Exists())
            _libDbPaths.push_back(db);
    }
}


/**
 *  \brief
 */
void DbConfig::DbPathsToBuf(CText& buf, TCHAR separator) const
{
    if (!_libDbPaths.size())
        return;

    buf += _libDbPaths[0];

    for (unsigned i = 1; i < _libDbPaths.size(); ++i)
    {
        buf += separator;
        buf += _libDbPaths[i];
    }
}


/**
 *  \brief
 */
const DbConfig& DbConfig::operator=(const DbConfig& rhs)
{
    if (this != &rhs)
    {
        _parserIdx  = rhs._parserIdx;
        _autoUpdate = rhs._autoUpdate;
        _useLibDb   = rhs._useLibDb;
        _libDbPaths = rhs._libDbPaths;
    }

    return *this;
}


/**
 *  \brief
 */
bool DbConfig::operator==(const DbConfig& rhs) const
{
    if (this == &rhs)
        return true;

    return (_parserIdx == rhs._parserIdx && _autoUpdate == rhs._autoUpdate &&
            _useLibDb == rhs._useLibDb && _libDbPaths == rhs._libDbPaths);
}


/**
 *  \brief
 */
Settings::Settings()
{
    SetDefaults();
}


/**
 *  \brief
 */
bool Settings::IsOptionRecognized(const TCHAR* option)
{
    if (!_tcsncmp(option, cUseDefDbKey, _countof(cUseDefDbKey) - 1) ||
        !_tcsncmp(option, cDefDbPathKey, _countof(cDefDbPathKey) - 1) ||
        !_tcsncmp(option, cREOptionKey, _countof(cREOptionKey) - 1) ||
        !_tcsncmp(option, cMCOptionKey, _countof(cMCOptionKey) - 1))
        return true;

    return false;
}


/**
 *  \brief
 */
void Settings::SetDefaults()
{
    _useDefDb = false;
    _defDbPath.Clear();
    _re = false;
    _mc = true;

    _genericDbCfg.SetDefaults();
}


/**
 *  \brief
 */
bool Settings::Load()
{
    SetDefaults();

    CPath cfgFile;
    INpp::Get().GetPluginsConfDir(cfgFile);
    cfgFile += cPluginCfgFileName;

    if (!cfgFile.FileExists())
        return false;

    FILE* fp;
    _tfopen_s(&fp, cfgFile.C_str(), _T("rt"));
    if (fp == NULL)
        return false;

    TCHAR line[8192];
    while (_fgetts(line, _countof(line), fp))
    {
        // Comment or empty line
        if (line[0] == _T('#') || line[0] == _T('\n'))
            continue;

        // Strip newline from the end of the line
        line[_tcslen(line) - 1] = 0;

        if (DbConfig::IsOptionRecognized(line))
            continue;

        if (!_tcsncmp(line, cUseDefDbKey, _countof(cUseDefDbKey) - 1))
        {
            unsigned pos = _countof(cUseDefDbKey) - 1;
            if (!_tcsncmp(&line[pos], _T("yes"), _countof(_T("yes")) - 1))
                _useDefDb = true;
            else
                _useDefDb = false;
        }
        else if (!_tcsncmp(line, cDefDbPathKey, _countof(cDefDbPathKey) - 1))
        {
            unsigned pos = _countof(cDefDbPathKey) - 1;
            _defDbPath = &line[pos];
            _defDbPath.StripTrailingSpaces();
            if (!_defDbPath.Exists())
                _defDbPath.Clear();
        }
        else if (!_tcsncmp(line, cREOptionKey, _countof(cREOptionKey) - 1))
        {
            unsigned pos = _countof(cREOptionKey) - 1;
            if (!_tcsncmp(&line[pos], _T("yes"), _countof(_T("yes")) - 1))
                _re = true;
            else
                _re = false;
        }
        else if (!_tcsncmp(line, cMCOptionKey, _countof(cMCOptionKey) - 1))
        {
            unsigned pos = _countof(cMCOptionKey) - 1;
            if (!_tcsncmp(&line[pos], _T("yes"), _countof(_T("yes")) - 1))
                _mc = true;
            else
                _mc = false;
        }
        else
        {
            SetDefaults();
            fclose(fp);
            return false;
        }
    }

    fclose(fp);

    return true;
}


/**
 *  \brief
 */
bool Settings::Save() const
{
    CPath cfgFile;
    INpp::Get().GetPluginsConfDir(cfgFile);
    cfgFile += cPluginCfgFileName;

    FILE* fp;
    _tfopen_s(&fp, cfgFile.C_str(), _T("wt"));
    if (fp == NULL)
        return false;

    bool success = false;
    if (_ftprintf_s(fp, _T("%s\n"), cInfo) > 0)
    if (_ftprintf_s(fp, _T("%s%s\n"), cUseDefDbKey, (_useDefDb ? _T("yes") : _T("no"))) > 0)
    if (_ftprintf_s(fp, _T("%s%s\n"), cDefDbPathKey, _defDbPath.C_str()) > 0)
    if (_ftprintf_s(fp, _T("%s%s\n"), cREOptionKey, (_re ? _T("yes") : _T("no"))) > 0)
    if (_ftprintf_s(fp, _T("%s%s\n\n"), cMCOptionKey, (_mc ? _T("yes") : _T("no"))) > 0)
    if (_genericDbCfg.Write(fp))
        success = true;

    fclose(fp);

    return success;
}


/**
 *  \brief
 */
const Settings& Settings::operator=(const Settings& rhs)
{
    if (this != &rhs)
    {
        _useDefDb       = rhs._useDefDb;
        _defDbPath      = rhs._defDbPath;
        _re             = rhs._re;
        _mc             = rhs._mc;
        _genericDbCfg   = rhs._genericDbCfg;
    }

    return *this;
}


/**
 *  \brief
 */
bool Settings::operator==(const Settings& rhs) const
{
    if (this == &rhs)
        return true;

    return (_useDefDb == rhs._useDefDb && _defDbPath == rhs._defDbPath &&
            _re == rhs._re && _mc == rhs._mc && _genericDbCfg == rhs._genericDbCfg);
}

} // namespace GTags
