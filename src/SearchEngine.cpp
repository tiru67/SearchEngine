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
const int MAX_RESULTS = 10;

// Vector for the stop words
vector<string> stop_words;

// struct for associating word with documents where it appears
struct indexItem {
	string word;
	vector<string> docs;
};
// vector of index items
vector<indexItem> indexItems;

// struct for associating a doc with the number of matches in a query
struct docsOrder {
	string doc;
	int number;
};

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
 * Find index by word.
 * @param string word
 * @return int
 */
int findIndex(string word) {
	int size = indexItems.size();
	for (int i = 0; i < size; i++) {
		if (indexItems[i].word == word) {
			return i;
		}
	}
	return -1;
}

/**
 * Creates and writes the index file.
 */
void createIndexFile() {
	ofstream myfile;
	myfile.open(INDEX_FILE);
	int size = indexItems.size();
	for (int i = 0; i < size; i++) {
		myfile << indexItems[i].word << " ";
		int size2 = indexItems[i].docs.size();
		for (int j = 0; j < size2; j++) {
			myfile << indexItems[i].docs[j] << " ";
		}
		myfile << "\n";
	}
	myfile.close();
}

/**
 * Builds the index file
 * @param string file_name
 * @param string s
 */
void indexing(string file_name, string s) {
	int i = findIndex(s);
	if (i < 0) {
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
	string line;
	ifstream myfile(INDEX_FILE);
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			if (!line.empty()) {
				istringstream iss(line);
				string word;
				iss >> word;
				indexItem idxItm;
				idxItm.word = word;
				string doc;
				do {
					iss >> doc;
					idxItm.docs.push_back(doc);
				} while (iss);
				indexItems.push_back(idxItm);
			}
		}
		myfile.close();
	}
}

/**
 * Find the documents where the words in the query appear.
 * @param string query
 * @return string docs
 */
string search(string query) {
	istringstream iss(query);
	string word;
	vector<docsOrder> docsOrders;

	int count = 0;
	do {
		iss >> word;
		// if it is a stop word, move on
		if (word.empty() || find(stop_words.begin(), stop_words.end(), word) != stop_words.end()) {
			continue;
		}
		stemming(word);
		// search for the word in the index
		int k = findIndex(word);
		if (k < 0) {
			continue;
		}
		// for each doc where the word appears
		for (int i = 0; i < indexItems[k].docs.size() && count < MAX_RESULTS; i++) {
			// store the docs that matched the word
			int j;
			for (j = 0; j < docsOrders.size(); j++) {
				if (docsOrders[j].doc == indexItems[k].docs[i]) {
					docsOrders[j].number += 1;
					break;
				}
			}
			if (j == docsOrders.size()) {
				docsOrder docOrder;
				docOrder.doc = indexItems[k].docs[i];
				docOrder.number = 1;
				docsOrders.push_back(docOrder);
				count++;
			}
		}
	} while (iss && count < MAX_RESULTS);

	// return message if nothing was found
	if (docsOrders.empty()) {
		return "Sorry, we could not find any relevant document to your query";
	}

	// ordering
	for (int i = 1; i < docsOrders.size() - 1; i++) {
		int j = i;
		while (j > 0 && docsOrders[j-1].number < docsOrders[j].number) {
			iter_swap(docsOrders.begin()+j, docsOrders.begin()+j-1);
			j--;
		}
	}

	// build result
	string docs;
	int i;
	for (i = 0; i < docsOrders.size() - 1; i++) {
		docs += docsOrders[i].doc + ",";
	}
	docs += docsOrders[i].doc;

	return docs;
}

void print() {
	int size = indexItems.size();
	for (int i = 0; i < size; i++) {
		cout << indexItems[i].word << " : ";
		int size2 = indexItems[i].docs.size();
		for (int j = 0; j < size2; j++) {
			cout << indexItems[i].docs[j] << " ";
		}
		cout << endl << endl;
	}
}

int main() {
	ifstream file(INDEX_FILE);
	// if index file already exists
	if (file) {
		// load from file
		loadIndex();
	} else {
		// build index
		buildIndex();
	}

	//print();

	cout << "Type your query: " << endl;
	string query;
	getline(cin, query);
	string res = search(query);
	cout << res << endl;

	return 0;
}
