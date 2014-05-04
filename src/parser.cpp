#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include "parser.h"

#include <string.h>

#define BUFF_SIZE 1024
using namespace std;

Grammar::Grammar()
	:m_pbegin(NULL), m_pend(NULL)
{
	m_name = "";
	m_raw = "";
	m_fraw = NULL;
	m_type = GT_MAIN;
	m_seg = 1;
}

Grammar::Grammar(int seg)
{
	m_pbegin = NULL;
	m_pend = NULL;
	m_name = "";
	m_raw = "";
	m_type = GT_MAIN;
	m_fraw = NULL;
	m_seg = seg;
}

Grammar::Grammar(string name, string raw) 
	:m_pbegin(NULL), m_pend(NULL), m_name(name), m_raw(raw)
{
	m_type = GT_MAIN;
	m_fraw = NULL;
	m_seg = 1;
}

Grammar::Grammar(string name, string value, GType type, FILE* fraw)
	:m_pbegin(NULL), m_pend(NULL), m_type(type), m_fraw(fraw), 
	m_name(name), m_raw(value)
{
	m_seg = 1;
}

Grammar::~Grammar() {}



Node * Grammar::parse() {
	
	if(m_type == GT_VAR) {
		if(m_fraw == NULL) {
			fprintf(stderr, "variable has no file!\n");
			return NULL;
		}
		fseek(m_fraw, 0, 0);
		int flag = 0;
		char line[BUFF_SIZE] = "";
		char mark[BUFF_SIZE] = "";
		m_raw = "";
		sprintf(mark, "<%s>", m_name.c_str());
		while(fgets(line, BUFF_SIZE, m_fraw) != NULL) {
			if(strncmp(line, mark, strlen(mark)) == 0) {
				flag = 1;
				m_raw += '(';
				continue;
			}
			if(flag == 1) {
				if(strchr(line, ';') == NULL)
					m_raw += line;
				else {
					m_raw += ')';
					break;
				}
			}
		}
	}
	Grammar g(m_seg);
	const char* pstart = NULL;
	int partial = 0;
	Node* tmp = NULL;


	Node* l_pbegin = new Node();

	Node* iter = l_pbegin;
	Node* pre = l_pbegin;
	int count = 0;
	const char * ptr  = m_raw.c_str();
	int sub = 0;
	int nChild = 0;

	char value[BUFF_SIZE] = "" ;
	while(*ptr != 0){
		switch (*ptr) {
			case '(':
				if(m_pbegin == NULL) {
					m_pbegin = new Node();
					sub = 1;
				}else {
					if(m_type != GT_TOTAL && sub == 0) {
						sub =1 ;
						break;
					}
					count ++;
					pstart = ptr;
					while(ptr ++) {
						if(*ptr == '(')
							count++;
						if(*ptr == ')') {
							count --;
							if(count == 0)
								break;
						}
					}
					memset(value, 0, sizeof(char)*BUFF_SIZE);
					strncpy(value, pstart, ptr + 1 - pstart);
					
					g.setRaw(value);
					g.setFraw(m_fraw);
					if(m_type == GT_TOTAL)
						g.setType(GT_MAIN);
					else 
						g.setType(GT_SUB);
					g.setBeginNode(iter);

					while(*++ptr == ' ' || *ptr == '\n')
						;
					if(*ptr == '|' || *ptr == ')') {
						if(m_pend == NULL) {
							tmp = new Node();
							pre = iter;
							iter = tmp;
							m_pend  = iter;
							g.setEndNode(m_pend);
						}else {
							g.setEndNode(m_pend);
						}
						iter = l_pbegin;
						pre = l_pbegin;
					} else {
						pre = iter;
						tmp = new Node();
						iter = tmp;
						g.setEndNode(iter);
						ptr --;
					}
					g.parse();
				}
				break;
			case ')':
				if(NULL == m_pend)
					m_pend = iter;
				else {
					if(iter->getCount() == 0)
						pre->change_last_child(m_pend);
					else
						iter->append_child(m_pend, "eps");
				}

				break;
			case '<':
				pstart = ++ptr;
				while(*ptr ++ != '>')
					;
				ptr --;
	
				memset(value, 0, 1024);
				strncpy(value, pstart, ptr-pstart);
			
				g.setBeginNode(iter);
				g.setName(value);
				g.setType(GT_VAR);
				g.setFraw(m_fraw);
				while(*++ptr == ' ' || *ptr == '\n')
					;
				if(*ptr == '|' || *ptr == ')') {
					if(m_pend == NULL) {
						tmp = new Node();
						pre = iter;
						iter = tmp;
						m_pend  = iter;
						g.setEndNode(m_pend);
					}else {
						g.setEndNode(m_pend);
					}
					iter = l_pbegin;
					pre = l_pbegin;
				} else {
					pre = iter;
					tmp = new Node();
					iter = tmp;
					g.setEndNode(iter);
					ptr --;
				}
				g.parse();
				break;
			case '"':
				pstart = ptr + 1;
				while(*++ptr != '"')
					;
				memset(value, 0, sizeof(char)*BUFF_SIZE);
				strncpy(value, pstart, ptr-pstart);
				//fprintf(stderr, "%s ", value);
				tmp = new Node();
				iter->append_child(tmp, value);
				pre = iter;
				iter = iter->get_last_child();
				break;
			case ' ':
			case '\n':
				break;
			case '|':
				if(NULL == m_pend) {
					m_pend = iter;
				}else {
					pre->change_last_child(m_pend);
				}
				//fprintf(stderr, "\n");
				iter = l_pbegin;
				pre = l_pbegin;
				break;
			case '[':
				pstart = ptr + 1;
				count ++;
				while(ptr++) {
					if(*ptr == '[')
						count ++;
					if(*ptr == ']') {
						count --;
						if(count == 0)
							break;
					}
				}
				memset(value, 0, 1024);
				strcat(value, "(");
				strncat(value, pstart, ptr-pstart);
				strcat(value, ")");
				
				g.setRaw(value);
				g.setFraw(m_fraw);
				g.setType(GT_SUB);
				g.setBeginNode(iter);

				while(*++ptr == ' ' || *ptr == '\n')
					;
				if(*ptr == '|' || *ptr == ')') {
					if(m_pend == NULL) {
						tmp = new Node();
						pre = iter;
						iter = tmp;
						m_pend = iter;
						g.setEndNode(m_pend);
						pre->append_child(tmp, "eps");
					}else {
						iter->append_child(m_pend, "eps");
						g.setEndNode(m_pend);
					}
					iter = l_pbegin;
					pre = l_pbegin;
				} else {
					pre = iter;
					tmp = new Node();
					iter = tmp;
					pre->append_child(iter, "eps");
					g.setEndNode(iter);
					ptr --;
				}
				g.parse();
				m_eps = 1;
				break;

			default:
				pstart = ptr ;
				while(*ptr  != ' ' && *ptr != '\n' && *ptr != '|'&& *ptr != ']'&& *ptr != ')')
					ptr ++;
				memset(value,0, BUFF_SIZE*sizeof(char));
				strncpy(value,  pstart, ptr-pstart);
				
				nChild = iter->getCount();
				int i ;
				if(nChild != 0 && m_seg == 1) {
					for(i = 0; i< nChild; i ++) {
						if(strcmp(value, iter->get_value(i).c_str()) == 0) 
							break;
					}
					if(i == nChild) {
						tmp = new Node();
						iter->append_child(tmp, value);
						//fprintf(stderr,"%s ", value);
						pre = iter;
						iter = iter->get_last_child();
					}else {
						pre = iter;
						iter = iter->get_child(i);
					}
				}else {
					if(iter == m_pend && m_seg == 1) {
						tmp = new Node();
						pre->pop_child(-1);
						string s = pre->pop_value(-1);
						pre->append_child(tmp, s);

						iter= pre->get_last_child();
						pre = iter;

						iter->append_child(m_pend, value);
						iter->append_child(m_pend, "eps");
						iter = l_pbegin;
						pre = l_pbegin;
						
						while(*ptr != '|' && *ptr != ')')
							ptr ++;
						ptr ++;
					}else {
						tmp = new Node();
						iter->append_child(tmp, value);
						//fprintf(stderr,"%s ", value);
						pre = iter;
						iter = iter->get_last_child();
					}
				}
				nChild = 0;

				ptr --;
				break;
		}
		ptr++;
	}
	if(m_type == GT_TOTAL || m_raw == "(<Contact>|<PhoneNumber>)") {
	//if(m_type == GT_TOTAL || m_name == "PhoneNumber") {
		if(m_type == GT_TOTAL)  {
			l_pbegin->eraseEps(m_pend, APPEND);
			merge(l_pbegin, APPEND);
		}else {
			l_pbegin->eraseEps(m_pend, COPY);
			merge(l_pbegin, APPEND);
			l_pbegin->setLeader(1);
			l_pbegin->setEnd(m_pend);
		}
	}



	int i;
	int gc = l_pbegin->getCount();
	for(i = 0; i < gc; i ++) {
		m_pbegin->append_child(l_pbegin->get_child(i), l_pbegin->get_value(i));
	}
	for(i = 0; i < gc; i ++) {
		l_pbegin->pop_child(-1);
	}
	m_pbegin->setLeader(l_pbegin->isLeader());
	m_pbegin->setEnd(l_pbegin->getEnd());
	delete l_pbegin; 

	return m_pbegin;
}

