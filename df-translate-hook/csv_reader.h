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

string &reprString(string& str) {
    return str.replace(str.begin(), str.end(), "\\", R"(\\)")
        .replace(str.begin(), str.end(), "\t", R"(\t)")
        .replace(str.begin(), str.end(), "\r", R"(\r)")
        .replace(str.begin(), str.end(), "\n", R"(\n)")
        .replace(str.begin(), str.end(), "\"", R"(")");
}

void splitString(const string& fullstr, map<string, string>& dict) {
    const regex re(R"r(^"(.*?)","(.*?)"$)r");

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