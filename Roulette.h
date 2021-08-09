#pragma once
#include "Demon.h"
#include <stdexcept>
#include <set>

class Roulette {
	std::vector<Demon> demons;
	size_t percentage = 0;
	unsigned int seed;
	std::string difficulty;

public:
	Roulette(std::vector<Demon> rouletteDemons, unsigned int seed_, std::string avg_diff) : demons(rouletteDemons), seed(seed_), difficulty(avg_diff) {}

	Roulette(Demonlist* demonList) {
		size_t LIST_SIZE = demonList->getDemons().size();
		size_t MAX_SIZE = LIST_SIZE > 100 ? 100 : LIST_SIZE;
		std::vector<Demon> demons_;
		while (demons_.size() < MAX_SIZE) {
			Demon* demon = demonList->getRandomDemon();
			if (std::find_if(demons_.begin(), demons_.end(), [&](const Demon& d) {return d.id == demon->id; }) != demons_.end()) { continue; }
			demons_.push_back(*demon);
		}

		demons = demons_;
		seed = demonList->getSeed();
		difficulty = demonList->getDifficulty();
	}

	unsigned int getSeed() { return seed; }

	std::string getDifficulty() { return difficulty; }

	size_t getPercentage() { return percentage; }
	
	Demon next() {
		size_t pos = ++percentage;
		if (pos < 1 || pos > demons.size()) {
			throw std::invalid_argument("Percentage out of range 1-" + demons.size());
		}
		return at(pos);
	}

	std::string nextString() {
		Demon demon = next();
		return demon.string(percentage);
	}

	bool ended() {
		return percentage >= demons.size();
	}

private:
	Demon at(int position) {
		Demon x = demons.at(position-1);
		return x;
	}
};

