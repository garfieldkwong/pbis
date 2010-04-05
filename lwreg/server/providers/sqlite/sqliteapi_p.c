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
 *        sqliteapi.c
 *
 * Abstract:
 *
 *        Registry
 *
 *        Inter-process communication (Server) API for Users
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 *          Marc Guy (mguy@likewisesoftware.com)
 */
#include "includes.h"

NTSTATUS
SqliteGetKeyToken(
    PCWSTR pwszInputString,
    wchar16_t c,
    PWSTR *ppwszOutputString
    )
{
	NTSTATUS status = STATUS_SUCCESS;
    // Do not free
    PCWSTR pwszFound = NULL;
    PWSTR pwszOutputString = NULL;

    BAIL_ON_NT_INVALID_STRING(pwszInputString);

    pwszFound = RegStrchr(pwszInputString, c);
    if (pwszFound)
    {
        status = LW_RTL_ALLOCATE((PVOID*)&pwszOutputString, wchar16_t,
        		                  sizeof(*pwszOutputString)* (pwszFound - pwszInputString +1));
        BAIL_ON_NT_STATUS(status);

        memcpy(pwszOutputString, pwszInputString,(pwszFound - pwszInputString) * sizeof(*pwszOutputString));
    }

    *ppwszOutputString = pwszOutputString;

cleanup:

    return status;

error:
    LWREG_SAFE_FREE_MEMORY(pwszOutputString);

    goto cleanup;
}

NTSTATUS
SqliteGetParentKeyName(
    PCWSTR pwszInputString,
    wchar16_t c,
    PWSTR *ppwszOutputString
    )
{
	NTSTATUS status = STATUS_SUCCESS;
    // Do not free
    PCWSTR pwszFound = NULL;
    PWSTR pwszOutputString = NULL;

    BAIL_ON_NT_INVALID_STRING(pwszInputString);

    pwszFound = RegStrrchr(pwszInputString, c);
    if (pwszFound)
    {
        status = LW_RTL_ALLOCATE((PVOID*)&pwszOutputString, wchar16_t,
        		                  sizeof(*pwszOutputString)* (pwszFound - pwszInputString +1));
        BAIL_ON_NT_STATUS(status);

        memcpy(pwszOutputString, pwszInputString,(pwszFound - pwszInputString) * sizeof(*pwszOutputString));
    }

    *ppwszOutputString = pwszOutputString;

cleanup:

    return status;

error:
    LWREG_SAFE_FREE_MEMORY(pwszOutputString);

    goto cleanup;
}

NTSTATUS
SqliteCreateKeyHandle(
    IN ACCESS_MASK AccessGranted,
    IN PREG_KEY_CONTEXT pKey,
    OUT PREG_KEY_HANDLE* ppKeyHandle
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PREG_KEY_HANDLE pKeyHandle = NULL;

    BAIL_ON_INVALID_KEY_CONTEXT(pKey);

    status = LW_RTL_ALLOCATE((PVOID*)&pKeyHandle, REG_KEY_HANDLE, sizeof(*pKeyHandle));
    BAIL_ON_NT_STATUS(status);

    pKeyHandle->AccessGranted = AccessGranted;
    pKeyHandle->pKey = pKey;

    *ppKeyHandle = pKeyHandle;

cleanup:
    return status;

error:
    LWREG_SAFE_FREE_MEMORY(pKeyHandle);

    goto cleanup;
}

NTSTATUS
SqliteCreateKeyContext(
    IN ACCESS_MASK AccessGranted,
    IN PREG_DB_KEY pRegEntry,
    OUT PREG_KEY_CONTEXT* ppKeyResult
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PREG_KEY_CONTEXT pKeyResult = NULL;

    BAIL_ON_INVALID_REG_ENTRY(pRegEntry);

    status = LW_RTL_ALLOCATE((PVOID*)&pKeyResult, REG_KEY_CONTEXT, sizeof(*pKeyResult));
    BAIL_ON_NT_STATUS(status);

    pKeyResult->refCount = 1;

    pthread_rwlock_init(&pKeyResult->mutex, NULL);
    pKeyResult->pMutex = &pKeyResult->mutex;

    status = LwRtlWC16StringDuplicate(&pKeyResult->pwszKeyName, pRegEntry->pwszFullKeyName);
    BAIL_ON_NT_STATUS(status);

    status = SqliteGetParentKeyName(pKeyResult->pwszKeyName, (wchar16_t)'\\',&pKeyResult->pwszParentKeyName);
    BAIL_ON_NT_STATUS(status);

    pKeyResult->qwId = pRegEntry->version.qwDbId;

    pKeyResult->qwSdId = pRegEntry->qwAclIndex;

    // Cache ACL
    if (pRegEntry->ulSecDescLength)
    {
        status = LW_RTL_ALLOCATE((PVOID*)&pKeyResult->pSecurityDescriptor, VOID, pRegEntry->ulSecDescLength);
        BAIL_ON_NT_STATUS(status);

        memcpy(pKeyResult->pSecurityDescriptor, pRegEntry->pSecDescRel, pRegEntry->ulSecDescLength);
        pKeyResult->ulSecDescLength = pRegEntry->ulSecDescLength;
        pKeyResult->bHasSdInfo = TRUE;
    }

    *ppKeyResult = pKeyResult;

cleanup:
    return status;

error:
    RegSrvSafeFreeKeyContext(pKeyResult);
    *ppKeyResult = NULL;

    goto cleanup;
}

