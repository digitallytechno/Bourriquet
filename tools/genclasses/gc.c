/***************************************************************************
 GenClasses - MUI class dispatcher generator
 Copyright (C) 2001 by Andrew Bell <mechanismx@lineone.net>
***************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <string.h>
#include "gc.h"
#include "lists.h"
#include "crc32.h"

static const char * const verstr = "0.37";

/* Every shitty hack wouldn't be complete without some shitty globals... */

char  arg_classdir          [256];
char  arg_basename          [256];
char  arg_includes          [256];
char  arg_mkfile            [256];
char  arg_mkfile_dest       [256];
char *arg_mkfile_cc       = NULL;
char *arg_mkfile_outarg   = NULL;
char *arg_mkfile_ccopts   = NULL;
int   arg_gpl             = 0;     /* booleans */
int   arg_storm           = 0;
int   arg_v               = 0;
int   arg_q               = 0;

#include <dirent.h>
#ifndef TAG_USER
#define TAG_USER ((unsigned int)(1UL<<31))
#endif

/*******************************************************************************
 *
 * Assorted routines
 *
 *******************************************************************************
 *
 *
 */

char *skipwhitespaces(char *p) /* Note: isspace() sucks... */
{
    unsigned char c;

    while((c = *p))
      {
        if (c == '\t' || c == '\r' || c == '\n' || c == ' ' || c == 0xa0)
          {
            ++p;
          }
        else break;
      }
    return(p);
}

char *stralloc(char *str)
{
    char *strvec, *estr;
    unsigned c;
  
    if(!str) str = "";
    if(!(strvec = calloc(1, strlen(str) + 1))) return(NULL);
    if(*str == 0) return(strvec); 
    while((c = *str)) if(c == '\t' || c == '\r' || c == '\n' || c == ' ' || c == 0xa0)
    ++str; else break;
    if(*str == 0) return(strvec); /* Empty string still? */
    estr = str + (strlen(str) - 1);
    while(estr > str)
      {
        c = *estr;
        if(c == '\t' || c == '\r' || c == '\n' || c == ' ' || c == 0xa0) --estr; else break;
      }
    memcpy(strvec, str, (size_t)(estr - str) + 1);
    return(strvec);
}

int myaddpart(char *path, char *name, size_t len)
{
    char c;
  
    c = path[strlen(path) - 1];
    if(c != ':' && c != '/') strncat(path, "/", len);
    strncat(path, name, len);
    return(1);
}

char *mypathpart(char *path)
{
    char *p;
    
    if(!path) return(NULL);
    if((p = strrchr(path, '/'))) return(p);
    if((p = strrchr(path, ':'))) return(++p);
    return(path);
}

char *myfilepart(char *path)
{
    char *p;
    
    if(!path) return(NULL);
    if((p = strrchr(path, '/'))) return ++p;
    if((p = strrchr(path, ':'))) return ++p;
    return(path);
}

/*******************************************************************************
 *
 * Tag value generation
 *
 *******************************************************************************
 *
 *
 */

int *collision_cnts;

static unsigned int gethash(char *str)
{
    static unsigned int primes[10] = {3, 5, 7, 11, 13, 17, 19, 23, 27, 31};
    unsigned int i = 0;
    unsigned int hash = strlen(str);
    while(*str)
      {
        hash *= 13;
        hash += (*str++) * primes[(++i) % 10];
      }
    return(hash % BUCKET_CNT);
}

static unsigned int gettagvalue(char *tag, int checkcol)
{
    unsigned int hash = gethash(tag);
    unsigned int val = TAG_USER + (hash << 8) + collision_cnts[hash];

    if(val < TAG_USER || val >= 0x80010000)
	fprintf(stderr, "ATTENTION: generer une valeur de balise '%x' de la classe '%s' entre en collision avec les valeurs de balises BOOPSI/MUI!\n", val, tag);
    if(checkcol) ++collision_cnts[hash];
    if(arg_v) fprintf(stdout, "Assignation d'une balise %-35s avec la valeur %x\n", tag, val);
    return(val);
}

/*******************************************************************************
 *
 * Parsing routines
 *
 *******************************************************************************/
void free_overload(struct overloaddef *od)
{
    if(!od) return;
    free(od->name);
}

void free_declare(struct declaredef *dd)
{
    if(!dd) return;
    free(dd->name);
    free(dd->params);
}

void free_exportblk(struct exportdef *ed)
{
    if(!ed) return;
    free(ed->exporttext);
}

void free_includeblk(struct includedef *id)
{
    if(!id) return;
    free(id->includetext);
}

void free_attr(struct attrdef *ad)
{
    if(!ad) return;
    free(ad->name);
}

void free_classdef(struct classdef *cd)
{
    struct node *n;
    
    if(!cd) return;
    free(cd->classdata);

    while((n = list_remhead(&cd->overloadlist)))
      {
        free_overload(n->data);
        free(n);
      }

    while((n = list_remhead(&cd->declarelist)))
      {
        free_declare(n->data);
        free(n);
      }

    while((n = list_remhead(&cd->attrlist)))
      {
        free_attr(n->data);
        free(n);
      }

    while((n = list_remhead(&cd->exportlist)))
      {
        free_exportblk(n->data);
        free(n);
      }

    while((n = list_remhead(&cd->includelist)))
      {
        free_includeblk(n->data);
        free(n);
      }

    free(cd);
}

void add_overload(struct classdef *cd, char *name)
{
    struct overloaddef *od;
  
    if(!(od = (struct overloaddef *)calloc(1, sizeof(struct overloaddef)))) return;
    if(!(od->name = stralloc(name)))
      {
        free(od);
        return;
      }
    list_saveitem(&cd->overloadlist, od->name, od);
}

void add_declare(struct classdef *cd, char *name, char *params)
{
    struct declaredef *dd;
  
    if(!(dd = (struct declaredef *)calloc(1, sizeof(struct declaredef)))) return;
    dd->name = stralloc(name);
    dd->params = stralloc(params);
    if(!dd->name || !dd->params)
      {
        free_declare(dd);
        free(dd);
        return;
      }
  list_saveitem(&cd->declarelist, dd->name, dd);
}

void add_exportblk( struct classdef *cd, char *textblk )
{
    struct exportdef *ed;
    
    if(!(ed = (struct exportdef *) calloc(1, sizeof(struct exportdef)))) return;
    if(!(ed->exporttext = stralloc(textblk)))
      {
        free(ed);
        return;
      }
    list_saveitem(&cd->exportlist, ed->exporttext, ed);
}

void add_includeblk( struct classdef *cd, char *textblk )
{
    struct includedef *id;
    
    if(!(id = (struct includedef *) calloc(1, sizeof(struct includedef)))) return;
    if(!(id->includetext = stralloc(textblk)))
      {
        free(id);
        return;
      }
    list_saveitem(&cd->includelist, id->includetext, id);
}

void add_attr(struct classdef *cd, char *name)
{
    struct attrdef *ad;
  
    if(!(ad = (struct attrdef *) calloc(1, sizeof(struct attrdef)))) return;
    if(!(ad->name = stralloc(name))) return;
    if(list_findname(&cd->attrlist, name))
      {
	if(arg_v) fprintf(stdout, "ATTR '%s' déjà collecté, ignoré.\n", name);
        free_attr(ad);
        free(ad);
        return;
      }
    list_saveitem(&cd->attrlist, ad->name, ad);
}

