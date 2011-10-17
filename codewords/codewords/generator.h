#include "create_board.h"
#include "info_struct.h"

class board_consultant{
public:
	board_consultant(){}
	board_consultant(int rows, int cols, int tile_size);
	void fill_strategy();
	std::string get_parents(int w_counter, std::string w_type);


private:
	std::string mostCHeuristic();
	int calcHeuristicValue(std::string whichDB, int letter_count,
		std::string query_word, int i);
	std::string switcherDB(int len_word);
	std::string random_seed();

	float ratio;
	int num_rows, num_cols, backtrack;
	std::vector<std::vector<tile*>> tiles;
	word** across;
	word** down;
	int across_size, down_size;
	std::vector<std::string> open_list;
	std::vector<info_struct> closed_list;
	update_board* update;
};

board_consultant::board_consultant(int rows, int cols, int tile_size){
	num_rows = rows;
	num_cols = cols;
	backtrack = 0;	

	create_board board(rows, cols, tile_size);
	tiles = board.get_tiles();
	across = board.get_across();
	down = board.get_down();
	across_size = board.get_asize();
	down_size = board.get_dsize();

	//Hash hasher = new Hash(num_rows, num_cols, tiles, down, across;
	update = new update_board(tiles, num_rows, num_cols);

}


////////////////////////
//Generation algorithm//
////////////////////////

//return most constrained pattern in the board
std::string board_consultant::mostCHeuristic(){
	std::string which = "", type = "", query_word = "";
	int len = 0, counter = 0, letter_count = 0, prev_counter = 0;

	for(int i=0; i<across_size; i++){
		len = across[i]->get_end()->y - across[i]->get_end()->x;
		which = switcherDB(len+1);
		query_word = update->get_word(across[i]->get_start()->x, across[i]->get_start()->y, across[i]->get_end()->y, "across");
		for(int k=0; k<query_word.length(), k++)
			if(query_word[k] != '_')
				letter_count++;
		counter = calcHeuristicValue(which, letter_count, query_word, i);
		if(counter != -1){
			type = "across";
			prev_counter = counter;
		}
		else
			counter = prev_counter;
		letter_count = 0;
	}
	letter_count = 0;

	for(int i=0; i<down_size; i++){
		len = down[i]->get_end()->x - down[i]->get_start()->x;
		which = switcherDB(len+1);
		query_word = update->get_word(down[i]->get_start()->x, down[i]->get_start()->y,
			down[i]->get_end()->y, "down");
		for(int k=0; k<query_word.length(); k++)
			if(query_word[k] != '_')
				letter_count++;
		counter = calcHeuristicValue(which, letter_count, query_word, i);
		if(counter!= -1){
			type = "down";
			prev_counter = counter;
		}
		else
			counter = prev_counter;
		letter_count = 0;
	}

	if(type=="across")
		query_word = update->get_word(across[counter]->get_start()->x, across[counter]->get_start()->y, across[counter]->get_end()->y, "across");
	else if(type == "down")
		query_word = update->get_word(down[counter]->get_start()->x, down[counter]->get_start()->y, down[counter]->get_end().y, "down");
	return counter+"#"+type;

}

int board_consultant::calcHeuristicValue(std::string which, int letter_count, std::string query_word, int i){
	int counter = 01;
	std::string sql = "";
	if(ratio <= ((float)letter_count)/((float)query_word.length()) &&
		letter_count != query_word.length() && letter_count != 0){
			ratio = ((float)letter_count)/(float)query_word.length());
			letter_count = 0;

	}


}