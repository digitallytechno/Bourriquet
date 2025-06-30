/***************************************************************************
Bourriquet MorphOS
 Copyright (C) 2022 digitally
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
***************************************************************************/
#include <stdlib.h>
#include <string.h>

#include <clib/alib_protos.h>
#include <mui/BetterString_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/NListview_mcc.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include "Extrasrc.h"
#include "SDI_hook.h"

#include "Bourriquet.h"
#include "Global.h"

#include "mui/ClassesExtra.h"

#include "Locale.h"
#include "MUIObjects.h"
#include "Debug.h"

/// MakeCycle
//  Creates a MUI cycle object
Object *MakeCycle(const char *const *labels, const char *label)
{
    return CycleObject,
           MUIA_CycleChain,    TRUE,
           MUIA_Font,          MUIV_Font_Button,
           MUIA_Cycle_Entries, labels,
           MUIA_ControlChar,   ShortCut(label),
           End;
}

///
/// MakeButton
//  Creates a MUI button
Object *MakeButton(const char *txt)
{
    Object *obj;

    if((obj = MUI_MakeObject(MUIO_Button,(IPTR)txt)) != NULL) set(obj, MUIA_CycleChain, TRUE);
    return(obj);
}

///
/// MakeCheck
//  Creates a MUI checkmark object
Object *MakeCheck(const char *label)
{
  return ImageObject,
           ImageButtonFrame,
           MUIA_InputMode   , MUIV_InputMode_Toggle,
           MUIA_Image_Spec  , MUII_CheckMark,
           MUIA_Background  , MUII_ButtonBack,
           MUIA_ShowSelState, FALSE,
           MUIA_ControlChar , ShortCut(label),
           MUIA_CycleChain  , TRUE,
         End;
}

///
/// MakeCheckGroup
//  Creates a labelled MUI checkmark object
Object *MakeCheckGroup(Object **check, const char *label)
{
  return HGroup,
           Child, *check = MakeCheck(label),
           Child, Label1(label),
           Child, HSpace(0),
         End;
}

///
/// MakeString
//  Creates a MUI string object
Object *MakeString(int maxlen, const char *label)
{
  return BetterStringObject,
           StringFrame,
           MUIA_String_MaxLen,      maxlen,
           MUIA_String_AdvanceOnCR, TRUE,
           MUIA_ControlChar,        ShortCut(label),
           MUIA_CycleChain,         TRUE,
         End;
}

///
/// MakePassString
//  Creates a MUI string object with hidden text
Object *MakePassString(const char *label)
{
  return BetterStringObject,
           StringFrame,
           MUIA_String_MaxLen,       SIZE_PASSWORD,
           MUIA_String_Secret,       TRUE,
           MUIA_String_AdvanceOnCR,  TRUE,
           MUIA_ControlChar,         ShortCut(label),
           MUIA_CycleChain,          TRUE,
         End;
}

///
/// MakeInteger
//  Creates a MUI string object for numeric input
Object *MakeInteger(int maxlen, const char *label)
{
  return BetterStringObject,
           StringFrame,
           MUIA_String_MaxLen,       maxlen+1,
           MUIA_String_AdvanceOnCR,  TRUE,
           MUIA_ControlChar,         ShortCut(label),
           MUIA_CycleChain,          TRUE,
           MUIA_String_Integer,      0,
           MUIA_String_Accept,       "0123456789",
         End;
}

///
/// MakeAddressField
//  Creates a recipient field
Object *MakeAddressField(Object **string, const char *label, const struct fcstr * const help, int abmode, int winnr, ULONG flags)
{
    Object *obj = NULL;

    ENTER();
    /*if((obj = AddressFieldObject,
              MUIA_AddressField_Flags, flags,
              MUIA_AddressField_Shortcut, ShortCut(label),
              MUIA_AddressField_Help, help,
              MUIA_AddressField_Mode, abmode,
              MUIA_AddressField_WindowNumber, winnr,
              End) != NULL)
      {
        *string = (Object *)xget(obj, MUIA_AddressField_RecipientString);
      }  */
    RETURN(obj);
    return obj;
}

///
/// MakeNumeric
//  Creates a MUI numeric slider
Object *MakeNumeric(int min, int max, BOOL percent)
{
  return NumericbuttonObject,
           MUIA_Numeric_Min, min,
           MUIA_Numeric_Max, max,
           MUIA_Numeric_Format, percent ? "%ld%%" : "%ld",
           MUIA_CycleChain, TRUE,
         End;
}

