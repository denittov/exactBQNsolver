/*
 * transition.h
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

using namespace std;

class transition
{
	public:
		/* indexes of the two states involved in the transition */
		unsigned int source;
		unsigned int destination;
		/* q(i,j) value in the transition matrix Q */
		double frequency;

		transition(unsigned int x, unsigned int y, double f);

		transition();

		transition(const transition &t);

		void print();
};

class transitions_list
{
	protected:
		vector<transition*> list;

	public:
		transitions_list() {

		}

		int size();

		transition get(int i);

		void print();

		void add(transition *t);

		void print_to_file(char* file_name, int E);
};
