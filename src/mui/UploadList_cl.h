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
/* Ce fichier est automatiquement g�n�r� avec GenClasses - NE PAS EDITER!!! */

#ifndef UPLOADLIST_CL_H
#define UPLOADLIST_CL_H 1

#ifndef CLIB_ALIB_PROTOS_H
  #include <clib/alib_protos.h>
#endif
#ifndef PROTO_EXEC_H
  #include <proto/exec.h>
#endif
#ifndef PROTO_INTUITION_H
  #include <proto/intuition.h>
#endif
#ifndef PROTO_UTILITY_H
  #include <proto/utility.h>
#endif
#ifndef EXEC_TYPES_H
  #include <exec/types.h>
#endif
#ifndef LIBRARIES_MUI_H
  #include <libraries/mui.h>
#endif
#ifndef CLASSES_CLASSES_EXTRA_H
  #include "ClassesExtra.h"
#endif
#ifndef EXTRASRC_H
  #include "Extrasrc.h"
#endif
#include "mui/UploadList.h"

#define inittags(msg)    (((struct opSet *)msg)->ops_AttrList)
#define GETDATA          struct Data *data = (struct Data *)INST_DATA(cl,obj)
#define DECLARE(method)  ULONG m_UploadList_## method(UNUSED struct IClass *cl, UNUSED Object *obj, UNUSED struct MUIP_UploadList_## method *msg )
#define OVERLOAD(method) ULONG m_UploadList_## method(UNUSED struct IClass *cl, UNUSED Object *obj, UNUSED Msg msg )
#define METHOD(method)   MUIM_UploadList_## method
#define ATTR(attr)       MUIA_UploadList_## attr

/* Exported CLASSDATA */

struct Data
{

  	char user[SIZE_ADDRESS];
  	char file[SIZE_LARGE];
  	char speed[SIZE_LARGE];
  	char transferred[SIZE_LARGE];
	char excepted[SIZE_DEFAULT];
  	char send_time[SIZE_DEFAULT];
  	char status[SIZE_DEFAULT];
	char parts_obtained[SIZE_DEFAULT];

};

ULONG UploadListGetSize( void ) { return sizeof(struct Data); }

#endif /* UPLOADLIST_CL_H */