/* Create a new key, if the key exists already,
 * open the existing key
 */
NTSTATUS
SqliteCreateKeyInternal(
    IN OPTIONAL HANDLE handle,
    IN PREG_KEY_CONTEXT pParentKeyCtx,
    IN PWSTR pwszFullKeyName, // Full Key Path
    IN ACCESS_MASK AccessDesired,
    IN OPTIONAL PSECURITY_DESCRIPTOR_RELATIVE pSecDescRel,
    IN ULONG ulSecDescLength,
    OUT OPTIONAL PREG_KEY_HANDLE* ppKeyHandle,
    OUT OPTIONAL PDWORD pdwDisposition
    )
{
	NTSTATUS status = STATUS_SUCCESS;
    PREG_DB_KEY pRegEntry = NULL;
    PREG_KEY_HANDLE pKeyHandle = NULL;
    PREG_KEY_CONTEXT pKeyCtx = NULL;
    BOOLEAN bInLock = FALSE;
    PREG_SRV_API_STATE pServerState = (PREG_SRV_API_STATE)handle;
    ACCESS_MASK AccessGranted = 0;
    PSECURITY_DESCRIPTOR_RELATIVE pSecDescRelToSet = NULL;
    ULONG ulSecDescLengthToSet = 0;
    DWORD dwDisposition = 0;

    // Full key path
    BAIL_ON_NT_INVALID_STRING(pwszFullKeyName);

    LWREG_LOCK_MUTEX(bInLock, &gActiveKeyList.mutex);

	status = SqliteOpenKeyInternal_inlock(
			        handle,
			        pwszFullKeyName, // Full Key Path
	                AccessDesired,
	                &pKeyHandle);
	if (!status)
	{
		dwDisposition = REG_OPENED_EXISTING_KEY;

		goto done;
	}
	else if (STATUS_OBJECT_NAME_NOT_FOUND == status)
	{
		status = 0;
	}
	BAIL_ON_NT_STATUS(status);

	// Root Key has to be created with a given SD
	if (!pParentKeyCtx && !pSecDescRel)
	{
		status = STATUS_INTERNAL_ERROR;
		BAIL_ON_NT_STATUS(status);
	}

	// ACL check
	// Get key's security descriptor
	// Inherit from its direct parent or given by caller
	if (!pSecDescRel || !ulSecDescLength)
	{
		BAIL_ON_INVALID_KEY_CONTEXT(pParentKeyCtx);

		status = SqliteCacheKeySecurityDescriptor(pParentKeyCtx);
		BAIL_ON_NT_STATUS(status);

		pSecDescRelToSet = pParentKeyCtx->pSecurityDescriptor;
		ulSecDescLengthToSet = pParentKeyCtx->ulSecDescLength;
	}
	else
	{
		pSecDescRelToSet = pSecDescRel;
		ulSecDescLengthToSet = ulSecDescLength;
	}

	// Make sure SD has at least owner information
	if (!RtlValidRelativeSecurityDescriptor(pSecDescRelToSet,
			                                ulSecDescLengthToSet,
			                                OWNER_SECURITY_INFORMATION))
	{
		status = STATUS_INVALID_SECURITY_DESCR;
		BAIL_ON_NT_STATUS(status);
	}

    // when starting up lwregd pServerState is NULL and
	// creating root key can skip ACL check
    if (pServerState)
    {
        if (!pServerState->pToken)
        {
    	    status = RegSrvCreateAccessToken(pServerState->peerUID,
    	    		                         pServerState->peerGID,
    	    		                         &pServerState->pToken);
            BAIL_ON_NT_STATUS(status);
        }

    	status = RegSrvAccessCheckKey(pServerState->pToken,
		    	                      pSecDescRelToSet,
		    	                      ulSecDescLengthToSet,
	                                  AccessDesired,
	                                  &AccessGranted);
	    BAIL_ON_NT_STATUS(status);
    }

	// Create key with SD
	status = RegDbCreateKey(ghCacheConnection,
							pwszFullKeyName,
							pSecDescRelToSet,
							ulSecDescLengthToSet,
							&pRegEntry);
	BAIL_ON_NT_STATUS(status);

    if (pParentKeyCtx)
	{
	    SqliteCacheResetParentKeySubKeyInfo_inlock(pParentKeyCtx->pwszKeyName);
	}

	status = SqliteCreateKeyContext(AccessDesired, pRegEntry, &pKeyCtx);
	BAIL_ON_NT_STATUS(status);

	// Cache this new key in gActiveKeyList
	status = SqliteCacheInsertActiveKey_inlock(pKeyCtx);
	BAIL_ON_NT_STATUS(status);

	status = SqliteCreateKeyHandle(AccessGranted, pKeyCtx, &pKeyHandle);
	BAIL_ON_NT_STATUS(status);
	pKeyCtx = NULL;

	dwDisposition = REG_CREATED_NEW_KEY;

done:
    if (ppKeyHandle)
	{
		*ppKeyHandle = pKeyHandle;
	}
	else
	{
		SqliteSafeFreeKeyHandle_inlock(pKeyHandle);
	}

    if (pdwDisposition)
    {
    	*pdwDisposition =  dwDisposition;
    }

cleanup:

    SqliteReleaseKeyContext_inlock(pKeyCtx);

    LWREG_UNLOCK_MUTEX(bInLock, &gActiveKeyList.mutex);

    RegDbSafeFreeEntryKey(&pRegEntry);

    return status;

error:

    if (ppKeyHandle)
    {
	    *ppKeyHandle = NULL;
    }

    SqliteSafeFreeKeyHandle_inlock(pKeyHandle);

    goto cleanup;
}

