/*************************************************************
 Bourriquet MorphOS
 	2025-2025 digitally
 
 ***********************************************************/
#include <string.h>

#include <dos/dos.h>
#include <proto/dos.h>

#include "Utilities.h"
#include "FileInfo.h"
#include "Extrasrc.h"
#include "Debug.h"

#define FIB_IS_FILE(fib)     ((fib)->fib_DirEntryType < 0)
#define FIB_IS_DRAWER(fib)   ((fib)->fib_DirEntryType >= 0 && (fib)->fib_DirEntryType != ST_SOFTLINK && (fib)->fib_DirEntryType != ST_LINKDIR)

/// ObtainFileInfo
// query file <name> for the information <which>. If successful, the queried
// value is returned in the variable that <valuePtr> points to.
// Depending on <which> these variable types MUST be used for proper function:
//   FI_SIZE          LONG
//   FI_PROTECTION    ULONG
//   FI_COMMENT       char *
//   FI_DATE          struct DateStamp *
//   FI_TIME          ULONG
//   FI_TYPE          enum FileType
// File comments (FI_COMMENT) will be strdup()'ed and must be free()'d after usage to
// avoid memory leaks.
BOOL ObtainFileInfo(const char *name, enum FileInfo which, void *valuePtr)
{
	BOOL result = FALSE;
	BPTR lock;
	
    ENTER();
	*((ULONG *)valuePtr) = 0;
	D(DBF_UTIL, "Obtenir des informations sur le fichier '%s'", name);
    if((lock = Lock(name, ACCESS_READ)))
		{
			struct FileInfoBlock *fib;

			if((fib = AllocDosObject(DOS_FIB, NULL)) != NULL)
				{
					if(Examine(lock, fib))
						{
							switch(which)
								{
									case FI_SIZE:
										{
											*((ULONG *)valuePtr) = fib->fib_Size;
											result = TRUE;
										}
										break;
									case FI_PROTECTION:
										{
											*((ULONG *)valuePtr) = fib->fib_Protection;
											result = TRUE;
										}
										break;
									case FI_COMMENT:
										{
											if((*((char **)valuePtr) = strdup(fib->fib_Comment)) != NULL) result = TRUE;
										}
										break;
									case FI_DATE:
										{
											memcpy((struct DateStamp *)valuePtr, &fib->fib_Date, sizeof(struct DateStamp));
											result = TRUE;
										}
										break;
									case FI_TIME:
										{
											*((ULONG *)valuePtr) = ((fib->fib_Date.ds_Days + 2922) * 1440 + fib->fib_Date.ds_Minute) * 60 + fib->fib_Date.ds_Tick / TICKS_PER_SECOND;
											result = TRUE;
										}
										break;
									case FI_TYPE:
										{
											if(FIB_IS_FILE(fib)) *((ULONG *)valuePtr) = FIT_FILE;
											else if(FIB_IS_DRAWER(fib)) *((ULONG *)valuePtr) = FIT_DRAWER;
											else *((ULONG *)valuePtr) = FIT_UNKNOWN;
											result = TRUE;
										}
										break;
								}
						}
					FreeDosObject(DOS_FIB, fib);
				}
			UnLock(lock);
		}
	if(result == FALSE && which == FI_TYPE)
		{
			*((ULONG *)valuePtr) = FIT_NONEXIST;
			result = TRUE;
		}
	RETURN(result);
	return result;
}

///
/// FileExists
//  retourne true/false si le fichier/r�pertoire existe
BOOL FileExists(const char *filename)
{
	BOOL exists = FALSE;
	BPTR lock;

	ENTER();

	if(IsStrEmpty(filename) == FALSE && (lock = Lock(filename, ACCESS_READ)))
		{
			D(DBF_UTIL, "le fichier/r�pertoire '%s' existe", filename);
			exists = TRUE;
			UnLock(lock);
		}
	else D(DBF_UTIL, "le fichier/r�pertoire '%s' n'existe pas", SafeStr(filename));
	RETURN(exists);
	return exists;
}

///

