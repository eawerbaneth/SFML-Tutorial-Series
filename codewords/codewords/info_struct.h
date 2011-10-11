#include <string>

class info_struct{
public:
	info_struct();
	info_struct(int nsize, int npos, std::string ntype, int ncounter){
		size=nsize; pos=npos; type=ntype; counter=ncounter;
	}
	int get_size(){return size;}
	int get_pos(){return pos;}
	int get_counter(){return counter;}
	std::string get_type(){return type;}
	void set_size(int nsize){size=nsize;}
	void set_pos(int npos){pos=npos;}
	void set_counter(int ncounter){counter=ncounter;}
	void set_type(std::string ntype){type=ntype;}

private:
	int size, pos, counter;
	std::string type;
};