struct classdef *processclasssrc(char *path)
{
    FILE *fp;
    int lineno = 0;
    char line[256], *p, *ob, *cb, *sub;
    struct classdef *cd;
    int spos, epos = 0, exlineno = lineno;
    char *blk = NULL;

    if(!(cd = calloc(1, sizeof(struct classdef)))) return(NULL);
    if(!(fp = fopen(path, "r")))
      {
        fprintf(stderr, "ERREUR: Impossible d'ouvrir %s\n", path);
        free(cd);
        return(NULL);
      }
    list_init(&cd->overloadlist);
    list_init(&cd->declarelist);
    list_init(&cd->attrlist);
    list_init(&cd->exportlist);
    list_init(&cd->includelist);

    if((cd->name = stralloc(myfilepart(path))))
      {
        if((p = strrchr(cd->name, '.'))) *p = 0;
      }

    while (fgets(line, 255, fp))
      {
        lineno++;
        p = skipwhitespaces(line);
        if(!*p) continue;
        ob = cb = NULL;
        if(!cd->superclass && (sub = strstr(p, KEYWD_SUPERCLASS)))
          {
	    if(arg_v) fprintf(stdout, KEYWD_SUPERCLASS " Un mot-clé trouvé à la ligne %d dans le fichier %s\n", lineno, path);
            sub += sizeof(KEYWD_SUPERCLASS) - 1;
            cd->superclass = stralloc(skipwhitespaces(sub));
            cd->index = -1;
          }
        else if(!cd->desc && (sub = strstr(p, KEYWD_DESC)))
          {
	    if(arg_v) fprintf(stdout, KEYWD_DESC " Un mot-clé trouvé à la ligne %d\n", lineno);
            sub += sizeof(KEYWD_DESC) - 1;
            cd->desc = stralloc(skipwhitespaces(sub));
          }
        else if(!cd->classdata && strstr(line, KEYWD_CLASSDATA))
          {
	    if(arg_v) fprintf(stdout, KEYWD_CLASSDATA " Un mot-clé trouvé à la ligne %d\n", lineno);
            spos = ftell(fp);
            while(fgets(p = line, 255, fp))
              {
                lineno++; 
                epos = ftell(fp);
                if(!(p = strstr(line, "*/"))) continue;
                epos += (p - line) - 3; /* + offset into line... */
                fseek(fp, spos, SEEK_SET);
                if(!(blk = calloc(1, (size_t)(epos - spos + 1))))
                  {
		    fprintf(stderr, "ATTENTION: Ne peut pas lire le bloc " KEYWD_CLASSDATA " à la ligne %d, mémoire insuffisante!\n", exlineno); break;
                  }
                if(fread(blk, (size_t)(epos - spos), 1, fp) != 1)
                  {
		    fprintf(stderr, "ATTENTION: Ne peut pas lire le bloc " KEYWD_CLASSDATA " à la ligne %d, fread() a echoué!\n", exlineno); break;
                  }
                if ((ob = strchr(blk, '{')))
                  {
                    if (!(cb = strchr(ob + 1, '}'))) cb = blk + strlen(blk);
                    *cb = 0;
                    if ((cd->classdata = calloc(1, strlen(++ob) + 1))) strcpy(cd->classdata, ob);
                  }
                else if ((cd->classdata = calloc(1, strlen(blk) + 1))) strcpy(cd->classdata, blk);
                free(blk);
                blk = NULL;
                break;
              }
          }
        else if (strncmp(KEYWD_OVERLOAD, p, sizeof(KEYWD_OVERLOAD) - 1) == 0)
          {
	    if(arg_v) fprintf(stdout, KEYWD_OVERLOAD " Un mot-clé trouvé à la ligne %d dans le fichier %s\n", lineno, path);
            p += sizeof(KEYWD_OVERLOAD) - 1;
            if(!(ob = strchr(p, '('))) continue; 
            if(!(cb = strchr(ob, ')'))) cb = p + strlen(p);
            *cb = 0; add_overload(cd, ++ob);
          }
        else if(strncmp(KEYWD_DECLARE, p, sizeof(KEYWD_DECLARE)-1) == 0)
          {
	    if(arg_v) fprintf(stdout, KEYWD_DECLARE " Un mot-clé trouvé à la ligne %d dans le fichier %s\n", lineno, path);
            p += sizeof(KEYWD_DECLARE) - 1;
            if(!(ob = strchr(p, '('))) continue; /* There's no open bracket, ignore it... */
            if(!(cb = strchr(ob, ')'))) cb = p + strlen(p);
            if((p = strstr(cb + 1, "//"))) p += 2;
            *cb = 0; add_declare(cd, ++ob, p);
	    if(p != NULL && strstr(p, "BOOL ") != NULL) fprintf(stderr, "ATTENTION: " KEYWD_DECLARE "() - Le paramêtre de type BOOL est dangereux à la ligne %d dans le fichier %s\n", lineno, path);
          }
        else if((sub = strstr(p, KEYWD_ATTR)) != NULL)
          {
	    if(arg_v) fprintf(stdout, KEYWD_ATTR " Un mot-clé trouvé à la ligne %d dans le fichier %s\n", lineno, path);
            p = sub + sizeof(KEYWD_ATTR) - 1;
            if(!(ob = strchr(p, '('))) continue; 
            if(!(cb = strchr(ob, ')'))) cb = p + strlen(p);
            *cb = 0; add_attr(cd, ++ob);
          }
        else if(strncmp("/*", p, 2) == 0) 
          {
            p = skipwhitespaces(p + 2);
            if(strncmp(KEYWD_EXPORT, p, sizeof(KEYWD_EXPORT)- 1) == 0)
              {
		if(arg_v) fprintf(stdout, KEYWD_EXPORT " Un mot-clé trouvé à la ligne %d dans le fichier %s\n", lineno, path);
                p += sizeof(KEYWD_EXPORT) - 1;
                spos = ftell(fp);
                while(fgets(p = line, 255, fp))
                  {
                    lineno++; 
                    epos = ftell(fp);
                    if(!(p = strstr(line, "*/"))) continue;
                    epos += (p - line) - 3; 
                    fseek(fp, spos, SEEK_SET);
                    if(!(blk = calloc(1, (size_t)(epos - spos + 1))))
                      {
			fprintf(stderr, "ATTENTION: Ne peut pas lire le bloc " KEYWD_EXPORT " à la ligne %d, mémoire insuffisante!\n", exlineno); break;
                      }
                    if(fread(blk, (size_t)(epos - spos), 1, fp) != 1)
                      {
			fprintf(stderr, "ATTENTION: Ne peut pas lire le bloc " KEYWD_EXPORT " à la ligne %d, fread() a échoué!\n", exlineno); break;
                      }
                    add_exportblk(cd, blk);
                    free(blk);
                    blk = NULL;
                    break;
                  }
		if(epos == 0) fprintf(stderr, "ATTENTION: bloc EXPORT non raccordé à la ligne %d\n", lineno);
              }
            else if(strncmp(KEYWD_INCLUDE, p, sizeof(KEYWD_INCLUDE)-1) == 0)
              {
		if(arg_v) fprintf(stdout, KEYWD_INCLUDE " Un mot-clé trouvé à la ligne %d dans le fichier %s\n", lineno, path);
                p += sizeof(KEYWD_INCLUDE) - 1;
                spos = ftell(fp);
                while(fgets(p = line, 255, fp))
                  {
                    lineno++; 
                    epos = ftell(fp);
                    if(!(p = strstr(line, "*/"))) continue;
                    epos += (p - line) - 3; 
                    if(fseek(fp, spos, SEEK_SET) == 0)
                      {
                        if(!(blk = calloc(1, (size_t)(epos - spos + 1))))
                          {
			    fprintf(stderr, "ATTENTION: Ne peut pas lire le bloc " KEYWD_INCLUDE " à la ligne %d, mémoire insuffisante!\n", exlineno); break;
                          }
                        if(fread(blk, (size_t)(epos - spos), 1, fp) != 1)
                          {
			    fprintf(stderr, "ATTENTION: Ne peut pas lire le bloc " KEYWD_INCLUDE " à la ligne %d, fread() a échoué!\n", exlineno); break;
                          }
                        add_includeblk(cd, blk);
                        free(blk);
                        blk = NULL;
                      }
                    break;
                  }
		if(epos == 0) fprintf(stderr, "ATTENTION: bloc INCLUDE non-terminé à la ligne %d\n", lineno);
              }
          }
      }  
    if(cd->superclass == NULL)
      {
	fprintf(stderr, "ATTENTION: fichier source '%s' ne contient pas le mot-clé " KEYWD_SUPERCLASS ". sauté.\n", path);
        free_classdef(cd);
        cd = NULL;
      }
    if(blk != NULL) free(blk);
    fclose(fp);
    return(cd);
}

