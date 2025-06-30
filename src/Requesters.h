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
#ifndef REQUESTERS_H
#define REQUESTERS_H

struct Connection;
struct Certificate;

// macros et drapeaux attachment requester 
#define ATTREQ_DISP       (1<<0)
#define ATTREQ_SAVE       (1<<1)
#define ATTREQ_PRINT      (1<<2)
#define ATTREQ_MULTI      (1<<3)
#define isDisplayReq(v)   (isFlagSet((v), ATTREQ_DISP))
#define isSaveReq(v)      (isFlagSet((v), ATTREQ_SAVE))
#define isPrintReq(v)     (isFlagSet((v), ATTREQ_PRINT))
#define isMultiReq(v)     (isFlagSet((v), ATTREQ_MULTI))

// drapeaux MUI_Request()
#define MUIF_REQ_FLOATTEXT (1<<0)

LONG BOURRIQUETMUIRequest(const Object *app, const Object *win, LONG flags, const char *title, const char *gadgets, const char *format, ...);
LONG BOURRIQUETMUIRequestA(const Object *app, const Object *parent, LONG flags, const char *tit, const char *gad, const char *reqtxt);

#endif /* REQUESTERS_H */
