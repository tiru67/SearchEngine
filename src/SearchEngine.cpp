#define M_LOG2E 1.44269504088896340736 //log2(e)

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <math.h>
#include "dirent.h"
#include "stem.h"

using namespace std;

// constants
const char DIR_TEXTS[] = "./texts_project/";
//const char DIR_TEXTS[] = "./Test-Data/";
const char STOP_WORDS[] = "./stop_words.txt";
const char INDEX_FILE[] = "./index_file.txt";
const int MAX_RESULTS = 10;

inline double log2(double x){
    return  log(x) * M_LOG2E;
}

inline double loge(double x){

        return  log10(x) /log10(2.718281828459);
}

// Vector for the stop words
vector<string> stopWords;

// struct for relating a doc with a word
struct DocumentWord {
    string doc; // doc's name
    int numTimes; // number of appearances of the word in the doc
    double tf; // term frequency
    double tf_idf;
    DocumentWord() {
        doc.empty();
        numTimes = 0;
        tf = 0;
        tf_idf = 0;
    }
};

// struct for associating word with documents where it appears
struct Word {
    string word;
    vector<DocumentWord> docs;
    double idf; // inverse document frequency
    double tf_idf;
    Word() {
        word.empty();
        docs.empty();
        idf = 0;
        tf_idf=0;
    }
};
// struct for relating a doc with a word
struct Document {
    string name; // doc's name
    int totalWords; // total number of words in the doc
    Document() {
        name.empty();
        totalWords = 0;
    }
};

//To capture the document name and distance.
struct DocumentDistance{
    string docName;
    double distance;

    bool operator > (const DocumentDistance& str) const
    {
        return (distance > str.distance);
    }
};

bool compareDistances(DocumentDistance a, DocumentDistance b){

    return (a.distance < b.distance);
}

// vector of documents
vector<Document> docs;
// vector of index items
vector<Word> indexItems;

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

string getImmediateSibling (string query){

    return "";
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
 * Find document in indexItem.
 * @param indexItem item
 * @param string name
 * @return int
 */
int findIndexDocument(Word item, string name) {
    int size = item.docs.size();
    for (int i = 0; i < size; i++) {
        if (item.docs[i].doc == name) {
            return i;
        }
    }
    return -1;
}

/**
 * Find index of doc in docs when doc name is given*/

int findDocumentIndexByName(string docName) {
    int size = docs.size();
    for (int i = 0; i < size; i++) {
        if (docs[i].name == docName) {
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
        myfile << indexItems[i].word;
        int size2 = indexItems[i].docs.size();
        for (int j = 0; j < size2; j++) {
            myfile << " " << indexItems[i].docs[j].doc << " " << indexItems[i].docs[j].tf_idf;
        }
        myfile << "\n";
    }
    myfile.close();
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
                stopWords.push_back(line);
            }
        }
        myfile.close();
    }
}

/**
 * Builds the index file
 * @param string fileName
 * @param string s
 */
