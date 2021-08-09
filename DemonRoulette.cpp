#include "framework.h"

std::string join(const std::vector<std::string>& vec, const char* delim) {
	std::ostringstream imploded;
	std::copy(vec.begin(), vec.end(),
		std::ostream_iterator<std::string>(imploded, delim));
	return imploded.str();
}

std::string humanReadableTime(long long seconds) {
	auto seconds_ = seconds % 60;
	auto minutes = seconds / 60 % 60;
	auto hours = seconds / 3600 % 24;

	std::vector<std::string> toreturn;
	if (hours > 0) toreturn.push_back(std::to_string(hours) + " hours");
	if (minutes > 0) toreturn.push_back(std::to_string(minutes) + " minutes");
	toreturn.push_back(std::to_string(seconds_) + " seconds");

	return join(toreturn, " ");
}

int main() {
	std::map<std::string, int> map;
	Demonlist demonlist;
	while (true) {
		std::cout << "Which roulette are you doing?" << std::endl << "1 - Easy demon, 2 - Medium demon, 3 - Hard demon, 4 - Insane demon, 5 - Extreme demon" << std::endl << ">>";
		std::string resp;
		getline(std::cin, resp);
		switch (atoi(resp.c_str())) {
		case 1:
			demonlist = EASY_DEMONS; break;
		case 2:
			demonlist = MEDIUM_DEMONS; break;
		case 3:
			demonlist = HARD_DEMONS; break;
		case 4:
			demonlist = INSANE_DEMONS; break;
		case 5:
			demonlist = EXTREME_DEMONS; break;
		default:
			std::cout << "Invalid response!" << std::endl;
		}

		if (demonlist.isInitialized()) break;
	}

	Roulette roulette(&demonlist);

	std::cout << "Starting up the roulette. Press enter to get next demon or type 'quit'/'exit' to finish your roulette." << std::endl;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	while (!roulette.ended()) {
		std::string demon = roulette.nextString();
		std::cout << demon << std::endl;

		std::string a;
		getline(std::cin, a);

		if (a == "exit" || a == "quit") {
			break;
		}
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	long long timeTook = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
	std::cout << "Roulette stats:" << std::endl <<
		"Seed: " << roulette.getSeed() << std::endl <<
		"Difficulty: " << roulette.getDifficulty() << std::endl <<
		"Total score: " << roulette.getPercentage() << std::endl <<
		"Time took: " << humanReadableTime(timeTook);

	std::cout << std::endl << "Press any key to continue.." << std::endl;
	std::string a;
	std::cin >> a;
}