#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <algorithm>

class dictionary{
public:
	dictionary();
	void load_dict(std::string filename);
	void big_sort();
	bool find(std::string word);
	bool insert(std::string word);
	bool print_dict(std::string filename);
	void save_dict(std::string filename);
	void populate_dict(std::string filename);
	void view_dict();

private:
	std::vector <std::vector <std::string>> dict;
	int size;
	void small_sort(int i);
	bool stripper(std::string &word);

};

dictionary::dictionary(){
	size=0;
	for(int i=0; i<26; i++){
		std::vector <std::string> letter;
		dict.push_back(letter);
	}
}

void dictionary::view_dict(){
	for(unsigned int i=0; i<26; i++)
		for(unsigned int k=0; k<dict[i].size(); k++)
			std::cout << dict[i][k] << "\n";

	std::cout << "\nSize: " << size << "\n";
}

void dictionary::load_dict(std::string filename){
	std::fstream input(filename.c_str());
	for(unsigned int i=0; i<26; i++)
		dict[i].clear();
	size = 0;
	char first = 'X';
	char second = 'X';

	std::cout << "Progress:";
	while(!input.eof()){
		std::string word;
		input >> word;
		first = word[0];
		insert(word);
		if(first!=second)
			std::cout << "-";
		second = first;
	}
	
	big_sort();

	std::cout << "Dictionary size is " << size << "\n";

	input.close();
}

//returns true if successful
bool dictionary::insert(std::string word){
	if(stripper(word)){
		char c = (int)word[0]-97;
		if(c<0) return false;
		if(!find(word)){
			dict[c].push_back(word);
			size++;
			small_sort(c);
			return true;
		}
	}
	return false;
}

int bin_search(std::vector <std::string> &dir, std::string word, int low, int hi){
	if(hi<low)
		return -1;
	int mid = low + (hi-low)/2;
	if(dir[mid] > word)
		return bin_search(dir, word, low, mid-1);
	else if(dir[mid] < word)
		return bin_search(dir, word, mid+1, hi);
	else
		return mid;
}

bool dictionary::find(std::string word){
	char c = (int)word[0]-97;
	if(c<0) return false;

	int index = bin_search(dict[c], word, 0, dict[c].size()-1);
	if(index==-1)
		return false;
	else
		return true;
}

void dictionary::small_sort(int i){
	sort(dict[i].begin(), dict[i].end());
}

void dictionary::big_sort(){
	for(unsigned int i=0; i<dict.size(); i++)
		small_sort(i);
}

void dictionary::save_dict(std::string filename){
	std::fstream output(filename.c_str());

	for(unsigned int i=0; i<26; i++)
		for(unsigned int k=0; k<dict[i].size(); k++)
			output << dict[i][k] << "\r\n";

	output.close();
}

bool dictionary::stripper(std::string &word){
	for(unsigned int i=0; i<word.length(); i++){
		if(word[i]=='\'')
			return false;
		if(word[i]>=65 && (int)word[i]<=90)
			return false;
		if((int)word[i]<97 || (int)word[i]>122){
			word.erase(i, 1);
			i--;
		}
	}
	return true;
}

void dictionary::populate_dict(std::string filename){
	std::fstream output(filename.c_str());

	while(!output.eof()){
		std::string word;
		output >> word;
		insert(word);
	}


	std::cout << "New dictionary size is " << size << "\n";
	_getch();

	output.close();

	_getch();

}