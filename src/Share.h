/***************************************************************************
Bourriquet MorphOS
	2024-2025 Digitally
 
***************************************************************************/
#ifndef SHARE_H
#define SHARE_H

#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/types.h>
#include <proto/exec.h>

#include "Lists.h"
#include "Stringsizes.h"

struct Share
{
	struct node node;
   	int status;
   	char FileName[SIZE_LARGE];
   	int Size;
   	char FileType[SIZE_DEFAULT];
   	char Priority[SIZE_DEFAULT];
   	char Permission[SIZE_DEFAULT];
   	char FileID[2048];
   	int nb_parts;
   	int count;
   	char part_hash[SIZE_DEFAULT];
};

void SearchDirShare(char *dname, char *extension);
int write_edk_tags_shared(unsigned char *buf, int tag, struct Share *sh);

#endif /* SHARE_H */
