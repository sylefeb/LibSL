// --------------------------------------------------------
// Author: Sylvain.Lefebvre@sophia.inria.fr
// --------------------------------------------------------
#include "gluxLoader.h"
#include "gluxPlugin.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
// --------------------------------------------------------

using namespace glux;
using namespace std;

// --------------------------------------------------------

// string of wgl extensions
const char *g_glux__wglExtensions = NULL;

// core profile extensions
std::set<std::string>  g_glux__glExtensions;

// --------------------------------------------------------

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
// typedef of wglGetExtensionsStringARB
typedef const char * (APIENTRYP t_wglGetExtensionsStringARB) (HDC hdc);
#endif

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// --------------------------------------------------------

// for core profile compatibility
#define GL_NUM_EXTENSIONS                 0x821D // how to avoid this?
#define GL_EXTENSIONS                     0x1F03 // how to avoid this?
typedef const GLubyte *(APIENTRYP t_glGetStringi) (GLenum name, GLuint index);

// --------------------------------------------------------

#include <set>
#include <fstream>

// --------------------------------------------------------

static map<string,gluxPlugin *> *s_Plugins     = NULL;
static bool                      s_Initialized = false;

// --------------------------------------------------------

void glux::registerPlugin(gluxPlugin *pl)
{
  if (s_Plugins == NULL) {
    s_Plugins = new map<string,gluxPlugin *>;
  }

  map<string,gluxPlugin *>::iterator P=s_Plugins->find(pl->getIdString());

  if (P == s_Plugins->end()) {
    (*s_Plugins)[pl->getIdString()] = pl;
  } else {
    pl->linkTo((*P).second);
  }
}

// --------------------------------------------------------

void glux::init(int flags,const char *profile)
{
  if (s_Initialized) return;
  s_Initialized = true;

  bool all_ok      = true;
  bool use_profile = false;
  stringstream strout;
  set<string>  ext_profile;

  // MessageBoxA(NULL,(const char *)glGetString(GL_EXTENSIONS), "", MB_OK);

  strout << "-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
  strout << "      glux v1.96"          << endl;
  strout << "-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;

  // check if OpenGL is initialized
  if (glGetString(GL_VENDOR) == NULL) {
    strout << "OpenGL should be initialized before calling gluxInit() !" << endl;
#ifndef GLUX_NO_OUTPUT
    cerr << strout.str() << endl;
#if defined(_WIN32) || defined(_WIN64)
    MessageBox(NULL,strout.str().c_str(),"gluX - Fatal error",MB_OK | MB_ICONSTOP);
#endif
#endif
    exit (-1);
  }

	// output strings
  strout << "Vendor   string: " << glGetString(GL_VENDOR)   << std::endl;
	strout << "Renderer string: " << glGetString(GL_RENDERER) << std::endl;
  strout << "Version  string: " << glGetString(GL_VERSION)  << std::endl;

  // read profile
  if (profile != NULL) {
    ifstream fprof(profile);
    strout << endl;
    strout << "-=-=-=-=-=-=-=-=-=-=-=-=-"   << endl;
    strout << "  Using profile " << profile << endl;
    strout << "-=-=-=-=-=-=-=-=-=-=-=-=-"   << endl;
    while (fprof) {
      static char str[512];
      fprof.getline(str,512);
      if (strlen(str) > 0) {
        if (str[0] != '#') {
          use_profile=true;
          ext_profile.insert(string(str));
        } else {
          strout << str << endl;
        }
      }
    }
    strout << "-=-=-=-=-=-=-=-=-=-=-=-=-" << endl << endl << endl;
  }

#if defined(_WIN32) || defined(_WIN64)
  // read wgl extension string (if possible)
  t_wglGetExtensionsStringARB __wglGetExtensionsStringARB;
  __wglGetExtensionsStringARB = (t_wglGetExtensionsStringARB)GLUX_LOAD_PROC("wglGetExtensionsStringARB");
  if (__wglGetExtensionsStringARB != NULL)
    g_glux__wglExtensions = __wglGetExtensionsStringARB(wglGetCurrentDC());
#endif

  // attempt to load extension list with getStringi
  t_glGetStringi __glGetStringi = NULL;
  __glGetStringi = (t_glGetStringi)GLUX_LOAD_PROC("glGetStringi");
  if (__glGetStringi != NULL) {
    int num_extensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
    for (int i = 0; i < num_extensions; i++) {
      g_glux__glExtensions.insert(std::string((const char*)__glGetStringi(GL_EXTENSIONS, i)));
    }
  }

  // check if plugins are available
  if (s_Plugins != NULL) {
    strout << "-> initializing extensions" << endl;
    // parse extensions and call init
    for (map<string,gluxPlugin *>::iterator I=s_Plugins->begin(); I!=s_Plugins->end();I++) {
      bool ok = validateExtension((*I),flags,use_profile,profile,ext_profile,strout);
      if ((*I).second->isRequired()) {
        all_ok &= ok;
      }
    }
  } else {
    // no plugins explicitly loaded
    strout << "-> No Plugin loaded: you have to use GLUX_LOAD or GLUX_REQUIRE before using extensions." << endl;
  }

  if (!all_ok) {
    strout << "Failed to load at least one required extension !" << endl;
#if defined(_WIN32) || defined(_WIN64)
    MessageBox(NULL,strout.str().c_str(),"Sorry, gluX cannot find required extensions",MB_OK | MB_ICONSTOP);
#else
    cerr << strout.str() << endl;
#endif
    exit (-1);
  }

#ifndef GLUX_NO_OUTPUT
  strout << "-=-=-=-=-=-=-=-=-=-=-=-=-" << endl << endl;
  cerr   << strout.str();
#endif

}