NTSTATUS
SqliteOpenKeyInternal(
	IN OPTIONAL HANDLE handle,
    IN PCWSTR pwszFullKeyName, // Full Key Path
    IN ACCESS_MASK AccessDesired,
    OUT OPTIONAL PREG_KEY_HANDLE* ppKeyHandle
    )
{
	NTSTATUS status = STATUS_SUCCESS;
    BOOLEAN bInLock = FALSE;

    BAIL_ON_NT_INVALID_STRING(pwszFullKeyName);

    LWREG_LOCK_MUTEX(bInLock, &gActiveKeyList.mutex);

    status = SqliteOpenKeyInternal_inlock(handle,
    		                              pwszFullKeyName,
    		                              AccessDesired,
    		                              ppKeyHandle);
    BAIL_ON_NT_STATUS(status);

cleanup:
    LWREG_UNLOCK_MUTEX(bInLock, &gActiveKeyList.mutex);

    return status;

error:
    goto cleanup;
}

NTSTATUS
SqliteOpenKeyInternal_inDblock(
	IN OPTIONAL HANDLE handle,
    IN PCWSTR pwszFullKeyName, // Full Key Path
    IN ACCESS_MASK AccessDesired,
    OUT OPTIONAL PREG_KEY_HANDLE* ppKeyHandle
    )
{
	NTSTATUS status = STATUS_SUCCESS;
    BOOLEAN bInLock = FALSE;

    LWREG_LOCK_MUTEX(bInLock, &gActiveKeyList.mutex);

    status = SqliteOpenKeyInternal_inlock_inDblock(handle,
    		                                       pwszFullKeyName,
    		                                       AccessDesired,
    		                                       ppKeyHandle);
    BAIL_ON_NT_STATUS(status);

cleanup:
    LWREG_UNLOCK_MUTEX(bInLock, &gActiveKeyList.mutex);

    return status;

error:
    goto cleanup;
}

/* Open a key, if not found,
 * do not create a new key */
