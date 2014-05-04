#ifndef _NODE_H_
#define _NODE_H_

#include <iostream>
#include <vector>
#include <string>

#define APPEND 0
#define COPY 1

class Node 
{
public:
	Node();
	~Node();

	void append_child(Node*,  std::string);

	Node * get_child(int i);
	std::string get_value(int i);
	
	Node * pop_child(int i);
	std::string pop_value(int i);


	Node* get_last_child();
	void change_last_child(Node*);
	
	int getNumber() {return no;}
	int getCount() { return count;}

	std::vector<Node*>& getAllNodes(std::vector<Node*> &nodes);

	void setPos(int p) {pos = p;}
	int  getPos(){return pos;}

	
	void freeChildren(Node* end);
	void copyNode(Node* from, Node* end);
	void eraseEps(Node* end, int mode);

	void mergeChildren(Node* end, int mode);
	void addFather() {nFather ++;}
	void subFather() {nFather --;}
	int  getFather() {return nFather;}

	int isLeader() {return isleader;}
	int setLeader(int i) {isleader = i;}
	Node* getEnd() {return end;}
	void  setEnd(Node* n) {end = n;}

	void eraseNodeRepeat();

private:
	static int number;
	int no;
	int isleader;
	Node* end;
	int pos;
	int count;
	int nFather;
	std::vector<Node*> children;
	std::vector<std::string> values;
};

#endif
