// --------------------------------------------------------
// Author: Sylvain.Lefebvre@sophia.inria.fr
// -------------------------------------------------------- 

#include "gluxPlugin.h" 

#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace glux; 

// -------------------------------------------------------- 
// Load proc address
// -------------------------------------------------------- 

#ifdef WIN32 

# include <windows.h>
void* GLUX_LOAD_PROC(const char *name)
{
  return ::wglGetProcAddress(name);
}

#else 

#ifdef __APPLE__

// Credits to GLEW 1.7

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AvailabilityMacros.h>
#include <dlfcn.h>

void* dlGetProcAddress (const char* name)
{
  static void* h = NULL;
  static void* gpa;

  if (h == NULL)
  {
    if ((h = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL)) == NULL) return NULL;
    gpa = dlsym(h, "glXGetProcAddress");
  }

  if (gpa != NULL)
    return ((void*(*)(const unsigned char*))gpa)((const unsigned char*)name);
  else
    return dlsym(h, (const char*)name);
}

#ifdef MAC_OS_X_VERSION_10_3

void* GLUX_LOAD_PROC(const char *name)
{
  static void* image = NULL;
  if (NULL == image) 
  {
    image = dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);
  }
  if( !image ) return NULL;
  void* addr = dlsym(image, (const char*)name);
  if( addr ) return addr;
  return dlGetProcAddress( name ); // try next for glx symbols
}
#else

#include <mach-o/dyld.h>

void* GLUX_LOAD_PROC(const char *name)
{
  static const struct mach_header* image = NULL;
  NSSymbol symbol;
  char* symbolName;
  if (NULL == image)
  {
    image = NSAddImage("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", NSADDIMAGE_OPTION_RETURN_ON_ERROR);
  }
  /* prepend a '_' for the Unix C symbol mangling convention */
  symbolName = malloc(strlen((const char*)name) + 2);
  strcpy(symbolName+1, (const char*)name);
  symbolName[0] = '_';
  symbol = NULL;
  /* if (NSIsSymbolNameDefined(symbolName))
	 symbol = NSLookupAndBindSymbol(symbolName); */
  symbol = image ? NSLookupSymbolInImage(image, symbolName, NSLOOKUPSYMBOLINIMAGE_OPTION_BIND | NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR) : NULL;
  free(symbolName);
  if( symbol ) return NSAddressOfSymbol(symbol);
  return dlGetProcAddress( name ); // try next for glx symbols
}
#endif /* MAC_OS_X_VERSION_10_3 */

#else  /* not __APPLE__  */

#ifdef EMSCRIPTEN /* Emscripten */

void* GLUX_LOAD_PROC(const char *name)
{
  return (void*)NULL;
}

#else /* Unix */

# define APIENTRY
# include <GL/glx.h>
void* GLUX_LOAD_PROC(const char *name)
{
  return (void*)::glXGetProcAddressARB((GLubyte *)name);
}
# define GLX_GLXEXT_PROTOTYPES
# define GLX_GLXEXT_LEGACY

#endif

#endif

#endif 

// -------------------------------------------------------- 

gluxPlugin::gluxPlugin(bool required)
{
  m_szIdString = "[unknown]";
  m_bAvailable = false;
  m_bInitDone  = false;
  m_bRequired  = required;
  m_Linked     = NULL;
  m_bDevel     = false;
  m_bDisabled  = false;
} 

// -------------------------------------------------------- 

const char *gluxPlugin::getIdString() const
{
  return (m_szIdString);
} 

// -------------------------------------------------------- 

bool gluxPlugin::isAvailable()
{
  if (m_Linked != NULL) {
    return m_Linked->isAvailable();
  } else {
    if (!m_bInitDone) {
      std::cerr << "gluX - FATAL ERROR (1) - please send a bug report" << std::endl;
      exit (-1);
    }
    return (m_bAvailable && (!isDisabled()));
  }
}

// -------------------------------------------------------- 

bool gluxPlugin::isRequired()
{
  return (m_bRequired);
}

// -------------------------------------------------------- 

void gluxPlugin::linkTo(gluxPlugin *l)
{
  m_Linked       = l;
  bool required  = m_bRequired | l->m_bRequired;
  m_bRequired    = required;
  l->m_bRequired = required;
  // NOTE: this is not a chain, ie. l->m_Linked == NULL
  if (m_Linked->m_Linked != NULL) {
    std::cerr << "gluX - FATAL ERROR (2) - please send a bug report" << std::endl;
    exit (-1);
  }
}

// -------------------------------------------------------- 

bool gluxPlugin::init(int flags)
{
  if (m_bInitDone) {
    return (m_bAvailable);
  }
  m_bInitDone = true;

  if (m_Linked != NULL) {
    if (!m_Linked->m_bInitDone) {
      m_Linked->init(flags);
    }
    m_bAvailable = m_Linked->isAvailable();
  } else {
    m_bAvailable = load();
  }
  m_bAvailable  = m_bAvailable && (!isDevel() || (flags & GLUX_DEVL));
  return (m_bAvailable);
} 

// -------------------------------------------------------- 

bool gluxPlugin::load() 
{
  return (true);
}

// --------------------------------------------------------

bool gluxPlugin::isDisabled() const
{
  if (m_Linked != NULL) {
    return (m_Linked->isDisabled());
  } else {
    return (m_bDisabled);
  }
}

// -------------------------------------------------------- 

void gluxPlugin::setDisabled(bool b)
{
  m_bDisabled = b;
}

// --------------------------------------------------------
