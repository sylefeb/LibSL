#include "styleManager.h"
#include <fstream>

#include <iostream>
#include <regex>

using namespace std;

#define regex_comment   string("((\\/\\/.*?)\\n|\\/\\*(?:.|\\s)*?\\*\\/)")

#define regex_float_0_1 string("\\b(1(?:\\.0*)?|0(?:\\.\\d*)?)\\b")
#define regex_int_0_255 string("\\b([0-1]?[0-9]{1,2}|2[0-4][0-9]|25[0-5])\\b")

#define regex_rgb_float string("rgb[a]?\\(" + regex_float_0_1 + "," + regex_float_0_1  + "," + regex_float_0_1 + "(?:," + regex_float_0_1 + ")?\\)")
#define regex_rgb_int   string("rgb[a]?\\(" + regex_int_0_255 + "," + regex_int_0_255  + "," + regex_int_0_255 + "(?:," + regex_int_0_255 + ")?\\)")
#define regex_hex       string("#(?:[0-9a-fA-F]{2}){3,4}")
#define regex_color     string("(?:" + regex_rgb_float + "|" + regex_rgb_int + "|" + regex_hex +")")

#define regex_label     string("\\b\\w*\\b")

#define regex_context          string("(" + regex_label + ")" + "\\{(.*?)\\}")
#define regex_rule             string("(" + regex_label + ")" + ":(.*?);")
#define regex_color_definition string("(" + regex_label + ")" + "=(.*?);")

styleManager *styleManager::m_singleton = nullptr;

ImVec4 hexToRGBA(string& hex)
{
  for (auto & c : hex) c = toupper(c);

  float rgba[4] = { 0,0,0,0 };

  for (int i = 0; i < 4; i++)
  {
    if (i == 3 && hex.size() == 7)
    {
      rgba[i] = 1.0f;
    } else {

      rgba[i] = (hex[i * 2 + 1] >= '0' && hex[i * 2 + 1] <= '9' ? hex[i * 2 + 1] - '0' : 10 + hex[i * 2 + 1] - 'A') * 16.f / 255.f
              + (hex[i * 2 + 2] >= '0' && hex[i * 2 + 2] <= '9' ? hex[i * 2 + 2] - '0' : 10 + hex[i * 2 + 2] - 'A') * 1.f / 255.f;
    }
  }

  return ImVec4(rgba[0], rgba[1], rgba[2], rgba[3]);
}

ImVec4 rgbfloatToRGBA(string& rgb)
{
  float rgba[4] = { 0.f, 0.f, 0.f, 1.f };
  int i = 0;

  string toParse = rgb;
  smatch sm_color;
  while (regex_search(toParse, sm_color, regex(regex_float_0_1))) {
    std::string col = sm_color[1];
    rgba[i++] = atof(col.c_str());
    toParse = sm_color.suffix().str();
  }
  return ImVec4(rgba[0], rgba[1], rgba[2], rgba[3]);
}

ImVec4 rgbintToRGBA(string& rgb)
{
  float rgba[4] = { 0.f, 0.f, 0.f, 1.f};
  int i = 0;

  string toParse = rgb;
  smatch sm_color;
  while (regex_search(toParse, sm_color, regex(regex_int_0_255))) {
    std::string col = sm_color[1];
    rgba[i++] = atof( col.c_str() ) / 255.f;
    toParse = sm_color.suffix().str();
  }
  return ImVec4(rgba[0], rgba[1], rgba[2], rgba[3]);
}

ImVec4 parseColor(string& input) {
  ImVec4 color;
  smatch sm_color;
  if (regex_search(input, sm_color, regex(regex_hex))) {
    color = hexToRGBA(input);
  }

  if (regex_search(input, sm_color, regex(regex_rgb_float))) {
    color = rgbfloatToRGBA(input);
  }

  if (regex_search(input, sm_color, regex(regex_rgb_int))) {
    color = rgbintToRGBA(input);
  }

  return color;
}

void styleManager::load(const char* fname)
{
  // ToDo use regex


  ifstream stream;
  stream.open(fname);

  string input( (std::istreambuf_iterator<char>(stream)),
                (std::istreambuf_iterator<char>()) );

  stream.close();
  

  if (!input.empty()) {
    styleSheetCol.clear();
    styleSheetVar.clear();
  }
 
  string escaped = "";
  { // Remove comments and whitespaces
    string uncommented;
    regex_replace(back_inserter(uncommented), input.begin(), input.end(), regex(regex_comment), "$2");
    escaped.clear();
    regex_replace(back_inserter(escaped), uncommented.begin(), uncommented.end(), regex("(\\s)"), "$2");
  }

  string context;
  string rule;
  ImVec4 color;

  string toParse = escaped;
  std::map<string, ImVec4> colors;
  // Parse color definitions
  try {
    smatch sm_def;
    while (regex_search(toParse, sm_def, regex(regex_color_definition))) {
      string color_name = sm_def[1], color_def = sm_def[2];
      color = parseColor(color_def);
      colors.emplace(color_name, color);
      toParse = sm_def.suffix().str();
    }
  } catch (const std::regex_error& e) {
    cerr << "ParseInput: regex_error caught: " << e.what() << endl;
  }

  // parse rules
  toParse = escaped;
  try {
    smatch sm_context, sm_rule, sm_color;
    while (regex_search(toParse, sm_context, regex(regex_context))) {
      vector<pair<string, ImVec4>> colorStyle;
      vector<pair<string, ImVec4>> varStyle;
      context = sm_context[1];

      string sm_1 = sm_context[2];
      while (regex_search(sm_1, sm_rule, regex(regex_rule))) {
        rule = "ImGuiCol_" + string(sm_rule[1]);
        string sm_2 = sm_rule[2];

        if (regex_search(sm_2, sm_color, regex(regex_color))) {
          color = parseColor(string(sm_rule[2]));
        } else if (regex_search(sm_2, sm_color, regex(regex_label))) {
          color = colors[string(sm_rule[2])];
        }

        

        colorStyle.push_back(make_pair(rule, color));
        sm_1 = sm_rule.suffix().str();
      }

      styleSheetCol.emplace(context, colorStyle);
      toParse = sm_context.suffix().str();
    }
  } catch (const std::regex_error& e) {
    cerr << "ParseInput: regex_error caught: " << e.what() << endl;
  }

}

void styleManager::push(const string context) {
  if (styleSheetCol.find(context) == styleSheetCol.end()) {
    popCounter.push_back(0);
    return;
  }
  std::vector<pair<string, ImVec4>> style = styleSheetCol.at(context);
  int pop = 0;
  for (auto s : style) {
    ImGuiColor::ImGuiColor type = ImGuiColor::FromString(s.first);
    ImGui::PushStyleColor(type, s.second);
    pop++;
  }
  popCounter.push_back(pop);
}

void styleManager::pop() {
  int pop = popCounter.back();
  popCounter.pop_back();
  ImGui::PopStyleColor(pop);
}