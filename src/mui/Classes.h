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

#ifndef CLASSES_CLASSES_H
#define CLASSES_CLASSES_H

/* Ce fichier est automatiquement généré avec GenClasses - NE PAS EDITER!!! */

#include "ClassesExtra.h"
#include "ClassesSetup.h"

#define NUMBEROFCLASSES 22

extern struct MUI_CustomClass *BourriquetClasses[NUMBEROFCLASSES];

#include "AboutWindow.h"
ULONG AboutWindowGetSize(void);
ULONG m_AboutWindow_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_AboutWindow_OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg);

#include "BourriquetApplication.h"
ULONG BourriquetApplicationGetSize(void);
ULONG m_BourriquetApplication_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_BourriquetApplication_OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg);
ULONG m_BourriquetApplication_OM_GET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_BourriquetApplication_OM_SET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_BourriquetApplication_MUIM_Application_PushMethod(struct IClass *cl, Object *obj, Msg msg);
ULONG m_BourriquetApplication_ShowError(struct IClass *cl, Object *obj, struct MUIP_BourriquetApplication_ShowError *msg);
ULONG m_BourriquetApplication_ShowWarning(struct IClass *cl, Object *obj, struct MUIP_BourriquetApplication_ShowWarning *msg);
ULONG m_BourriquetApplication_DisposeWindow(struct IClass *cl, Object *obj, struct MUIP_BourriquetApplication_DisposeWindow *msg);
ULONG m_BourriquetApplication_GotoURL(struct IClass *cl, Object *obj, struct MUIP_BourriquetApplication_GotoURL *msg);
ULONG m_BourriquetApplication_PopUp(struct IClass *cl, Object *obj, struct MUIP_BourriquetApplication_PopUp *msg);
ULONG m_BourriquetApplication_OpenAboutWindow(struct IClass *cl, Object *obj, struct MUIP_BourriquetApplication_OpenAboutWindow *msg);
ULONG m_BourriquetApplication_CloseAboutWindow(struct IClass *cl, Object *obj, struct MUIP_BourriquetApplication_CloseAboutWindow *msg);
ULONG m_BourriquetApplication_MUIRequestA(struct IClass *cl, Object *obj, struct MUIP_BourriquetApplication_MUIRequestA *msg);

#include "ConfigPage.h"
ULONG ConfigPageGetSize(void);
ULONG m_ConfigPage_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ConfigPage_OM_GET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ConfigPage_ConfigToGUI(struct IClass *cl, Object *obj, struct MUIP_ConfigPage_ConfigToGUI *msg);
ULONG m_ConfigPage_GUIToConfig(struct IClass *cl, Object *obj, struct MUIP_ConfigPage_GUIToConfig *msg);
ULONG m_ConfigPage_ConfigUpdate(struct IClass *cl, Object *obj, struct MUIP_ConfigPage_ConfigUpdate *msg);

#include "DownloadList.h"
ULONG DownloadListGetSize(void);
ULONG m_DownloadList_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_DownloadList_OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg);
ULONG m_DownloadList_OM_GET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_DownloadList_MUIM_NList_Display(struct IClass *cl, Object *obj, Msg msg);
ULONG m_DownloadList_MakeFormat(struct IClass *cl, Object *obj, struct MUIP_DownloadList_MakeFormat *msg);

#include "GenericRequestWindow.h"
ULONG GenericRequestWindowGetSize(void);
ULONG m_GenericRequestWindow_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_GenericRequestWindow_OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg);
ULONG m_GenericRequestWindow_OM_GET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_GenericRequestWindow_FinishInput(struct IClass *cl, Object *obj, struct MUIP_GenericRequestWindow_FinishInput *msg);