NTSTATUS
SqliteOpenKeyInternal_inlock(
	IN OPTIONAL HANDLE handle,
    IN PCWSTR pwszFullKeyName, // Full Key Path
    IN ACCESS_MASK AccessDesired,
    OUT OPTIONAL PREG_KEY_HANDLE* ppKeyHandle
    )
{
	NTSTATUS status = STATUS_SUCCESS;
	PREG_SRV_API_STATE pServerState = (PREG_SRV_API_STATE)handle;
    PREG_DB_KEY pRegEntry = NULL;
    PREG_KEY_HANDLE pKeyHandle = NULL;
    PREG_KEY_CONTEXT pKeyCtx = NULL;
    ACCESS_MASK AccessGranted = AccessDesired;

    BAIL_ON_NT_INVALID_STRING(pwszFullKeyName);

    pKeyCtx = SqliteCacheLocateActiveKey_inlock(pwszFullKeyName);
    if (!pKeyCtx)
    {
        status = RegDbOpenKey(ghCacheConnection, pwszFullKeyName, &pRegEntry);
        BAIL_ON_NT_STATUS(status);

        status = SqliteCreateKeyContext(AccessGranted, pRegEntry, &pKeyCtx);
        BAIL_ON_NT_STATUS(status);

        // Cache this new key in gActiveKeyList
        status = SqliteCacheInsertActiveKey_inlock(pKeyCtx);
        BAIL_ON_NT_STATUS(status);
    }

    if (pServerState)
    {
        if (!pServerState->pToken)
        {
    	    status = RegSrvCreateAccessToken(pServerState->peerUID,
    	    		                         pServerState->peerGID,
    	    		                         &pServerState->pToken);
            BAIL_ON_NT_STATUS(status);
        }

    	status = RegSrvAccessCheckKey(pServerState->pToken,
	                                  pKeyCtx->pSecurityDescriptor,
	                                  pKeyCtx->ulSecDescLength,
	                                  AccessDesired,
	                                  &AccessGranted);
	    BAIL_ON_NT_STATUS(status);
    }

    status = SqliteCreateKeyHandle(AccessGranted, pKeyCtx, &pKeyHandle);
	BAIL_ON_NT_STATUS(status);
	pKeyCtx = NULL;

    *ppKeyHandle = pKeyHandle;

cleanup:
    SqliteReleaseKeyContext_inlock(pKeyCtx);
    RegDbSafeFreeEntryKey(&pRegEntry);

    return status;

error:
    SqliteSafeFreeKeyHandle_inlock(pKeyHandle);
    *ppKeyHandle = NULL;

    goto cleanup;
}

NTSTATUS
SqliteOpenKeyInternal_inlock_inDblock(
	IN OPTIONAL HANDLE handle,
	IN PCWSTR pwszFullKeyName, // Full Key Path
	IN ACCESS_MASK AccessDesired,
	OUT OPTIONAL PREG_KEY_HANDLE* ppKeyHandle
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	PREG_SRV_API_STATE pServerState = (PREG_SRV_API_STATE)handle;
	PREG_DB_KEY pRegEntry = NULL;
	PREG_KEY_HANDLE pKeyHandle = NULL;
	PREG_KEY_CONTEXT pKeyCtx = NULL;
	ACCESS_MASK AccessGranted = AccessDesired;

	BAIL_ON_NT_INVALID_STRING(pwszFullKeyName);

	pKeyCtx = SqliteCacheLocateActiveKey_inlock(pwszFullKeyName);
	if (!pKeyCtx)
	{
		status = RegDbOpenKey_inlock(ghCacheConnection, pwszFullKeyName, &pRegEntry);
		BAIL_ON_NT_STATUS(status);

		status = SqliteCreateKeyContext(AccessGranted, pRegEntry, &pKeyCtx);
		BAIL_ON_NT_STATUS(status);

		// Cache this new key in gActiveKeyList
		status = SqliteCacheInsertActiveKey_inlock(pKeyCtx);
		BAIL_ON_NT_STATUS(status);
	}

	if (pServerState)
	{
        if (!pServerState->pToken)
        {
    	    status = RegSrvCreateAccessToken(pServerState->peerUID,
    	    		                         pServerState->peerGID,
    	    		                         &pServerState->pToken);
            BAIL_ON_NT_STATUS(status);
        }

		status = RegSrvAccessCheckKey(pServerState->pToken,
								      pKeyCtx->pSecurityDescriptor,
								      pKeyCtx->ulSecDescLength,
								      AccessDesired,
								      &AccessGranted);
		BAIL_ON_NT_STATUS(status);
	}

	status = SqliteCreateKeyHandle(AccessGranted, pKeyCtx, &pKeyHandle);
	BAIL_ON_NT_STATUS(status);
	pKeyCtx = NULL;

	*ppKeyHandle = pKeyHandle;

cleanup:
	SqliteReleaseKeyContext_inlock(pKeyCtx);
	RegDbSafeFreeEntryKey(&pRegEntry);

	return status;

error:
	SqliteSafeFreeKeyHandle_inlock(pKeyHandle);
	*ppKeyHandle = NULL;

	goto cleanup;
}

