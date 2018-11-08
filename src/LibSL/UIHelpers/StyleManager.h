#pragma once

#include <imgui.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>

using namespace std;
class styleManager {
public :
	map<string, vector<pair<string, ImVec4>>> styleSheetCol;
	map<string, vector<pair<string, float>>>  styleSheetVar;
	
	void load(const char* fname);

	void push(const string);

	void pop();

private :
	vector<int> popCounter;

}; 


// Search and remove whitespace from both ends of the string
static std::string TrimEnumString(const std::string &_s)
{
	std::string::const_iterator it = _s.begin();
	while (it != _s.end() && isspace(*it)) { it++; }
	std::string::const_reverse_iterator rit = _s.rbegin();
	while (rit.base() != it && isspace(*rit)) { rit++; }
	return std::string(it, rit.base());
}

static void SplitEnumArgs(const char* _szArgs, std::string _Array[], int _nMax)
{
	std::stringstream ss(_szArgs);
	std::string strSub;
	int nIdx = 0;
	while (ss.good() && (nIdx < _nMax)) {
		getline(ss, strSub, ',');
		_Array[nIdx] = TrimEnumString(strSub);
		nIdx++;
	}
};
// This will to define an enum that is wrapped in a namespace of the same name along with ToString(), FromString(), and COUNT
#define DECLARE_ENUM(ename, ...) \
    namespace ename { \
        enum ename { __VA_ARGS__, COUNT }; \
        static std::string _Strings[COUNT]; \
        static const char* ToString(ename e) { \
            if (_Strings[0].empty()) { SplitEnumArgs(#__VA_ARGS__, _Strings, COUNT); } \
            return _Strings[e].c_str(); \
        } \
        static ename FromString(const std::string& strEnum) { \
            if (_Strings[0].empty()) { SplitEnumArgs(#__VA_ARGS__, _Strings, COUNT); } \
            for (int i = 0; i < COUNT; i++) { if (_Strings[i] == strEnum) { return (ename)i; } } \
            return (ename)COUNT; \
        } \
    }

DECLARE_ENUM(ImGuiColor, ImGuiCol_Text,
ImGuiCol_TextDisabled,
ImGuiCol_WindowBg,
ImGuiCol_ChildBg,
ImGuiCol_PopupBg,
ImGuiCol_Border,
ImGuiCol_BorderShadow,
ImGuiCol_FrameBg,
ImGuiCol_FrameBgHovered,
ImGuiCol_FrameBgActive,
ImGuiCol_TitleBg,
ImGuiCol_TitleBgActive,
ImGuiCol_TitleBgCollapsed,
ImGuiCol_MenuBarBg,
ImGuiCol_ScrollbarBg,
ImGuiCol_ScrollbarGrab,
ImGuiCol_ScrollbarGrabHovered,
ImGuiCol_ScrollbarGrabActive,
ImGuiCol_CheckMark,
ImGuiCol_SliderGrab,
ImGuiCol_SliderGrabActive,
ImGuiCol_Button,
ImGuiCol_ButtonHovered,
ImGuiCol_ButtonActive,
ImGuiCol_Header,
ImGuiCol_HeaderHovered,
ImGuiCol_HeaderActive,
ImGuiCol_Separator,
ImGuiCol_SeparatorHovered,
ImGuiCol_SeparatorActive,
ImGuiCol_ResizeGrip,
ImGuiCol_ResizeGripHovered,
ImGuiCol_ResizeGripActive,
ImGuiCol_CloseButton,
ImGuiCol_CloseButtonHovered,
ImGuiCol_CloseButtonActive,
ImGuiCol_PlotLines,
ImGuiCol_PlotLinesHovered,
ImGuiCol_PlotHistogram,
ImGuiCol_PlotHistogramHovered,
ImGuiCol_TextSelectedBg,
ImGuiCol_ModalWindowDarkening,
ImGuiCol_DragDropTarget,
ImGuiCol_COUNT,
ImGuiCol_ERROR);

