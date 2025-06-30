/***************************************************************************
Bourriquet MorphOS
 Copyright (C) 2024-2025 digitally

***************************************************************************/
#include <stdio.h>
#include <time.h>
#include <dos/dos.h>
#include <proto/intuition.h>
#include <intuition/classusr.h>
#include "SDI_compiler.h"
#include "Stringsizes.h"
#include "Extrasrc.h"

struct codeset;
struct TimeVal;

enum ReqFileType
{
 	ASL_ABOOK=0,
  	ASL_CONFIG,
  	ASL_DETACH,
  	ASL_ATTACH,
  	ASL_PHOTO,
  	ASL_IMPORT,
  	ASL_EXPORT,
  	ASL_FOLDER,
  	ASL_ABOOK_LDIF,
  	ASL_ABOOK_CSV,
  	ASL_ABOOK_TAB,
  	ASL_ABOOK_XML,
  	ASL_GENERIC,
  	ASL_UPDATE,
  	ASL_FILTER,
  	ASL_MAX
};

enum SizeFormat
{
  	SF_DEFAULT=0, // taille du format dans un vieux style 1,234,567 (bytes)
  	SF_MIXED,     // format dans un mode mixé  1.234 GB - 12.34 MB - 123.4 KB - 1234 B
  	SF_1PREC,     // format dans une précision de 1 1.2 GB - 12.3 MB - 123.4 KB - 1234 B
  	SF_2PREC,     // format dans une précision de 2  1.23 GB - 12.34 MB - 123.45 KB - 1234 B
  	SF_3PREC,     // format dans une précision de 3   1.234 GB - 12.345 MB - 123.456 KB - 1234 B
  	SF_AUTO       // format automatique par C->SizeFormat
};

struct FileReqCache
{
 	char *file;     // pointeur sur la chaîne du nom de fichier
  	char *drawer;   // pointeur sur la châine du répertoire
  	char *pattern;  // pointeur sur la chaîne de modéle
  	int  numArgs;   // si plusieurs fichiers ont été séléctionnés (>0)
  	char **argList; // liste de pointeurs d'arguments.
  	long left_edge; // dernière position du côté gauche du requester
  	long top_edge;  // dernière position supérieur du requester
  	long width;     // dernière largeur du requester
  	long height;    // dernière hauteur du requester
  	BOOL used;      // le cache est en utilisation
};

#ifndef MEMF_SHARED
	#define MEMF_SHARED MEMF_PUBLIC
#endif
#ifndef MEMF_PRIVATE
	#define MEMF_PRIVATE MEMF_PUBLIC
#endif
// define la valeur d'un BPTR invalide
#ifndef ZERO
	#define ZERO (BPTR)NULL
#endif
// calcul la décalage d'un élément au sein d'une structure
#ifndef OFFSET_OF
#define OFFSET_OF(s, o)    (IPTR)(&((s *)NULL)->o)
#endif
// macros 
#define INITLIB(lname, v, r, lbase, iname, iversion, ibase, req, url)  (InitLib((lname), (v), (r), (APTR)(lbase), (req), (url)))
#define CLOSELIB(lib, iface)                                           { if((lib)) { CloseLibrary((struct Library *)lib); lib = NULL; } }
#define NOERRORS            (1<<4)
#define KEEPLOG             (1<<5)
#define hasNoErrorsFlag(v)  (isFlagSet((v), NOERRORS))
#define hasKeepLogFlag(v)   (isFlagSet((v), KEEPLOG))
// RequesterMode drapeaux & macros
#define REQF_NONE             0
#define REQF_SAVEMODE         (1<<0)
#define REQF_MULTISELECT      (1<<1)
#define REQF_DRAWERSONLY      (1<<2)
#define REQF_NOICONS          (1<<3)
#define hasSaveModeFlag(v)    (isFlagSet((v), REQF_SAVEMODE))
#define hasMultiSelectFlag(v) (isFlagSet((v), REQF_MULTISELECT))
#define hasDrawersOnlyFlag(v) (isFlagSet((v), REQF_DRAWERSONLY))
#define hasNoIconsFlag(v)     (isFlagSet((v), REQF_NOICONS))

#ifndef MAX
#define MAX(a,b)              (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)              (((a) < (b)) ? (a) : (b))
#endif

