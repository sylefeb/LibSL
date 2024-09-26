/* --------------------------------------------------------------------
Author: Sylvain Lefebvre    sylvain.lefebvre@sophia.inria.fr

Simple Library for Graphics (LibSL)

This software is a computer program whose purpose is to offer a set of
tools to simplify programming real-time computer graphics applications
under OpenGL and DirectX.

This software is governed by the CeCILL-C license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-C
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-C license and that you accept its terms.
-------------------------------------------------------------------- */
#include "LibSL.precompiled.h"
//---------------------------------------------------------------------------

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include <LibSL/Errors/Errors.h>
#include <LibSL/UIHelpers/SimpleUI.h>
#include <LibSL/UIHelpers/BindImGui.h>

#include <imgui.h>

#define NAMESPACE LibSL::UIHelpers::SimpleUI

#ifdef ANDROID
#include <dlfcn.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

typedef void (GL_APIENTRYP PFNGLBINDVERTEXARRAYOESPROC) (GLuint array);
typedef void (GL_APIENTRYP PFNGLDELETEVERTEXARRAYSOESPROC) (GLsizei n, const GLuint *arrays);
typedef void (GL_APIENTRYP PFNGLGENVERTEXARRAYSOESPROC) (GLsizei n, GLuint *arrays);
typedef GLboolean (GL_APIENTRYP PFNGLISVERTEXARRAYOESPROC) (GLuint array);

PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays;
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays;
PFNGLISVERTEXARRAYOESPROC glIsVertexArray;
void androidInitGLEXT()
{
    void *libhandle = dlopen("libGLESv2.so", RTLD_LAZY);
    glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC) dlsym(libhandle, "glBindVertexArrayOES");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC) dlsym(libhandle,
                                                                   "glDeleteVertexArraysOES");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC) dlsym(libhandle, "glGenVertexArraysOES");
    glIsVertexArray = (PFNGLISVERTEXARRAYOESPROC) dlsym(libhandle, "glIsVertexArrayOES");

}
#endif

// ----------------------------------------------------

static void ImGui_generic_init()
{
  ImGuiIO& io = ::ImGui::GetIO();
  // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
  io.KeyMap[ImGuiKey_Tab] = '\t';
  io.KeyMap[ImGuiKey_LeftArrow] = LIBSL_KEY_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = LIBSL_KEY_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = LIBSL_KEY_UP;
  io.KeyMap[ImGuiKey_DownArrow] = LIBSL_KEY_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = LIBSL_KEY_PAGE_UP;
  io.KeyMap[ImGuiKey_PageDown] = LIBSL_KEY_PAGE_DOWN;
  io.KeyMap[ImGuiKey_Home] = LIBSL_KEY_HOME;
  io.KeyMap[ImGuiKey_End] = LIBSL_KEY_END;
  io.KeyMap[ImGuiKey_Delete] = LIBSL_KEY_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = LIBSL_KEY_BK_SPACE;
  io.KeyMap[ImGuiKey_Enter] = LIBSL_KEY_ENTER;
  io.KeyMap[ImGuiKey_Escape] = LIBSL_KEY_ESC;
  io.KeyMap[ImGuiKey_A] = 'a';
  io.KeyMap[ImGuiKey_C] = 'c';
  io.KeyMap[ImGuiKey_V] = 'v';
  io.KeyMap[ImGuiKey_X] = 'x';
  io.KeyMap[ImGuiKey_Y] = 'y';
  io.KeyMap[ImGuiKey_Z] = 'z';
  // Disable imgui.ini
  io.IniFilename = NULL;
#ifdef ANDROID
  androidInitGLEXT();
#endif
}

// ----------------------------------------------------

#ifdef OPENGL

#ifdef OPENGL4
#include <LibSL/LibSL_gl4.h>
#else
#include <LibSL/LibSL_gl.h>
#endif

#include <LibSL/GLHelpers/GLHelpers.h>

#if defined(EMSCRIPTEN) | defined(ANDROID)
#define glActiveTextureARB glActiveTexture
#endif

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

