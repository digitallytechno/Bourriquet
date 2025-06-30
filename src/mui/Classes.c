/***************************************************************************
Bourriquet MorphOS
Copyright (C) 2024-2025 digitally

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

/* Ce fichier est automatiquement généré avec GenClasses - NE PAS EDITER!!! */

#define INCLUDE_KITCHEN_SINK 1
#include "Classes.h"
#include "Debug.h"

struct MUI_CustomClass *BourriquetClasses[NUMBEROFCLASSES];

/*** Custom Class Dispatchers ***/
/// AboutWindowDispatcher()
SDISPATCHER(AboutWindowDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_AboutWindow_OM_NEW                        (cl, obj, msg);
    case OM_DISPOSE                              : return m_AboutWindow_OM_DISPOSE                    (cl, obj, msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// BourriquetApplicationDispatcher()
SDISPATCHER(BourriquetApplicationDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_BourriquetApplication_OM_NEW                        (cl, obj, msg);
    case OM_DISPOSE                              : return m_BourriquetApplication_OM_DISPOSE                    (cl, obj, msg);
    case OM_GET                                  : return m_BourriquetApplication_OM_GET                        (cl, obj, msg);
    case OM_SET                                  : return m_BourriquetApplication_OM_SET                        (cl, obj, msg);
    case MUIM_Application_PushMethod             : return m_BourriquetApplication_MUIM_Application_PushMethod   (cl, obj, msg);
    case MUIM_BourriquetApplication_ShowError    : return m_BourriquetApplication_ShowError                     (cl, obj, (APTR)msg);
    case MUIM_BourriquetApplication_ShowWarning  : return m_BourriquetApplication_ShowWarning                   (cl, obj, (APTR)msg);
    case MUIM_BourriquetApplication_DisposeWindow: return m_BourriquetApplication_DisposeWindow                 (cl, obj, (APTR)msg);
    case MUIM_BourriquetApplication_GotoURL      : return m_BourriquetApplication_GotoURL                       (cl, obj, (APTR)msg);
    case MUIM_BourriquetApplication_PopUp        : return m_BourriquetApplication_PopUp                         (cl, obj, (APTR)msg);
    case MUIM_BourriquetApplication_OpenAboutWindow: return m_BourriquetApplication_OpenAboutWindow               (cl, obj, (APTR)msg);
    case MUIM_BourriquetApplication_CloseAboutWindow: return m_BourriquetApplication_CloseAboutWindow              (cl, obj, (APTR)msg);
    case MUIM_BourriquetApplication_MUIRequestA  : return m_BourriquetApplication_MUIRequestA                   (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// ConfigPageDispatcher()
SDISPATCHER(ConfigPageDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_ConfigPage_OM_NEW                        (cl, obj, msg);
    case OM_GET                                  : return m_ConfigPage_OM_GET                        (cl, obj, msg);
    case MUIM_ConfigPage_ConfigToGUI             : return m_ConfigPage_ConfigToGUI                   (cl, obj, (APTR)msg);
    case MUIM_ConfigPage_GUIToConfig             : return m_ConfigPage_GUIToConfig                   (cl, obj, (APTR)msg);
    case MUIM_ConfigPage_ConfigUpdate            : return m_ConfigPage_ConfigUpdate                  (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// DownloadListDispatcher()
SDISPATCHER(DownloadListDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_DownloadList_OM_NEW                        (cl, obj, msg);
    case OM_DISPOSE                              : return m_DownloadList_OM_DISPOSE                    (cl, obj, msg);
    case OM_GET                                  : return m_DownloadList_OM_GET                        (cl, obj, msg);
    case MUIM_NList_Display                      : return m_DownloadList_MUIM_NList_Display            (cl, obj, msg);
    case MUIM_DownloadList_MakeFormat            : return m_DownloadList_MakeFormat                    (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// GenericRequestWindowDispatcher()
SDISPATCHER(GenericRequestWindowDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_GenericRequestWindow_OM_NEW                        (cl, obj, msg);
    case OM_DISPOSE                              : return m_GenericRequestWindow_OM_DISPOSE                    (cl, obj, msg);
    case OM_GET                                  : return m_GenericRequestWindow_OM_GET                        (cl, obj, msg);
    case MUIM_GenericRequestWindow_FinishInput   : return m_GenericRequestWindow_FinishInput                   (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// ImageAreaDispatcher()
SDISPATCHER(ImageAreaDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_ImageArea_OM_NEW                        (cl, obj, msg);
    case OM_DISPOSE                              : return m_ImageArea_OM_DISPOSE                    (cl, obj, msg);
    case OM_GET                                  : return m_ImageArea_OM_GET                        (cl, obj, msg);
    case OM_SET                                  : return m_ImageArea_OM_SET                        (cl, obj, msg);
    case MUIM_Setup                              : return m_ImageArea_MUIM_Setup                    (cl, obj, msg);
    case MUIM_Cleanup                            : return m_ImageArea_MUIM_Cleanup                  (cl, obj, msg);
    case MUIM_AskMinMax                          : return m_ImageArea_MUIM_AskMinMax                (cl, obj, msg);
    case MUIM_Draw                               : return m_ImageArea_MUIM_Draw                     (cl, obj, msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// InfosServeurDispatcher()
SDISPATCHER(InfosServeurDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_InfosServeur_OM_NEW                        (cl, obj, msg);
    case MUIM_NList_Display                      : return m_InfosServeur_MUIM_NList_Display            (cl, obj, msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// LogDispatcher()
SDISPATCHER(LogDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_Log_OM_NEW                        (cl, obj, msg);
    case MUIM_NList_Display                      : return m_Log_MUIM_NList_Display            (cl, obj, msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// MainWindowDispatcher()
SDISPATCHER(MainWindowDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_SET                                  : return m_MainWindow_OM_SET                        (cl, obj, msg);
    case MUIM_Window_Snapshot                    : return m_MainWindow_MUIM_Window_Snapshot          (cl, obj, msg);
    case MUIM_MainWindow_ShowErrors              : return m_MainWindow_ShowErrors                    (cl, obj, (APTR)msg);
    case MUIM_MainWindow_Relayout                : return m_MainWindow_Relayout                      (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// ResearchListDispatcher()
SDISPATCHER(ResearchListDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_ResearchList_OM_NEW                        (cl, obj, msg);
    case OM_DISPOSE                              : return m_ResearchList_OM_DISPOSE                    (cl, obj, msg);
    case OM_GET                                  : return m_ResearchList_OM_GET                        (cl, obj, msg);
    case MUIM_NList_Display                      : return m_ResearchList_MUIM_NList_Display            (cl, obj, msg);
    case MUIM_ResearchList_MakeFormat            : return m_ResearchList_MakeFormat                    (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// ResearchPageDispatcher()
SDISPATCHER(ResearchPageDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_ResearchPage_OM_NEW                        (cl, obj, msg);
    case OM_GET                                  : return m_ResearchPage_OM_GET                        (cl, obj, msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// ServerListDispatcher()
SDISPATCHER(ServerListDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_ServerList_OM_NEW                        (cl, obj, msg);
    case OM_DISPOSE                              : return m_ServerList_OM_DISPOSE                    (cl, obj, msg);
    case OM_GET                                  : return m_ServerList_OM_GET                        (cl, obj, msg);
    case MUIM_ContextMenuBuild                   : return m_ServerList_MUIM_ContextMenuBuild         (cl, obj, msg);
    case MUIM_NList_Compare                      : return m_ServerList_MUIM_NList_Compare            (cl, obj, msg);
    case MUIM_NList_Display                      : return m_ServerList_MUIM_NList_Display            (cl, obj, msg);
    case MUIM_ServerList_MakeFormat              : return m_ServerList_MakeFormat                    (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// ServerPageDispatcher()
SDISPATCHER(ServerPageDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_ServerPage_OM_NEW                        (cl, obj, msg);
    case OM_GET                                  : return m_ServerPage_OM_GET                        (cl, obj, msg);
    case MUIM_ServerPage_Update                  : return m_ServerPage_Update                        (cl, obj, (APTR)msg);
    case MUIM_ServerPage_SingleConnection        : return m_ServerPage_SingleConnection              (cl, obj, (APTR)msg);
    case MUIM_ServerPage_Entry                   : return m_ServerPage_Entry                         (cl, obj, (APTR)msg);
    case MUIM_ServerPage_AddServer               : return m_ServerPage_AddServer                     (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// SharedListDispatcher()
SDISPATCHER(SharedListDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_SharedList_OM_NEW                        (cl, obj, msg);
    case OM_DISPOSE                              : return m_SharedList_OM_DISPOSE                    (cl, obj, msg);
    case OM_GET                                  : return m_SharedList_OM_GET                        (cl, obj, msg);
    case MUIM_NList_Display                      : return m_SharedList_MUIM_NList_Display            (cl, obj, msg);
    case MUIM_SharedList_MakeFormat              : return m_SharedList_MakeFormat                    (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// SharedPageDispatcher()
SDISPATCHER(SharedPageDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_SharedPage_OM_NEW                        (cl, obj, msg);
    case OM_GET                                  : return m_SharedPage_OM_GET                        (cl, obj, msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// ShutdownWindowDispatcher()
SDISPATCHER(ShutdownWindowDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_ShutdownWindow_OM_NEW                        (cl, obj, msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// SplashWindowDispatcher()
SDISPATCHER(SplashWindowDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_SplashWindow_OM_NEW                        (cl, obj, msg);
    case OM_DISPOSE                              : return m_SplashWindow_OM_DISPOSE                    (cl, obj, msg);
    case MUIM_SplashWindow_StatusChange          : return m_SplashWindow_StatusChange                  (cl, obj, (APTR)msg);
    case MUIM_SplashWindow_ProgressChange        : return m_SplashWindow_ProgressChange                (cl, obj, (APTR)msg);
    case MUIM_SplashWindow_SelectUser            : return m_SplashWindow_SelectUser                    (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// StatusBarDispatcher()
SDISPATCHER(StatusBarDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_StatusBar_OM_NEW                        (cl, obj, msg);
    case OM_GET                                  : return m_StatusBar_OM_GET                        (cl, obj, msg);
    case MUIM_StatusBar_Update                   : return m_StatusBar_Update                        (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// ThemeListDispatcher()
SDISPATCHER(ThemeListDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_ThemeList_OM_NEW                        (cl, obj, msg);
    case MUIM_NList_Construct                    : return m_ThemeList_MUIM_NList_Construct          (cl, obj, msg);
    case MUIM_NList_Destruct                     : return m_ThemeList_MUIM_NList_Destruct           (cl, obj, msg);
    case MUIM_NList_Compare                      : return m_ThemeList_MUIM_NList_Compare            (cl, obj, msg);
    case MUIM_NList_Display                      : return m_ThemeList_MUIM_NList_Display            (cl, obj, msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// ThemeListGroupDispatcher()
SDISPATCHER(ThemeListGroupDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_ThemeListGroup_OM_NEW                        (cl, obj, msg);
    case MUIM_ThemeListGroup_Update              : return m_ThemeListGroup_Update                        (cl, obj, (APTR)msg);
    case MUIM_ThemeListGroup_SelectionChanged    : return m_ThemeListGroup_SelectionChanged              (cl, obj, (APTR)msg);
    case MUIM_ThemeListGroup_ActivateTheme       : return m_ThemeListGroup_ActivateTheme                 (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// TransfertPageDispatcher()
SDISPATCHER(TransfertPageDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_TransfertPage_OM_NEW                        (cl, obj, msg);
    case OM_GET                                  : return m_TransfertPage_OM_GET                        (cl, obj, msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///
/// UploadListDispatcher()
SDISPATCHER(UploadListDispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                                  : return m_UploadList_OM_NEW                        (cl, obj, msg);
    case OM_DISPOSE                              : return m_UploadList_OM_DISPOSE                    (cl, obj, msg);
    case OM_GET                                  : return m_UploadList_OM_GET                        (cl, obj, msg);
    case MUIM_NList_Display                      : return m_UploadList_MUIM_NList_Display            (cl, obj, msg);
    case MUIM_UploadList_MakeFormat              : return m_UploadList_MakeFormat                    (cl, obj, (APTR)msg);
  }
 
  return DoSuperMethodA(cl, obj, msg);
}

///

/*** Custom Class Support ***/
/// struct MCCInfo
const struct
{
  CONST_STRPTR Name;
  CONST_STRPTR SuperClass;
  LONG SuperMCCIndex;
  ULONG (*GetSize)(void);
  APTR Dispatcher;
} MCCInfo[NUMBEROFCLASSES] =
{
  { MUIC_AboutWindow, MUIC_Window, -1, AboutWindowGetSize, ENTRY(AboutWindowDispatcher) },
  { MUIC_BourriquetApplication, MUIC_Application, -1, BourriquetApplicationGetSize, ENTRY(BourriquetApplicationDispatcher) },
  { MUIC_ConfigPage, MUIC_Virtgroup, -1, ConfigPageGetSize, ENTRY(ConfigPageDispatcher) },
  { MUIC_DownloadList, MUIC_NList, -1, DownloadListGetSize, ENTRY(DownloadListDispatcher) },
  { MUIC_GenericRequestWindow, MUIC_Window, -1, GenericRequestWindowGetSize, ENTRY(GenericRequestWindowDispatcher) },
  { MUIC_ImageArea, MUIC_Area, -1, ImageAreaGetSize, ENTRY(ImageAreaDispatcher) },
  { MUIC_InfosServeur, MUIC_NList, -1, InfosServeurGetSize, ENTRY(InfosServeurDispatcher) },
  { MUIC_Log, MUIC_NList, -1, LogGetSize, ENTRY(LogDispatcher) },
  { MUIC_MainWindow, MUIC_Window, -1, NULL, ENTRY(MainWindowDispatcher) },
  { MUIC_ResearchList, MUIC_NList, -1, ResearchListGetSize, ENTRY(ResearchListDispatcher) },
  { MUIC_ResearchPage, MUIC_Virtgroup, -1, ResearchPageGetSize, ENTRY(ResearchPageDispatcher) },
  { MUIC_ServerList, MUIC_NList, -1, ServerListGetSize, ENTRY(ServerListDispatcher) },
  { MUIC_ServerPage, MUIC_Virtgroup, -1, ServerPageGetSize, ENTRY(ServerPageDispatcher) },
  { MUIC_SharedList, MUIC_NList, -1, SharedListGetSize, ENTRY(SharedListDispatcher) },
  { MUIC_SharedPage, MUIC_Virtgroup, -1, SharedPageGetSize, ENTRY(SharedPageDispatcher) },
  { MUIC_ShutdownWindow, MUIC_Window, -1, NULL, ENTRY(ShutdownWindowDispatcher) },
  { MUIC_SplashWindow, MUIC_Window, -1, SplashWindowGetSize, ENTRY(SplashWindowDispatcher) },
  { MUIC_StatusBar, MUIC_Group, -1, StatusBarGetSize, ENTRY(StatusBarDispatcher) },
  { MUIC_ThemeList, MUIC_NList, -1, NULL, ENTRY(ThemeListDispatcher) },
  { MUIC_ThemeListGroup, MUIC_Group, -1, ThemeListGroupGetSize, ENTRY(ThemeListGroupDispatcher) },
  { MUIC_TransfertPage, MUIC_Virtgroup, -1, TransfertPageGetSize, ENTRY(TransfertPageDispatcher) },
  { MUIC_UploadList, MUIC_NList, -1, UploadListGetSize, ENTRY(UploadListDispatcher) },
};

///
/// Bourriquet_NewObject()
Object * Bourriquet_NewObject(CONST_STRPTR className, ...)
{
  Object *obj = NULL;
  unsigned int i;

  ENTER();

  for(i = 0; i < NUMBEROFCLASSES; i++)
  {
    if(strcmp(MCCInfo[i].Name, className) == 0)
    {
      unsigned int j;
      struct TagItem tags[128];
      va_list args;

      va_start(args, className);
      for(j=0; j < (sizeof(tags) / sizeof(tags[0])); j++)
      {
        tags[j].ti_Tag = va_arg(args, ULONG);
        if(tags[j].ti_Tag != TAG_DONE)
          tags[j].ti_Data = va_arg(args, ULONG);
        else
        {
          tags[j].ti_Data = 0;
          break;
        }
      }

      #if defined(DEBUG)
      if(j >= (sizeof(tags) / sizeof(tags[0])))
      {
        E(DBF_ALWAYS, "FATAL ERROR: size of tags[%ld] array exhausted or no TAG_DONE in Bourriquet_NewObject(%s, ...) call!!!!", (sizeof(tags) / sizeof(tags[0])), className);
        ASSERT(j < (sizeof(tags) / sizeof(tags[0])));
      }
      #endif

      obj = NewObjectA(BourriquetClasses[i]->mcc_Class, NULL, (struct TagItem *)&tags);

      va_end(args);

      break;
    }
  }

  RETURN(obj);
  return obj;
}

///
/// Bourriquet_SetupClasses()
BOOL Bourriquet_SetupClasses(const char **failClass, const char **failSuperClass)
{
  BOOL success = TRUE;
  unsigned int i;

  ENTER();

  *failClass = NULL;
  *failSuperClass = NULL;

  memset(BourriquetClasses, 0, sizeof(BourriquetClasses));
  for (i = 0; i < NUMBEROFCLASSES; i++)
  {
    const char *superClassName;
    struct MUI_CustomClass *superMCC;
    ULONG classdataSize;

    if(MCCInfo[i].SuperMCCIndex == -1)
    {
      superClassName = MCCInfo[i].SuperClass;
      superMCC = NULL;
    }
    else
    {
      superClassName = NULL;
      superMCC = BourriquetClasses[MCCInfo[i].SuperMCCIndex];
      if(superMCC == NULL)
      {
        E(DBF_STARTUP, "La superclasse '%s' de la classe '%s' n'est pas encore créée!", MCCInfo[i].SuperClass, MCCInfo[i].Name);
        *failClass = MCCInfo[i].Name;
        *failSuperClass = MCCInfo[i].SuperClass;
        success = FALSE;
        break;
      }
    }

    if(MCCInfo[i].GetSize != NULL)
      classdataSize = MCCInfo[i].GetSize();
    else
      classdataSize = 0;

    D(DBF_STARTUP, "Création de la classe '%s' comme sous-classe de '%s'", MCCInfo[i].Name, MCCInfo[i].SuperClass);
    BourriquetClasses[i] = MUI_CreateCustomClass(NULL, superClassName, superMCC, classdataSize, MCCInfo[i].Dispatcher);
    if(BourriquetClasses[i] == NULL)
    {
      E(DBF_STARTUP, "Impossible de créer la classe '%s' comme sous-classe de '%s'", MCCInfo[i].Name, MCCInfo[i].SuperClass);
      *failClass = MCCInfo[i].Name;
      *failSuperClass = MCCInfo[i].SuperClass;
      success = FALSE;
      break;
    }
    else
    {
      if(LIB_VERSION_IS_AT_LEAST(MUIMasterBase, 20, 0) == TRUE)
        BourriquetClasses[i]->mcc_Class->cl_ID = (STRPTR)MCCInfo[i].Name;
    }
  }

  if(success == FALSE)
    Bourriquet_CleanupClasses();

  RETURN(success);
  return success;
}

///
/// Bourriquet_CleanupClasses()
void Bourriquet_CleanupClasses(void)
{
  int i;

  ENTER();

  for(i = NUMBEROFCLASSES-1; i >= 0; i--)
  {
    if(BourriquetClasses[i] != NULL)
    {
      if(BourriquetClasses[i]->mcc_Class->cl_ObjectCount != 0)
        E(DBF_STARTUP, "La classe '%s' a toujours %ld objet(s) en vie!", MCCInfo[i].Name, BourriquetClasses[i]->mcc_Class->cl_ObjectCount);

      MUI_DeleteCustomClass(BourriquetClasses[i]);
      BourriquetClasses[i] = NULL;
    }
  }

  LEAVE();
}

///