#define ARRAY_SIZE(x)         (sizeof(x[0]) ? sizeof(x)/sizeof(x[0]) : 0)
#define VERSION_IS_AT_LEAST(ver, rev, minver, minrev) (((ver) > (minver)) || ((ver) == (minver) && (rev) == (minrev)) || ((ver) == (minver) && (rev) > (minrev)))
#define VERSION_IS_LOWER(ver, rev, maxver, maxrev) ((ver) < (maxver) || ((ver) == (maxver) && (rev) < (maxrev)))
#define LIB_VERSION_IS_AT_LEAST(lib, minver, minrev)  VERSION_IS_AT_LEAST(((struct Library *)(lib))->lib_Version, ((struct Library *)(lib))->lib_Revision, minver, minrev)
#define isFlagSet(v,f)        (((v) & (f)) == (f))  
#define isAnyFlagSet(v,f)     (((v) & (f)) != 0)    
#define isFlagClear(v,f)      (((v) & (f)) == 0)    
#define setFlag(v,f)          ((v) |= (f))          
#define clearFlag(v,f)        ((v) &= ~(f))     
#define maskFlag(v,f)         ((v) &= (f))     
#define DisposeModulePush(m)  DoMethod(G->App, MUIM_Application_PushMethod, G->App, 3, MUIM_CallHook, &DisposeModuleHook, m)
#define isSpace(c)            ((BOOL)(G->Locale ? (IsSpace(G->Locale, (ULONG)(c)) != 0) : (isspace((c)) != 0)))
#define isGraph(c)            ((BOOL)(G->Locale ? (IsGraph(G->Locale, (ULONG)(c)) != 0) : (isgraph((c)) != 0)))
#define isAlNum(c)            ((BOOL)(G->Locale ? (IsAlNum(G->Locale, (ULONG)(c)) != 0) : (isalnum((c)) != 0)))
#define Bool2Txt(b)           ((b) ? "Y" : "N")
#define Txt2Bool(t)           (BOOL)(toupper((int)*(t)) == 'Y' || (int)*(t) == '1')
#define SafeStr(str)          (((str) != NULL) ? (str) : "<NULL>")
#define IsStrEmpty(str)       ((str) == NULL || (str)[0] == '\0')
#define IterateList(list, type, node)           for((node) = (type)GetHead((struct List *)(list)); (node) != NULL; (node) = (type)GetSucc((struct Node *)(node)))
#define SafeIterateList(list, type, node, succ) for((node) = (type)GetHead((struct List *)(list)); (node) != NULL && (((succ) = (type)GetSucc((struct Node *)(node))) != NULL || (succ) == NULL); (node) = (succ))
#if !defined(IsMinListEmpty)
#define IsMinListEmpty(x) \
    ( (struct MinList *)((x)->mlh_TailPred) == (struct MinList *)(x) )
#endif

extern int BusyLevel;
extern struct Hook DisposeModuleHook;

char *AllocReqText(const char *s);
char *strippedCharsetName(const struct codeset* codeset);
BOOL CompareLists(const struct List *lh1, const struct List *lh2, BOOL (* compare)(const struct Node *, const struct Node *));
void DisposeModule(void *modptr);
void GetPubScreenName(const struct Screen *screen, char *pubName, ULONG pubNameSize);
char *FileToBuffer(const char *file, size_t *outsize);
char *AddPath(char *dst, const char *src, const char *add, const size_t size);
void FormatSize(LONG size, char *buffer, int buflen, enum SizeFormat forcedPrecision);
ULONG GetDateStamp(void);
BOOL GotoURLPossible(void);
BOOL GotoURL(const char *url, const BOOL newWindow);
struct FileReqCache *ReqFile(enum ReqFileType num, Object *win, const char *title, int mode, const char *drawer, const char *file);
void FreeFileReqCache(struct FileReqCache *frc);
BOOL SafeOpenWindow(Object *obj);
void ToLowerCase(char *str);
char *Trim(char *s);
char *TrimEnd(char *s);
char *TrimStart(const char *s);
char **SplitString(const char *str, const char *delimiters);
void FreeStrArray(char **array);
void MyBltMaskBitMapRastPort(struct BitMap *srcBitMap, LONG xSrc, LONG ySrc, struct RastPort *destRP, LONG xDest, LONG yDest, LONG xSize, LONG ySize, ULONG minterm, APTR bltMask);
char *IdentifyFile(char *fname);
 
