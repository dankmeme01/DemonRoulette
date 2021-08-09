#pragma once
#include "Demon.h"
class Demonlist {
	bool initialized = false;
	rng_type rng;
	rng_type::result_type seed = 0;
	std::vector<Demon> demons = {};
	std::uniform_int_distribution<rng_type::result_type> udist;
	DemonType difficulty = DemonType::UNSPECIFIED;

public:
	Demonlist(std::vector<Demon> listDemons, DemonType difficulty_) {
		for (Demon& demon : listDemons) {
			demons.push_back(demon);
		}
		difficulty = difficulty_;
		Initialize();
	}
	Demonlist() {}
	
	bool isInitialized() { return initialized; }

	DemonType getDifficulty() { return difficulty; }

	rng_type::result_type getSeed() { return seed; }

	void setSeed(rng_type::result_type seed_) { seed = seed_; Initialize(); }

	std::vector<Demon> getDemons() {
		if (!initialized) throw std::invalid_argument("Demonlist object not initialized");
		return demons;
	}

	Demon* getRandomDemon() {
		if (!initialized) throw std::invalid_argument("Demonlist object not initialized");
		rng_type::result_type rand = udist(rng);
		Demon* demon = &demons.at(rand);
		return demon;
	}

private:
	void Initialize() {
		if (seed <= 0) {
			seed = get_seed();
		}
		rng.seed(seed);
		udist = std::uniform_int_distribution<rng_type::result_type>(0, (unsigned int) demons.size() - 1);
		initialized = true;
	}

	rng_type::result_type get_seed() {
		return (rng_type::result_type) time(NULL); //ok
	}
};

