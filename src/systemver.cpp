/*
    * WinDBG Anti-RootKit extension
    * Copyright � 2013-2015  Vyacheslav Rusakoff
    * 
    * This program is free software: you can redistribute it and/or modify
    * it under the terms of the GNU General Public License as published by
    * the Free Software Foundation, either version 3 of the License, or
    * (at your option) any later version.
    * 
    * This program is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    * GNU General Public License for more details.
    * 
    * You should have received a copy of the GNU General Public License
    * along with this program.  If not, see <http://www.gnu.org/licenses/>.

    * This work is licensed under the terms of the GNU GPL, version 3.  See
    * the COPYING file in the top-level directory.
*/

#include <engextcpp.hpp>

#include <string>
#include <sstream>
#include <set>

#include "systemver.hpp"
#include "manipulators.hpp"
#include "./ddk.h"

namespace wa {

WDbgArkSystemVer::WDbgArkSystemVer() : m_inited(false),
                                       m_platform_id(0),
                                       m_major_build(0),
                                       m_minor_build(0),
                                       m_service_pack_number(0),
                                       m_strict_minor_build(0),
                                       m_known_windows_builds(),
                                       out(),
                                       warn(),
                                       err() {
    HRESULT result = g_Ext->m_Control->GetSystemVersion(reinterpret_cast<PULONG>(&m_platform_id),
                                                        reinterpret_cast<PULONG>(&m_major_build),
                                                        reinterpret_cast<PULONG>(&m_minor_build),
                                                        nullptr,
                                                        0,
                                                        nullptr,
                                                        reinterpret_cast<PULONG>(&m_service_pack_number),
                                                        nullptr,
                                                        0,
                                                        nullptr);

    if ( !SUCCEEDED(result) ) {
        err << wa::showminus << __FUNCTION__ ": GetSystemVersion failed with result = " << result << endlerr;
        return;
    }

    if ( !SetWindowsStrictMinorBuild() ) {
        err << wa::showminus << __FUNCTION__ ": SetWindowsStrictMinorBuild failed" << endlerr;
        return;
    }

    InitKnownWindowsBuilds();
    CheckWindowsBuild();

    m_inited = true;
}

void WDbgArkSystemVer::InitKnownWindowsBuilds(void) {
    m_known_windows_builds = { WXP_VER, W2K3_VER, VISTA_RTM_VER, VISTA_SP1_VER, VISTA_SP2_VER, W7RTM_VER, W7SP1_VER,
                               W8RTM_VER, W81RTM_VER, W10RTM_VER, W10TH2_VER };
}

bool WDbgArkSystemVer::SetWindowsStrictMinorBuild(void) {
    if ( m_minor_build <= WXP_VER )
        m_strict_minor_build = WXP_VER;
    else if ( m_minor_build > WXP_VER && m_minor_build <= W2K3_VER )
        m_strict_minor_build = W2K3_VER;
    else if ( m_minor_build > W2K3_VER && m_minor_build <= VISTA_RTM_VER )
        m_strict_minor_build = VISTA_RTM_VER;
    else if ( m_minor_build > VISTA_RTM_VER && m_minor_build <= VISTA_SP1_VER )
        m_strict_minor_build = VISTA_SP1_VER;
    else if ( m_minor_build > VISTA_SP1_VER && m_minor_build <= VISTA_SP2_VER )
        m_strict_minor_build = VISTA_SP2_VER;
    else if ( m_minor_build > VISTA_SP2_VER && m_minor_build <= W7RTM_VER )
        m_strict_minor_build = W7RTM_VER;
    else if ( m_minor_build > W7RTM_VER && m_minor_build <= W7SP1_VER )
        m_strict_minor_build = W7SP1_VER;
    else if ( m_minor_build > W7SP1_VER && m_minor_build <= W8RTM_VER )
        m_strict_minor_build = W8RTM_VER;
    else if ( m_minor_build > W8RTM_VER && m_minor_build <= W81RTM_VER )
        m_strict_minor_build = W81RTM_VER;
    else if ( m_minor_build > W81RTM_VER && m_minor_build <= W10RTM_VER )
        m_strict_minor_build = W10RTM_VER;
    else if ( m_minor_build > W10RTM_VER && m_minor_build <= W10TH2_VER )
        m_strict_minor_build = W10TH2_VER;

    return (m_strict_minor_build ? true : false);
}

void WDbgArkSystemVer::CheckWindowsBuild(void) {
    if ( m_known_windows_builds.find(m_minor_build) == m_known_windows_builds.end() ) {
        warn << wa::showqmark << __FUNCTION__ << ": unknown Windows version. Be careful and look sharp!" << endlwarn;
    }
}

}   // namespace wa
