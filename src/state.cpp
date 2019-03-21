/*
 * state.cpp
 * 
 * Last modify: 2010/11/02
 * 
 * Author: Massimiliano Macchia
 * E-mail: massimiliano.macchia@gmail.com
 * 
 * 
 * Copyright (C) 2010 Massimiliano Macchia.
 * 
 * This file is part of qnsolver.
 * 
 * qnsolver is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "state.h"

state::state() {
	index = 0;

	n = (unsigned char*)calloc(M,sizeof(unsigned char));
	unsigned char temp = N;
	unsigned char i = 0;
	while(temp > 0) {
		if(temp > B[i]) {
			n[i] = B[i];
			temp -= B[i];
		}
		else {
			n[i] = temp;
			temp = 0;
		}
		++i;
	}

	b = (unsigned char*)calloc(M,sizeof(unsigned char));

	blockedPredecessors = (vector<unsigned char>*)calloc(M,sizeof(vector<unsigned char>));
}
		
state::state(const state &s) {
	index = s.index;

	n = (unsigned char*)calloc(M,sizeof(unsigned char));

	b = (unsigned char*)calloc(M,sizeof(unsigned char));

	blockedPredecessors = (vector<unsigned char>*)calloc(M,sizeof(vector<unsigned char>));

	vector<unsigned char>::const_iterator it, end;
	for(unsigned short i=0; i<M; ++i) {
		n[i] = s.n[i];
		b[i] = s.b[i];
		it = s.blockedPredecessors[i].begin();
		end = s.blockedPredecessors[i].end();
		while(it != end) {
			blockedPredecessors[i].push_back( *it );
			++it;
		}
	}
}

void state::print() const {
	cout << "(";

	vector<unsigned char>::reverse_iterator it, end;
	for(unsigned short i=0; i<M; ++i) {
		cout << "("<<(unsigned short)n[i]<<","<<(unsigned short)b[i]<<",[";
		it = blockedPredecessors[i].rbegin();
		end = blockedPredecessors[i].rend();
		while(it!=end) {
			cout << (unsigned short)*it << " ";
			++it;
		}
		cout << "]) ";
	}

	cout << ")";
}
		
bool operator==(state s1, state s2) {
	/* ignores index values because not relevant */

	/* two states are equal if they have:
	 * - same job distribution over stations
	 * - same blocked stations
	 * - same blocked predecessors configuration */
	vector<unsigned char>::iterator it1, it2, end1, end2;
	for(unsigned short i=0; i<M; ++i) {
		if(s1.n[i] != s2.n[i]) return false;
		if(s1.b[i] != s2.b[i]) return false;
		if(s1.blockedPredecessors[i].size() != s2.blockedPredecessors[i].size()) return false;
		it1 = s1.blockedPredecessors[i].begin();
		it2 = s2.blockedPredecessors[i].begin();
		end1 = s1.blockedPredecessors[i].end();
		end2 = s2.blockedPredecessors[i].end();
		while(it1 != end1) {
			if(*it1 != *it2) return false;
			++it1;
			++it2;
		}
	}

	return true;
}
		
state *state::gen(unsigned char source_center, unsigned char dest_center) {

	if(source_center == dest_center) {
		return this;
	}

	unsigned char busy_servers = (n[source_center] <= m[source_center]) ? n[source_center] : m[source_center];

	if(b[source_center]==busy_servers) { //source center is blocked, nothing to do
		return this;
	}

	if(n[dest_center]==B[dest_center]) {
		if(blockType[source_center]==BAS) {
			state *temp;
			temp = new state(*this);
			++(temp->b[source_center]);
			temp->blockedPredecessors[dest_center].push_back(source_center);
			return temp;
		}
		return this; //RS-RD
	}

	state *temp;
	temp = new state(*this);
	++(temp->n[dest_center]);
	unsigned char index1, index2;
	if(blockedPredecessors[source_center].size()) {
		index1 = source_center;
		while(blockedPredecessors[index1].size()) {
			index2 = blockedPredecessors[index1].back(); //return last element (center blocked by the source)
			--(temp->b[index2]); //unblock the center preceding the source
			temp->blockedPredecessors[index1].pop_back(); //remove the last element (center blocked by the source)
			index1 = index2;
		}
		--(temp->n[index2]); //decrease the number of jobs in the center preceding the source
	}
	else {
		--(temp->n[source_center]);
	}
	return temp;
}
		
states_list::states_list() {
	tree = new searchtree(B,M);
	/* automatically generate first state and add it in the list */
	state *s;
	s = new state();
	add(s);
}
		
unsigned int states_list::add(state *s) {
	/* check if the state s is already in the list */
	int i = tree->add(s);

	if(i != -1) { //state already present
		delete(s);
		return i;
	}

	/* if state is not present */
	s->index = list.size();
	list.push_back(*s);
	return (s->index);
}
		
int states_list::size() {
	return list.size();
}

state states_list::get(int i) {
	return list[i];
}

void states_list::print() {
	for(unsigned int i=0; i<list.size(); ++i)
	{
		list[i].print();
		cout << endl;
	}
}

void states_list::print_to_file(char* file_name) {

	ofstream file;
	file.open(file_name, ios::out);

	state s;
	//vector<unsigned char>::const_iterator it, end;
	for(int i=0; i<list.size(); ++i)
	{
		s = list[i];

		for(int j=0; j<M; ++j) {
			file << "("<< (unsigned short)s.n[j] <<","<< (unsigned short)s.b[j] <<") ";
		}
		file << "\n";
	}
}

void states_list::init() {

	state source, *dest;
	int i, active_servers;
	unsigned short sc, dc;

	for(int k=0; k<list.size(); ++k) {

		source = list[k];
		for(sc=0; sc<M; ++sc) {

			for(dc=0; dc<M; ++dc) {

				if(P[sc][dc] && source.n[sc]) {

					dest = source.gen(sc,dc);

					if(dest!=NULL && !(source==*dest)) {

						i = add(dest);
						active_servers = (source.n[sc] <= m[sc]) ? source.n[sc]-source.b[sc] : m[sc]-source.b[sc];
						tl.add(new transition(k,i,active_servers*mu[sc]*P[sc][dc]));
					}
				}
			}
		}
	}
}
		
int bucket::add(state *s) {

	it = vec.begin();
	end = vec.end();
	while(it != end) {
		if(**it == *s) {
			return (*it)->index;
		}
		++it;
	}

	vec.push_back(s);
	return -1;
}
		
node::node(unsigned short *B, unsigned short index, unsigned short M) {

	height = index;
	int n_sons = B[height]+1;
	if(height == (M-1)) {
		bucks = new bucket [n_sons];
		sons=NULL;
	}
	else {
		//sons = (node**)calloc(n_sons,sizeof(node*));
		sons = new node* [n_sons];
		for(int i=0; i<n_sons; ++i) {
			sons[i] = new node(B,height+1,M);
		}
		bucks=NULL;
	}
}
		
int node::add(state *s) {

	if(sons != NULL) {

		return sons[s->n[height]]->add(s);
	}
	else {

		return bucks[s->n[height]].add(s);
	}
}
