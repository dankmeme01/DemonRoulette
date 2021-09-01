#ifndef CURL_STATICLIB
#define CURL_STATICLIB
#endif
#include "framework.h"

const std::string WHITESPACE = " \n\r\t\f\v";
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

std::string saveRoulette(Roulette*);
Roulette getRoulette(std::variant<std::string, Demonlist>);
template <typename Out>
void split(const std::string&, char, Out);
std::vector<std::string> split(const std::string&, char);
std::string trim(const std::string&);

int main() {
	CURL* curl;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	try {
		downloadDemons(curl);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	Demonlist demonlist;
	Roulette roulette;
	bool usingSavecode = false;
	while (true) {
		std::cout << "Which roulette are you doing?" << std::endl << "1 - Easy demon, 2 - Medium demon, 3 - Hard demon, 4 - Insane demon, 5 - Extreme demon" << std::endl <<
			"If you have played before, enter your savecode instead, to continue the roulette." << std::endl <<">>";
		std::string resp;
		getline(std::cin, resp);
		resp = trim(resp);
		if(resp == "1")
			demonlist = EASY_DEMONS;
		else if (resp == "2")
			demonlist = MEDIUM_DEMONS;
		else if (resp == "3")
			demonlist = HARD_DEMONS;
		else if (resp == "4")
			demonlist = INSANE_DEMONS;
		else if (resp == "5")
			demonlist = EXTREME_DEMONS;
		else {
			try {
				roulette = getRoulette(resp);
				usingSavecode = true;
				std::cout << "Loading savecode.." << std::endl;
			}
			catch (std::invalid_argument& e) { std::cout << e.what() << std::endl << "Invalid response!" << std::endl; }
		}

		if (demonlist.isInitialized() || usingSavecode) break;
	}
	
	if(!usingSavecode) roulette = getRoulette(demonlist);

	std::cout << "Starting up the roulette. After each demon enter your progress or type 'quit'/'exit' to finish your roulette." << std::endl;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	int nextL = 1;
	do {
		std::string demon;
		try { demon = roulette.nextString(nextL); }
		catch (std::invalid_argument& e) { break; }

		std::cout << demon << std::endl << "Enter percentage :>";
		std::string a;
		getline(std::cin, a);
		if (a == "exit" || a == "quit") {
			break;
		}
		else {
			auto thing = std::atoi(a.c_str());
			if (thing > 100 || (size_t)thing < roulette.getPercentage()) {
				std::cout << "Invalid percentage. Should be " + std::to_string(roulette.getPercentage()) + "-100" << std::endl;
				nextL = 0;
			}
			else { nextL = thing - (int)roulette.getPercentage() + 1; }
		} 
	} while (!roulette.ended());

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	long long timeTook = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
	std::cout << "Game over!" << std::endl;
	std::cout << "Roulette stats:" << std::endl <<
		(
			roulette.ended() ?
				("Seed: " + std::to_string(roulette.getSeed())) :
				("Savecode: " + (saveRoulette(&roulette)))
		)
		<< std::endl << "Difficulty: " << getDemonString(roulette.getDifficulty()) << std::endl <<
		"Total score: " << roulette.getPercentage() - 1 << std::endl <<
		"Time took: " << humanReadableTime(timeTook);

	std::cout << std::endl << "Press enter to exit..";
	std::string a;
	getline(std::cin, a);
}

Roulette getRoulette(std::variant<std::string, Demonlist> saveOrList) {
	try {
		std::string savecode_ = std::get<std::string>(saveOrList);
		std::string savecode = base64_decode(savecode_);

		std::vector<std::string> tokens = split(savecode, '|');

		int diff_ = atoi(tokens.at(0).c_str());
		int seed = atoi(tokens.at(1).c_str());
		int percentage = atoi(tokens.at(2).c_str());

		Demonlist list;
		switch(diff_) {
		case 1: list = EASY_DEMONS;  break;
		case 2: list = MEDIUM_DEMONS; break;
		case 3: list = HARD_DEMONS; break;
		case 4: list = INSANE_DEMONS; break;
		case 5: list = EXTREME_DEMONS; break;
		default:
			throw std::invalid_argument("Invalid value 1: " + diff_); 
			break;
		}

		if (percentage < 0 || percentage >= 100) throw std::invalid_argument("Invalid value 3: " + percentage);

		Roulette r(&list, seed, percentage);
		return r;
	}
	catch (const std::bad_variant_access&) {
		Demonlist list = std::get<Demonlist>(saveOrList);
		return Roulette(&list);
	}
}

std::string saveRoulette(Roulette *roulette) {
	DemonType diff_ = roulette->getDifficulty();
	int diff;
	switch (diff_) {
	case DemonType::EASY_DEMON: diff = 1; break;
	case DemonType::MEDIUM_DEMON: diff = 2; break;
	case DemonType::HARD_DEMON: diff = 3; break;
	case DemonType::INSANE_DEMON: diff = 4; break;
	case DemonType::EXTREME_DEMON: diff = 5; break;
	default: diff = 0; break;
	}
	int percentage = (int) roulette->getPercentage() - 1;
	int seed = (int) roulette->getSeed();

	std::string savecode = std::to_string(diff) + "|" + std::to_string(seed) + "|" + std::to_string(percentage);
	return base64_encode(savecode);
}

template <typename Out>
void split(const std::string& s, char delim, Out result) {
	std::istringstream iss(s);
	std::string item;
	while (std::getline(iss, item, delim)) {
		*result++ = item;
	}
}
std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

std::string ltrim(const std::string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}
std::string rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
std::string trim(const std::string& s) {
	return rtrim(ltrim(s));
}