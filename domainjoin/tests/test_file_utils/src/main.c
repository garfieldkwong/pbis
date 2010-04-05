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

#include "domainjoin.h"

#define TEST_GET_MATCHING_FILE_PATHS_IN_FOLDER 1

#if TEST_GET_MATCHING_FILE_PATHS_IN_FOLDER
CENTERROR
TestGetMatchingFilePathsInFolder()
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    PSTR* ppszFilePaths = NULL;
    DWORD nPaths = 0;
    int   iPath = 0;

    ceError = CTGetMatchingFilePathsInFolder("/etc",
                                             "nss.*",
                                             &ppszFilePaths,
                                             &nPaths);
    BAIL_ON_CENTERIS_ERROR(ceError);

    for (iPath = 0; iPath < nPaths; iPath++)
        printf("File path:%s\n", *(ppszFilePaths+iPath));

    if (nPaths == 0)
       printf("No paths were found\n");

error:

    if (ppszFilePaths)
       CTFreeStringArray(ppszFilePaths, nPaths);

    return ceError;
}
#endif

int
main(int argc, char* argv[])
{
    CENTERROR ceError = CENTERROR_SUCCESS;
 
#if TEST_CREATE_DIRECTORY
    ceError = DJCreateDirectory("/tmp/mydir", S_IRUSR);
    BAIL_ON_CENTERIS_ERROR(ceError);
#endif

#if TEST_GET_MATCHING_FILE_PATHS_IN_FOLDER
    ceError = TestGetMatchingFilePathsInFolder();
    BAIL_ON_CENTERIS_ERROR(ceError);
#endif
 
error:

    return(ceError);
}
