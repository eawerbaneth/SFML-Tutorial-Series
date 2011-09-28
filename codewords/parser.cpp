#include "dictionary.h"
#include <Windows.h>
#include <stdlib.h>
using namespace std;

/*
void populate_database(std::vector<std::vector <std::string>> &words){
	std::fstream output;
	output.open("database.txt");
	int working =0;
	while(!output.eof()){
		std::string word;
		output >> word;
		int c = (int)word[0]-97;
		if(c<0)
			continue;
		words[c].push_back(word);
		working++;
	}
	std::cout << "Current size: " << working << "\n";
	output.close();

	_getch();
}
*/

int main(){
	
	char input=1;
	dictionary my_dict;
	std::string str;



	while(input!='0'){
		system("CLS");
		cout << "------------BETH'S DICTIONARY PROGRAM------------\n";
		cout << "\tNext Command:\n";
		cout << "\t(1) Load Existing Dictionary from File\n";
		cout << "\t(2) Load Document from File\n";
		cout << "\t(3) View Current Dictionary\n";
		cout << "\t(4) Save Current Dictionary to File\n";
		cout << "\t(0) Exit Program\n";
		cin >> input;

		if(input=='1'){
				system("CLS");
				cout << "Please enter dictionary filename from which to load...\n";
				cin >> str;
				my_dict.load_dict(str);
		}
		else if(input=='2'){
				system("CLS");
				cout << "Please enter document filename from which to load...\n";
				cin >> str;
				my_dict.populate_dict(str);
		}
		else if(input=='3'){
				system("CLS");
				my_dict.view_dict();
				_getch();
		}
		else if(input=='4'){
			system("CLS");
				cout << "Enter filename to save to...\n";
				cin >> str;
				my_dict.save_dict(str);
		}
		else if(input!='0'){
				system("CLS");
				cout << "Invalid command... Enter {1, 2, 3, 4}\n";
				_getch();
		}
	}


	/*
	std::fstream input;
	std::fstream output;
	input.open("test3.txt");
	std::string word;

	std::vector <std::vector <std::string>> words;
	
	for(unsigned int i=0; i<26; i++){
		std::vector <std::string> new_vector;
		words.push_back(new_vector);
	}

	populate_database(words);

	output.open("database.txt");

	while(!input.eof()){
		input >> word;
		if(stripper(word)){
			bool found = false;
			int c = (int)word[0]-97;
			//something weird happened (picked up a newline), throw it away
			if(c<0)
				continue;
			for(unsigned int i=0; i<words[c].size(); i++){
				if(words[c][i]==word)
					found=true;
			}
			if(!found){
				words[c].push_back(word);
				output << word <<"\r\n";
			}
	
			std::cout << "next!\n";
		}

	}

	int working_set = 0;

	for(unsigned int k=0; k<words.size(); k++){
		for(unsigned int j=0; j<words[k].size(); j++){
			output << words[k][j] << "\r\n";
			working_set++;
		}
	}

	std::cout << working_set << "\n";

	input.close();
	output.close();

	*/

}