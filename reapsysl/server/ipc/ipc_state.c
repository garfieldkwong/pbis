/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.  You should have received a copy of the GNU General
 * Public License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        ipc_state.c
 *
 * Abstract:
 *
 *        Reaper for syslog
 *
 *        Server Connection State API
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */
#include "includes.h"

static
DWORD
RSysSrvIpcCheckPermissions(
    LWMsgSecurityToken* pToken,
    uid_t* puid,
    gid_t* pgid
    )
{
    DWORD dwError = 0;
    uid_t euid;
    gid_t egid;

    if (pToken == NULL || strcmp(lwmsg_security_token_get_type(pToken), "local"))
    {
        RSYS_LOG_WARNING("Unsupported authentication type %s", lwmsg_security_token_get_type(pToken));
        dwError = RSYS_ERROR_NOT_SUPPORTED;
        BAIL_ON_RSYS_ERROR(dwError);
    }

    dwError = MAP_LWMSG_ERROR(lwmsg_local_token_get_eid(pToken, &euid, &egid));
    BAIL_ON_RSYS_ERROR(dwError);

    RSYS_LOG_VERBOSE("Permission granted for (uid = %i, gid = %i) to open RSysIpcServer",
                    (int) euid,
                    (int) egid);

    *puid = euid;
    *pgid = egid;

error:
    return dwError;
}

void
RSysSrvIpcDestructSession(
    LWMsgSecurityToken* pToken,
    void* pSessionData
    )
{
    RSysSrvCloseServer(pSessionData);
}

LWMsgStatus
RSysSrvIpcConstructSession(
    LWMsgSecurityToken* pToken,
    void* pData,
    void** ppSessionData
    )
{
    DWORD dwError = 0;
    HANDLE Handle = (HANDLE)NULL;
    uid_t UID;
    gid_t GID;

    RSYS_LOG_VERBOSE("RSysSrvIpcConstructSession open hServer");
    
    dwError = RSysSrvIpcCheckPermissions(pToken, &UID, &GID);
    if (!dwError)
    {
        RSYS_LOG_VERBOSE("Successfully opened hServer");
        
        dwError = RSysSrvOpenServer(UID, GID, &Handle);
        BAIL_ON_RSYS_ERROR(dwError);
    }
    
    BAIL_ON_RSYS_ERROR(dwError);
    
    *ppSessionData = Handle;

cleanup:

    return MAP_RSYS_ERROR_IPC(dwError);

error:

    goto cleanup;
}
