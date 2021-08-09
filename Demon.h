#pragma once
#ifndef DEMON_H
#define DEMON_H
enum class DemonType {
	EASY_DEMON,
	MEDIUM_DEMON,
	HARD_DEMON,
	INSANE_DEMON,
	EXTREME_DEMON,
	UNSPECIFIED
};

std::string getDemonString(DemonType);

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

private:
	std::string getName() {
		return name + " by " + uploader + " (" + std::to_string(id) + ")";
	}
};

std::string getDemonString(DemonType d) {
	switch (d) {
	case DemonType::EASY_DEMON: return "Easy demon";
	case DemonType::MEDIUM_DEMON: return "Medium demon";
	case DemonType::HARD_DEMON: return "Hard demon";
	case DemonType::INSANE_DEMON: return "Insane demon";
	case DemonType::EXTREME_DEMON: return "Extreme demon";
	case DemonType::UNSPECIFIED: return "Unspecified";
	default: return "";
	};
}
#endif