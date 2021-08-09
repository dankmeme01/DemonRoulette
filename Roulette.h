#pragma once
#include "Demon.h"
#include <stdexcept>
#include <set>

class Roulette {
	std::vector<Demon> demons;
	size_t percentage = 0;
	unsigned int seed;
	DemonType difficulty = DemonType::UNSPECIFIED;
	bool initialized;

public:
	Roulette(std::vector<Demon> rouletteDemons, unsigned int seed_, DemonType diff) : demons(rouletteDemons), seed(seed_), difficulty(diff), initialized(true) {}

	Roulette(Demonlist* demonList, unsigned int seed_ = 0, size_t percentage_ = 0) {
		if (seed_ != 0) demonList->setSeed(seed_);
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

		percentage = percentage_;
		difficulty = demonList->getDifficulty();

		initialized = true;
	}

	Roulette() {}

	unsigned int getSeed() { return seed; }

	DemonType getDifficulty() { return difficulty; }

	size_t getPercentage() { return percentage; }
	
	Demon next() {
		if (!initialized) throw std::invalid_argument("Roulette class not initialized.");
		size_t pos = ++percentage;
		if (pos < 1 || pos > demons.size()) {
			throw std::invalid_argument("Percentage out of range 1-" + demons.size());
		}
		return at(pos);
	}

	std::string nextString() {
		Demon demon = next();
		return demon.string((int)percentage);
	}

	bool ended() {
		if (!initialized) throw std::invalid_argument("Roulette class not initialized.");
		return percentage >= demons.size();
	}

private:
	Demon at(size_t position) {
		if (!initialized) throw std::invalid_argument("Roulette class not initialized.");
		Demon x = demons.at(position-1);
		return x;
	}
};