// --------------------------------------------------------

bool glux::validateExtension(const pair<string,gluxPlugin*>& ext,int flags,
                             bool use_profile,const char *profile,const set<string>& ext_profile,
                             stringstream& strout)
{
  // init extension
  bool r = ext.second->init(flags);

  // available ?
  if (r) { // -> yes

    // check against profile
    if (use_profile) {
      set<string>::const_iterator P = ext_profile.find(ext.first);
      if (P == ext_profile.end()) {
        ext.second->setDisabled(true);
        r = false;
      }
    }

    // message
    if (ext.second->isDisabled()) {
      strout << "[XXXX]";
    } else if (ext.second->isDevel()) {
      strout << "[DEVL]";
    } else {
      strout << "[ OK ]";
    }

  } else { // -> no, not available

    strout  << "[FAIL]";
    // check against profile
    if (use_profile) {
      set<string>::const_iterator P = ext_profile.find(ext.first);
      if (P != ext_profile.end()) {

        strout << " " << ext.first << endl << endl;
        strout << "Cannot fully support profile " << profile << " : " << endl
          << "Extension " << ext.first
          << " not supported." << endl << endl;
        strout << strout.str() << endl;
#if defined(_WIN32) || defined(_WIN64)
        MessageBox(NULL,strout.str().c_str(),"Sorry, gluX cannot find required extensions",MB_OK | MB_ICONSTOP);
#else
        cerr << strout.str() << endl;
#endif
        exit (-1);

      }
    }
  }

  strout   << " " << ext.first;
  if (ext.second->isRequired()) {
    strout << " (required)" << endl;
  } else {
    strout << endl;
  }

  return (r);
}

// --------------------------------------------------------

void glux::shutdown()
{
  if (s_Plugins != NULL) {
    delete (s_Plugins);
  }
  s_Plugins = NULL;
}

// --------------------------------------------------------

void gluxInit(int flags,const char *profile)
{
  glux::init(flags,profile);
}

// --------------------------------------------------------

void gluxShutdown()
{
  glux::shutdown();
}

// --------------------------------------------------------

void gluxInit()
{
  gluxInit(0,NULL);
}

// --------------------------------------------------------

void gluxInit(int flags)
{
  gluxInit(flags,NULL);
}

// --------------------------------------------------------

int gluxIsAvailable(const char *s)
{
  map<string,gluxPlugin *>::iterator P;

  if (s_Plugins == NULL) {
    return (GLUX_NOT_LOADED);
  }
  P = s_Plugins->find(string(s));
  if (P == s_Plugins->end()) {
    return (GLUX_NOT_LOADED);
  } else {
    if ((*P).second->isAvailable()) {
      return (GLUX_AVAILABLE);
    } else {
      return (GLUX_NOT_AVAILABLE);
    }
  }
}

// --------------------------------------------------------

bool gluxIsExtensionAvailable(const char* s)
{
  return g_glux__glExtensions.find(s) != g_glux__glExtensions.end();
}

// --------------------------------------------------------

int gluxIsDevl(const char *s)
{
  map<string,gluxPlugin *>::iterator P;

  if (s_Plugins == NULL) {
    return (GLUX_NOT_LOADED);
  }
  P = s_Plugins->find(string(s));
  if (P == s_Plugins->end()) {
    return (GLUX_NOT_LOADED);
  } else {
    if ((*P).second->isDevel()) {
      return (GLUX_DEVL);
    } else {
      return (GLUX_NOT_DEVL);
    }
  }
}

// --------------------------------------------------------
