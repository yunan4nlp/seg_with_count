#include <iostream>
#include <climits>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <map>

using namespace std;

#include "Utf.h"

void load_count(char* path, map<string, long>& state);
void load_text(char* path, vector<string>& lines);
void seg(map<string, long>& state, vector<string>& lines, mt19937& gen, vector<vector<string> >& results);
void split_bychar(const string& str, vector<string>& vec, const char separator);
int choose_candid(vector<pair<string, long> >& candid, long& random);
void save_seg(ofstream& os, vector<vector<string> >& results);
static int verboseIter = 1000;

int main(int argc, char* argv[]) {
	random_device rd;
	mt19937 gen(rd());

	map<string, long> state;
	load_count(argv[1], state);

	ofstream os(argv[2]);
	if (os.is_open())
	{
		vector<string> lines;
		vector<vector<string> > results;
		for (int file_num = 3; file_num < argc; file_num++) {
			lines.clear();
			load_text(argv[file_num], lines);
			for (int i = 0; i < 5; i++) {
				results.clear();
				seg(state, lines, gen, results);
			}

			save_seg(os, results);
		}
		os.close();
		cout << "finish.." << endl;
	}
	else
		cout << argv[2] << "open error." << endl;
	return 0;
}

void load_count(char* path, map<string, long>& state){
	cout << "load state...." << endl;
	ifstream read(path);
	if (read.is_open()) {
		string line;
		vector<string> info;
		long line_num = 0;
		while (getline(read, line)) {
			split_bychar(line, info, '\t');
			long value = atoll(info[1].c_str());
			state[info[0]] = value;
			line_num++;
			if (line_num % verboseIter == 0)
				cout << line_num << " ";
		}
		cout << endl;
		read.close();
	}
	else
		cout << path << " file don't exist." << endl;
}

void load_text(char* path, vector<string>& lines){
	cout << "load text " << path << endl;
	ifstream read(path);
	if (read.is_open()) {
		string line;
		long line_num = 0;
		while (getline(read,line)) {
			lines.push_back(line);
			line_num++;
			if (line_num % verboseIter == 0)
				cout << line_num << " ";
		}
		cout << endl;
		read.close();
	}
	else
		cout << path << " file don't exist." << endl;
}

int choose_candid(vector<pair<string, long> >& candid, long& random){
	int maxnum = candid.size();
	int index = 0;
	for (int idx = 0; idx < maxnum; idx++) {
		if (candid[idx].second >= random) {
			index = idx;
			return index;
		}
	}
}

void seg(map<string, long>& state, vector<string>& lines, mt19937& gen, vector<vector<string> >& results){
	cout << "seg..." << endl;
	results.clear();
	int maxnum = lines.size();
	int idy;
	int char_num, line_num = 0;
	long value;
	string uni, bi, tri, four;
	vector<pair<string, long> > candid;
	vector<string> chs;
	for(int idx = 0; idx < maxnum; idx++) {
		const string& line = lines[idx];
		vector<string> words;
		chs.clear();
		getCharactersFromUTF8String(line, chs);
		char_num = chs.size();
		idy = 0;
		while(idy < char_num){
			candid.clear();
			uni = "UNI=" + chs[idy];
			value = state[uni];
			candid.push_back(make_pair(uni, value));
			if (idy + 1 < char_num) {
				bi = "BI=" + chs[idy] + "#" + chs[idy + 1];
				value += state[bi];
				candid.push_back(make_pair(bi, value));
			}
			if (idy + 2 < char_num) {
				tri = "TRI=" + chs[idy] + "#" + chs[idy + 1] + "#" + chs[idy + 2];
				value += state[tri];
				candid.push_back(make_pair(tri, value));
			}
			if (idy + 3 < char_num) {
				four = "FOUR=" + chs[idy] + "#" + chs[idy + 1] + "#" + chs[idy + 2] + "#" + chs[idy + 3];
				value += state[four];
				candid.push_back(make_pair(four,value));
			}
			uniform_int_distribution<> dist(0, value);
			long random = dist(gen);
			int offset = choose_candid(candid, random) + 1;
			string seg = "";
			for (int idz = 0; idz < offset; idz++){
				seg += chs[idy + idz];
			}
			words.push_back(seg);
			idy += offset;
		}
		results.push_back(words);
		line_num++;
		if (line_num % verboseIter == 0)
				cout << line_num << " ";
	}
	cout << endl;
}

void split_bychar(const string& str, vector<string>& vec, const char separator = ' ') {
	vec.clear();
	string::size_type pos1 = 0, pos2 = 0;
	string word;
	while ((pos2 = str.find_first_of(separator, pos1)) != string::npos) {
		word = str.substr(pos1, pos2 - pos1);
		pos1 = pos2 + 1;
		if (!word.empty())
			vec.push_back(word);
	}
	word = str.substr(pos1);
	if (!word.empty())
		vec.push_back(word);
}

void save_seg(ofstream& os, vector<vector<string> >& results){
	cout << "save..." << endl;
	int line_num = results.size();
	for (int idx = 0; idx < line_num; idx++) {
		int char_num = results[idx].size();
		for (int idy = 0; idy < char_num; idy++) {
			os << results[idx][idy] << " ";
		}
		os << endl;
	}
}
