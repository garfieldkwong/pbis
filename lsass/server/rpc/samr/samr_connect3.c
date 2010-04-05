/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 */

/*
 * Copyright Likewise Software    2004-2009
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the license, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.  You should have received a copy
 * of the GNU Lesser General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * LESSER GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        samr_connect3.c
 *
 * Abstract:
 *
 *        Remote Procedure Call (RPC) Server Interface
 *
 *        SamrConnect3 function
 *
 * Authors: Rafal Szczesniak (rafal@likewise.com)
 */

#include "includes.h"


NTSTATUS
SamrSrvConnect3(
    /* [in] */  handle_t        hBinding,
    /* [in] */  DWORD           dwSize,
    /* [in] */  PCWSTR          pwszSystemName,
    /* [in] */  DWORD           dwUnknown1,
    /* [in] */  DWORD           dwAccessMask,
    /* [out] */ CONNECT_HANDLE *hConn
    )
{
    const DWORD dwConnectVersion = 3;

    NTSTATUS ntStatus = STATUS_SUCCESS;
    PCONNECT_CONTEXT pConnCtx = NULL;

    ntStatus = SamrSrvConnectInternal(hBinding,
                                      pwszSystemName,
                                      dwAccessMask,
                                      dwConnectVersion,
                                      0,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &pConnCtx);
    BAIL_ON_NTSTATUS_ERROR(ntStatus);

    *hConn = (CONNECT_HANDLE)pConnCtx;

cleanup:
    return ntStatus;

error:
    if (pConnCtx)
    {
        CONNECT_HANDLE_rundown((CONNECT_HANDLE)pConnCtx);
    }

    *hConn = NULL;
    goto cleanup;
}


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
