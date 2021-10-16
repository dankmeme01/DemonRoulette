#pragma once
#include "Demonlist.h"
#include "Roulette.h"

#include <json.hpp>
#include <httplib.h>
#include <Windows.h>
#include <curl/curl.h>

#include <map>
#include <filesystem>
#include <string>
#include <fstream>
#include <streambuf>
#include <ctime>
#include <direct.h>

class HitLevelEnd : public std::runtime_error {
public: HitLevelEnd() : runtime_error("gdbrowser returned -1") {};
};

class GotRateLimited : public std::runtime_error {
public: GotRateLimited() : runtime_error("gdbrowser returned -1") {};
};

namespace fs = std::filesystem;
using json = nlohmann::json;
using ms = boost::chrono::milliseconds;
using timepoint = boost::chrono::system_clock::time_point;
using duration = boost::chrono::system_clock::duration;
typedef std::vector<Demon> Demons;

fs::path getDemonSavePath() {
    fs::path spath = fs::temp_directory_path().parent_path().parent_path().parent_path() / "Roaming" / "dankmeme01" / "DemonRoulette";
    if (!fs::exists(spath)) fs::create_directories(spath);
    return spath / "demons.json";
}

void sleep(int ms_) {
    boost::this_thread::sleep(ms(ms_));
}

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

size_t write_string_data(char* ptr, size_t size, size_t nmemb, void* udata) {
    std::ostringstream* stream = (std::ostringstream*)udata;
    size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}

Demonlist EASY_DEMONS;
Demonlist MEDIUM_DEMONS;
Demonlist HARD_DEMONS;
Demonlist INSANE_DEMONS;
Demonlist EXTREME_DEMONS;

void splitDemons(Demons* demons) {
    Demons easy, medium, hard, insane, extreme;
    for (Demon &demon : *demons) {
        if (demon.type == DemonType::EASY_DEMON) easy.push_back(demon);
        else if (demon.type == DemonType::MEDIUM_DEMON) medium.push_back(demon);
        else if (demon.type == DemonType::HARD_DEMON) hard.push_back(demon);
        else if (demon.type == DemonType::INSANE_DEMON) insane.push_back(demon);
        else if (demon.type == DemonType::EXTREME_DEMON) extreme.push_back(demon);
    }

    EASY_DEMONS = Demonlist(easy, DemonType::EASY_DEMON);
    MEDIUM_DEMONS = Demonlist(medium, DemonType::MEDIUM_DEMON);
    HARD_DEMONS = Demonlist(hard, DemonType::HARD_DEMON);
    INSANE_DEMONS = Demonlist(insane, DemonType::INSANE_DEMON);
    EXTREME_DEMONS = Demonlist(extreme, DemonType::EXTREME_DEMON);
}

void saveDemons(json demons) {
    timepoint now = boost::chrono::system_clock::now();
    duration dtn = now.time_since_epoch();
    unsigned int nowuint = static_cast<unsigned int>(boost::chrono::duration_cast<boost::chrono::milliseconds>(dtn).count());
    json objtosave = {
        {"date", nowuint},
        {"demons", demons}
    };
    auto savepoint = getDemonSavePath();
    std::cout << "Saving demons to " << savepoint.string() << std::endl;
    if (fs::exists(savepoint)) fs::remove(savepoint);
    std::ofstream file(savepoint);
    file << objtosave.dump(2);
    file.close();
}

Demons loadFromJSONv2(json jsonString, json* tosave) {
    // Loads a list of demons
    Demons kicks;
    for (json::iterator it = jsonString.begin(); it != jsonString.end(); ++it) {
        json demonfull = *it;
        json demon;
        for (std::string prop : std::vector({ "name", "id", "author", "difficulty" })) { demon[prop] = demonfull.at(prop); }

        if(tosave != nullptr) tosave->push_back(demon);
        auto diff = demon.at("difficulty");
        Demon demon_ = Demon(demon.at("name"), std::stoul(demon.at("id").get<std::string>()), demon.at("author"), toDType(diff.get<std::string>()));
        kicks.push_back(demon_);
    }
    return kicks;
}

Demons loadFromLocalJSONv2(json file) {
    return loadFromJSONv2(file.at("demons"), nullptr);
}

void getPage(CURL* curl, Demons* out, json* tosave, int page) {
    std::ostringstream ss;
    CURLcode res;
    std::string url = "https://gdbrowser.com/api/search/*?count=10&diff=-2&page=" + std::to_string(page);
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ss);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_string_data);
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            std::string output = ss.str();
            if (output == "-1") throw HitLevelEnd();
            if (boost::starts_with(output, "Rate limit")) throw GotRateLimited();
            try {
                json parsed = json::parse(output);
                *out = boost::push_back(*out, loadFromJSONv2(parsed, tosave));
            }
            catch (json::exception& e) {
                std::cout << e.what() << std::endl;
                std::cout << "Exception occured while scaning this: " << std::endl;
                std::cout << output << std::endl;
                return;
            }
        }
        else throw std::runtime_error(curl_easy_strerror(res));
    }
    else throw std::runtime_error("cURL was not initialized. Please check your firewall settings or internet connection. Using demons as of 2021.08.13");
}

Demons getAllDemons(CURL* curl) {
    std::cout << "Downloading demons from gdbrowser.." << std::endl;
    Demons demons;
    json jsonstruct;
    for (unsigned int page = 0;; page++) {
        try {
            getPage(curl, &demons, &jsonstruct, page);
            sleep(200);
            std::cout << "Got page " << page << std::endl;
        }
        catch (HitLevelEnd&) { break; }
        catch (GotRateLimited&) { std::cout << "Rate limited... waiting 5 seconds.." << std::endl; page--; sleep(7500); }
    }
    std::cout << "Total demons: " << std::to_string(demons.size()) << std::endl;
    saveDemons(jsonstruct);
    return demons;
}

void fetchDemons(CURL* curl) {
    Demons demons;
    auto savepoint = getDemonSavePath();
    if (fs::exists(savepoint)) {
        std::ifstream i(savepoint);
        json j;
        i >> j;
        i.close();
        unsigned int date = j.at("date");

        timepoint now = boost::chrono::system_clock::now();
        duration dtn = now.time_since_epoch();

        unsigned int nowuint = static_cast<unsigned int>(boost::chrono::duration_cast<boost::chrono::milliseconds>(dtn).count());
        unsigned int difference = nowuint - date;
        unsigned int day = 1000 * 60 * 60 * 24;
        if (difference > (day * 30)) {
            demons = getAllDemons(curl);
        }
        else {
            unsigned int days = static_cast<unsigned int>(floor(difference / day));
            std::cout << "Using pre-loaded demons from " << std::to_string(days) << " days ago." << std::endl;
            demons = loadFromLocalJSONv2(j);
            std::cout << "Total demons: " << std::to_string(demons.size()) << std::endl;
        }
    }
    else demons = getAllDemons(curl);
    splitDemons(&demons);
}