NTSTATUS
SqliteDeleteKeyInternal(
	IN HANDLE handle,
    IN PCWSTR pwszKeyName
    )
{
	NTSTATUS status = STATUS_SUCCESS;
    size_t sSubkeyCount = 0;
    PWSTR pwszParentKeyName = NULL;
    PREG_KEY_HANDLE pKeyHandle = NULL;
    // Do not free
    PREG_KEY_CONTEXT pKeyCtx = NULL;

    status = SqliteOpenKeyInternal(handle,
    		                       pwszKeyName,
    		                       0,
                                   &pKeyHandle);
    BAIL_ON_NT_STATUS(status);

    BAIL_ON_NT_INVALID_POINTER(pKeyHandle);
    pKeyCtx = pKeyHandle->pKey;
    BAIL_ON_INVALID_KEY_CONTEXT(pKeyCtx);

    // Delete key from DB
    // Make sure this key does not have subkey before go ahead and delete it
    // Also need to delete the all of this subkey's values
    status = RegDbQueryInfoKeyCount(ghCacheConnection,
    		                        pKeyCtx->qwId,
                                    QuerySubKeys,
                                    &sSubkeyCount);
    BAIL_ON_NT_STATUS(status);

    if (sSubkeyCount == 0)
    {
        // Delete all the values of this key
        status = RegDbDeleteKey(ghCacheConnection, pKeyCtx->qwId, pKeyCtx->qwSdId, pwszKeyName);
        BAIL_ON_NT_STATUS(status);

        status = SqliteGetParentKeyName(pwszKeyName, '\\',&pwszParentKeyName);
        BAIL_ON_NT_STATUS(status);

        if (!LW_IS_NULL_OR_EMPTY_STR(pwszParentKeyName))
        {
        	SqliteCacheResetParentKeySubKeyInfo(pwszParentKeyName);
        }
    }
    else
    {
    	status = STATUS_KEY_HAS_CHILDREN;
        BAIL_ON_NT_STATUS(status);
    }

cleanup:
    SqliteSafeFreeKeyHandle(pKeyHandle);

    LWREG_SAFE_FREE_MEMORY(pwszParentKeyName);

    return status;

error:
    goto cleanup;
}

// This can be called when a DB lock is already in held
NTSTATUS
SqliteDeleteKeyInternal_inDblock(
	IN HANDLE handle,
    IN PCWSTR pwszKeyName
    )
{
	NTSTATUS status = STATUS_SUCCESS;
    size_t sSubkeyCount = 0;
    PWSTR pwszParentKeyName = NULL;
    PREG_KEY_HANDLE pKeyHandle = NULL;
    // Do not free
    PREG_KEY_CONTEXT pKeyCtx = NULL;

    status = SqliteOpenKeyInternal_inDblock(handle,
    		                                pwszKeyName,
    		                                0,
                                            &pKeyHandle);
    BAIL_ON_NT_STATUS(status);

    BAIL_ON_NT_INVALID_POINTER(pKeyHandle);
    pKeyCtx = pKeyHandle->pKey;
    BAIL_ON_INVALID_KEY_CONTEXT(pKeyCtx);

    // Delete key from DB
    // Make sure this key does not have subkey before go ahead and delete it
    // Also need to delete the all of this subkey's values
    status = RegDbQueryInfoKeyCount_inlock(ghCacheConnection,
    		                        pKeyCtx->qwId,
                                    QuerySubKeys,
                                    &sSubkeyCount);
    BAIL_ON_NT_STATUS(status);

    if (sSubkeyCount == 0)
    {
        // Delete all the values of this key
        status = RegDbDeleteKey_inlock(ghCacheConnection, pKeyCtx->qwId, pKeyCtx->qwSdId, pwszKeyName);
        BAIL_ON_NT_STATUS(status);

        status = SqliteGetParentKeyName(pwszKeyName, '\\',&pwszParentKeyName);
        BAIL_ON_NT_STATUS(status);

        if (!LW_IS_NULL_OR_EMPTY_STR(pwszParentKeyName))
        {
        	SqliteCacheResetParentKeySubKeyInfo(pwszParentKeyName);
        }
    }
    else
    {
    	status = STATUS_KEY_HAS_CHILDREN;
        BAIL_ON_NT_STATUS(status);
    }

cleanup:
    SqliteSafeFreeKeyHandle(pKeyHandle);

    LWREG_SAFE_FREE_MEMORY(pwszParentKeyName);

    return status;

error:
    goto cleanup;
}

