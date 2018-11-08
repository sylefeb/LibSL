#include "styleManager.h"
#include <fstream>

void parseWS(ifstream& stream) {
	char trash = stream.peek();
	while (trash == ' ' || trash == '\t' || trash == '\n' || trash == '\r' || trash == '\0') {
		stream.get();
		trash = stream.peek();
	}
}

ImVec4 hexToRGBA(string& hex) {
	for (auto & c : hex) c = toupper(c);

	float rgba[4] = { 0,0,0,0 };

	for (int i = 0; i < 4; i++) {
		if (i == 3 && hex.size() == 7) {
			rgba[i] = 1.0f;
		}
		else {

			rgba[i] = (hex[i * 2 + 1] >= '0' && hex[i * 2 + 1] <= '9' ? hex[i * 2 + 1] - '0' : hex[i * 2 + 1] - 'A') * 16.f / 255.f
				    + (hex[i * 2 + 2] >= '0' && hex[i * 2 + 2] <= '9' ? hex[i * 2 + 2] - '0' : hex[i * 2 + 2] - 'A') * 1.f / 255.f;
		}
	}

	return ImVec4(rgba[0], rgba[1], rgba[2], rgba[3]);
}

void styleManager::load(const char* fname) {
	// ToDo use regex
	ifstream stream(fname);
	string cleaned = "";

	char c;
	while (!stream.eof()) {
		c = stream.get();
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
			continue;
		}
		cleaned += c;
	}

	string context;
	string rule;
	string color;
	vector<pair<string, ImVec4>> contextStyle;

	int i = 0;
	while(i < cleaned.size()-1) {
		c = cleaned.at(i);
		context = "";
		while (c != '{') {
			context += c;
			++i;
			c = cleaned.at(i);
		}
		++i; // ignore {
		while (c != '}') {
			rule = "";
			c = cleaned.at(i);
			while (c != ':') {
				rule += c;
				++i;
				c = cleaned.at(i);
			}
			++i; // ignore :
			color = "";
			c = cleaned.at(i);
			while (c != ';') {
				color += c;
				++i;
				c = cleaned.at(i);
			}
			++i; // ignore ;
			c = cleaned.at(i);
			ImVec4 col = hexToRGBA(color);
			contextStyle.push_back(make_pair("ImGuiCol_"+rule, col));
		}
		styleSheetCol.emplace(context, contextStyle);
		
		++i; // ignore }
	}
}

void styleManager::push(const string context) {
	std::vector<pair<string,ImVec4>> style = styleSheetCol.at(context);
	int pop = 0;
	for (auto s : style) {
		ImGuiColor::ImGuiColor type = ImGuiColor::FromString(s.first);
		ImGui::PushStyleColor(type , s.second);
		pop++;
	}
	popCounter.push_back(pop);
}

void styleManager::pop() {
	int pop = popCounter.back();
	popCounter.pop_back();
	ImGui::PopStyleColor(pop);
}