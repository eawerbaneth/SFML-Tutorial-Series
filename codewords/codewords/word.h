class Value{
public:
	int x, y;
	Value(){}
	Value(int new_x, int new_y){x=new_x; y=new_y;}
};

class word{
public:
	word(){}
	word(Value* nstart, Value* nend){start=nstart; end=nend;}
	Value* get_start(){return start;}
	Value* get_end(){return end;}
private:
	Value* start;
	Value* end;
};