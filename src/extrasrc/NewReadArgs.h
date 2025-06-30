/***************************************************************************
Kitty MorphOS
 Copyright (C) 2022 digitally
***************************************************************************/
#ifndef NEWREADARGS_H
#define NEWREADARGS_H

#ifndef WORKBENCH_WORKBENCH_H
#include <workbench/workbench.h>
#endif
#ifndef WORKBENCH_STARTUP_H
#include <workbench/startup.h>
#endif

struct NewRDArgs
{
    STRPTR Template;        // ReadArgs-like template
    STRPTR ExtHelp;         // ExtHelp string or NULL, shell ONLY
    STRPTR Window;          // WB window descriptor, eg. "CON:////Test"
    IPTR   *Parameters;     // array to store parsed parameters
    LONG   FileParameter;   // -1 means none, 0 means all
    LONG   PrgToolTypesOnly;
    struct RDArgs *RDArgs;    // RDArgs we give to ReadArgs()
    struct RDArgs *FreeArgs;  // RDArgs we get from ReadArgs()
    STRPTR *Args;
    ULONG MaxArgs;
    STRPTR ToolWindow;
    APTR Pool;
    BPTR WinFH;     // i/o window stream
    BPTR OldInput;  // old i/o streams
    BPTR OldOutput;
};

#endif /* NEWREADARGS_H */