void Grammar::merge(Node* root, int mode) {
	root->mergeChildren(m_pend,  mode);
}
	

Parser::Parser() 
{
	m_root = NULL;
	m_fin = NULL;
}

Parser::Parser(const char* filename)
	:m_root(NULL) 
{
	m_fin = fopen(filename, "r");
}

Parser::~Parser() 
{
}

int Parser::parse()
{
	char rootGram[BUFF_SIZE] = "";
	char raw[BUFF_SIZE] = "";
	char line[BUFF_SIZE] = "";
	char mark[BUFF_SIZE] = "";

	if(m_fin == NULL) {
		fprintf(stderr, "File error\n");
		return -1;
	}

	while(fgets(line, BUFF_SIZE, m_fin) != NULL) {
		if(line[0] == '#') 
			continue;
		if(strncmp(line, "grammar", strlen("grammar")) == 0) {
			char* pname = line + strlen("grammar");

			while(*pname != 0 && *pname++ == ' ' )
				;
			if(*pname == 0 || *pname == '\n' || *pname == ';') {
				break;
			}else {
				char* end =strchr(pname, ';');
				strncpy(rootGram, pname-1, end-pname + 1);
				break;
			}
		}
	}

	if(rootGram[0] == 0) {
		fprintf(stderr, "The root grammar missing!\n");
		return -1;
	}

	sprintf(mark, "<%s>", rootGram);

	char *ptr = NULL;
	int flag = 0;
	while(fgets(line, BUFF_SIZE, m_fin) != NULL) {
		if( (ptr = strstr(line, mark)) != NULL) {
			ptr += strlen(mark);
			while(*ptr != 0 && *ptr++ != '(')
				;

			if(*ptr != 0) {
				char* pend = strchr(ptr, ';');
				if(pend == NULL)
					strcat(raw, ptr -1);
				else {
					strncpy(raw, ptr-1, pend-ptr+1);
					flag = 1;
				}
			}
			break;
		}
	}
	if(flag == 0) {
		ptr = NULL;
		while(fgets(line, BUFF_SIZE, m_fin) != NULL) {
			if( (ptr = strchr(line, ';')) != NULL) {
				strncat(raw, line, ptr-line);
				break;
			}else {
				strcat(raw, line);
			}
		}
	}


	setRootName(rootGram);
	setRootRaw(raw);
	m_rootGram.setFraw(m_fin);
	m_rootGram.setType(Grammar::GT_TOTAL);
	m_root = m_rootGram.parse();

	fclose(m_fin);

	return 0;
}

