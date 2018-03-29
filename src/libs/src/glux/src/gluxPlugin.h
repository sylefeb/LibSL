// --------------------------------------------------------
// Author: Sylvain.Lefebvre@sophia.inria.fr
// -------------------------------------------------------- 

#ifndef __GLUX_PLUGIN__
#define __GLUX_PLUGIN__ 

// -------------------------------------------------------- 

#include "gluxLoader.h" 

// -------------------------------------------------------- 

#ifndef GLUX_NO_OUTPUT  
# include <iostream>
#endif 

// -------------------------------------------------------- 

#define GL_GLEXT_LEGACY
#define GLX_GLXEXT_LEGACY
#define WGL_WGLEXT_LEGACY 

// -------------------------------------------------------- 

void *GLUX_LOAD_PROC(const char *name);

#ifdef __APPLE__
#define APIENTRY
#endif

#define APIENTRYP APIENTRY * 

// -------------------------------------------------------- 

namespace glux
{
  class gluxPlugin
  {
  protected:

    const char *m_szIdString;
    bool        m_bAvailable;
    bool        m_bRequired;
    bool        m_bInitDone;
    bool        m_bDisabled;
    bool        m_bDevel;
    gluxPlugin *m_Linked;

  public:

    gluxPlugin(bool required);
    virtual ~gluxPlugin() {}

    const char  *getIdString() const;
    bool         isAvailable();
    bool         isRequired();
    void         linkTo(gluxPlugin *);
    bool         init(int flags = 0);
    bool         isDevel() const { return (m_bDevel); }
    bool         isDisabled() const;
    void         setDisabled(bool b);
    virtual bool load();
  };
}

// -------------------------------------------------------- 

#define GLUX_NEW_PLUGIN(idstr)      \
namespace glux \
{ \
    class gluxPlugin_##idstr : public gluxPlugin \
    { \
      public: \
      gluxPlugin_##idstr(bool b) : gluxPlugin(b) \
    	{ \
	        m_szIdString=#idstr; \
          registerPlugin(this); \
      } \
      virtual ~gluxPlugin_##idstr() {} \
      bool load(); \
    }; \
} 

// -------------------------------------------------------- 

#define GLUX_EMPTY_PLUGIN(idstr)      \
namespace glux \
{ \
    class gluxPlugin_##idstr : public gluxPlugin \
    { \
      public: \
      gluxPlugin_##idstr(bool b) : gluxPlugin(b) \
    	{ \
	        m_szIdString=#idstr; \
          registerPlugin(this); \
      } \
      virtual ~gluxPlugin_##idstr() {} \
    }; \
} 

// -------------------------------------------------------- 

#define GLUX_LOAD(idstr)    static glux::gluxPlugin_##idstr glux_plugin_##idstr(false); 
#define GLUX_REQUIRE(idstr) static glux::gluxPlugin_##idstr glux_plugin_##idstr(true);

// -------------------------------------------------------- 

#define GLUX_PLUGIN_LOAD(idstr) bool glux::gluxPlugin_##idstr::load() 

#ifdef LIBSL_OPENGL_CORE_PROFILE
#define GLUX_CHECK_EXTENSION_STRING(ext) (std::find(g_glcore_extensions.begin(), g_glcore_extensions.end(), ext) == g_glcore_extensions.end())
#else
#define GLUX_CHECK_EXTENSION_STRING(ext) (strstr((const char *)glGetString(GL_EXTENSIONS),ext) == NULL) 
#endif

// WGL extensions require special treatment
#define GLUX_CHECK_WGL_EXTENSION_STRING(ext)       \
  (                                                \
     (g_glux__wglExtensions == NULL)               \
  || (strstr(g_glux__wglExtensions, ext) == NULL)  \
  )

// -------------------------------------------------------- 
#endif 
// -------------------------------------------------------- 
