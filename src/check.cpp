#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
using namespace std;

enum NodeType {I, T, F};
typedef NodeType Type;

class Node
{
public:
	Node();
	Node(Type type, int number);
	~Node();

	int m_number;
	Type m_type;
	vector<int> m_next;
	vector<string> m_value;
};

Node::Node()
	:m_number(0), m_type(T)
{
}

Node::Node(Type type, int number):
	m_number(number), m_type(type) 
{
}

Node::~Node()
{
}

map<int, Node*>& genGraph(string filename, map<int, Node*>& g);
int  checkFileValidation(map<int, Node*>& graph);
void printNodes(vector<string>&, ostream& out);
void travelPath(Node* , vector<string>&);

int checkPathValidation(vector<string>&, map<int, Node*>& graph);

map<int, Node*> graph;
map<int, Node*> cgraph;

int main(int argc, char* argv[])
{
	graph = genGraph(argv[1], graph);
	cgraph = genGraph(argv[2], cgraph);

	if(checkFileValidation(cgraph) == 0) {
		cout<<"Invalid Graph"<<endl;
		return 0;
	}

	vector<string> strings;
	travelPath(graph[0], strings);
	cout<<endl<<endl;

	for(map<int, Node*>::iterator iter = graph.begin(); iter != graph.end(); iter ++) {
		delete iter->second;
	}

	return 0;
}

map<int, Node*>& genGraph(string filename, map<int, Node*>& graph) {
	string line;
	ifstream fin(filename.c_str());
	stringstream sstream;

	int number;
	char type;
	int next;
	string value;
	string value1;

	while(getline(fin,line))  {
		//cout<<line<<endl;
		if(line[0] == '#' || line[0] == 'I')
			continue;
		else if(line[0] == 'F') {
			sstream.clear();
			sstream<<line;
			sstream>>type>>number;
			Node * n = new Node(F, number);
			graph[number] = n;
		}else {
			sstream.clear();
			sstream<<line;
			sstream>>type>>number>>next>>value>>value1;

			map<int, Node*>::iterator it;
			it = graph.find(number);

			if(it == graph.end()) {
				Node *n = new Node(T, number);
				n->m_next.push_back(next);
				n->m_value.push_back(value);
				graph[number] = n;
			}else {
				it->second->m_next.push_back(next);
				it->second->m_value.push_back(value);
			}
				
		}
	}
	fin.close();
	return graph;
}

int  checkFileValidation(map<int, Node*>& graph) {
	for(map<int, Node*>::iterator iter=graph.begin(); iter != graph.end(); iter ++) {
		vector<string>& strings = iter->second->m_value;
		set<string> checkSet;
		for(vector<string>::iterator it = strings.begin(); it!= strings.end(); it ++) {
			if(checkSet.find(*it) != checkSet.end()) {
				cerr<<iter->first<<endl;
				cerr<<*it<<endl;
				return 0;
			}else {
				checkSet.insert(*it);
			}
		}
	}
	return 1;
}
void travelPath(Node* n, vector<string> & strings) {
	if(n->m_type == F)  {
		printNodes(strings, cout);
		if(checkPathValidation(strings, cgraph ) == 0) {
			cerr<<"Error in Validation"<<endl;
			exit(1);
		}
		return ;
	}
	set<int> checkedNodes;
	int max = n->m_next.size();
	for(int i = 0; i < n->m_next.size(); i += 1) {
		if(n->m_value[i] == "eps")
			continue;
		if(checkedNodes.find(n->m_next[i]) != checkedNodes.end()) 
			continue;
		else 
			checkedNodes.insert(n->m_next[i]);
		strings.push_back(n->m_value[i]);
		travelPath(graph[n->m_next[i]], strings);
		strings.pop_back();
	}
}

void printNodes(vector<string>& strings, ostream& out) {
	for(vector<string>::iterator iter=strings.begin(); iter != strings.end(); iter ++)
		if((*iter)!= "eps")
			out<<(*iter)<<" ";
	out<<endl;
}

int checkPathValidation(vector<string>& path, map<int, Node*>& graph) {
	static int number = 0;
	number ++;
	//cout<<number<<endl;
	Node* start = graph.find(0)->second;
	Node* pNode = start;
	
	for(vector<string>::iterator iter = path.begin(); iter != path.end(); iter ++) {
		int i;
		for(i = 0; i < pNode->m_value.size(); i++) {
			if(pNode->m_value[i] == *iter)
				break;
		}
		if(i == pNode->m_value.size()) {
			for(i = 0; i < pNode->m_value.size(); i++) {
				if(pNode->m_value[i] == "eps")
					break;
			}
			if(i == pNode->m_value.size())
				return 0;
			pNode = graph[pNode->m_next[i]];
			iter--;
		}
		else
			pNode = graph[pNode->m_next[i]];
	}
	return 1;
}
