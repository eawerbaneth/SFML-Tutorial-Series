#include "tile.h"
#include "word.h"
#include <vector>

sf::Image tile_img;

class create_board{
public:
	create_board(){}
	create_board(int rows, int cols, int tile_size);
	void set_board();
	void compile_words();
	void set_numbers();

	//accessing privates
	std::vector<std::vector<tile*>> get_tiles(){return tiles;}
	word** get_across(){return across;}
	word** get_down(){return down;}
	int get_asize(){return across_size;}
	int get_dsize(){return down_size;}

private:
	std::vector<std::vector<tile*>> tiles;
	word** across;
	word** down;
	int num_rows, num_cols;
	int across_size, down_size;
};


create_board::create_board(int rows, int cols, int tile_size){
	num_rows = rows;
	num_cols = cols;

	for(int a=0; a<num_rows; a++){
		std::vector <tile*> new_row;
		for(int b=0; b<num_cols; b++)
			new_row.push_back(new tile(a, b, tile_size, &tile_img));
		tiles.push_back(new_row);
	}


}

//sets states
void create_board::set_board(){
	for(int a=0; a<num_rows; a++)
		for(int b=0; b<num_cols; b++)
			tiles[a][b]->set_state(1);

	for(int i=1; i<14; i+=2)
		for(int j=0; j<15; j+=2)
			tiles[i][j]->set_state(0);


	tiles[0][6]->set_state(0);
	tiles[2][10]->set_state(0);
	tiles[4][8]->set_state(0);
	tiles[5][1]->set_state(0);
	tiles[5][11]->set_state(0);
	tiles[6][4]->set_state(0);
	tiles[6][5]->set_state(0);
	tiles[6][6]->set_state(0);
	tiles[8][8]->set_state(0);
	tiles[8][9]->set_state(0);
	tiles[8][10]->set_state(0);
	tiles[9][3]->set_state(0);
	tiles[9][13]->set_state(0);
	tiles[10][6]->set_state(0);
	tiles[12][4]->set_state(0);
	tiles[14][8]->set_state(0);
}

//stores every word in the board into 'word' object
void create_board::compile_words(){
	bool collecta = false, collectd = false;
	int counta = 0, countd = 0, r=0, c=0;
	for(r=0; r<num_rows; r++){
		for(c=0;c<num_cols;c++){
			if(tiles[r][c]->get_state() != 0 && !collecta && tiles[r][c+1]->get_state() != 0)
				collecta = true;
			if((tiles[r][c]->get_state()==0 || c == num_cols-1) && collecta){
				collecta = false;
				counta++;
			}
			if(tiles[c][r]->get_state() !=0 && !collectd && c!=num_rows-1 && tiles[c+1][r]->get_state()!=0)
				collectd = true;
			if((tiles[c][r]->get_state() == 0 || c == num_rows-1) && collectd){
				collectd = false;
				countd++;
			}
		}
	}
	
	across = new word*[counta];
	down = new word*[countd];
	across_size = counta;
	down_size = countd;

	collecta=collectd=false;
	counta=countd=0;
	int startxa = 0, startya = 0, startxd = 0, startyd = 0;
	for(r=0; r<num_rows; r++){
		for(c=0;c<num_cols;c++){
			if(tiles[r][c]->get_state() != 0 && !collecta && tiles[r][c+1]->get_state() != 0) {
				collecta = true;
				startxa = r;
				startya = c;
			}
			if(collecta)
				tiles[r][c]->increment_usage();
			if((tiles[r][c]->get_state() == 0 || c == (num_cols-1)) && collecta) {
				int val = c-1;
				if (c == num_cols-1)
					val = c;
				collecta = false;
				across[counta] = new word(new Value(startxa, startya), new Value(r,val));
				counta++;
			}
			if(tiles[c][r]->get_state() != 0 && !collectd && c != (num_rows-1) && tiles[c+1][r]->get_state() != 0) {
				collectd = true;
				startxd = c;
				startyd = r;
			}
			if(collectd)
				tiles[c][r]->increment_usage();
			if((tiles[c][r]->get_state() == 0 || c == (num_rows-1)) && collectd) {
				int val = c-1;
				if (c == num_rows-1)
					val = c;
				collectd = false;
				down[countd] = new word(new Value(startxd,startyd), new Value(val,r));
				countd++;
			}
		}
	}

}

void create_board::set_numbers(){
	for(int i=0;i<across_size;i++)
		tiles[across[i]->get_start()->x][across[i]->get_start()->y]->set_to_start();
	for(int i=0; i<down_size;i++)
		tiles[down[i]->get_start()->x][down[i]->get_start()->y]->set_to_start();

	int count = 0;
	for(int r=0; r<num_rows;r++){
		for(int c=0; c<num_cols;c++){
			if(tiles[r][c]->is_start()){
				tiles[r][c]->set_word_num(count);
				count++;
			}
		}
	}

}