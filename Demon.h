#pragma once
#ifndef DEMON_H
#define DEMON_H
enum class DemonType {
	EASY_DEMON = 1,
	MEDIUM_DEMON = 2,
	HARD_DEMON = 3,
	INSANE_DEMON = 4,
	EXTREME_DEMON = 5
};

class Demon {
	bool initialized = false;

public:
	unsigned int id;
	std::string name;
	std::string uploader;
	DemonType type;

	std::string string(int percentage = -1) {
		switch (percentage) {
		case -1:
			return getName();
		default:
			return getName() + " - " + std::to_string(percentage) + "%";
		}
	};

	Demon(std::string levelName, unsigned int levelID, std::string levelUploader, DemonType demonType) :
		name(levelName),
		id(levelID),
		uploader(levelUploader),
		type(demonType),
		initialized(true) {}

	Demon(std::vector<std::variant<std::string, DemonType, unsigned int>> options) :
		name(std::get<std::string>(options.at(0))),
		id(std::get<unsigned int>(options.at(1))),
		uploader(std::get<std::string>(options.at(2))),
		type(std::get<DemonType>(options.at(3))),
		initialized(true) {}

	Demon() {}

	friend bool operator ==(const Demon& d1, const Demon& d2);

private:
	std::string getName() {
		return name + " by " + uploader + " (" + std::to_string(id) + ")";
	}
};

bool operator == (const Demon& d1, const Demon& d2) {
	return d1.id == d2.id;
}

#endif