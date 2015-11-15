//============================================================================
// Name        : SearchEngine.cpp
// Author      : Tirumala Rao Kavala
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "dirent.h"

using namespace std;

int main() {

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("./texts_project")) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
	    printf ("%s\n", ent->d_name);
	    const string dir_name = "./texts_project";
	    const string file_name = ent->d_name;
	    const string full_file_name = dir_name + "/" + file_name;
		 string line;
		  ifstream myfile (full_file_name.c_str());
		  if (myfile.is_open())
		  {
		    while ( getline (myfile,line) )
		    {
		      cout << line << '\n';

		    }
		    myfile.close();
		  }

		  else cout << "Unable to open file";
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
	  perror ("");
	  return EXIT_FAILURE;
	}

	  return 0;
}