NTSTATUS
SqliteDeleteActiveKey(
    IN PCWSTR pwszKeyName
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PREG_KEY_CONTEXT pFoundKey = NULL;
    BOOLEAN bInLock = FALSE;

    LWREG_LOCK_MUTEX(bInLock, &gActiveKeyList.mutex);

    pFoundKey = SqliteCacheLocateActiveKey_inlock(pwszKeyName);
    if (pFoundKey)
    {
    	status = STATUS_RESOURCE_IN_USE;
        BAIL_ON_NT_STATUS(status);
    }

cleanup:
    SqliteReleaseKeyContext_inlock(pFoundKey);
    LWREG_UNLOCK_MUTEX(bInLock, &gActiveKeyList.mutex);

    return status;

error:
    goto cleanup;
}

/*delete all subkeys and values of hKey*/
NTSTATUS
SqliteDeleteTreeInternal_inDblock(
    IN HANDLE Handle,
    IN HKEY hKey
    )
{
	NTSTATUS status = STATUS_SUCCESS;
    HKEY hCurrKey = NULL;
    int iCount = 0;
    DWORD dwSubKeyCount = 0;
    LW_WCHAR psubKeyName[MAX_KEY_LENGTH];
    DWORD dwSubKeyLen = 0;
    PWSTR* ppwszSubKey = NULL;
    PREG_KEY_HANDLE pKeyHandle = (PREG_KEY_HANDLE)hKey;
    PREG_KEY_CONTEXT pKeyCtx = NULL;

    BAIL_ON_NT_INVALID_POINTER(pKeyHandle);
    pKeyCtx = pKeyHandle->pKey;
    BAIL_ON_INVALID_KEY_CONTEXT(pKeyCtx);

    status = RegDbQueryInfoKeyCount_inlock(ghCacheConnection,
    		                               pKeyCtx->qwId,
                                           QuerySubKeys,
                                           (size_t*)&dwSubKeyCount);
    BAIL_ON_NT_STATUS(status);

    if (dwSubKeyCount)
    {
        status = LW_RTL_ALLOCATE((PVOID*)&ppwszSubKey, PWSTR, sizeof(*ppwszSubKey) * dwSubKeyCount);
        BAIL_ON_NT_STATUS(status);
    }

    for (iCount = 0; iCount < dwSubKeyCount; iCount++)
    {
        dwSubKeyLen = MAX_KEY_LENGTH;
        memset(psubKeyName, 0, MAX_KEY_LENGTH);

        status = SqliteEnumKeyEx_inDblock(Handle,
                                  hKey,
                                  iCount,
                                  psubKeyName,
                                  &dwSubKeyLen,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL);
        BAIL_ON_NT_STATUS(status);

    	status = LwRtlWC16StringDuplicate(&ppwszSubKey[iCount], psubKeyName);
        BAIL_ON_NT_STATUS(status);
    }

    for (iCount = 0; iCount < dwSubKeyCount; iCount++)
    {
        status = SqliteOpenKeyEx_inDblock(Handle,
                                  hKey,
                                  ppwszSubKey[iCount],
                                  0,
                                  KEY_ALL_ACCESS,
                                  &hCurrKey);
        BAIL_ON_NT_STATUS(status);

        status = SqliteDeleteTreeInternal_inDblock(
        		                           Handle,
                                           hCurrKey);
        BAIL_ON_NT_STATUS(status);

        if (hCurrKey)
        {
            SqliteCloseKey(hCurrKey);
            hCurrKey = NULL;
        }

        status = SqliteDeleteKey_inDblock(Handle, hKey, ppwszSubKey[iCount]);
        BAIL_ON_NT_STATUS(status);
    }

cleanup:
    if (hCurrKey)
    {
        SqliteCloseKey(hCurrKey);
    }
    RegFreeWC16StringArray(ppwszSubKey, dwSubKeyCount);

    return status;


error:
    goto cleanup;
}
