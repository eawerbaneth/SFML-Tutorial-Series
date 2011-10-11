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



}