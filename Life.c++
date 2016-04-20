#include <vector>
#include <iostream>
#include <cassert>

#include "Life.h"

using namespace std;

// ------------
// AbstractCell
// ------------

ostream& operator<<(ostream& out, const AbstractCell& c) {
	return c.print(out);
}

istream& operator>>(istream& in, AbstractCell& c) {
	return in;
}

// ----------
// ConwayCell
// ----------

ConwayCell::ConwayCell(const char& input) {
	assert(input == '*' || input == '.');

	border = false;

	if (input == '*')
		alive = true;
	else
		alive = false;
}

ConwayCell operator+(ConwayCell old_cell, const vector<ConwayCell> neighbors) {
	std::vector<Cell> new_neighbors;
	for (ConwayCell c : neighbors) {
		Cell nc = Cell(c);
		new_neighbors.push_back(nc);
	}

	Cell evolved = old_cell.evolve(new_neighbors);
	ConwayCell new_cell = *(static_cast<ConwayCell *>(evolved.acell));

	return new_cell;
}


ConwayCell* ConwayCell::clone() const {
	return new ConwayCell(*this);
}

Cell ConwayCell::evolve(vector<Cell> neighbors) const {
	int live_neighbors = 0;

	for (Cell n : neighbors)
		if (!(n.acell->*(&ConwayCell::border)) && n.acell->*(&ConwayCell::alive))
			live_neighbors++;

	if (alive) {
		if (live_neighbors < 2 || live_neighbors > 3)
			return Cell(ConwayCell('.'));
		else
			return Cell(ConwayCell('*'));
	} else { // dead
		if (live_neighbors == 3)
			return Cell(ConwayCell('*'));
		else
			return Cell(ConwayCell('.'));
	}
}

ostream& ConwayCell::print(ostream& out) const {
	if (alive)
		return out << '*';
	else
		return out << '.';
}

// -----------
// FredkinCell
// -----------

FredkinCell::FredkinCell(const char& input) : AbstractCell(false) {
	if (input == '-') {
		age_ = 0;
		alive = false;
	} else if (input >= '0' && input <= '9') {
		age_ = input - '0';
		alive = true;
	} else {
		age_ = 10;
		alive = true;
	}
}

FredkinCell operator+(FredkinCell old_cell, const vector<FredkinCell> neighbors) {
	std::vector<Cell> new_neighbors;
	for (FredkinCell c : neighbors)
		new_neighbors.push_back(Cell(c));

	Cell evolved = old_cell.evolve(new_neighbors);
	FredkinCell new_cell = *(static_cast<FredkinCell *>(evolved.acell));

	new_neighbors.clear();

	return new_cell;
}

FredkinCell::FredkinCell(int a, bool alive_) : AbstractCell(false) {
	age_ = a;
	alive = alive_;
}

FredkinCell* FredkinCell::clone() const {
	return new FredkinCell(*this);
}

Cell FredkinCell::evolve(vector<Cell> neighbors) const {
	int live_neighbors = 0;

	for (int i = 0; i < 4; i++){
		Cell n = neighbors[i];
		if (!(n.acell->*(&FredkinCell::border)) && n.acell->*(&FredkinCell::alive))
			live_neighbors++;
	}

	if (alive) {
		if (live_neighbors == 0 || live_neighbors == 2 || live_neighbors == 4)
			return Cell(FredkinCell(age_, false));
		else {
			return Cell(FredkinCell(age_ + 1, true));
		}
	} else { // dead
		if (live_neighbors == 3 || live_neighbors == 1)
			return Cell(FredkinCell(age_, true));
		else
			return Cell(FredkinCell(age_, false));
	}

}

ostream& FredkinCell::print(ostream& out) const {
	if (!alive)
		return out << '-';
	else {
		if (age_ < 10) 
			return out << age_;
		else 
			return out << '+';
	}
}

const int FredkinCell::age() const {
	return age_;
}

// ----
// Cell
// ----

Cell::Cell(const Cell& c) {
	this->acell = c.acell->clone();
}

Cell::Cell(const char& c) {
	if (c == '.' || c == '*')
		this->acell = new ConwayCell(c);
	else
		this->acell = new FredkinCell(c);
}

Cell::~Cell() {
	delete acell;
}

Cell& Cell::operator=(const Cell& rhs) {
	acell = rhs.acell->clone();
	return *this;
}

Cell operator+(Cell old_cell, const vector<Cell> neighbors) {
	Cell new_cell = old_cell.acell->evolve(neighbors);

	// If Life is instantiated with Cell, then when a FredkinCell's age is to become 2, and only then, it becomes a live ConwayCell instead.
	FredkinCell *fc;
	if ((fc = dynamic_cast<FredkinCell *>(new_cell.acell)) != nullptr)
		if (fc->age() == 2)
			new_cell = Cell(ConwayCell('*'));

	return new_cell;
}

ostream& operator<<(ostream& out, const Cell c) {
	return c.acell->print(out);
}

istream& operator>>(istream& in, Cell& c) {
	char input = in.get();

	c = Cell(input);

	return in;
}

bool Cell::is_alive() const {
	return acell->is_alive();
}
