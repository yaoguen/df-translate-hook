#pragma once
#include <map>
#include <regex>
#include <string>
#include <fstream>

using std::map;
using std::smatch;
using std::regex;
using std::string;
using std::ifstream;

void Replace(string& subject, const string& search, const string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

string& reprString(string& str) {
    Replace(str, R"(\\)", "\\");
    Replace(str, R"(\t)", "\t");
    Replace(str, R"(\r)", "\r");
    Replace(str, R"(\n)", "\n");
    Replace(str, R"(")", "\"");
    return str;
}

void splitString(const string& fullstr, map<string, string>& dict) {
    const regex re(R"r(^"(.*?)","(.*?)")r");

    smatch match;
    if (regex_search(fullstr, match, re)) {
        string key = match[1];
        string arg = match[2];
        dict.insert(make_pair(reprString(key), reprString(arg)));
    }
}

void readData(const string& filename, map<string, string>& dictionary) {
    ifstream fin(filename.c_str());

    if (fin.is_open()) {
        string s;
        while (!fin.eof()) {
            getline(fin, s);

            if (!s.empty()) {
                splitString(s, dictionary);
            }
        }
        fin.close();
    }
}