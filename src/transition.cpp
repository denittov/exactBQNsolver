/*
 * transition.cpp
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

#include "transition.h"

transition::transition(unsigned int x, unsigned int y, double f) {
	source = x;
	destination = y;
	frequency = f;
}

transition::transition() {
	source = 0;
	destination = 0;
	frequency = 0.0;
}

transition::transition(const transition &t) {
	source = t.source;
	destination = t.destination;
	frequency = t.frequency;
}

void transition::print() {
	cout << "["
		<< (unsigned int)source << ","
		<< (unsigned int)destination << ","
		<< frequency
		<< "]";
}
		
int transitions_list::size() {
	return list.size();
}

transition transitions_list::get(int i) {
	return *list[i];
}

void transitions_list::print() {
	for(int i=0; i<list.size(); ++i)
	{
		(*list[i]).print();
		cout << endl;
	}
}

void transitions_list::add(transition *t) {
	if(t != NULL)
	{
		list.push_back(t);
	}
}

void transitions_list::print_to_file(char* file_name, int E) {

	ofstream file;
	file.open(file_name, ios::out);

	file << E << endl; /* first line in the file describe the number of identified states */

	transition *t;
	for(int i=0; i<list.size(); ++i)
	{
		t = list[i];

		file << "["
			<< (unsigned int)t->source << ","
			<< (unsigned int)t->destination << ","
			<< setprecision(8) << t->frequency
			<< "]" << endl;
	}
}
