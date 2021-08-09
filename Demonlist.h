#pragma once
#include "Demon.h"
class Demonlist {
	bool initialized = false;
	rng_type rng;
	rng_type::result_type seed;
	std::vector<Demon> demons = {};
	std::uniform_int_distribution<rng_type::result_type> udist;
	std::string avg_diff;

public:
	Demonlist(std::vector<Demon> listDemons, std::string difficulty) {
		for (Demon& demon : listDemons) {
			demons.push_back(demon);
		}
		avg_diff = difficulty;
		Initialize();
	}
	Demonlist() {}
	
	bool isInitialized() { return initialized; }

	std::string getDifficulty() { return avg_diff; }

	rng_type::result_type getSeed() { return seed; }

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
		this->seed = get_seed();
		rng.seed(seed);
		udist = std::uniform_int_distribution<rng_type::result_type>(0,demons.size()-1);
		initialized = true;
	}

	rng_type::result_type get_seed() {
		return (rng_type::result_type)1234567;
		return (rng_type::result_type) time(NULL); //ok
	}
};