///
/// MakeCloseButton
// create a close button
Object *MakeCloseButton(void)
{
    Object *obj;

    ENTER();
    if(LIB_VERSION_IS_AT_LEAST(MUIMasterBase, 20, 5500))
      {
    // make use of MUI4's close button image
        obj = ImageObject,
      // Image.mui will kill frame and background if the image requires this
              ButtonFrame,
              MUIA_Background, MUII_ButtonBack,
              MUIA_Image_Spec, MUII_Close,
              MUIA_InputMode, MUIV_InputMode_RelVerify,
              MUIA_CycleChain, TRUE,
            End;
      }
    else
      {
    // create a simple button with a bold "X"
        obj = TextObject,
                ButtonFrame,
                MUIA_CycleChain,     TRUE,
                MUIA_Font,           MUIV_Font_Tiny,
                MUIA_InputMode,      MUIV_InputMode_RelVerify,
                MUIA_Background,     MUII_ButtonBack,
                MUIA_Text_SetMax,    TRUE,
                MUIA_Text_Copy,      FALSE,
                MUIA_Text_PreParse,  MUIX_B,
                MUIA_Text_Contents,  "X",
              End;
      }
    RETURN(obj);
    return(obj);
}

///
/// ShortCut
//  Finds keyboard shortcut in text label
char ShortCut(const char *label)
{
    char scut = '\0';
    char *ptr;

    ENTER();
    if(label != NULL && (ptr = strchr(label, '_')) != NULL) scut = (char)ToLower((ULONG)(*++ptr));
    RETURN(scut);
    return(scut);
}

///
/// FilereqStartFunc
//  Will be executed as soon as the user wants to popup a file requester
//  for selecting files
HOOKPROTONO(FilereqStartFunc, BOOL, struct TagItem *tags)
{
    Object *strObj = (Object *)hook->h_Data;
    char *str;

    ENTER();

    str = (char *)xget(strObj, MUIA_String_Contents);
    if(IsStrEmpty(str) == FALSE)
      {
        int i=0;
        static char buf[SIZE_PATHFILE];
        char *p;

    // make sure the string is unquoted.
        strlcpy(buf, str, sizeof(buf));
	//UnquoteString(buf, FALSE);

        if((p = PathPart(buf)))
          {
            static char drawer[SIZE_PATHFILE];

            strlcpy(drawer, buf, MIN(sizeof(drawer), (unsigned int)(p - buf + 1)));
            tags[i].ti_Tag = ASLFR_InitialDrawer;
            tags[i].ti_Data= (ULONG)drawer;
            i++;
          }
        tags[i].ti_Tag = ASLFR_InitialFile;
        tags[i].ti_Data = (ULONG)FilePart(buf);
        i++;
        tags[i].ti_Tag = TAG_DONE;
      }
    RETURN(TRUE);
    return(TRUE);
}
MakeHook(FilereqStartHook, FilereqStartFunc);

///
/// FilereqStopFunc
//  Will be executed as soon as the user selected a file
HOOKPROTONO(FilereqStopFunc, void, struct FileRequester *fileReq)
{
    Object *strObj = (Object *)hook->h_Data;

    ENTER();

  // check if a file was selected or not
    if(IsStrEmpty(fileReq->fr_File) == FALSE)
      {
        char buf[SIZE_PATHFILE];

	//AddPath(buf, fileReq->fr_Drawer, fileReq->fr_File, sizeof(buf));

    // check if there is any space in the path
        if(strchr(buf, ' ') != NULL)
          {
            int len = strlen(buf);

            memmove(&buf[1], buf, len+1);
            buf[0] = '"';
            buf[len+1] = '"';
            buf[len+2] = '\0';
          }
        set(strObj, MUIA_String_Contents, buf);
      }
    LEAVE();
}
MakeHook(FilereqStopHook, FilereqStopFunc);

///
/// PO_Window
// Window hook for popup objects
HOOKPROTONH(PO_Window, void, Object *pop, Object *win)
{
    ENTER();
    set(win, MUIA_Window_DefaultObject, pop);
    LEAVE();
}
MakeHook(PO_WindowHook, PO_Window);

///
/// GetMUIString
// copy the contents of a MUI string object to a string
void GetMUIString(char *s, Object *o, size_t len)
{
    char *c;

    ENTER();

    if((c = (char *)xget(o, MUIA_String_Contents)) != NULL) strlcpy(s, c, len);
    else
      {
        E(DBF_GUI, "Le contenu de la chaîne de l'objet %08lx est NULL", o);
        s[0] = '\0';
      }
    LEAVE();
}

///
/// GetMUIText
// copy the contents of a MUI text object to a string
void GetMUIText(char *s, Object *o, size_t len)
{
    char *c;

    ENTER();
    if((c = (char *)xget(o, MUIA_Text_Contents)) != NULL) strlcpy(s, c, len);
    else
      {
        E(DBF_GUI, "NULL text contents of object %08lx", o);
        s[0] = '\0';
      }
    LEAVE();
}

///
/// CreateScreenTitle
const char *CreateScreenTitle(char *dst, size_t dstlen, const char *text)
{
    char *result = NULL;
  
    ENTER();
    if(dst != NULL && dstlen > 0)
      {
        if(text != NULL) snprintf(dst, dstlen, "Bourriquet %s - %s", bourriquetver, text);
        else snprintf(dst, dstlen, "Bourriquet %s", bourriquetver);
        result = dst;
      }
    RETURN(result);
    return(result);
}

///