// Data
static double       g_Time = 0.0f;
static bool         g_MousePressed[5] = { false, false, false, false, false };
static float        g_MouseWheel = 0.0f;
static GLuint       g_FontTexture = 0;
static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;
static int          g_ScreenX = 0;
static int          g_ScreenY = 0;
static int          g_MouseX = 0;
static int          g_MouseY = 0;

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
static void ImGui_ImplSimpleUI_RenderDrawLists(ImDrawData* draw_data)
{
  // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
  ImGuiIO& io = ::ImGui::GetIO();
  int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
  int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
  if (fb_width == 0 || fb_height == 0) {
    return;
  }
  draw_data->ScaleClipRects(io.DisplayFramebufferScale);

  // Backup GL state
  GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
  GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
  GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
  GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
#endif
  //GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
  //GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
  GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
  GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
  GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
  GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
  GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
  GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
  GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

  // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
  glEnable(GL_BLEND);
  ::glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glActiveTextureARB(GL_TEXTURE0);

  // Setup viewport, orthographic projection matrix
  glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
  const float ortho_projection[4][4] =
  {
    { 2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f },
    { 0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f, 0.0f },
    { -1.0f, 1.0f, 0.0f, 1.0f },
  };
  glUseProgram(g_ShaderHandle);
  glUniform1i(g_AttribLocationTex, 0);
  glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
  
  glBindVertexArray(g_VaoHandle);

  for (int n = 0; n < draw_data->CmdListsCount; n++)
  {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    const ImDrawIdx* idx_buffer_offset = 0;

    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

    glEnableVertexAttribArray(g_AttribLocationPosition);
    glEnableVertexAttribArray(g_AttribLocationUV);
    glEnableVertexAttribArray(g_AttribLocationColor);
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

    for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
    {
      if (pcmd->UserCallback)
      {
        pcmd->UserCallback(cmd_list, pcmd);
      } else
      {
        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
        glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
        glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
      }
      idx_buffer_offset += pcmd->ElemCount;
    }
  }

  // Restore modified GL state
  glUseProgram(last_program);
  glActiveTextureARB(last_active_texture);
  glBindTexture(GL_TEXTURE_2D, last_texture);
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glBindVertexArray(last_vertex_array);
#endif
  glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
  glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
  //glBlendFunc(last_blend_src, last_blend_dst);
  if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
  if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
  if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
  if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
  glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

static void ImGui_ImplSimpleUI_RenderDrawData(ImDrawData* draw_data)
{
  ImGui_ImplSimpleUI_RenderDrawLists(draw_data);
}

static const char* ImGui_ImplSimpleUI_GetClipboardText(void* user_data)
{
  return "[clipboard not implemented]";
}

static void ImGui_ImplSimpleUI_SetClipboardText(void* user_data, const char* text)
{

}

static bool ImGui_ImplSimpleUI_CreateFontsTexture()
{
  // Build texture atlas
  ImGuiIO& io = ::ImGui::GetIO();
  unsigned char* pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

  // Upload texture to graphics system
  GLint last_texture;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGenTextures(1, &g_FontTexture);
  glBindTexture(GL_TEXTURE_2D, g_FontTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  // Store our identifier
  io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

  // Restore state
  glBindTexture(GL_TEXTURE_2D, last_texture);

  return true;
}

static void checkGLSLCompiled(GLuint id)
{
  GLint compiled;
#ifdef OPENGLES
  glGetShaderiv(id, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
#else
  glGetObjectParameterivARB(id, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
#endif
  if (!compiled) {
    std::cerr << "**** BindImGui GLSL shader failed to compile ****" << std::endl;
    GLint maxLength;
#ifdef OPENGLES
    glGetShaderiv(id, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
#else
    glGetObjectParameterivARB(id, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
#endif
    Array<GLcharARB> infoLog(maxLength + 1);
    GLint len = 0;
#ifdef OPENGLES
    glGetShaderInfoLog(id, maxLength, &len, infoLog.raw());
#else
    glGetInfoLogARB(id, maxLength, &len, infoLog.raw());
#endif
    std::cerr << Console::yellow << infoLog.raw() << Console::gray << std::endl;
    glDeleteObjectARB(id);
    throw GLException("\n\n**** GLSL shader failed to compile ****\n%s\n", infoLog.raw() != nullptr ? infoLog.raw() : "<unknown error>");
  }
}

static bool ImGui_ImplSimpleUI_CreateDeviceObjects()
{
  // Backup GL state
  GLint last_texture, last_array_buffer;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  GLint last_vertex_array;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
#endif

#ifdef OPENGL4
  const GLchar* vertex_shader =
    "#version 410\n"
    "uniform mat4 ProjMtx;\n"
    "in      vec2 Position;\n"
    "in      vec2 UV;\n"
    "in      vec4 Color;\n"
    "out     vec2 Frag_UV;\n"
    "out     vec4 Frag_Color;\n"
    "void main()\n"
    "{\n"
    "	Frag_UV     = UV;\n"
    "	Frag_Color  = Color;\n"
    "	gl_Position = ProjMtx * vec4(Position.xy,0.0,1.0);\n"
    "}\n";

  const GLchar* fragment_shader =
    "#version 410\n"
#if defined(EMSCRIPTEN) | defined(ANDROID)
    "precision mediump float;\n"
#endif
    "uniform sampler2D Texture;\n"
    "in vec2 Frag_UV;\n"
    "in vec4 Frag_Color;\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "	 color = Frag_Color * texture( Texture, Frag_UV.st);\n"
    "}\n";

#else // OPENGL4

  const GLchar *vertex_shader =
    "uniform mat4 ProjMtx;\n"
    "attribute vec2 Position;\n"
    "attribute vec2 UV;\n"
    "attribute vec4 Color;\n"
    "varying vec2 Frag_UV;\n"
    "varying vec4 Frag_Color;\n"
    "void main()\n"
    "{\n"
    "	Frag_UV = UV;\n"
    "	Frag_Color = Color;\n"
    "	gl_Position = ProjMtx * vec4(Position.xy,0.0,1.0);\n"
    "}\n";

  const GLchar* fragment_shader =
#if defined(EMSCRIPTEN) | defined(ANDROID)
    "precision mediump float;\n"
#endif
    "uniform sampler2D Texture;\n"
    "varying vec2 Frag_UV;\n"
    "varying vec4 Frag_Color;\n"
    "void main()\n"
    "{\n"
    "	gl_FragColor = Frag_Color * texture2D( Texture, Frag_UV.st);\n"
    "}\n";

#endif // OPENGL4

  g_ShaderHandle = glCreateProgram();
  g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
  g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
  glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
  glCompileShader(g_VertHandle);
  checkGLSLCompiled(g_VertHandle);
  glCompileShader(g_FragHandle);
  checkGLSLCompiled(g_FragHandle);
  glAttachShader(g_ShaderHandle, g_VertHandle);
  glAttachShader(g_ShaderHandle, g_FragHandle);
  glLinkProgram(g_ShaderHandle);

  GLint linked;
#ifdef OPENGLES
  glGetProgramiv(m_Shader, GL_OBJECT_LINK_STATUS_ARB, &linked);
#else
  glGetObjectParameterivARB(g_ShaderHandle, GL_OBJECT_LINK_STATUS_ARB, &linked);
#endif
  if (!linked) {
    std::cerr << "**** BindImGui GLSL program failed to link ****" << std::endl;
    GLint maxLength;
#ifdef OPENGLES
    glGetProgramiv(m_Shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
#else
    glGetObjectParameterivARB(g_ShaderHandle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
#endif
    Array<GLcharARB> infoLog(maxLength);
#ifdef OPENGLES
    glGetProgramInfoLog(m_Shader, maxLength, NULL, infoLog.raw());
#else
    glGetInfoLogARB(g_ShaderHandle, maxLength, NULL, infoLog.raw());
#endif
    throw GLException("\n\n**** GLSL program failed to link (%s) ****\n%s", "BindImGui <internal>", infoLog.raw());
  }

  g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
  g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
  g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
  g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
  g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

  glGenBuffers(1, &g_VboHandle);
  glGenBuffers(1, &g_ElementsHandle);

  glGenVertexArrays(1, &g_VaoHandle);
  glBindVertexArray(g_VaoHandle);
  glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
  glEnableVertexAttribArray(g_AttribLocationPosition);
  glEnableVertexAttribArray(g_AttribLocationUV);
  glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
  glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
  glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
  glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

  ImGui_ImplSimpleUI_CreateFontsTexture();

  // Restore modified GL state
  glBindTexture(GL_TEXTURE_2D, last_texture);
  glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
#if !defined(EMSCRIPTEN) && !defined(ANDROID)
  glBindVertexArray(last_vertex_array);
#endif

  return true;
}

static void    ImGui_ImplSimpleUI_InvalidateDeviceObjects()
{
  if (g_VaoHandle) glDeleteVertexArrays(1, &g_VaoHandle);
  if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
  if (g_ElementsHandle) glDeleteBuffers(1, &g_ElementsHandle);
  g_VaoHandle = g_VboHandle = g_ElementsHandle = 0;

  glDetachShader(g_ShaderHandle, g_VertHandle);
  glDeleteShader(g_VertHandle);
  g_VertHandle = 0;

  glDetachShader(g_ShaderHandle, g_FragHandle);
  glDeleteShader(g_FragHandle);
  g_FragHandle = 0;

  glDeleteProgram(g_ShaderHandle);
  g_ShaderHandle = 0;

  if (g_FontTexture)
  {
    glDeleteTextures(1, &g_FontTexture);
    ::ImGui::GetIO().Fonts->TexID = 0;
    g_FontTexture = 0;
  }
}

static void ImGui_ImplSimpleUI_NewFrame()
{
  ImGuiIO& io = ImGui::GetIO();

  // Setup display size (every frame to accommodate for window resizing)
  int w = g_ScreenX, h = g_ScreenY;
  io.DisplaySize = ImVec2((float)w, (float)h);
  io.DisplayFramebufferScale = ImVec2(1,1);

  // Setup time step
  double current_time = (double)milliseconds();
  io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time)/1000.0f : (float)(1.0f / 60.0f);
  g_Time = current_time;

  // Setup inputs
  // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
  io.MousePos = ImVec2((float)g_MouseX, (float)g_MouseY);   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)

  for (int i = 0; i < 3; i++) {
    io.MouseDown[i] = g_MousePressed[i];
    //g_MousePressed[i] = false;
  }
  io.MouseWheel = g_MouseWheel;
  g_MouseWheel = 0.0f;

  // Start the frame
  ImGui::NewFrame();
}

static NAMESPACE::f_onKeyPressed         prevKeyPressed = NULL;
static NAMESPACE::f_onKeyUnpressed       prevKeyUnpressed = NULL;
static NAMESPACE::f_onScanCodePressed    prevScanCodePressed = NULL;
static NAMESPACE::f_onScanCodeUnpressed  prevScanCodeUnpressed = NULL;
static NAMESPACE::f_onMouseButtonPressed prevMouseButtonPressed = NULL;
static NAMESPACE::f_onMouseMotion        prevMouseMotion = NULL;
static NAMESPACE::f_onReshape            prevReshape = NULL;
static NAMESPACE::f_onMouseWheel         prevMouseWheel = NULL;
static NAMESPACE::f_onRender             prevOnRender = NULL;

static void bindOnRender()
{
  ImGui_ImplSimpleUI_NewFrame();

  if (prevOnRender) prevOnRender();
}

static void bindMouseMotion(uint x, uint y)
{
  g_MouseX = x;
  g_MouseY = y;
  ImGuiIO& io = ImGui::GetIO();
  if (!io.WantCaptureMouse) {
    if (prevMouseMotion) prevMouseMotion(x, y);
  }
}

static void bindMouseButtonPressed(uint x, uint y, uint button, uint flags)
{
  g_MouseX = x;
  g_MouseY = y;
  int btn = -1;
  switch (button)
  {
  case LIBSL_LEFT_BUTTON:   btn = 0; break;
  case LIBSL_RIGHT_BUTTON:  btn = 1; break;
  case LIBSL_MIDDLE_BUTTON: btn = 2; break;
  case LIBSL_WHEEL_UP:      btn = 3; break;
  case LIBSL_WHEEL_DOWN:    btn = 4; break;
  }
  if (flags == LIBSL_BUTTON_DOWN) {
    g_MousePressed[btn] = true;
  }
  if (flags == LIBSL_BUTTON_UP) {
    g_MousePressed[btn] = false;
  }
  ImGuiIO& io = ImGui::GetIO();
  if (!io.WantCaptureMouse) {
    if (prevMouseButtonPressed) prevMouseButtonPressed(x, y, button, flags);
  }
}

static void bindKeyPressed(uchar key)
{
  // std::cerr << "[BindImGui] bindKeypressed " << (int)key << std::endl;
  ImGuiIO& io = ImGui::GetIO();
  io.AddInputCharacter((unsigned short)key);
  io.KeysDown[key] = true;
  if (!io.WantCaptureKeyboard) {
    if (prevKeyPressed) prevKeyPressed(key);
  } 
}

static void bindKeyUnpressed(uchar key)
{
  // std::cerr << "[BindImGui] bindKeyUnpressed " << (int)key << std::endl;
  ImGuiIO& io = ImGui::GetIO();
  io.KeysDown[key] = false;
  if (!io.WantCaptureKeyboard) {
    if (prevKeyPressed) prevKeyUnpressed(key);
  }
}

static void bindScanCodePressed(uint sc)
{
  // std::cerr << "[BindImGui] bindScanCodePressed " << sc << std::endl;
  ImGuiIO& io = ImGui::GetIO();
  io.KeysDown[sc] = true;
  if (!io.WantCaptureKeyboard) {
    if (prevScanCodePressed) prevScanCodePressed(sc);
  }
}

static void bindScanCodeUnpressed(uint sc)
{
  // std::cerr << "[BindImGui] bindScanCodeUnpressed " << sc << std::endl;
  ImGuiIO& io = ImGui::GetIO();
  io.KeysDown[sc] = false;
  if (!io.WantCaptureKeyboard) {
    if (prevScanCodeUnpressed) prevScanCodeUnpressed(sc);
  }
}

static void bindMouseWheel(int increment)
{
  g_MouseWheel += (float)increment * NAMESPACE::mouseWheelScale();
  ImGuiIO& io = ImGui::GetIO();
  if (!io.WantCaptureMouse) {
    if (prevMouseWheel) prevMouseWheel(increment);
  }
}

static void bindReshape(uint x, uint y)
{
  g_ScreenX = x;
  g_ScreenY = y;
  if (prevReshape) prevReshape(x, y);
}

void NAMESPACE::bindImGui()
{
  // chain with SimpleUI
  prevKeyPressed = SimpleUI::onKeyPressed;
  prevKeyUnpressed = SimpleUI::onKeyUnpressed;
  prevScanCodePressed = SimpleUI::onScanCodePressed;
  prevScanCodeUnpressed = SimpleUI::onScanCodeUnpressed;
  prevMouseButtonPressed = SimpleUI::onMouseButtonPressed;
  prevMouseMotion = SimpleUI::onMouseMotion;
  prevReshape = SimpleUI::onReshape;
  prevMouseWheel = SimpleUI::onMouseWheel;
  prevOnRender = SimpleUI::onRender;
  // redirect
  SimpleUI::onMouseButtonPressed = bindMouseButtonPressed;
  SimpleUI::onKeyPressed = bindKeyPressed;
  SimpleUI::onKeyUnpressed = bindKeyUnpressed;
  SimpleUI::onScanCodePressed = bindScanCodePressed;
  SimpleUI::onScanCodeUnpressed = bindScanCodeUnpressed;
  SimpleUI::onMouseWheel = bindMouseWheel;
  SimpleUI::onReshape = bindReshape;
  SimpleUI::onMouseMotion = bindMouseMotion;
  SimpleUI::onRender = bindOnRender;
}

void NAMESPACE::initImGui()
{
  ::ImGui::CreateContext();
  ImGui_generic_init();

  ImGuiIO& io = ::ImGui::GetIO();
  //io.RenderDrawListsFn  = ImGui_ImplSimpleUI_RenderDrawLists;
  io.SetClipboardTextFn = ImGui_ImplSimpleUI_SetClipboardText;
  io.GetClipboardTextFn = ImGui_ImplSimpleUI_GetClipboardText;

  ForIndex(i, IM_ARRAYSIZE(io.KeysDown)) {
    io.KeysDown[i] = false;
  }

  ImGui_ImplSimpleUI_CreateDeviceObjects();
}

void NAMESPACE::terminateImGui()
{
  ImGui_ImplSimpleUI_InvalidateDeviceObjects();

  ::ImGui::DestroyContext();
}

void NAMESPACE::renderImGui()
{
  ::ImGui::Render();
  ImGui_ImplSimpleUI_RenderDrawData(ImGui::GetDrawData());
}


#else // ! OPENGL

void NAMESPACE::BindImGui()
{
  throw LibSL::Errors::Fatal("[LibSL::BindImGui::bind()] not implemented");
}

#endif

//---------------------------------------------------------------------------