void printclasslist( struct list *classlist )
{
    struct classdef *cd;
    struct overloaddef *od;
    struct declaredef *dd;
    struct attrdef *ad;
    struct node *n, *nn;

    fprintf(stdout, "Les mot-clés suivants sont extraits:\n");
    for(nn = NULL; (nn = list_getnext(classlist, nn, (void **)&cd)); )
      {
        fprintf(stdout, "CLASS: %s\n", cd->name);
        for(n = NULL; (n = list_getnext(&cd->overloadlist, n, (void **) &od)); )
          fprintf(stdout, "  OVERLOAD: %s\n", od->name);
        for(n = NULL; (n = list_getnext(&cd->declarelist, n, (void **) &dd)); )
          fprintf(stdout, "   DECLARE: %s\n", dd->name);
        for(n = NULL; (n = list_getnext(&cd->attrlist, n, (void **) &ad)); )
          fprintf(stdout, "      ATTR: %s\n", ad->name);
      }
}

int compareclasses(const struct node *n1, const struct node *n2)
{
    struct classdef *cd1 = n1->data;
    struct classdef *cd2 = n2->data;

    return(strcasecmp(cd1->name, cd2->name));
}

int scanclasses(char *dirname, struct list *classlist)
{
    DIR *dir;
    struct dirent *de;
    char *n, dirbuf[256];
    int len, srccnt = 0;
    struct classdef *cd;
    
    strncpy(dirbuf, dirname, 255);
    if(arg_v) fprintf(stdout, "analyse du répertoire des classes '%s'\n", dirbuf);
    if(!(dir = opendir(dirname)))
      {
	fprintf(stderr, "ERREUR: Incapable d'ouvrir le repertoire %s\n", dirname);
        return(0);
      }

    while((de = readdir(dir)))
      {
        n = de->d_name; len = strlen(n);
        if(len < 2) continue;
        if((n[len - 2] != '.') || (tolower(n[len - 1]) != 'c'))
          {
	    if(arg_v) fprintf(stdout, "Saute: %s\n", n);
            continue;
          }
        if(!strcmp(SOURCE_NAME, n) || !strcmp(HEADER_NAME, n)) continue;
        ++srccnt;
        myaddpart(dirbuf, de->d_name, 255);
        if(arg_v) fprintf(stdout, "Traitement: %s\n", dirbuf);
        if((cd = processclasssrc(dirbuf))) list_saveitem(classlist, cd->name, cd);
        *mypathpart(dirbuf) = 0;
      }
    closedir(dir);
    if(srccnt == 0)
      {
        fprintf(stderr, "ERREUR: N'a pu trouver de sources dans %s\n", dirname);
        return(0);
      }
    list_sort(classlist, compareclasses);
    if(arg_v) printclasslist(classlist);
    return(1);
}

void buildclasstree(struct list *classlist)
{
    struct classdef *outercd;
    struct node *outern;

    for(outern = NULL; (outern = list_getnext(classlist, outern, (void **)&outercd)); )
      {
        struct classdef *innercd;
        struct node *innern;

        for(innern = NULL; (innern = list_getnext(classlist, innern, (void **)&innercd)); )
          {
            if(innercd != outercd)
              {
                if(outercd->supernode == NULL && strcmp(&outercd->superclass[5], innercd->name) == 0) outercd->supernode = innercd;
              }
            else
              {
                if(strcmp(&outercd->superclass[5], outercd->name) == 0) outercd->supernode = outercd;
              }
          }
      }
}

unsigned long read_crc(const char *crcfile)
{
    FILE *fp;
    unsigned long crc = INVALID_CRC;

    if((fp = fopen(crcfile, "r")) != NULL)
      {
        char line[32];

        if(fgets(line, 31, fp)) crc = strtoul(line, NULL, 16);
        fclose(fp);
      }
    return(crc);
}

void write_crc(const char *crcfile, const unsigned long crc)
{
    FILE *fp;

    if((fp = fopen(crcfile, "w")) != NULL)
      {
        fprintf(fp, "0x%08lx", crc);
        fclose(fp);
      }
}

unsigned long gen_class_crc(struct classdef *cd, unsigned long crc)
{
    struct declaredef *nextdd;
    struct exportdef *nexted;
    struct attrdef *nextad;
    struct overloaddef *nextod;
    struct includedef *nextid;
    struct node *n;

    if(cd->name != NULL) crc = crc32(cd->name, strlen(cd->name), crc);
    if(cd->superclass != NULL) crc = crc32(cd->superclass, strlen(cd->superclass), crc);
    if(cd->desc != NULL) crc = crc32(cd->desc, strlen(cd->desc), crc);
    if(cd->classdata != NULL) crc = crc32(cd->classdata, strlen(cd->classdata), crc);
    for(n = NULL; (n = list_getnext(&cd->overloadlist, n, (void **)&nextod));) crc = crc32(nextod->name, strlen(nextod->name), crc);
    for(n = NULL; (n = list_getnext(&cd->declarelist, n, (void **)&nextdd));)
      {
        crc = crc32(nextdd->name, strlen(nextdd->name), crc);
        crc = crc32(nextdd->params, strlen(nextdd->params), crc);
      }
    for(n = NULL; (n = list_getnext(&cd->attrlist, n, (void **)&nextad));) crc = crc32(nextad->name, strlen(nextad->name), crc);
    for(n = NULL; (n = list_getnext(&cd->exportlist, n, (void **)&nexted));) crc = crc32(nexted->exporttext, strlen(nexted->exporttext), crc);
    for(n = NULL; (n = list_getnext(&cd->includelist, n, (void **)&nextid));) crc = crc32(nextid->includetext, strlen(nextid->includetext), crc);
    return(crc);
}

