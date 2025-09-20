// see https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_entrypts/opengl_entrypts.html

#import <mach-o/dyld.h>
#import <stdlib.h>
#import <string.h>

void * MyNSGLGetProcAddress (const char *name)
{
    NSSymbol symbol;
    char *symbolName;
    symbolName = malloc (strlen (name) + 2); // 1
    strcpy(symbolName + 1, name); // 2
    symbolName[0] = '_'; // 3
    symbol = NULL;
    if (NSIsSymbolNameDefined (symbolName)) // 4
        symbol = NSLookupAndBindSymbol (symbolName);
    free (symbolName); // 5
    return symbol ? NSAddressOfSymbol (symbol) : NULL; // 6
}