void indexing(string fileName, string word) {
    DocumentWord docWord;
    docWord.doc = fileName;
    docWord.numTimes = 1;
    int i = findIndex(word);
    if (i < 0) {
        // if the word is not in the index, add it
        Word newWord;
        newWord.word = word;
        newWord.docs.push_back(docWord);
        indexItems.push_back(newWord);
    } else {
        // if the word is in the index
        int docIndex = findIndexDocument(indexItems[i], fileName);
        // if the doc is already in the docs list
        if (docIndex >= 0) {
            // increment num times
            indexItems[i].docs[docIndex].numTimes += 1;
        } else {
            // if not, add the doc
            indexItems[i].docs.push_back(docWord);
        }
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
 * @param string fileName
 * @param string line
 * @param int &totalWords
 */
void parseLine(string fileName, string &line, int &totalWords) {
    removeNewLine(line);
    removeSymbols(line);
    toLowercase(line);
    istringstream iss(line);
    do {
        string word;
        iss >> word;
        // if it is not empty and not a stop word
        if (!word.empty() && find(stopWords.begin(), stopWords.end(), word) == stopWords.end()) {
            stemming(word);
            indexing(fileName, word);
            totalWords++;
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
    string fileName;
    string fullFileName;
    if ((dir = opendir(DIR_TEXTS)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            fileName = ent->d_name;
            if (fileName == "." || fileName == "..") {
                continue;
            }
            fullFileName = DIR_TEXTS + fileName;
            ifstream myfile(fullFileName.c_str());
            if (myfile.is_open()) {
                int totalWords = 0; // for counting total of words in the doc
                // parse lines
                while (getline(myfile, line)) {
                    if (!line.empty()) {
                        parseLine(fileName, line, totalWords);
                    }
                }
                myfile.close();
                // add document to the list
                if (totalWords > 0) {
                    Document doc;
                    doc.name = fileName;
                    doc.totalWords = totalWords;
                    docs.push_back(doc);
                }
            }
        }
        closedir(dir);
    }
}

/**
 * After all the words were loaded, calculate their frequencies in the docs
 */
void calculateTF() {
    string docName;
    double totalWords;
    int indexItemsSize;
    int docIndex;
    int docsSize = docs.size();
    for (int i = 0; i < docsSize; i++) {
        docName = docs[i].name;
        totalWords = static_cast<double>(docs[i].totalWords);
        indexItemsSize = indexItems.size();
        for (int j = 0; j < indexItemsSize; j++) {
            docIndex = findIndexDocument(indexItems[j], docName);
            if (docIndex >= 0) {
                indexItems[j].docs[docIndex].tf = static_cast<double>(indexItems[j].docs[docIndex].numTimes) / totalWords;
            }
        }
    }
}

/**
 * After all the words were loaded, calculate their inverse document frequencies and TF-IDF
 */
void calculateTF_IDF() {
    int numDocuments = static_cast<double>(docs.size());
    int size = indexItems.size();
    for (int i = 0; i < size; i++) {
        int docsSize = indexItems[i].docs.size();
        double idf;
        if(docsSize!=0){
            idf = log2(numDocuments / static_cast<double>(docsSize));
        }else{
            idf =0;
        }
        for (int j = 0; j < docsSize; j++) {
            indexItems[i].docs[j].tf_idf = idf * indexItems[i].docs[j].tf;
        }
    }
}


/**
 * Builds the index file.
 */
void buildIndex() {
    loadStopWords();
    loadTexts();
    calculateTF();
    calculateTF_IDF();
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
                Word idxItm;
                idxItm.word = word;
                string doc;
                double tf_idf;
                do {
                    iss >> doc;
                    iss >> tf_idf;
                    DocumentWord docWord;
                    docWord.doc = doc;
                    docWord.tf_idf = tf_idf;
                    idxItm.docs.push_back(docWord);

                    //check if the doc is already added
                    bool isDocAdd = false;
                    for(int j=0; j<docs.size();j++){
                        if(docs[j].name==doc){
                            isDocAdd =  true;
                            break;
                        }
                    }

                    if(!isDocAdd){
                        Document documnet;
                        documnet.name = doc;
                        docs.push_back(documnet);
                    }

                } while (iss);
                idxItm.docs.pop_back(); // Idk why it is inserting the last element twice, so...
                indexItems.push_back(idxItm);

                //load available docs
                for(int i=0; i<indexItems.size();i++){


                }


            }
        }
        myfile.close();
    }
}

vector<DocumentDistance> eclideanDistance(double query_idf, string queryWord){

  //  cout << "query tf_idf" << query_idf;
  //  cout << endl << endl;

    vector<DocumentDistance> ditanceArray;
    for (int i=0;i<docs.size();i++){
        double sqrSum = 0;
        string docName = docs[i].name;
        bool isDocHasQuery = false;

    //    cout << "Doc Name: " << docName << " :: ";
        //iterate through indexItems to find the vector of this document.
        for(int d=0; d< indexItems.size();d++){

            //iterating through the docs under each indexed word.
            for(int w =0;w<indexItems[d].docs.size(); w++){
                if(indexItems[d].docs[w].doc==docName){
      //              cout << indexItems[d].word << "::";
      //              cout << indexItems[d].docs[w].tf_idf << "    ";
                    if(indexItems[d].word==queryWord){
                        isDocHasQuery=true;
                        sqrSum +=pow(indexItems[d].docs[w].tf_idf-query_idf,2);
                    }else{
                        sqrSum +=pow(indexItems[d].docs[w].tf_idf,2);
                    }
                }
            }
        }

        if (!isDocHasQuery){
             sqrSum +=pow(query_idf,2);
        }

        DocumentDistance docDistnace;
        docDistnace.docName = docName;
        docDistnace.distance = sqrt(sqrSum);
        ditanceArray.push_back(docDistnace);

        //cout << "Distance : " << docDistnace.distance;
        //cout << endl << endl;
    }

    // Sorting the DocumentDistance vector
    sort(ditanceArray.begin(), ditanceArray.end(), compareDistances);
    return ditanceArray;
}

/**
 * Finds the ecludianDistance between 2 vectors
 * @param
 * Find the documents where the words in the query appear.
 * @param string query
 * @return string docs
 *
 * Define the data structure of the query
 * calculate the TF-IDF of the query
 * calculate the distance from every other document.
 * sort the documents in the order of their weights.
 * */
string search(string query) {
    istringstream iss(query);
    string word;
    vector<DocumentDistance> ditanceArray;

    double queryDocumentWord_tf_idf = 0;;


        iss >> word;
        // if it is a stop word, move on
        if (word.empty() || find(stopWords.begin(), stopWords.end(), word) != stopWords.end()) {
           return "Sorry, we could not find any relevant document to your query";
        }
        stemming(word);
        // search for the word in the index
        int k = findIndex(word);
        if (k < 0) {
            return "Sorry, we could not find any relevant document to your query";
        }

        //calculate the tf_idf
        if(indexItems[k].docs.size()!=0){
            double temp = static_cast<double>(docs.size())/ static_cast<double>(indexItems[k].docs.size());
            queryDocumentWord_tf_idf = log2(temp);
        }else{
            //condition to call knowledge base
        }

        ditanceArray = eclideanDistance(queryDocumentWord_tf_idf, word);



    // return message if nothing was found
    if (ditanceArray.empty()) {
        return "Sorry, we could not find any relevant document to your query";
    }


    /*   for (int i = 0; i != ditanceArray.size(); ++i)
     cout << ditanceArray[i].docName << " " << ditanceArray[i].distance;

     cout << endl; */



    // build result
    string docs;
    int i;
    for (i = 0; i < ditanceArray.size(); i++) {
        docs += ditanceArray[i].docName + ",";
    }

    return docs;
}

void print() {
    int size = indexItems.size();
    for (int i = 0; i < size; i++) {
        cout << indexItems[i].word << " : ";
        int size2 = indexItems[i].docs.size();
        for (int j = 0; j < size2; j++) {
            cout << indexItems[i].docs[j].doc << " " << indexItems[i].docs[j].tf_idf << " ";
        }
        cout << endl << endl;
    }
}

int main() {

    //invoke_class();

    ifstream file(INDEX_FILE);
    // if index file already exists
    if (file) {
        // load from file
        loadIndex();
    } else {
        // build index
        buildIndex();
    }

    print();

    cout << "Type your query: " << endl;
    string query;
    getline(cin, query);
    string res = search(query);
    cout << res << endl;

    return 0;
}

