/*
 * state.h
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

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
#include <vector>

#include "constants.h"
#include "transition.h"

using namespace std;

extern unsigned short *B;
extern unsigned short M;
extern unsigned char N;
extern unsigned short *blockType;
extern unsigned short *m;
extern double *mu;
extern double **P;

class state {
	public:
		/* store the state index (a unique identifier of the state) */
		int index;

		/* store the number of job in each station for the state */
		unsigned char *n;

		/* store the active/blocked condition for each BAS center (RS-RD station never blocks servers) */
		unsigned char *b;

		/* store the upward blocked stations for each station in the net */
		vector<unsigned char> *blockedPredecessors;

		/* default constructor */
		state();

		state(const state &s);

		void print() const;
        
        
		friend bool operator==(state s1, state s2);

		state *gen(unsigned char source_center, unsigned char dest_center);
};

class bucket {

    protected:
		vector<state*> vec;
		vector<state*>::const_iterator it, end;

    public:
		bucket() {

		}

		int add(state *s);
};

class node {

	protected:
		node **sons;
		bucket *bucks;
		unsigned char height;

    public:
		node() {}

		node(unsigned short *B, unsigned short index, unsigned short M);

		int add(state *s);
};

class searchtree : public node {

    public: 
		searchtree(unsigned short *B, unsigned short M): node(B,0,M) {

		}
};

class states_list
{
	protected:
		vector<state> list; /* store all identified states */
		vector<state>::const_iterator it, end;
		/* searchtree allows to efficiently check if a state is already present */
		searchtree *tree;

		unsigned int add(state *s);

	public:
		transitions_list tl;

		states_list();

		int size();

		state get(int i);

		void print();

        void print_to_file(char* file_name);

		void init();
};
