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
#ifndef LOCALE_H
#define LOCALE_H

// flexcat string structure
struct fcstr
	{
  		const char * const defmsg; // la cha�ne du catalogue par d�faut
  		const char * msg;          // la cha�ne traduite/convertie du catalogue
  		const long id;             // identifiant cha�ne du catalogue
	};

// fonctions catalogue init/open/close
extern void InitCatalogue(void);
extern BOOL OpenCatalogue(void);
extern void CloseCatalogue(void);

// define to retrieve the translated catalog string
#define tr(fc)    ((fc)->msg)

// ensemble des phrases de traduction du catalogue
extern struct fcstr Bourriquet_Strings[201];

// MSG_XXXX pointantsur les phrases correspondantes
#define MSG_MA_PROJECT (&Bourriquet_Strings[0])
#define MSG_MA_ABOUT (&Bourriquet_Strings[1])
#define MSG_MA_ABOUT_MUI (&Bourriquet_Strings[2])
#define MSG_PROJECT_ABOUT (&Bourriquet_Strings[3])
#define MSG_SETTINGS_MUI (&Bourriquet_Strings[4])
#define MSG_HIDE (&Bourriquet_Strings[5])
#define MSG_QUIT (&Bourriquet_Strings[6])
#define MSG_COPYRIGHT1 (&Bourriquet_Strings[7])
#define MSG_VERSION (&Bourriquet_Strings[8])
#define MSG_COMPILATION_DATE (&Bourriquet_Strings[9])
#define MSG_COPYRIGHT2 (&Bourriquet_Strings[10])
#define MSG_USED_SOFTWARE (&Bourriquet_Strings[11])
#define MSG_APPLICATION_DESCRIPTION (&Bourriquet_Strings[12])
#define MSG_OK (&Bourriquet_Strings[13])
#define MSG_CANCEL (&Bourriquet_Strings[14])
#define MSG_YES (&Bourriquet_Strings[15])
#define MSG_NO (&Bourriquet_Strings[16])
#define MSG_CREATING_GUI (&Bourriquet_Strings[17])
#define MSG_INIT_LIBS (&Bourriquet_Strings[18])
#define MSG_INIT_TIMER (&Bourriquet_Strings[19])
#define MSG_LOADING_GFX (&Bourriquet_Strings[20])
#define MSG_LOADING_CONFIG (&Bourriquet_Strings[21])
#define MSG_NO_MCC (&Bourriquet_Strings[22])
#define MSG_MCC_OLD (&Bourriquet_Strings[23])
#define MSG_MCC_IN_USE (&Bourriquet_Strings[24])
#define MSG_RETRY_QUIT_GAD (&Bourriquet_Strings[25])
#define MSG_OPENING_GUI (&Bourriquet_Strings[26])
#define MSG_QuitANGReq (&Bourriquet_Strings[27])
#define MSG_CONFIRM_REQ (&Bourriquet_Strings[28])
#define MSG_YES_NO_REQ (&Bourriquet_Strings[29])
#define MSG_WINDOW_TITLE (&Bourriquet_Strings[30])
#define MSG_BT_QUIT (&Bourriquet_Strings[31])
#define MSG_ERROR_STARTUP (&Bourriquet_Strings[32])
#define MSG_ERROR_LIB (&Bourriquet_Strings[33])
#define MSG_ERROR_MUI_APPLICATION (&Bourriquet_Strings[34])
#define MSG_ERROR_CLASSES (&Bourriquet_Strings[35])
#define MSG_ERROR_TIMER (&Bourriquet_Strings[36])
#define MSG_ERROR_TASK (&Bourriquet_Strings[37])
#define MSG_ERROR_SIGNAL (&Bourriquet_Strings[38])
#define MSG_ERROR_METHODSTASK (&Bourriquet_Strings[39])
#define MSG_ERROR_THREADS (&Bourriquet_Strings[40])
#define MSG_ERROR_LIB_URL (&Bourriquet_Strings[41])
#define MSG_ERROR_CREATE_SEMAPHORE (&Bourriquet_Strings[42])
#define MSG_RETRY_HOMEPAGE_QUIT_GAD (&Bourriquet_Strings[43])
#define MSG_ABOUT_CURRENT_DEVELOPERS (&Bourriquet_Strings[44])
#define MSG_ABOUT_CONTRIBUTORS (&Bourriquet_Strings[45])
#define MSG_ABOUT_LOCALIZATION_CONTRIBUTORS (&Bourriquet_Strings[46])
#define MSG_ABOUT_GPL (&Bourriquet_Strings[47])
#define MSG_ABOUT_3RD_PARTY_SOFTWARE (&Bourriquet_Strings[48])
#define MSG_ABOUT_BOURRIQUET_NEWS (&Bourriquet_Strings[49])
#define MSG_ABOUTBOURRIQUET (&Bourriquet_Strings[50])
#define MSG_SHUTDOWN (&Bourriquet_Strings[51])
#define MSG_THREAD_EXIT_WARNING_TITLE (&Bourriquet_Strings[52])
#define MSG_THREAD_EXIT_WARNING (&Bourriquet_Strings[53])
#define MSG_THREAD_EXIT_WARNING_BT (&Bourriquet_Strings[54])
#define MSG_DB_CONNECT (&Bourriquet_Strings[55])
#define MSG_DB_CANCEL (&Bourriquet_Strings[56])
#define MSG_DB_DISCONNECT (&Bourriquet_Strings[57])
#define MSG_DB_SERVER (&Bourriquet_Strings[58])
#define MSG_DB_TRANSFER (&Bourriquet_Strings[59])
#define MSG_DB_RESEARCH (&Bourriquet_Strings[60])
#define MSG_DB_SHARED (&Bourriquet_Strings[61])
#define MSG_DB_PREFS (&Bourriquet_Strings[62])
#define MSG_SE_TITLE (&Bourriquet_Strings[63])
#define MSG_SE_SUMMARY (&Bourriquet_Strings[64])
#define MSG_SE_SERVERLIST (&Bourriquet_Strings[65])
#define MSG_SE_NB_SERVERLIST (&Bourriquet_Strings[66])
#define MSG_SE_NEWSERVER (&Bourriquet_Strings[67])
#define MSG_SE_NEWSERVER_IP (&Bourriquet_Strings[68])
#define MSG_SE_NEWSERVER_PORT (&Bourriquet_Strings[69])
#define MSG_SE_NEWSERVER_NAME (&Bourriquet_Strings[70])
#define MSG_SE_NEWSERVER_ADD (&Bourriquet_Strings[71])
#define MSG_SE_UPDATE_MET (&Bourriquet_Strings[72])
#define MSG_SE_UPDATE (&Bourriquet_Strings[73])
#define MSG_SE_MYINFOS (&Bourriquet_Strings[74])
#define MSG_SE_NAME (&Bourriquet_Strings[75])
#define MSG_SE_IP (&Bourriquet_Strings[76])
#define MSG_SE_DESCRIPTION (&Bourriquet_Strings[77])
#define MSG_SE_PING (&Bourriquet_Strings[78])
#define MSG_SE_USERS (&Bourriquet_Strings[79])
#define MSG_SE_MAXUSERS (&Bourriquet_Strings[80])
#define MSG_SE_FILES (&Bourriquet_Strings[81])
#define MSG_SE_PREFS (&Bourriquet_Strings[82])
#define MSG_SE_FAILED (&Bourriquet_Strings[83])
#define MSG_SE_STATIC (&Bourriquet_Strings[84])
#define MSG_SE_SOFTFILES (&Bourriquet_Strings[85])
#define MSG_SE_HARDFILES (&Bourriquet_Strings[86])
#define MSG_SE_VERSION (&Bourriquet_Strings[87])
#define MSG_SE_LOWIDUSERS (&Bourriquet_Strings[88])
#define MSG_SE_INFOS (&Bourriquet_Strings[89])
#define MSG_SE_LOG (&Bourriquet_Strings[90])
#define MSG_SE_LOWPRI (&Bourriquet_Strings[91])
#define MSG_SE_NORMALPRI (&Bourriquet_Strings[92])
#define MSG_SE_HIGHPRI (&Bourriquet_Strings[93])
#define MSG_SE_CTXT_CONNECT (&Bourriquet_Strings[94])
#define MSG_SE_CTXT_PRIORITY (&Bourriquet_Strings[95])
#define MSG_SE_CTXT_LISTADD (&Bourriquet_Strings[96])
#define MSG_SE_CTXT_LISTREMOVE (&Bourriquet_Strings[97])
#define MSG_SE_CTXT_ERASE (&Bourriquet_Strings[98])
#define MSG_SE_HELP_IP (&Bourriquet_Strings[99])
#define MSG_SE_HELP_PORT (&Bourriquet_Strings[100])
#define MSG_SE_HELP_NAME (&Bourriquet_Strings[101])
#define MSG_SE_HELP_ADDSERVER (&Bourriquet_Strings[102])
#define MSG_SE_HELP_UPDATE_MET (&Bourriquet_Strings[103])
#define MSG_SE_HELP_UPDATE (&Bourriquet_Strings[104])
#define MSG_SE_TXT (&Bourriquet_Strings[105])
#define MSG_SE_USERS_FILES (&Bourriquet_Strings[106])
#define MSG_SE_DL_UL (&Bourriquet_Strings[107])
#define MSG_RS_TITLE (&Bourriquet_Strings[108])
#define MSG_RS_SUMMARY (&Bourriquet_Strings[109])
#define MSG_RS_SEARCHPARAMS (&Bourriquet_Strings[110])
#define MSG_RS_FILENAME (&Bourriquet_Strings[111])
#define MSG_RS_FILETYPE (&Bourriquet_Strings[112])
#define MSG_RS_START (&Bourriquet_Strings[113])
#define MSG_RS_ABORT (&Bourriquet_Strings[114])
#define MSG_RS_REMOVEALL (&Bourriquet_Strings[115])
#define MSG_RS_SEARCHRESULTS (&Bourriquet_Strings[116])
#define MSG_RS_NAME (&Bourriquet_Strings[117])
#define MSG_RS_SIZE (&Bourriquet_Strings[118])
#define MSG_RS_AVAILABILITY (&Bourriquet_Strings[119])
#define MSG_RS_FULL (&Bourriquet_Strings[120])
#define MSG_RS_TYPE (&Bourriquet_Strings[121])
#define MSG_RS_ARTIST (&Bourriquet_Strings[122])
#define MSG_RS_ALBUM (&Bourriquet_Strings[123])
#define MSG_RS_ALBUM_TITLE (&Bourriquet_Strings[124])
#define MSG_RS_DURATION (&Bourriquet_Strings[125])
#define MSG_RS_RATE (&Bourriquet_Strings[126])
#define MSG_RS_CODEC (&Bourriquet_Strings[127])
#define MSG_RS_KNOWN (&Bourriquet_Strings[128])
#define MSG_RS_TYPE_ARCHIVE (&Bourriquet_Strings[129])
#define MSG_RS_TYPE_PICTURE (&Bourriquet_Strings[130])
#define MSG_RS_TYPE_AUDIO (&Bourriquet_Strings[131])
#define MSG_RS_TYPE_VIDEO (&Bourriquet_Strings[132])
#define MSG_DLUL_TITLE (&Bourriquet_Strings[133])
#define MSG_DLUL_SUMMARY (&Bourriquet_Strings[134])
#define MSG_DL_TITLE (&Bourriquet_Strings[135])
#define MSG_DL_NAME (&Bourriquet_Strings[136])
#define MSG_DL_SIZE (&Bourriquet_Strings[137])
#define MSG_DL_DONE (&Bourriquet_Strings[138])
#define MSG_DL_SPEED (&Bourriquet_Strings[139])
#define MSG_DL_PROGRESS (&Bourriquet_Strings[140])
#define MSG_DL_SOURCE (&Bourriquet_Strings[141])
#define MSG_DL_PRIORITY (&Bourriquet_Strings[142])
#define MSG_DL_STATUS (&Bourriquet_Strings[143])
#define MSG_DL_REMAINING (&Bourriquet_Strings[144])
#define MSG_DL_ADDTO (&Bourriquet_Strings[145])
#define MSG_UL_TITLE (&Bourriquet_Strings[146])
#define MSG_UL_NAME_USER (&Bourriquet_Strings[147])
#define MSG_UL_FILE (&Bourriquet_Strings[148])
#define MSG_UL_SPEED (&Bourriquet_Strings[149])
#define MSG_UL_TRANSFERRED (&Bourriquet_Strings[150])
#define MSG_UL_EXCEPTED (&Bourriquet_Strings[151])
#define MSG_UL_SEND_TIME (&Bourriquet_Strings[152])
#define MSG_UL_STATUS (&Bourriquet_Strings[153])
#define MSG_UL_PARTS_OBTAINED (&Bourriquet_Strings[154])
#define MSG_SF_TITLE (&Bourriquet_Strings[155])
#define MSG_SF_SUMMARY (&Bourriquet_Strings[156])
#define MSG_SF_NAME (&Bourriquet_Strings[157])
#define MSG_SF_SIZE (&Bourriquet_Strings[158])
#define MSG_SF_TYPE (&Bourriquet_Strings[159])
#define MSG_SF_PRIORITY (&Bourriquet_Strings[160])
#define MSG_SF_REQUESTS (&Bourriquet_Strings[161])
#define MSG_SF_TRANSFERRED (&Bourriquet_Strings[162])
#define MSG_SF_SHARED_PARTS (&Bourriquet_Strings[163])
#define MSG_SF_COMPLETE (&Bourriquet_Strings[164])
#define MSG_SF_SHARED_EDK (&Bourriquet_Strings[165])
#define MSG_CO_TITLE (&Bourriquet_Strings[166])
#define MSG_CO_SUMMARY (&Bourriquet_Strings[167])
#define MSG_CO_SAVE (&Bourriquet_Strings[168])
#define MSG_CO_USE (&Bourriquet_Strings[169])
#define MSG_CO_ABORT (&Bourriquet_Strings[170])
#define MSG_CO_GENERAL (&Bourriquet_Strings[171])
#define MSG_CO_GENERAL_USERNAME (&Bourriquet_Strings[172])
#define MSG_CO_GENERAL_CLIENT_PORT (&Bourriquet_Strings[173])
#define MSG_CO_DIRECTORIES_TITLE (&Bourriquet_Strings[174])
#define MSG_CO_DIRECTORIES_PATHS (&Bourriquet_Strings[175])
#define MSG_CO_DIRECTORIES_INCOMING_PATHS (&Bourriquet_Strings[176])
#define MSG_CO_DIRECTORIES_TEMPORARY_PATHS (&Bourriquet_Strings[177])
#define MSG_CO_DIRECTORIES_SHARED_PATHS (&Bourriquet_Strings[178])
#define MSG_CO_THEMES_TITLE (&Bourriquet_Strings[179])
#define MSG_CO_THEMES_APPEARANCE (&Bourriquet_Strings[180])
#define MSG_CO_THEMES_ACTIVATE (&Bourriquet_Strings[181])
#define MSG_CO_THEMES_PREVIEW (&Bourriquet_Strings[182])
#define MSG_CO_THEMES_AUTHOR (&Bourriquet_Strings[183])
#define MSG_CO_THEMES_URL (&Bourriquet_Strings[184])
#define MSG_CO_THEMES_INFORMATION (&Bourriquet_Strings[185])
#define MSG_CO_UPDATE_TITLE (&Bourriquet_Strings[186])
#define MSG_CO_UPDATE_ALL (&Bourriquet_Strings[187])
#define MSG_CO_UPDATE_FREQUENCY (&Bourriquet_Strings[188])
#define MSG_CO_UPDATE_FREQUENCY_NEVER (&Bourriquet_Strings[189])
#define MSG_CO_UPDATE_FREQUENCY_DAILY (&Bourriquet_Strings[190])
#define MSG_CO_UPDATE_FREQUENCY_WEEKLY (&Bourriquet_Strings[191])
#define MSG_CO_UPDATE_FREQUENCY_MONTHLY (&Bourriquet_Strings[192])
#define MSG_CO_UPDATE_SEARCH (&Bourriquet_Strings[193])
#define MSG_CN_TITLE (&Bourriquet_Strings[194])
#define MSG_CN_ABORT (&Bourriquet_Strings[195])
#define MSG_ER_THEMEVER_FALLBACK (&Bourriquet_Strings[196])
#define MSG_ER_THEMEVER_IGNORE (&Bourriquet_Strings[197])
#define MSG_ER_THEME_FATAL (&Bourriquet_Strings[198])
#define MSG_ER_THEME_FALLBACK (&Bourriquet_Strings[199])
#define NUM_OF_MSGS (&Bourriquet_Strings[200])

#endif /* LOCALE_H */