#include "ImageArea.h"
ULONG ImageAreaGetSize(void);
ULONG m_ImageArea_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ImageArea_OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ImageArea_OM_GET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ImageArea_OM_SET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ImageArea_MUIM_Setup(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ImageArea_MUIM_Cleanup(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ImageArea_MUIM_AskMinMax(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ImageArea_MUIM_Draw(struct IClass *cl, Object *obj, Msg msg);

#include "InfosServeur.h"
ULONG InfosServeurGetSize(void);
ULONG m_InfosServeur_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_InfosServeur_MUIM_NList_Display(struct IClass *cl, Object *obj, Msg msg);

#include "Log.h"
ULONG LogGetSize(void);
ULONG m_Log_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_Log_MUIM_NList_Display(struct IClass *cl, Object *obj, Msg msg);

#include "MainWindow.h"
ULONG m_MainWindow_OM_SET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_MainWindow_MUIM_Window_Snapshot(struct IClass *cl, Object *obj, Msg msg);
ULONG m_MainWindow_ShowErrors(struct IClass *cl, Object *obj, struct MUIP_MainWindow_ShowErrors *msg);
ULONG m_MainWindow_Relayout(struct IClass *cl, Object *obj, struct MUIP_MainWindow_Relayout *msg);

#include "ResearchList.h"
ULONG ResearchListGetSize(void);
ULONG m_ResearchList_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ResearchList_OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ResearchList_OM_GET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ResearchList_MUIM_NList_Display(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ResearchList_MakeFormat(struct IClass *cl, Object *obj, struct MUIP_ResearchList_MakeFormat *msg);

#include "ResearchPage.h"
ULONG ResearchPageGetSize(void);
ULONG m_ResearchPage_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ResearchPage_OM_GET(struct IClass *cl, Object *obj, Msg msg);

#include "ServerList.h"
ULONG ServerListGetSize(void);
ULONG m_ServerList_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ServerList_OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ServerList_OM_GET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ServerList_MUIM_ContextMenuBuild(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ServerList_MUIM_NList_Compare(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ServerList_MUIM_NList_Display(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ServerList_MakeFormat(struct IClass *cl, Object *obj, struct MUIP_ServerList_MakeFormat *msg);

#include "ServerPage.h"
ULONG ServerPageGetSize(void);
ULONG m_ServerPage_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ServerPage_OM_GET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ServerPage_Update(struct IClass *cl, Object *obj, struct MUIP_ServerPage_Update *msg);
ULONG m_ServerPage_SingleConnection(struct IClass *cl, Object *obj, struct MUIP_ServerPage_SingleConnection *msg);
ULONG m_ServerPage_Entry(struct IClass *cl, Object *obj, struct MUIP_ServerPage_Entry *msg);
ULONG m_ServerPage_AddServer(struct IClass *cl, Object *obj, struct MUIP_ServerPage_AddServer *msg);

#include "SharedList.h"
ULONG SharedListGetSize(void);
ULONG m_SharedList_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_SharedList_OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg);
ULONG m_SharedList_OM_GET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_SharedList_MUIM_NList_Display(struct IClass *cl, Object *obj, Msg msg);
ULONG m_SharedList_MakeFormat(struct IClass *cl, Object *obj, struct MUIP_SharedList_MakeFormat *msg);

#include "SharedPage.h"
ULONG SharedPageGetSize(void);
ULONG m_SharedPage_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_SharedPage_OM_GET(struct IClass *cl, Object *obj, Msg msg);

#include "ShutdownWindow.h"
ULONG m_ShutdownWindow_OM_NEW(struct IClass *cl, Object *obj, Msg msg);

#include "SplashWindow.h"
ULONG SplashWindowGetSize(void);
ULONG m_SplashWindow_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_SplashWindow_OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg);
ULONG m_SplashWindow_StatusChange(struct IClass *cl, Object *obj, struct MUIP_SplashWindow_StatusChange *msg);
ULONG m_SplashWindow_ProgressChange(struct IClass *cl, Object *obj, struct MUIP_SplashWindow_ProgressChange *msg);
ULONG m_SplashWindow_SelectUser(struct IClass *cl, Object *obj, struct MUIP_SplashWindow_SelectUser *msg);

#include "StatusBar.h"
ULONG StatusBarGetSize(void);
ULONG m_StatusBar_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_StatusBar_OM_GET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_StatusBar_Update(struct IClass *cl, Object *obj, struct MUIP_StatusBar_Update *msg);

#include "ThemeList.h"
ULONG m_ThemeList_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ThemeList_MUIM_NList_Construct(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ThemeList_MUIM_NList_Destruct(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ThemeList_MUIM_NList_Compare(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ThemeList_MUIM_NList_Display(struct IClass *cl, Object *obj, Msg msg);

#include "ThemeListGroup.h"
ULONG ThemeListGroupGetSize(void);
ULONG m_ThemeListGroup_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_ThemeListGroup_Update(struct IClass *cl, Object *obj, struct MUIP_ThemeListGroup_Update *msg);
ULONG m_ThemeListGroup_SelectionChanged(struct IClass *cl, Object *obj, struct MUIP_ThemeListGroup_SelectionChanged *msg);
ULONG m_ThemeListGroup_ActivateTheme(struct IClass *cl, Object *obj, struct MUIP_ThemeListGroup_ActivateTheme *msg);

#include "TransfertPage.h"
ULONG TransfertPageGetSize(void);
ULONG m_TransfertPage_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_TransfertPage_OM_GET(struct IClass *cl, Object *obj, Msg msg);

#include "UploadList.h"
ULONG UploadListGetSize(void);
ULONG m_UploadList_OM_NEW(struct IClass *cl, Object *obj, Msg msg);
ULONG m_UploadList_OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg);
ULONG m_UploadList_OM_GET(struct IClass *cl, Object *obj, Msg msg);
ULONG m_UploadList_MUIM_NList_Display(struct IClass *cl, Object *obj, Msg msg);
ULONG m_UploadList_MakeFormat(struct IClass *cl, Object *obj, struct MUIP_UploadList_MakeFormat *msg);


#endif /* CLASSES_CLASSES_H */

