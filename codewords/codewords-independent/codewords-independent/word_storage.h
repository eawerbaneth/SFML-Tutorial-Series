#include <vector>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>

// --create a hash table and store the words based on length
class words_hash{
private:
	std::vector <std:: vector <std::string>> table;

public:
	words_hash(){}
	words_hash(std::string filename);
	std::string get_random_word(int i);
	int get_words(int i);
};

int words_hash::get_words(int i){
	return table[i].size();
}

std::string words_hash::get_random_word(int i){
	int rand = sf::Randomizer.Random(0, table[i].size());
	std::string word = table[i][rand];
	table[i].erase(table[i].begin() + rand);
	return word;

}

words_hash::words_hash(std::string filename){
	std::ifstream input(filename.c_str());
	std::string reader;

	for(unsigned int i=0; i<20; i++){
		std::vector <std::string> temp;
		table.push_back(temp);
	}
	
	while(!input.eof()){
		input >> reader;
		table[reader.length()].push_back(reader);
	}
}
