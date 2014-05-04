#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>
#include <vector>
#include <string>
#include "node.h"

class Grammar
{
public:
	enum GType {GT_TOTAL ,GT_MAIN, GT_SUB, GT_VAR } ;
public :
	Grammar();
	Grammar(int);
	Grammar(std::string name, std::string value, GType type, FILE* fin);
	Grammar(std::string name, std::string raw);
	~Grammar();
	
	Node* parse();
	Node* getBegin() {return m_pbegin;}
	Node* getEnd() {return m_pend;};
	int   getEps() {return m_eps;}


	void setName(std::string name){m_name = name;}
	void setRaw(std::string raw){m_raw = raw;}
	void setType(GType type) {m_type = type;}
	void setFraw(FILE* f) {m_fraw =f;}
	void setBeginNode(Node* n){m_pbegin = n;} 
	void setEndNode(Node* n) {m_pend = n;}
	
	void setSeg(int s) {m_seg = s;}
	

	void merge(Node* , int);
private:
	int m_eps;

	FILE* m_fraw;
	GType m_type;
	int m_seg;
	std::string m_name;
	std::string m_raw;
	Node* m_pbegin;
	Node* m_pend;
};


class Parser 
{
public :
	Parser();
	Parser(const char* filename);
	~Parser();

	void setRootName(std::string name) {m_rootGram.setName(name);}
	void setRootRaw(std::string raw) {m_rootGram.setRaw(raw);}
	
	void printNode(const char* filename);
	int parse();
private:
	FILE* m_fin;
	Node* m_root;
	Grammar m_rootGram;
};


#endif