void Parser::printNode(const char* filename) 
{
	//cout<<endl;
	ofstream fout;
	fout.open(filename);
	fout<<"I 0"<<endl;

	list<Node*> allNodes;
	int pos = 0;
	m_root->setPos(pos);
	pos ++;
	allNodes.push_back(m_root);
	
	list<Node*>::iterator iter = allNodes.begin();
	for(; iter != allNodes.end();iter ++) {
		int count = (*iter)->getCount();

		if(count == 0) {
			fout<<"F "<<(*iter)->getPos()<<endl;
			//cout<<"F "<<(*iter)->getPos()<<endl;
			continue;
		}
		Node* tmp = NULL;

		for(int i = 0; i < count; i ++){
			tmp = (*iter)->get_child(i);

			list<Node*>::iterator it = allNodes.begin();
			for(; it != allNodes.end(); it ++) {
				if((*it)->getNumber() == tmp->getNumber())
					break;
			}
			if(it == allNodes.end())  {
				tmp->setPos(pos++);
				allNodes.push_back(tmp);
			}

			fout<<"T "<<(*iter)->getPos()<<" "<<tmp->getPos()<<" "<<(*iter)->get_value(i)<<" "<<(*iter)->get_value(i)<<endl;
			//cout<<"T "<<(*iter)->getPos()<<" "<<tmp->getPos()<<" "<<(*iter)->get_value(i)<<" "<<(*iter)->get_value(i)<<endl;

		}
	}

	for(iter = allNodes.begin(); iter != allNodes.end(); iter++) {
		delete *iter;
	}
}
