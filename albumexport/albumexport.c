#include <Windows.h>
#include "include/iTunes/Interface.h"
#include "include/iTunes/Interface_i.c"
#include "albumasset.h"
#include <PathCch.h>
#include <strsafe.h>
#include <Shlwapi.h>

#pragma comment( lib, "PathCch.lib" )
#pragma comment(lib, "Shlwapi.lib")


WCHAR   szDiscordStatusCpp[8192] = { 0 };
SIZE_T  nBufferPosition = 0;

static VOID ValidateAlbumName( BSTR pszSource, LPWSTR lpszBuffer, SIZE_T cchBuffer )
{
    for ( SIZE_T cOriginal = 0, cBuffer = 0; cOriginal < wcslen( pszSource ) && cBuffer < cchBuffer; ++cOriginal, ++cBuffer )
    {
        WCHAR Character = pszSource[cOriginal];

        /* Character is not allowed */
        if ( FALSE == AssetCharacterAllowed( Character ) )
        {
            /* Replace character with underscore */
            lpszBuffer[cBuffer] = L'_';
            if ( Character == L':' && pszSource[cOriginal + 1] == L' ' )
            {
                /* Add extra underscore if next character is space */
                lpszBuffer[++cBuffer] = L'_';
            }
            else if ( Character == L' ' && pszSource[cOriginal + 1] == L' ' )
            {
                continue;
            }

            /* Search until valid character is found */
            ++cOriginal;
            while ( FALSE == AssetCharacterAllowed( pszSource[cOriginal] ) )
            {
                ++cOriginal;
                if ( cOriginal >= wcslen( pszSource ) )
                {
                    break;
                }
            }

            --cOriginal;
            continue;
        }

        /* Discord automatically sets all asset-name characters to lowercase */
        lpszBuffer[cBuffer] = towlower( Character );
    }
}

static VOID WriteDiscordStatusCpp( LPCWSTR lpszArrayName )
{
    LPWSTR lpszBuffer = szDiscordStatusCpp + nBufferPosition;

    StringCchPrintf( lpszBuffer,
        ARRAYSIZE( szDiscordStatusCpp ) - nBufferPosition,
        L"\t\tfor ( int i = 0; i < ARRAYSIZE( %s ) && lpszApplicationID == NULL; ++i )\n"
        L"\t\t{\n"
        L"\t\t\tif (0 == strcmp( %s[i], Album ) ) {\n"
        L"\t\t\t\t/* TODO : REPLACE COMMENT WITH CORRESPONDING APPLICATION ID */\n"
        L"\t\t\t}\n"
        L"\t\t}\n",
        lpszArrayName,
        lpszArrayName
    );
    nBufferPosition = wcslen( szDiscordStatusCpp );
}

