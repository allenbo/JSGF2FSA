#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include "node.h"
#include <stdio.h>
using namespace std;


int Node::number = 0;
Node::Node() {
	no = number++;
	count = 0;
	pos = 0;
	nFather = 0;
	isleader = 0;
	end = NULL;
	//cout <<"Node "<<no<<endl;
}
Node::~Node() {
	//cout<<"Node " <<no <<" deconstructed"<<endl;
}

void Node::append_child(Node* n, std::string s) {
	if(NULL == n) {
		cerr<<"Node Is Empty~"<<endl;
		return ;
	}
	
	children.push_back(n);
	n->addFather();
	values.push_back(s);
	count ++;
}

Node* Node::get_child(int i)  {
	return children[i];
}

string Node::get_value(int i) {
	return values[i];
}

Node* Node::pop_child(int i) {
	if(i < 0) {
		Node* n = children.back();
		n->subFather();
		children.pop_back();
		count --;
		return n;
	}else {
		Node* n = children[i];
		n->subFather();
		children.erase(children.begin()+i);
		count --;
		return n;
	}
}

string Node::pop_value(int i) {
	if(i < 0) {
		string s = values.back();
		values.pop_back();
		return s;
	}else {
		string s = values[i];
		values.erase(values.begin()+i);
		return s;
	}
}

Node* Node::get_last_child() {
	return get_child(count-1);
}

void Node::change_last_child(Node* n)
{
	Node * pre = pop_child(-1);
	delete pre;
	children.push_back(n);
	n->addFather();
	count ++;
}

vector<Node*>& Node::getAllNodes(vector<Node*> &nodes) {
	if(count == 0) {
		return nodes;
	}

	//nodes.push_back(children[0]);
	
	for(int i = 0 ; i < count; i ++) {
		if(find(nodes.begin(), nodes.end(), children[i]) == nodes.end()) 
			nodes.push_back(children[i]);
	}

	return nodes;
}



void Node::freeChildren(Node* end) {
	if(this == end) {
		return;
	}

	list<Node*> allNodes ;
	
	if(nFather == 1) {
		allNodes.push_back(this);
	}

	list<Node*>::iterator iter = allNodes.begin();

	for(; iter != allNodes.end(); iter++) {
		int count  = (*iter)->getCount();
		if(count == 0) 
			continue;
		
		Node * tmp = NULL;
		for(int i = 0; i< count; i ++) {
			tmp = (*iter)->get_child(i);
			list<Node*>::iterator it = allNodes.begin();
			for(; it != allNodes.end(); it++){
				if((*it)->getNumber() == tmp->getNumber())
					break;
			}

			if(it == allNodes.end() && tmp != end && tmp->getFather() == 1) 
				allNodes.push_back(tmp);
		}
	}

	for(iter = allNodes.begin(); iter != allNodes.end(); iter++)
		delete *iter;

}

void Node::copyNode(Node* from, Node* end)  {
	if(this == end) {
		fprintf(stderr, "Node is end when copy from others!\n");
		return ;
	}

	int oc = from->getCount();

	Node* tmp = NULL;
	Node* n = NULL;
	
	int i;
	for(i = 0; i < oc; i ++) {
		if(from->get_child(i) == end) {
			append_child(end, from->get_value(i));
			continue;
		}
		
		if(from->get_child(i) != tmp) 
		{
			if(tmp != NULL) {
				n->copyNode(tmp, end);
			}
			n = new Node();
			tmp = from->get_child(i);
			append_child(n, from->get_value(i));
		} else {
			append_child(n, from->get_value(i));
		}
	}
	if(NULL != n) 
		n->copyNode(tmp, end);
}


void Node::eraseEps(Node* end, int mode) {
	if(this == end) 
		return;
	 
	int i;
	int j;
	Node* nEps = NULL;
	int number = 0;
	static int cc = 0;
	
	//cout<<"Erase time "<<cc++<<endl;
	for(i = 0;i < getCount(); i++) {
		number = 0;
		if(values[i] == "eps") {
			nEps = children[i];
			if(nEps == end)
				return;
			
			/*if(nEps->isLeader()) {
				this->setLeader(1);
				this->setEnd(nEps->getEnd());
			}*/
			for(j = 0; j < getCount(); j ++) {
				if(children[j] == nEps)
					number ++;
			}
			if(mode == APPEND) {
				int j;
				for(j = 0; j < nEps->getCount(); j++) {
					append_child(nEps->get_child(j), nEps->get_value(j));
				}
			}else {
				this->copyNode(nEps, end);
			}

			if(number == 1 && nEps->getFather() == 1) {
				delete nEps;
			}

			pop_child(i);
			pop_value(i);
			i -= 1;
		}
	}
	vector<Node*> nodes;
	nodes = getAllNodes(nodes);
	Node* tmp;
	for(vector<Node*>::iterator iter = nodes.begin(); iter != nodes.end(); iter ++ ) {
		tmp = (*iter);
		tmp->eraseEps(end, mode);
	}

}

void Node::eraseNodeRepeat() {
	if(end == this)
		return;

	vector<Node*> childrenNodes;
	childrenNodes = getAllNodes(childrenNodes);

	if(childrenNodes.size() == count)
		return ;

	vector<string> singleNodes;
	int i;
	for(vector<Node*>::iterator iter = childrenNodes.begin(); iter != childrenNodes.end(); iter ++) {
		singleNodes.clear();
		for(i = 0; i < count; i ++) {
			if(children[i] == *iter) {
				if(find(singleNodes.begin(), singleNodes.end(), values[i]) != singleNodes.end()){
					pop_child(i);
					pop_value(i);
					i -= 1;
				}else {
					singleNodes.push_back(values[i]);
				}
			}
		}
	}
}