unsigned long gen_crc( struct list *classlist )
{
    unsigned long crc;
    struct classdef *nextcd;
    struct node *n;

    crc = 0;
    for(n = NULL; (n = list_getnext(classlist, n, (void **)&nextcd));) crc = gen_class_crc(nextcd, crc);
    return(crc);
}

/*******************************************************************************
 *
 * Source code generation
 *
 *******************************************************************************/

void gen_gpl(FILE *fp)
{
    if(!fp || !arg_gpl) return;
    fprintf(fp,
            "/***************************************************************************\n"
 			"Bourriquet MorphOS\n"
 			"Copyright (C) 2024-2025 digitally\n"
 			"\n"
 			"This program is free software; you can redistribute it and/or modify\n"
 			"it under the terms of the GNU General Public License as published by\n"
 			"the Free Software Foundation; either version 2 of the License, or\n"
 			"(at your option) any later version.\n"
			"\n"
 			"This program is distributed in the hope that it will be useful,\n"
 			"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
 			"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
 			"GNU General Public License for more details.\n"
			"\n"
 			"You should have received a copy of the GNU General Public License\n"
 			"along with this program; if not, write to the Free Software\n"
 			"Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
 			"\n"
			"***************************************************************************/\n");
}

void gen_supportroutines(FILE *fp)
{
    char *bn = arg_basename;

    fprintf(fp, "%s%s%s", arg_storm ? "/// " : "", arg_storm ? bn : "", arg_storm ? "_NewObject()\n" : "");
    fprintf(fp, "Object * %s_NewObject(CONST_STRPTR className, ...)\n", bn);
    fprintf(fp, "{\n");
    fprintf(fp, "  Object *obj = NULL;\n");
    fprintf(fp, "  unsigned int i;\n");
    fprintf(fp, "\n");
    fprintf(fp, "  ENTER();\n");
    fprintf(fp, "\n");
    fprintf(fp, "  for(i = 0; i < NUMBEROFCLASSES; i++)\n");
    fprintf(fp, "  {\n");
    fprintf(fp, "    if(strcmp(MCCInfo[i].Name, className) == 0)\n");
    fprintf(fp, "    {\n");
    fprintf(fp, "      unsigned int j;\n");
    fprintf(fp, "      struct TagItem tags[128];\n");
    fprintf(fp, "      va_list args;\n");
    fprintf(fp, "\n");
    fprintf(fp, "      va_start(args, className);");
    fprintf(fp, "\n");
    fprintf(fp, "      for(j=0; j < (sizeof(tags) / sizeof(tags[0])); j++)\n");
    fprintf(fp, "      {\n");
    fprintf(fp, "        tags[j].ti_Tag = va_arg(args, ULONG);\n");
    fprintf(fp, "        if(tags[j].ti_Tag != TAG_DONE)\n");
    fprintf(fp, "          tags[j].ti_Data = va_arg(args, ULONG);\n");
    fprintf(fp, "        else\n");
    fprintf(fp, "        {\n");
    fprintf(fp, "          tags[j].ti_Data = 0;\n");
    fprintf(fp, "          break;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "      }\n");
    fprintf(fp, "\n");
    fprintf(fp, "      #if defined(DEBUG)\n");
    fprintf(fp, "      if(j >= (sizeof(tags) / sizeof(tags[0])))\n");
    fprintf(fp, "      {\n");
    fprintf(fp, "        E(DBF_ALWAYS, \"FATAL ERROR: size of tags[%%ld] array exhausted or no TAG_DONE in %s_NewObject(%%s, ...) call!!!!\", (sizeof(tags) / sizeof(tags[0])), className);\n", bn);
    fprintf(fp, "        ASSERT(j < (sizeof(tags) / sizeof(tags[0])));\n");
    fprintf(fp, "      }\n");
    fprintf(fp, "      #endif\n");
    fprintf(fp, "\n");
    fprintf(fp, "      obj = NewObjectA(%sClasses[i]->mcc_Class, NULL, (struct TagItem *)&tags);\n", bn);
    fprintf(fp, "\n");
    fprintf(fp, "      va_end(args);\n");
    fprintf(fp, "\n");
    fprintf(fp, "      break;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "  }\n");
    fprintf(fp, "\n");
    fprintf(fp, "  RETURN(obj);\n");
    fprintf(fp, "  return obj;\n");
    fprintf(fp, "}\n");
    fprintf(fp, "%s\n", arg_storm ? "\n///" : "");

    fprintf(fp, "%s%s%s", arg_storm ? "/// " : "", arg_storm ? bn : "", arg_storm ? "_SetupClasses()\n" : "");
    fprintf(fp, "BOOL %s_SetupClasses(const char **failClass, const char **failSuperClass)\n", bn);
    fprintf(fp, "{\n");
    fprintf(fp, "  BOOL success = TRUE;\n");
    fprintf(fp, "  unsigned int i;\n");
    fprintf(fp, "\n");
    fprintf(fp, "  ENTER();\n");
    fprintf(fp, "\n");
    fprintf(fp, "  *failClass = NULL;\n");
    fprintf(fp, "  *failSuperClass = NULL;\n");
    fprintf(fp, "\n");
    fprintf(fp, "  memset(%sClasses, 0, sizeof(%sClasses));\n", bn, bn);
    fprintf(fp, "  for (i = 0; i < NUMBEROFCLASSES; i++)\n");
    fprintf(fp, "  {\n");
    fprintf(fp, "    const char *superClassName;\n");
    fprintf(fp, "    struct MUI_CustomClass *superMCC;\n");
    fprintf(fp, "    ULONG classdataSize;\n");
    fprintf(fp, "\n");
    fprintf(fp, "    if(MCCInfo[i].SuperMCCIndex == -1)\n");
    fprintf(fp, "    {\n");
    fprintf(fp, "      superClassName = MCCInfo[i].SuperClass;\n");
    fprintf(fp, "      superMCC = NULL;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    else\n");
    fprintf(fp, "    {\n");
    fprintf(fp, "      superClassName = NULL;\n");
    fprintf(fp, "      superMCC = %sClasses[MCCInfo[i].SuperMCCIndex];\n", bn);
    fprintf(fp, "      if(superMCC == NULL)\n");
    fprintf(fp, "      {\n");
    fprintf(fp, "        E(DBF_STARTUP, \"La superclasse '%%s' de la classe '%%s' n'est pas encore créée!\", MCCInfo[i].SuperClass, MCCInfo[i].Name);\n");
    fprintf(fp, "        *failClass = MCCInfo[i].Name;\n");
    fprintf(fp, "        *failSuperClass = MCCInfo[i].SuperClass;\n");
    fprintf(fp, "        success = FALSE;\n");
    fprintf(fp, "        break;\n");
    fprintf(fp, "      }\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "\n");
    fprintf(fp, "    if(MCCInfo[i].GetSize != NULL)\n");
    fprintf(fp, "      classdataSize = MCCInfo[i].GetSize();\n");
    fprintf(fp, "    else\n");
    fprintf(fp, "      classdataSize = 0;\n");
    fprintf(fp, "\n");
    fprintf(fp, "    D(DBF_STARTUP, \"Création de la classe '%%s' comme sous-classe de '%%s'\", MCCInfo[i].Name, MCCInfo[i].SuperClass);\n");
    fprintf(fp, "    %sClasses[i] = MUI_CreateCustomClass(NULL, superClassName, superMCC, classdataSize, MCCInfo[i].Dispatcher);\n", bn);
    fprintf(fp, "    if(%sClasses[i] == NULL)\n", bn);
    fprintf(fp, "    {\n");
    fprintf(fp, "      E(DBF_STARTUP, \"Impossible de créer la classe '%%s' comme sous-classe de '%%s'\", MCCInfo[i].Name, MCCInfo[i].SuperClass);\n");
    fprintf(fp, "      *failClass = MCCInfo[i].Name;\n");
    fprintf(fp, "      *failSuperClass = MCCInfo[i].SuperClass;\n");
    fprintf(fp, "      success = FALSE;\n");
    fprintf(fp, "      break;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    else\n");
    fprintf(fp, "    {\n");
    fprintf(fp, "      if(LIB_VERSION_IS_AT_LEAST(MUIMasterBase, 20, 0) == TRUE)\n");
    fprintf(fp, "        %sClasses[i]->mcc_Class->cl_ID = (STRPTR)MCCInfo[i].Name;\n", bn);
    fprintf(fp, "    }\n");
    fprintf(fp, "  }\n");
    fprintf(fp, "\n");
    fprintf(fp, "  if(success == FALSE)\n");
    fprintf(fp, "    %s_CleanupClasses();\n", bn);
    fprintf(fp, "\n");
    fprintf(fp, "  RETURN(success);\n");
    fprintf(fp, "  return success;\n");
    fprintf(fp, "}\n");
    fprintf(fp, "%s", arg_storm ? "\n///" : "");
    fprintf(fp, "\n");

    fprintf(fp, "%s%s%s", arg_storm ? "/// " : "", arg_storm ? bn : "", arg_storm ? "_CleanupClasses()\n" : "");
    fprintf(fp, "void %s_CleanupClasses(void)\n", bn);
    fprintf(fp, "{\n");
    fprintf(fp, "  int i;\n");
    fprintf(fp, "\n");
    fprintf(fp, "  ENTER();\n");
    fprintf(fp, "\n");
    fprintf(fp, "  for(i = NUMBEROFCLASSES-1; i >= 0; i--)\n");
    fprintf(fp, "  {\n");
    fprintf(fp, "    if(%sClasses[i] != NULL)\n", bn);
    fprintf(fp, "    {\n");
    fprintf(fp, "      if(%sClasses[i]->mcc_Class->cl_ObjectCount != 0)\n", bn);
    fprintf(fp, "        E(DBF_STARTUP, \"La classe '%%s' a toujours %%ld objet(s) en vie!\", MCCInfo[i].Name, %sClasses[i]->mcc_Class->cl_ObjectCount);\n", bn);
    fprintf(fp, "\n");
    fprintf(fp, "      MUI_DeleteCustomClass(%sClasses[i]);\n", bn);
    fprintf(fp, "      %sClasses[i] = NULL;\n", bn);
    fprintf(fp, "    }\n");
    fprintf(fp, "  }\n");
    fprintf(fp, "\n");
    fprintf(fp, "  LEAVE();\n");
    fprintf(fp, "}\n");
    if(arg_storm)
      {
        fprintf(fp, "\n");
        fprintf(fp, "///\n");
      }
    fprintf(fp, "\n");
}

int gen_source( char *destfile, struct list *classlist )
{
    FILE *fp;
    struct classdef *nextcd;
    struct overloaddef *nextod;
    struct declaredef *nextdd;
    struct node *n, *nn;
    int goOn;
    int listindex;

    if(arg_v) fprintf(stdout, "Creation de source      : %s\n", destfile);
    if((fp = fopen(destfile, "w")) == NULL)
      {
          fprintf(stderr, "ERREUR: Incapable d'ouvrir %s\n", destfile);
          return(0);
      }

  /***************************************/
  /*           Write header...           */
  /***************************************/

    gen_gpl(fp);
    fprintf(fp, "\n");
    fprintf(fp, "/* " EDIT_WARNING " */\n");
    fprintf(fp, "\n");
    fprintf(fp, "#define INCLUDE_KITCHEN_SINK 1\n");
    fprintf(fp, "#include \"Classes.h\"\n");
    fprintf(fp, "#include \"Debug.h\"\n");
    fprintf(fp, "\n");
    fprintf(fp, "struct MUI_CustomClass *%sClasses[NUMBEROFCLASSES];\n", arg_basename);
    fprintf(fp, "\n");

  /***************************************/
  /*        Write dispatchers...         */
  /***************************************/

    fprintf(fp, "/*** Custom Class Dispatchers ***/\n");
    for(nn = NULL; (nn = list_getnext(classlist, nn, (void **)&nextcd)); )
      {
        if(arg_storm) fprintf(fp, "/// %sDispatcher()\n", nextcd->name);
        fprintf(fp, "SDISPATCHER(%sDispatcher)\n", nextcd->name);
        fprintf(fp, "{\n  switch(msg->MethodID)\n  {\n");
        for(n = NULL; (n = list_getnext(&nextcd->overloadlist, n, (void **)&nextod)); )
          {
            fprintf(fp, "    case %-40s: return m_%s_%-30s(cl, obj, msg);\n", nextod->name, nextcd->name, nextod->name);
          }
        for(n = NULL; (n = list_getnext(&nextcd->declarelist, n, (void **)&nextdd)); )
          {
            char name[128];

            snprintf(name, sizeof(name), "MUIM_%s_%s", nextcd->name, nextdd->name);
            fprintf(fp, "    case %-40s: return m_%s_%-30s(cl, obj, (APTR)msg);\n", name, nextcd->name, nextdd->name);
          }
        fprintf(fp, "  }\n");
        fprintf(fp, " \n");
        fprintf(fp, "  return DoSuperMethodA(cl, obj, msg);\n");
        fprintf(fp, "}\n");
        fprintf(fp, "\n");
        if(arg_storm) fprintf(fp, "///\n");
      }

  /*****************************************/
  /*        Write MCCInfo struct           */
  /*****************************************/

    fprintf(fp, "\n");
    fprintf(fp, "/*** Custom Class Support ***/\n");
    if(arg_storm) fprintf(fp, "/// struct MCCInfo\n");
    fprintf(fp, "const struct\n");
    fprintf(fp, "{\n");
    fprintf(fp, "  CONST_STRPTR Name;\n");
    fprintf(fp, "  CONST_STRPTR SuperClass;\n");
    fprintf(fp, "  LONG SuperMCCIndex;\n");
    fprintf(fp, "  ULONG (*GetSize)(void);\n");
    fprintf(fp, "  APTR Dispatcher;\n");
    fprintf(fp, "} MCCInfo[NUMBEROFCLASSES] =\n");
    fprintf(fp, "{\n");
    listindex = 0;
    do
      {
        goOn = 0;
        for(n = NULL; (n = list_getnext(classlist, n, (void **) &nextcd)); )
          {
            if(nextcd->finished == 0 && (nextcd->supernode == NULL || nextcd->supernode->index != -1))
              {
                if(nextcd->classdata != NULL)
                  fprintf(fp, "  { MUIC_%s, %s, %d, %sGetSize, ENTRY(%sDispatcher) },\n", nextcd->name, nextcd->superclass, nextcd->supernode != NULL ? nextcd->supernode->index : -1, nextcd->name, nextcd->name);
                else fprintf(fp, "  { MUIC_%s, %s, %d, NULL, ENTRY(%sDispatcher) },\n", nextcd->name, nextcd->superclass, nextcd->supernode != NULL ? nextcd->supernode->index : -1, nextcd->name);
                nextcd->index = listindex;
                listindex++;
                if(nextcd->supernode != NULL) goOn = 1;
                nextcd->finished = 1;
              }
          }
      }
    while(goOn == 1);
    if(listindex < classlist->cnt)
      {
        fprintf(fp, "#erreur %ld classes non-finies, possible dependance des boucles dans ces classes.", classlist->cnt-listindex);
        for(n = NULL; (n = list_getnext(classlist, n, (void **) &nextcd)); )
          {
            if(nextcd->finished == 0 && nextcd->supernode != NULL)
              fprintf(fp, " %s (sous-classe de %s)", nextcd->name, nextcd->supernode->name);
          }
        fprintf(fp, "\n");
      }
    fprintf(fp, "};\n");
    fprintf(fp, "\n");
    if(arg_storm) fprintf(fp, "///\n");

  /*****************************************/
  /*        Append support routines        */
  /*****************************************/

    gen_supportroutines(fp);
    fclose(fp);
    return(1);
}

int gen_header(char *destfile, struct list *classlist)
{
    FILE *fp;
    char *bn = arg_basename, *cn;
    struct classdef *nextcd;
    struct declaredef *nextdd;
    struct overloaddef *nextod;
    struct node *n, *nn;

    if(arg_v) fprintf(stdout, "Création d'en-tête   : %s\n", destfile);
    if((fp = fopen(destfile, "w")) == NULL)
      {
        fprintf(stderr, "ERREUR: Incapable d'ouvrir %s\n", destfile);
        return(0);
      }

  /***************************************/
  /*           Write header...           */
  /***************************************/

    gen_gpl(fp);
    fprintf(fp, "\n");
    fprintf(fp, "#ifndef CLASSES_CLASSES_H\n");
    fprintf(fp, "#define CLASSES_CLASSES_H\n");
    fprintf(fp, "\n");
    fprintf(fp, "/* " EDIT_WARNING " */\n");
    fprintf(fp, "\n");

  /***************************************/
  /*          Write includes...          */
  /***************************************/

    if(arg_includes[0])
      {
        char *nx, *p = arg_includes;

        do
          {
            if((nx = strchr(p, ',')) != NULL) *nx++ = 0;
            fprintf(fp, "#include \"%s\"\n", p);
          }
        while((p = nx) != NULL);
      }

  /***************************************/
  /*            Write misc...            */
  /***************************************/

    fprintf(fp, "\n");
    fprintf(fp, "#define NUMBEROFCLASSES %ld\n", classlist->cnt);
    fprintf(fp, "\n");
    fprintf(fp, "extern struct MUI_CustomClass *%sClasses[NUMBEROFCLASSES];\n", bn);
    fprintf(fp, "\n");

  /***************************************/
  /*             Class loop              */
  /***************************************/

    for(nn = NULL; (nn = list_getnext(classlist, nn, (void **) &nextcd)); )
      {
        cn = nextcd->name;
        fprintf(fp, "#include \"%s.h\"\n", cn);

    /***************************************/
    /* Write protos for this class         */
    /***************************************/

        if(nextcd->classdata != NULL) fprintf(fp, "ULONG %sGetSize(void);\n", cn);

    /* Write OVERLOADs */
        for(n = NULL; (n = list_getnext(&nextcd->overloadlist, n, (void **)&nextod));)
          {
            fprintf(fp, "ULONG m_%s_%s(struct IClass *cl, Object *obj, Msg msg);\n", nextcd->name, nextod->name);
          }
    /* Write DECLAREs */
        for(n = NULL; (n = list_getnext(&nextcd->declarelist, n, (void **)&nextdd));)
          {
            fprintf(fp, "ULONG m_%s_%s(struct IClass *cl, Object *obj, struct MUIP_%s_%s *msg);\n", nextcd->name, nextdd->name, cn, nextdd->name);
          }
        fprintf(fp, "\n");

      }
    fprintf(fp, "\n#endif /* CLASSES_CLASSES_H */\n\n");
    fclose(fp);
    return(1);
}

int gen_classheaders( struct list *classlist )
{
    struct node *n;
    struct node *nn;
    struct classdef *nextcd;
    struct declaredef *nextdd;
    struct exportdef *nexted;
    struct attrdef *nextad;
    struct includedef *nextid;
    FILE *fp;

    for(n = NULL; (n = list_getnext(classlist, n, (void **)&nextcd));)
      {
        char crcname[128];
        char *cn = nextcd->name;
        char *bn = arg_basename;
        unsigned long old_crc;
        unsigned long new_crc;

    /* get the previous CRC of the current class */
        snprintf(crcname, sizeof(crcname), "%s.crc", cn);
        myaddpart(arg_classdir, crcname, 255);
        old_crc = read_crc(arg_classdir);
        *mypathpart(arg_classdir) = 0;

    /* calculate the CRC of the current class */
        new_crc = gen_class_crc(nextcd, 0);

    /* generate a new header only if there was no CRC before
     * or if the old CRC does not match the new one
     */
        if(old_crc == INVALID_CRC || old_crc != new_crc)
          {
            char name[128], buf[128], *p;

            snprintf(name, sizeof(name), "%s_cl.h", cn);
            myaddpart(arg_classdir, name, 255);
            if(arg_v) fprintf(stdout, "Creation de l'en-tête de la classe privée: %s\n", arg_classdir);
            if((fp = fopen(arg_classdir, "w")) == NULL)
              {
                fprintf(stderr, "WARNING: Incapable d'écrire %s\n", name);
		*mypathpart(arg_classdir) = 0;
                continue;
              }
            strncpy(buf, cn, 127);
            for(p = buf; *p; p++) *p = toupper(*p);
            *mypathpart(arg_classdir) = 0;

      /* write the gpl to this class header also */
            gen_gpl(fp);
            fprintf(fp, "\n");
            fprintf(fp, "/* " EDIT_WARNING " */\n");
            fprintf(fp, "\n");
            fprintf(fp, "#ifndef %s_CL_H\n", buf);
            fprintf(fp, "#define %s_CL_H 1\n", buf);
            fprintf(fp, "\n");
            fprintf(fp, "#ifndef CLIB_ALIB_PROTOS_H\n");
            fprintf(fp, "  #include <clib/alib_protos.h>\n");
            fprintf(fp, "#endif\n");
            fprintf(fp, "#ifndef PROTO_EXEC_H\n");
            fprintf(fp, "  #include <proto/exec.h>\n");
            fprintf(fp, "#endif\n");
            fprintf(fp, "#ifndef PROTO_INTUITION_H\n");
            fprintf(fp, "  #include <proto/intuition.h>\n");
            fprintf(fp, "#endif\n");
            fprintf(fp, "#ifndef PROTO_UTILITY_H\n");
            fprintf(fp, "  #include <proto/utility.h>\n");
            fprintf(fp, "#endif\n");
            fprintf(fp, "#ifndef EXEC_TYPES_H\n");
            fprintf(fp, "  #include <exec/types.h>\n");
            fprintf(fp, "#endif\n");
            fprintf(fp, "#ifndef LIBRARIES_MUI_H\n");
            fprintf(fp, "  #include <libraries/mui.h>\n");
            fprintf(fp, "#endif\n");
            fprintf(fp, "#ifndef CLASSES_CLASSES_EXTRA_H\n");
            fprintf(fp, "  #include \"ClassesExtra.h\"\n");
            fprintf(fp, "#endif\n");
            fprintf(fp, "#ifndef EXTRASRC_H\n");
            fprintf(fp, "  #include \"Extrasrc.h\"\n");
            fprintf(fp, "#endif\n");
            fprintf(fp, "#include \"%s/%s.h\"\n", arg_classdir, cn);
            fprintf(fp, "\n");

            fprintf(fp, "#define inittags(msg)    (((struct opSet *)msg)->ops_AttrList)\n");
            fprintf(fp, "#define GETDATA          struct Data *data = (struct Data *)INST_DATA(cl,obj)\n");
            fprintf(fp, "#define DECLARE(method)  ULONG m_%s_## method(UNUSED struct IClass *cl, UNUSED Object *obj, UNUSED struct MUIP_%s_## method *msg )\n", cn, cn);
            fprintf(fp, "#define OVERLOAD(method) ULONG m_%s_## method(UNUSED struct IClass *cl, UNUSED Object *obj, UNUSED Msg msg )\n", cn);
            fprintf(fp, "#define METHOD(method)   MUIM_%s_## method\n", cn);
            fprintf(fp, "#define ATTR(attr)       MUIA_%s_## attr\n", cn);
            fprintf(fp, "\n");

            if(nextcd->classdata != NULL)
              {
                fprintf(fp, "/* Exported CLASSDATA */\n");
                fprintf(fp, "\n");
                fprintf(fp, "struct Data\n");
                fprintf(fp, "{\n");
                fprintf(fp, "%s\n", nextcd->classdata);
                fprintf(fp, "};\n");
                fprintf(fp, "\n");
                fprintf(fp, "ULONG %sGetSize( void ) { return sizeof(struct Data); }\n", cn);
              }

            fprintf(fp, "\n");
            fprintf(fp, "#endif /* %s_CL_H */\n", buf);
            fclose(fp);
            snprintf(name, sizeof(name), "%s.h", cn);
             myaddpart(arg_classdir, name, 255);
            if(arg_v) fprintf(stdout, "Création de l'entête de la classe publique: %s\n", arg_classdir);
            if((fp = fopen(arg_classdir, "w")) == NULL)
              {
                fprintf(stderr, "WARNING: Incapable d'écrire %s\n", name);
                *mypathpart(arg_classdir) = 0;
                continue;
              }
            strncpy(buf, cn, 127);
            for (p = buf; *p; p++) *p = toupper(*p);

      /* write the gpl to this class header also */
            gen_gpl(fp);
            fprintf(fp, "\n");
            fprintf(fp, "/* " EDIT_WARNING " */\n");
            fprintf(fp, "\n");

            fprintf(fp, "#ifndef MUI_%s_H\n", buf);
            fprintf(fp, "#define MUI_%s_H 1\n", buf);
            fprintf(fp, "\n");

            fprintf(fp, "#ifndef SDI_COMPILER_H\n");
            fprintf(fp, "  #include \"SDI_compiler.h\"\n");
            fprintf(fp, "#endif\n");
            fprintf(fp, "\n");

      /*************************************************/
      /* Write include statements text for this class  */
      /*************************************************/

            if(nextcd->includelist.cnt)
              {
                fprintf(fp, "/* Exported includes */\n\n");
                for(nn = NULL; (nn = list_getnext(&nextcd->includelist, nn, (void **) &nextid));)
                    fprintf(fp, "%s\n\n", nextid->includetext);
              }
            fprintf(fp, "Object * %s_NewObject(CONST_STRPTR className, ...);\n", bn);
            fprintf(fp, "\n");
            fprintf(fp, "#define MUIC_%s \"%s_%s\"\n", cn, bn, cn);
            fprintf(fp, "\n");
            fprintf(fp, "#define %sObject %s_NewObject(MUIC_%s\n", cn, bn, cn);
            fprintf(fp, "\n");
            for(nn = NULL; (nn = list_getnext(&nextcd->declarelist, nn, (void **) &nextdd));)
              {
                char name[128];

                snprintf(name, sizeof(name), "MUIM_%s_%s", cn, nextdd->name);
                fprintf(fp, "#define %-45s 0x%08x\n", name, gettagvalue(cn, 1));
              }

      /***************************************/
      /* Write attributes for this class     */
      /***************************************/

            for(nn = NULL; (nn = list_getnext(&nextcd->attrlist, nn, (void **) &nextad));)
              {
                char name[128];

                snprintf(name, sizeof(name), "MUIA_%s_%s", cn, nextad->name);
                fprintf(fp, "#define %-45s 0x%08x\n", name, gettagvalue(cn, 1));
              }
            fprintf(fp, "\n");

      /***************************************/
      /* Write exported text for this class  */
      /***************************************/

            if(nextcd->exportlist.cnt)
              {
                fprintf(fp, "/* Exported text */\n\n");
                for (nn = NULL; (nn = list_getnext(&nextcd->exportlist, nn, (void **) &nexted));)
                  fprintf(fp, "%s\n\n", nexted->exporttext);
              }

      /*****************************************/
      /* Write MUIP_ structures for this class */
      /*****************************************/

            for(nn = NULL; (nn = list_getnext(&nextcd->declarelist, nn, (void **) &nextdd));)
              {
                fprintf(fp, "struct MUIP_%s_%s\n", cn, nextdd->name);
                fprintf(fp, "{\n");
                fprintf(fp, "  ULONG methodID;\n");

                if(strlen(nextdd->params) > 0)
                  {
                    char *lp;

                    for(p = lp = nextdd->params;;)
                      {
                        if((p = strpbrk(lp, ",;")) != NULL)
                          {
                            *p++ = '\0';
                            fprintf(fp, "  %s;\n", lp);
                            lp = p;
                          }
                        else
                          {
                            if(strlen(lp) > 0) fprintf(fp, "  %s;\n", lp);
                            break;
                          }
                      }
                  }
                fprintf(fp, "};\n");
                fprintf(fp, "\n");
              }
            fprintf(fp, "#endif /* MUI_%s_H */\n", buf);
            fclose(fp);
            *mypathpart(arg_classdir) = 0;
          }
        snprintf(crcname, sizeof(crcname), "%s.crc", cn);
        myaddpart(arg_classdir, crcname, 255);
        write_crc(arg_classdir, new_crc);
        *mypathpart(arg_classdir) = 0;
      }
    return(1);
}

/*******************************************************************************
 *
 * Makefile generation
 *
 *
 * .p.s. I'm not an expert on makefiles, so someone improve this. ;-)
 *
 *******************************************************************************
 *
 *
 */

int gen_makefile( char *destfile, struct list *classlist )
{
    struct classdef *nextcd;
    char *cn, *p;
    FILE *fp;
    struct node *n;

    for(p = arg_mkfile_ccopts;;)
      {
        if((p = strchr(p, ','))) *p++ = ' ';
        else break;
      }

    if(arg_v) fprintf(stdout, "Creating makefile    : %s\n", destfile);
    if((fp = fopen(destfile, "w")) == NULL)
      {
        fprintf(stderr, "ERROR: Unable to open %s\n", destfile);
        return(0);
      }
    fprintf(fp, "#\n"
          "# " EDIT_WARNING "\n"
          "#\n");
    fprintf(fp, "\nCC = %s\n"
          "CCOPTS = %s\n\n", arg_mkfile_cc, arg_mkfile_ccopts);
    fprintf(fp, "all.o : %s ", OBJECT_NAME);
    for(n = NULL; (n = list_getnext(classlist, n, (void **) &nextcd));)
      {fprintf(fp, "%s.o ", nextcd->name);}
      fprintf(fp, "\n\tjoin " OBJECT_NAME " ");
    for(n = NULL; (n = list_getnext(classlist, n, (void **) &nextcd));)
      {fprintf(fp, "%s.o ", nextcd->name); }
      fprintf(fp, "AS all.o\n\n");
      fprintf(fp, "%s : %s %s ", OBJECT_NAME, SOURCE_NAME, HEADER_NAME);
      fprintf(fp, "\n\t$(CC) %s %s %s $(CCOPTS)\n\n", SOURCE_NAME, arg_mkfile_outarg, OBJECT_NAME);
    for(n = NULL; (n = list_getnext(classlist, n, (void **) &nextcd));)
      {
        cn = nextcd->name;
        fprintf(fp, "%s.o : %s.c\n"
              "\t$(CC) %s.c %s %s.o $(CCOPTS)\n\n", cn, cn, cn, arg_mkfile_outarg, cn);
      }
    fclose(fp);
    return(1);
}

/*******************************************************************************
 *
 * Argument processing and entry point
 *
 *******************************************************************************
 *
 *
 */

int getstrarg(char *argid, char *argline, char *dest, size_t destlen)
{
    char *p;
    size_t arglen = strlen(argid);
    if(strncmp(argid, argline, arglen) != 0) return(0);
    p = &argline[arglen];
    strncpy(dest, p, destlen);
    return(1);
}

int getblnarg(char *argid, char *argline, int *blnlong)
{
    if(strncmp(argid, argline, strlen(argid)) != 0) return(0);
    *blnlong = 1;
    return(1);
}

int doargs(unsigned int argc, char *argv[])
{
    unsigned int i;
    int success;

    arg_gpl = 0;
    for(i = 1; i < argc; i++)
      {
        if(getstrarg("-b",      argv[i], arg_basename, 255)) continue;
        if(getstrarg("-i",      argv[i], arg_includes, 255)) continue;
        if(getblnarg("-gpl",    argv[i], &arg_gpl         )) continue;
        if(getblnarg("-storm",  argv[i], &arg_storm       )) continue;
        if(getstrarg("-mkfile", argv[i], arg_mkfile,   255)) continue;
        if(getblnarg("-v",      argv[i], &arg_v           )) continue;
        if(getblnarg("-verbose",argv[i], &arg_v           )) continue;
        if(getblnarg("-q",      argv[i], &arg_q           )) continue;
      }

    if(arg_q == 0 || argc < 2) fprintf(stdout, "GenClasses v%s by Andrew Bell <mechanismx@lineone.net>\n\n", verstr);
    if(argc < 2)
      {
        fprintf(stderr,
            "Usage: GenClasses <classdir> -b<basename> <options>\n"
            "\n"
            "Options:\n"
            "\n"
            " -b<basename>                                 - basename used in sources (required)\n"
            " -gpl                                         - write GPL headers into sources\n");
        fprintf(stderr,
            " -storm                                       - include storm/GoldED fold markers\n"
            " -i<includes>                                 - includes for Classes.h (.i.e. -i\"Kitty.h\",\"Kitty_hook.h\",<stdlib.h>\n"
            " -v                                           - verbose output while generating files\n"
            " -q                                           - keep output quiet and only output errors\n"
            " -mkfile<makefile>,<cc>,<outarg>,<ccopts,...> - Create a makefile\n"
            "    (.i.e. -mkfileVMakefile,vc,-o,-O3,-+)\n");
        return(0);
      }

    success = 1;
    strncpy(arg_classdir, argv[1], 255);
    if(arg_classdir[0] == 0 || arg_classdir[0] == '-')
      {
        fprintf(stderr, "Pas de répertoire de classes spécifié, utilisation du répertoire courant.\n");
        strcpy(arg_classdir, "");
      }
    if(!arg_basename[0])
      {
        success = 0;
        fprintf(stderr, "ERREUR: Vous DEVEZ provide a basename en utilisant l'argument -b\n");
      }

    if(arg_mkfile[0])
      {
        if((arg_mkfile_cc     = strchr(arg_mkfile,        ',')))  *arg_mkfile_cc++     = 0;
        if(arg_mkfile_cc)
          {
            if((arg_mkfile_outarg = strchr(arg_mkfile_cc,     ','))) *arg_mkfile_outarg++ = 0;
          }
        if(arg_mkfile_outarg)
          {
            if((arg_mkfile_ccopts = strchr(arg_mkfile_outarg, ','))) *arg_mkfile_ccopts++ = 0;
          }
        if(!arg_mkfile_cc)     arg_mkfile_cc     = "cc";
        if(!arg_mkfile_outarg) arg_mkfile_outarg = "-o";
        if(!arg_mkfile_ccopts) arg_mkfile_ccopts = "";
        strncpy(arg_mkfile_dest,   arg_classdir, 255);
        myaddpart(arg_mkfile_dest, arg_mkfile,   255);
      }
  return(success);
}

int main(int argc, char *argv[])
{
    struct node *n;
    struct list classlist;

    list_init(&classlist);
    if(!doargs(argc, argv)) return(0);
    if((collision_cnts = calloc(BUCKET_CNT, sizeof(int))) == NULL) return(0);
    if(scanclasses(arg_classdir, &classlist))
      {
        unsigned long old_crc;
        unsigned long new_crc;

        buildclasstree(&classlist);
        myaddpart(arg_classdir, CRC_NAME, 255);
        old_crc = read_crc(arg_classdir);
        *mypathpart(arg_classdir) = 0;
        new_crc = gen_crc(&classlist);
        if(old_crc == INVALID_CRC || old_crc != new_crc)
          {
            myaddpart(arg_classdir, SOURCE_NAME, 255);
            gen_source(arg_classdir, &classlist);
            *mypathpart(arg_classdir) = 0;
            myaddpart(arg_classdir, HEADER_NAME, 255);
            gen_header(arg_classdir, &classlist);
            *mypathpart(arg_classdir) = 0;
            gen_classheaders(&classlist);
          }
        myaddpart(arg_classdir, CRC_NAME, 255);
        write_crc(arg_classdir, new_crc);
        *mypathpart(arg_classdir) = 0;
        if(arg_mkfile_dest[0]) gen_makefile(arg_mkfile_dest, &classlist);
      }
    while((n = list_remhead(&classlist)) != NULL)
      {
        free_classdef(n->data);
        free(n);
      }
    return(0);
}
