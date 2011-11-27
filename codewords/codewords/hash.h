#include "tile.h"
#include "word.h"
#include <vector>
#include <string>

/*
class hash_table{
public:


private:

};

class hash{
public:


private:
	int num_rows, num_cols;
	std::vector<std::vector<tile*>> tiles;


};*/

class update_board{
public:
	update_board(){}
	update_board(std::vector<std::vector<tile*>> &ntiles, int rows, int cols);
	std::vector<std::vector<tile*>> get_tiles(){return tiles;}
	void set_word(word* nword, std::string word_to_set);
	void remove_word(word* nword);
	std::string get_word(int startx, int starty, int endy, std::string type);
	void reset_all();

private:
	std::vector<std::vector<tile*>> tiles;
	int num_rows, num_cols;

};

update_board::update_board(std::vector<std::vector<tile*>> &ntiles, int rows, int cols){
	tiles = ntiles;
	num_rows=rows;
	num_cols=cols;
}

//sets a word to correspond to a string
void update_board::set_word(word* nword, std::string word_to_set){
	int len = word_to_set.length();
	if((nword->get_end()->x - nword->get_start()->x + 1) != len &&
		(nword->get_end()->y - nword->get_start()->y +1) != len)
		return;
	int count = 0;
	
	if(nword->get_start()->y != nword->get_end()->y){
		for(int i=nword->get_start()->y; i<nword->get_end()->y;i++){
			tiles[nword->get_start()->x][i]->set_letter(word_to_set[count]);
			count++;
		}
	}
	else if(nword->get_start()->x != nword->get_end()->x){
		for(int i=nword->get_start()->x; i<nword->get_end()->x; i++){
			tiles[i][nword->get_start()->y]->set_letter(word_to_set[count]);
			count++;
		}
	}

}

std::string update_board::get_word(int startx, int starty, int endy, std::string type){
	std::string completed = "";
	char letter;
	if(type=="across"){
		for(int j=starty;j<=endy;j++){
			letter = tiles[startx][j]->get_letter();
			if(letter==NULL)
				completed += "_";
			else
				completed += letter;
		}
	}
	else{
		for(int i=startx; i<=endy;i++){
			letter = tiles[i][starty]->get_letter();
			if(letter==NULL)
				completed += "_";
			else
				completed += letter;
		}
	}
	return completed;
}


//removes a word from the board
void update_board::remove_word(word* nword){
	if(nword->get_start()->y != nword->get_end()->y)
		for(int i=nword->get_start()->y; i<=nword->get_end()->y; i++)
			if(tiles[nword->get_start()->x][i]->get_letter_usage() <2)
				tiles[nword->get_start()->x][i]->remove_letter();
	else if (nword->get_start()->x != nword->get_end()->x)
		for(int i=nword->get_start()->x; i<= nword->get_end()->x; i++)
			if(tiles[i][nword->get_start()->y]->get_letter_usage() <2)
				tiles[i][nword->get_start()->y]->remove_letter();

}

void update_board::reset_all(){
	for(int r=0; r<num_rows; r++)
		for(int c=0; c<num_cols; c++)
			if(tiles[r][c]->get_state()!=0)
				tiles[r][c]->reset_all();
}