void Node::mergeChildren(Node* end, int mode) {
	if(this == end) {
		return;
	}

	//eraseEps(end);
	static int num = 0;
	int i;
	int j;
	
	eraseNodeRepeat();

	Node * n1 = NULL;
	Node * n2 = NULL;

	set<string> nodes1;
	set<string> nodes2;
	set<string> inter;
	set<string> diff1;
	set<string> diff2;

	vector<int> inds;
	
	int endFlag = 0;
	for(i = 0; i < count; i ++) {
		for(j = i+1; j < count; j++) {
			if(values[j] == values[i])
				break;
		}
		if(j != count) {
			n1 = children[i];
			n2 = children[j];
		}else 
			continue;
		
		nodes1.clear();
		nodes2.clear();

		for(j = 0; j < count; j ++) {
			if(n1 == children[j])
				nodes1.insert(values[j]);
			else if(n2 == children[j])
				nodes2.insert(values[j]);
		}
		
		inter.clear();
		diff1.clear();
		diff2.clear();

		set_intersection(nodes1.begin(), nodes1.end(),
				  nodes2.begin(), nodes2.end(),
				  insert_iterator< set<string> >( inter, inter.begin() )
				  );
		
		set_difference(nodes1.begin(), nodes1.end(),
				       inter.begin(), inter.end(),
					   insert_iterator< set<string> >(diff1, diff1.begin() )
					   );
		set_difference(nodes2.begin(), nodes2.end(),
				       inter.begin(), inter.end(),
					   insert_iterator< set<string> >(diff2, diff2.begin() )
					   );

		if(diff1.size() == 0) {
			//fprintf(stdout, "handle end copy in diff1   %d\n", num++);
			if(n1 == end) {
				Node* newNode = new Node();
				for(j = 0; j < count; j ++)
					if(children[j] == n1) {
						children[j] = newNode;
						n1->subFather();
					}

				newNode->append_child(end, "eps");
				n1 = newNode;
				endFlag = 1;
			}
			Node* tmp;
			if(mode == COPY || endFlag == 1) {
				endFlag = 0;
				n1->copyNode(n2, end);
				tmp = n2;
				if(diff2.size() == 0 && n2->getFather() == 0) {
					n2->freeChildren(end);
				}
				n2 = tmp;
			}else {
				if(n2 != end)
					for(j = 0; j < n2->getCount(); j ++) {
						n1->append_child(n2->get_child(j), n2->get_value(j));
					}
				else
					n1->append_child(end, "eps");
				tmp = n2;
				if(diff2.size() == 0 && n2->getFather() == 0) {
					for(j = 0; j < n2->getCount(); j ++) {
						n2->pop_child(-1);
						n2->pop_value(-1);
					}
					delete n2;
				}
				n2 = tmp;
			}
			
			for(j = 0; j < count; j ++) {
				if(children[j] == n2 && inter.find(values[j]) != inter.end() ) {
					pop_child(j);
					pop_value(j);
					j --;
				}
			}
			
		}else {
			//fprintf(stdout, "handle end copy in diff2 %d\n", num++);
			if(diff2.size() == 0) {
				if(n2 == end) {
					Node* newNode = new Node();
					for(j = 0; j < count; j ++)
						if(children[j] == n2) {
							children[j] = newNode;
							n2->subFather();
						}

					newNode->append_child(end, "eps");
					n2 = newNode;
				}
				if(mode == COPY || endFlag == 1) {
					endFlag = 0;
					n2->copyNode(n1, end);
				}else {
					for(j = 0; j < n1->getCount(); j ++) {
						n2->append_child(n1->get_child(j), n1->get_value(j));
					}
				}

				for(j = 0; j < count; j ++) {
					if(inter.find(values[j]) != inter.end() && children[j] == n1) {
						pop_child(j);
						pop_value(j);
						j --;
					}
				}
			}else {
				Node * newNode = new Node();
				for(set<string>::iterator it = inter.begin(); it != inter.end(); it++) 
					append_child(newNode, (*it));
				
				if(mode == COPY) {
					newNode->copyNode(n1, end);
					newNode->copyNode(n2, end);
				}else {
					if(n1 != end) 
						for(j = 0; j < n1->getCount(); j++) {
							newNode->append_child(n1->get_child(j), n1->get_value(j));
						}
					else
						newNode->append_child(end , "eps");
					
					if(n2 != end) 
						for(j = 0; j < n2->getCount(); j++) {
							newNode->append_child(n2->get_child(j), n2->get_value(j));
						}
					else
						newNode->append_child(end, "eps");
				}
				for(j = 0; j < count; j ++) {
					if(inter.find(values[j]) != inter.end() && (children[j] == n1 || children[j] == n2) ) {
						pop_child(j);
						pop_value(j);
						j --;
					}
				}
			}
		}

		i = -1;
	}
	//ismerged = 1;
	vector<Node*> childrenNodes;
	childrenNodes = getAllNodes(childrenNodes);
	for(vector<Node*>::iterator iter = childrenNodes.begin(); iter != childrenNodes.end(); iter++){
		Node *tmp = (*iter);
		
		if(tmp->isLeader()) 
			//tmp = tmp->getEnd();
			continue;

		tmp->mergeChildren(end, mode);
	}

}
