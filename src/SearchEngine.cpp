#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include "dirent.h"
#include "stem.h"
using namespace std;

// constants
const char DIR_TEXTS[] = "./texts_project/";
const char STOP_WORDS[] = "./stop_words.txt";
const char INDEX_FILE[] = "./index_file.txt";

// Vector for the stop words
vector<string> stop_words;

// struct for associating word with documents where it appears
struct indexItem {
	string word;
	vector<string> docs;
};
// vector of index items
vector<indexItem> indexItems;

void toLowercase(string &s) {
	transform(s.begin(), s.end(), s.begin(), ::tolower);
}

void removeSymbols(string &s) {
	char chars[] = ".,;:!?\"/\\'()[]{}";
	for (unsigned int i = 0; i < strlen(chars); i++) {
		replace(s.begin(), s.end(), chars[i], ' ');
	}
}

void removeNewLine(string &line) {
	if (*line.rbegin() == '\r') {
		line.erase(line.length() - 1);
	}
	if (*line.rbegin() == '\n') {
		line.erase(line.length() - 1);
	}
}

/**
 * Creates and writes the index file.
 */
void createIndexFile() {

}

/**
 * Builds the index file
 * @param string file_name
 * @param string s
 */
void indexing(string file_name, string s) {
	int size = indexItems.size();
	int i;
	for (i = 0; i < size; i++) {
		if (indexItems[i].word == s) {
			break;
		}
	}
	if (i == size) {
		// if the word is not in the index, add it
		indexItem idxItm;
		idxItm.word = s;
		idxItm.docs.push_back(file_name);
		indexItems.push_back(idxItm);
	} else if (find(indexItems[i].docs.begin(), indexItems[i].docs.end(), file_name) == indexItems[i].docs.end()) {
		// if the word is in the index, add the doc's name (if it is not already there)
		indexItems[i].docs.push_back(file_name);
	}
}

/**
 * Remove the extra parts of a word.
 * @param string s
 */
void stemming(string &s) {
    int newEndPoint = stem(&s[0],0,s.length()-1);
	s = s.substr(0,newEndPoint+1);
}

/**
 * Process line, gets each word and indexes them.
 * @param string file_name
 * @param string line
 */
void parseLine(string file_name, string &line) {
	removeNewLine(line);
	removeSymbols(line);
	toLowercase(line);
	istringstream iss(line);
	do {
		string word;
		iss >> word;
		// if it is not empty and not a stop word
		if (!word.empty() && find(stop_words.begin(), stop_words.end(), word) == stop_words.end()) {
			stemming(word);
			indexing(file_name, word);
		}
	} while (iss);
}

/**
 * Reads the texts' directory and process them.
 */
void loadTexts() {
	DIR *dir;
	struct dirent *ent;
	string line;
	string file_name;
	string full_file_name;
	if ((dir = opendir(DIR_TEXTS)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			file_name = ent->d_name;
			full_file_name = DIR_TEXTS + file_name;
			ifstream myfile(full_file_name.c_str());
			if (myfile.is_open()) {
				while (getline(myfile, line)) {
					if (!line.empty()) {
						parseLine(file_name, line);
					}
				}
				myfile.close();
			}
		}
		closedir(dir);
	}
}

/**
 * Load stop words from the file to the vector.
 */
void loadStopWords() {
	string line;
	ifstream myfile(STOP_WORDS);
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			if (!line.empty()) {
				removeNewLine(line);
				toLowercase(line);
				stop_words.push_back(line);
			}
		}
		myfile.close();
	}
}

/**
 * Builds the index file.
 */
void buildIndex() {
	loadStopWords();
	loadTexts();
	createIndexFile();
}

/**
 * Loads the index file.
 */
void loadIndex() {

}

int main() {
	// if index file already exists
		//loadIndex();
	//else
		buildIndex();

	int size = indexItems.size();
	for (int i = 0; i < size; i++) {
		cout << indexItems[i].word << " = ";
		int size2 = indexItems[i].docs.size();
		for (int j = 0; j < size2; j++) {
			cout << indexItems[i].docs[j] << " ";
		}
		cout << endl << endl;
	}

	return 0;
}