int main()
{
    HRESULT             hResult;
    IiTunes             *pITunes;
    IITLibraryPlaylist *pLibrary;
    IITTrackCollection *pTracks;
    LONG                nTracks;
    HANDLE              hOutputFile;
    LONG                nCount;

    CopyMemory( szDiscordStatusCpp, g_lpszDiscordStatusCppBegin, ( wcslen( g_lpszDiscordStatusCppBegin ) + 1 ) * sizeof( WCHAR ) );
    nBufferPosition = wcslen( szDiscordStatusCpp );

    CoInitialize( NULL );

    /* Connect to iTunes.exe */
    hResult = CoCreateInstance( &CLSID_iTunesApp, NULL, CLSCTX_ALL, &IID_IiTunes, &pITunes );
    if ( hResult != S_OK )
    {
        return 1;
    }

    // Get all tracks in the iTunes library
    pITunes->lpVtbl->get_LibraryPlaylist( pITunes, &pLibrary );
    pLibrary->lpVtbl->get_Tracks( pLibrary, &pTracks );

    pTracks->lpVtbl->get_Count( pTracks, &nTracks );

    DeleteFile( L"new_AlbumAssets.h" );
    DeleteFile( L"new_DiscordStatus.cpp" );

    hOutputFile = CreateFile( L"new_AlbumAssets.h",
        FILE_APPEND_DATA,
        0, 
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, 
        NULL 
    );
    WriteFile( hOutputFile, L"#pragma once\n\n", 14 * sizeof( WCHAR ), NULL, NULL );
    WriteFile( hOutputFile,
        L"/* \n"
        L"* TODO : Create definitions(#define) for each discord appliction\n"
        L"* RPC_ASSET{index}_APPLICATION_ID \"{APPLICATION_ID}\n"
        L"*/\n\n",
        125 * sizeof( WCHAR ),
        NULL,
        NULL
    );

    printf( "Exporting library albums -- May take up to a minute or more; depending on number of albums found in iTunes library \n" );

    nCount = 0;
    for ( long iTrack = 1; iTrack < nTracks; ++nCount )
    {
        WCHAR szFolderName[MAX_PATH];
        WCHAR szFileName[MAX_PATH];
        WCHAR szDestFile[MAX_PATH];
        WCHAR szArrayName[100];
        WCHAR szBeginData[100];

        GetModuleFileName( NULL, szFolderName, ARRAYSIZE( szFolderName ) );
        PathCchRemoveFileSpec( szFolderName, ARRAYSIZE( szFolderName ) );
        PathCchAddBackslash( szFolderName, ARRAYSIZE( szFolderName ) );

        StringCbPrintf( szFileName, sizeof( szFileName ), L"Apple Music %i", nCount + 1 );
        PathCchAppend( szFolderName, ARRAYSIZE( szFolderName ), szFileName );

        /* Delete all files in the folder, if it exists */
        if ( PathIsDirectory( szFolderName ) )
        {
            HANDLE          hFind;
            WIN32_FIND_DATA wfData;
            WCHAR           szFindPath[MAX_PATH];

            StringCbPrintf( szFindPath, sizeof( szFileName ), L"%s\\*", szFolderName );

            hFind = FindFirstFile( szFindPath, &wfData );
            if ( hFind != INVALID_HANDLE_VALUE )
            {
                do
                {
                    WCHAR szFileFullPath[MAX_PATH];

                    if ( 0 != wcscmp( L".", wfData.cFileName ) && 0 != wcscmp( L"..", wfData.cFileName ) )
                    {
                        StringCbPrintf( szFileFullPath, sizeof( szFileFullPath ), L"%s\\%s", szFolderName, wfData.cFileName );
                        DeleteFile( szFileFullPath );
                    }
                } while ( FindNextFile( hFind, &wfData ) );

                FindClose( hFind );
            }
        }

        CreateDirectory( szFolderName, NULL );

        // Declare the variable to the txt file
        StringCbPrintf( szArrayName, sizeof( szArrayName ), L"rgszAlbumNames_%i", nCount + 1 );
        StringCbPrintf( szBeginData,
            sizeof( szBeginData ),
            L"const LPCSTR %s[] = {\n",
            szArrayName,
            nCount + 1
        );
        WriteFile( hOutputFile, szBeginData, wcslen( szBeginData ) * sizeof( WCHAR ), NULL, NULL );

        for ( int nAlbumCount = 0; nAlbumCount < 300 && iTrack <= nTracks; ++iTrack )
        {
            IITTrack                *pCurrentTrack;
            BSTR                    pszAlbumName;
            IITArtworkCollection    *pArtworkCollection;
            IITArtwork              *pArtwork;
            WCHAR                   szOutputFileName[MAX_PATH];
            WCHAR                   szOutputFilePath[MAX_PATH];
            WCHAR                   szAppendData[100];
            BSTR                    pszOutputFile;
            BOOL                    bExists;

            ZeroMemory( szOutputFileName, sizeof( szOutputFileName ) );
            ZeroMemory( szOutputFilePath, sizeof( szOutputFilePath ) );

            /* Get the current song/track in the library */
            pTracks->lpVtbl->get_Item( pTracks, iTrack, &pCurrentTrack );
            if ( !pCurrentTrack )
            {
                break;
            }

            /* Get the associated album */
            pCurrentTrack->lpVtbl->get_Album( pCurrentTrack, &pszAlbumName );
            if ( !pszAlbumName )
            {
                continue;
            }

            ValidateAlbumName( pszAlbumName, szOutputFileName, ARRAYSIZE( szOutputFileName ) );
            StringCbPrintf( szOutputFilePath, sizeof( szOutputFilePath ), L"%s\\%s.jpeg", szFolderName, szOutputFileName );

            /* Make sure this album hasn't already been exported */
            bExists = PathFileExists( szOutputFilePath );
            for ( int i = 0; i < nCount && bExists == FALSE; ++i )
            {
                WCHAR szAlbumFile[MAX_PATH];

                StringCbCopy( szAlbumFile, sizeof( szAlbumFile ), szFolderName );
                PathCchRemoveFileSpec( szAlbumFile, ARRAYSIZE( szAlbumFile ) );
                StringCbPrintf( szAlbumFile + wcslen( szAlbumFile ), sizeof( szAlbumFile ), L"\\Apple Music %i\\%s.jpeg", i + 1, szOutputFileName );

                if ( PathFileExists( szAlbumFile ) )
                {
                    bExists = TRUE;
                    break;
                }
            }
            if ( bExists )
            {
                SysFreeString( pszAlbumName );
                pCurrentTrack->lpVtbl->Release( pCurrentTrack );
                continue;
            }

            /* Get the album artwork */
            pCurrentTrack->lpVtbl->get_Artwork( pCurrentTrack, &pArtworkCollection );
            if ( !pArtworkCollection )
            {
                pCurrentTrack->lpVtbl->Release( pCurrentTrack );
                continue;
            }

            pArtworkCollection->lpVtbl->get_Item( pArtworkCollection, 1, &pArtwork );
            if ( !pArtwork )
            {
                pArtworkCollection->lpVtbl->Release( pArtworkCollection );
                pCurrentTrack->lpVtbl->Release( pCurrentTrack );
                continue;
            }

            pszOutputFile = SysAllocString( szOutputFilePath );

            /* Save the artwork to file */
            hResult = pArtwork->lpVtbl->SaveArtworkToFile( pArtwork, pszOutputFile );
            if ( hResult == S_OK )
            {
                WCHAR szCopy[256] = { 0 };
                LPSTR pBuffer;

                for ( SIZE_T i = 0, c = 0; i < wcslen( pszAlbumName ); ++i, ++c )
                {
                    if ( pszAlbumName[i] == L'\"' )
                    {
                        szCopy[c++] = L'\\';
                    }
                    szCopy[c] = pszAlbumName[i];
                }

                ++nAlbumCount;
                if ( nAlbumCount == 300 || iTrack == nTracks ) {
                    StringCbPrintf( szAppendData, sizeof( szAppendData ), L"    \"%s\"\n", szCopy );
                }
                else {
                    StringCbPrintf( szAppendData, sizeof( szAppendData ), L"    \"%s\",\n", szCopy );
                }

                WriteFile( hOutputFile, szAppendData, wcslen( szAppendData ) * sizeof( WCHAR ), NULL, NULL );
            }

            SysFreeString( pszOutputFile );
            pArtwork->lpVtbl->Release( pArtwork );
            pArtworkCollection->lpVtbl->Release( pArtworkCollection );
            SysFreeString( pszAlbumName );
            pCurrentTrack->lpVtbl->Release( pCurrentTrack );
        }

        WriteFile( hOutputFile, L"};\n\n", wcslen( L"};\n\n" ) * sizeof( WCHAR ), NULL, NULL );
        WriteDiscordStatusCpp( szArrayName );
    }

    SetEndOfFile( hOutputFile );
    CloseHandle( hOutputFile );
    pTracks->lpVtbl->Release( pTracks );
    pLibrary->lpVtbl->Release( pLibrary );
    pITunes->lpVtbl->Release( pITunes );

    for ( SIZE_T i = 0; i < wcslen( g_lpszDiscordStatusCppEnd ); ++i, ++nBufferPosition )
    {
        szDiscordStatusCpp[nBufferPosition] = g_lpszDiscordStatusCppEnd[i];
    }

    hOutputFile = CreateFile( L"new_DiscordStatus.cpp", GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    WriteFile( hOutputFile, szDiscordStatusCpp, wcslen( szDiscordStatusCpp ) * sizeof( WCHAR ), NULL, NULL );
    CloseHandle( hOutputFile );

    system( "cls" );
    printf( "Number of Discord Applications Needed: %i\n", nCount );
    printf( "Press enter to exit\n" );
    getchar();

    return 1;
}
