/*-- encoding:GBK --*/
#include "boardRecord.h"

// hard core code
string addon		   = "";
string inFileSettings = addon +
	"{\n" +
	"	\"defaultSettings\": {\n" +
	"		\"changeBoard\": {\n" +
	"			\"askToSaveBoard\": false,\n" +
	"			\"defaultSaveBoard\": false\n" +
	"		},\n" +
	"		\"gameIsOver\": {\n" +
	"			\"askToReverse\": true,\n" +
	"			\"askToSaveBoard\": true,\n" +
	"			\"defaultReverse\": false,\n" +
	"			\"defaultSaveBoard\": false\n" +
	"		},\n" +
	"		\"inCustomMode\": {\n" +
	"			\"askToSaveBoard\": false,\n" +
	"			\"defaultSaveBoard\": false\n" +
	"		},\n" +
	"		\"inDebugMode\": {\n" +
	"			\"hintOn\": false,\n" +
	"			\"showCalculate\": false,\n" +
	"			\"showTime\": false,\n" +
	"			\"starrySky\": false,\n" +
	"			\"starsOn\": false,\n" +
	"			\"trackRoutes\": false\n" +
	"		},\n" +
	"		\"whenSaveGame\": {\n" +
	"			\"askGiveName\": true,\n" +
	"			\"defaultGiveName\": false\n" +
	"		}\n" +
	"	},\n" +
	"	\"otherSettings\": {\n" +
	"		\"maxcaltime\":81\n" +
	"	}\n" +
	"}\n";

BoardRecord::~BoardRecord() {
	writeSettings();
	if (!games.empty())
		writeGames();
}

std::ostream& operator<<(std::ostream& os, oneMove& move) {
	os << "    mode = " << move.mode << ";\n";
	if (move.mode == "normal" || move.mode == "debug" || move.mode == "test") {
		os << "    time used: " << move.time
		   << "ms; hint on: " << std::boolalpha << move.hintOn
		   << "; suggestion = " << move.suggestion
		   << "\n    word = " << move.word << "; list = [ ";
		for (short i : move.list)
			os << i << " ";
		os << "]\n    move = " << move.move;
		if (move.byComputer)
			os << " by computer '" << move.player << "':\n";
		else
			os << " by player '" << move.player << "':\n";
	}
	else if (move.mode == "add")
		os << "    add '" << move.player << "' in column " << move.move << endl;
	else if (move.mode == "reverse")
		os << "    remove column " << move.move << endl;
	return os;
}

void BoardRecord::getFile() {
	std::ifstream inGames(gamesFileName);
	if (inGames.is_open())
		inGames >> games;
	std::ifstream inSet(settingsFileName);
	if (!inSet.is_open()) {
		std::ofstream outSet(settingsFileName);
		outSet << inFileSettings;
		if (!outSet.is_open())
			throw runtime_error(
				"getFile: failed to create file, mission aborted");
		outSet.close();
		std::ifstream in(settingsFileName);
		in >> settings;
	}
	else
		inSet >> settings;
}

void BoardRecord::writeGames() {
	std::ofstream outFile(gamesFileName);
	if (!outFile.is_open()) {
		cout << "Failed to open file \"" << gamesFileName << "\" to write\n";
		cout << "write action aborted\n ";
		throw runtime_error("writeGames: failed to open file to write");
	}
	outFile << games;
}

void BoardRecord::writeSettings() {
	std::ofstream outFile(settingsFileName);
	if (!outFile.is_open()) {
		cout << "Failed to open file \"" << settingsFileName << "\" to write\n";
		cout << "write action aborted\n ";
		throw runtime_error("writeSettings: can't open file to write settings");
	}
	outFile << settings;
}

void BoardRecord::saveGame(BoardState& state) {
	string gameName = "no one";
	if (getDefaultSettings("whenSaveGame", "askGiveName")) {
		bool defaultGiveName =
			getDefaultSettings("whenSaveGame", "defaultGiveName");
#ifndef STARS_LANG_CHINESE
		if (defaultGiveName)
			printf(
				"Care to give the board a name? (yes as default) (no/Yes)> ");
		else
			printf("Care to give the board a name? (no as default) (No/yes)> ");
#else
		if (defaultGiveName)
			printf("���浵����? (Ĭ�� yes) (no/Yes)> ");
		else
			printf("���浵����? (Ĭ�� no) (No/yes)> ");
#endif // STARS_LANG_CHINESE

		char name[256];
		cin.getline(name, 256);
		if ((defaultGiveName && !strlen(name)) || !strcmp(name, "yes")) {
			cin.getline(name, 256);
			if (strlen(name))
				gameName = name;
		}
	}
	saveGame(gameName, state);
#ifndef STARS_LANG_CHINESE
	cout << "game \"" << gameName << "\" is saved.\n";
#endif // STARS_LANG_CHINESE
	cout << "��Ϸ \"" << gameName << "\" �Ѵ浵\n";
}

void BoardRecord::saveGame(const string& gameName, BoardState& state) {
	Json::Value oneGame;
	oneGame["name"]  = gameName;
	time_t now       = time(NULL);
	char*  date      = ctime(&now);
	oneGame["date"]  = date;
	oneGame["state"] = state;
	for (oneMove move : historyMove)
		oneGame["historyMove"].append(move);
	games.append(oneGame);
	writeGames();
}

bool BoardRecord::getDefaultSettings(const string& situ, const string& item) {
	int          i               = 0;
	Json::Value& defaultSettings = settings["defaultSettings"];
	while (i < 3) {
		if (defaultSettings.isMember(situ)) {
			if (defaultSettings[situ].isMember(item))
				return defaultSettings[situ][item].asBool();
			else if (i < 2) {
				std::ofstream outSet(settingsFileName);
				outSet << inFileSettings;
				outSet.close();
				std::ifstream in(settingsFileName);
				in >> settings;
				++i;
				continue;
			}
			else {
				cout << "situ = " << situ << " item = " << item << endl;
				throw runtime_error("no such item in Stars_settings.json");
			}
		}
		else if (i < 2) {
			std::ofstream outSet(settingsFileName);
			outSet << inFileSettings;
			outSet.close();
			std::ifstream in(settingsFileName);
			in >> settings;
			defaultSettings = settings["defaultSettings"];
			++i;
			continue;
		}
		else {
			cout << "situ = " << situ << endl;
			throw runtime_error("no such situation in Stars_settings.json");
		}
	}
#ifdef STARS_DEBUG_INFO
	throw logic_error(
		"control flow to the end of BoardRecord::getDefaultSettings");
#endif
	return false;
}

Json::Value& BoardRecord::getOtherSettings(const string& name) {
	bool         ret           = true;
	Json::Value& otherSettings = settings["otherSettings"];
	while (true) {
		if (otherSettings.isMember(name))
			return otherSettings[name];
		// if not, try writting first
		if (ret) {
			std::ofstream outSet(settingsFileName);
			outSet << inFileSettings;
			outSet.close();
			std::ifstream in(settingsFileName);
			in >> settings;
			otherSettings = settings["otherSettings"];
			ret           = false;
			continue;
		}
		// if tried and failed
		throw runtime_error("no such other settings");
	}
}

void BoardRecord::showSettingsWithTags() {
	Json::Value& defaultSettings = settings["defaultSettings"];
	members      member          = defaultSettings.getMemberNames();
#ifndef STARS_LANG_CHINESE
	cout << "situation\t"
		 << "item\t\t\t"
		 << "true/false\t"
		 << "tagNumber\n";
	char x = 'a';
	for (members::iterator i = member.begin(); i != member.end(); ++i) {
		char    y     = 'a';
		members inset = defaultSettings[*i].getMemberNames();
		printf("\n");
		for (members::iterator j = inset.begin(); j != inset.end(); ++j) {
			if ((*j).size() > 15)
				cout << *i << "\t" << *j << "\t" << defaultSettings[*i][*j]
					 << "\t\t" << x << y++ << endl;
			else if ((*j).size() < 8)
				cout << *i << "\t" << *j << "\t\t\t" << defaultSettings[*i][*j]
					 << "\t\t" << x << y++ << endl;
			else
				cout << *i << "\t" << *j << "\t\t" << defaultSettings[*i][*j]
					 << "\t\t" << x << y++ << endl;
		}
		++x;
	}
#else
	cout << "��� -- "
		 << "��Ŀ -- "
		 << "��� -- "
		 << "��ǩ\n";
	char x = 'a';
	for (members::iterator i = member.begin(); i != member.end(); ++i) {
		char y = 'a';
		members inset = defaultSettings[*i].getMemberNames();
		printf("\n");
		for (members::iterator j = inset.begin(); j != inset.end(); ++j) {
			cout << toChinese(*i) << " -- " << toChinese(*j) << " -- "
				 << toChinese(defaultSettings[*i][*j].asBool()) << " -- " << x
				 << y++ << endl;
		}
		++x;
	}
#endif // STARS_LANG_CHINESE
}

bool BoardRecord::changeSettingsUsingTags(int tag1, int tag2) {
	int          x               = 0;
	Json::Value& defaultSettings = settings["defaultSettings"];
	members      member          = defaultSettings.getMemberNames();
	for (members::iterator i = member.begin(); i != member.end(); ++i) {
		int     y     = 0;
		members inset = defaultSettings[*i].getMemberNames();
		for (members::iterator j = inset.begin(); j != inset.end(); ++j) {
			if (x == tag1 && y == tag2) {
				defaultSettings[*i][*j] = !defaultSettings[*i][*j].asBool();
#ifndef STARS_LANG_CHINESE
				cout << *i << ": " << *j << " is changed from "
					 << std::boolalpha << !defaultSettings[*i][*j].asBool()
					 << " to " << defaultSettings[*i][*j] << endl;
#else
				cout << toChinese(*i) << ": " << toChinese(*j) << " �Ѵ� "
					 << std::boolalpha
					 << toChinese(!defaultSettings[*i][*j].asBool())
					 << " ���ĵ� "
					 << toChinese(defaultSettings[*i][*j].asBool()) << endl;
#endif // STARS_LANG_CHINESE
				return true;
			}
			++y;
		}
		++x;
	}
	return false;
}

string BoardRecord::showSavedGames(Json::Value& ret) {
	unsigned int i = 0;
	while (i < games.size()) {
#ifndef STARS_LANG_CHINESE
		printf(
			"\ndate: %sname: %s\nboard:\n", games[i]["date"].asCString(),
			games[i]["name"].asCString());
		showSavedBoard(games[i]["state"]);
		printf("index number: %d/%d\n> ", i + 1, games.size());
#else
		printf(
			"\n����: %s��: %s\n����:\n", games[i]["date"].asCString(),
			games[i]["name"].asCString());
		showSavedBoard(games[i]["state"]);
		printf("������: %d/%d\n> ", i + 1, games.size());
#endif // STARS_LANG_CHINESE

		while (true) {
			char input[8];
			cin.getline(input, 8);
			if (input[0] == '\0')
				break;
			if (!strcmp(input, "e") || !strcmp(input, "exit"))
				return "exit";
			if (!strcmp(input, "q") || !strcmp(input, "quit"))
				return "quit";
			if (!strcmp(input, "c") || !strcmp(input, "current")) {
				ret = games[i];
				return "yes";
			}
			else if (
				!strcmp(input, "d") || !strcmp(input, "rm") ||
				!strcmp(input, "delete") || !strcmp(input, "rm")) {
				Json::Value removed;
				games.removeIndex(i--, &removed);
				writeGames();
				break;
			}
			int number;
			try {
				number = ToInt::myStoi(input);
			}
			catch (const std::invalid_argument&) {
				number = 0;
			}
			if (number && number <= getNumberOfSavedBoard()) {
				ret = games[number - 1];
				return "yes";
			}
#ifndef STARS_LANG_CHINESE
			cout << "Pardon?\n> ";
#else
			cout << "ɶ��\n> ";
#endif // STARS_LANG_CHINESE
		}
		++i;
	}
	return "exit";
}

void BoardRecord::showSavedBoard(const Json::Value& state) {
	BoardState bstate(state);
	bstate.show();
}

void BoardRecord::refreshHistoryMove(const Json::Value& hm) {
	for (oneMove om : hm) {
		historyMove.push_back(om);
	}
}

BoardRecord& BoardRecord::operator=(const BoardRecord& br) {
	if (this == &br)
		return *this;
	settings         = br.settings;
	gamesFileName    = br.gamesFileName;
	settingsFileName = br.settingsFileName;
	historyMove      = br.historyMove;
	games            = br.games;
	return *this;
}

// check if settings match
bool BoardRecord::match() {
	string dsString = "defaultSettings", osString = "otherSettings",
		   mctString = "maxcaltime";
#ifndef STARS_DEBUG_INFO
	if (!settings.isMember(dsString) || !settings.isMember(osString))
		return false;
	Json::Value &ds = settings["defaultSettings"],
				os  = settings["otherSettings"];
	if (!os.isMember(mctString) || os[mctString].asInt() < 10)
		return false;
#endif // !STARS_DEBUG_INFO
#ifdef STARS_DEBUG_INFO
	if (!settings.isMember(dsString)) {
		cout << "no such member as " << dsString << endl;
		return false;
	}
	if (!settings.isMember(osString)) {
		cout << "no such member as " << osString << endl;
		return false;
	}
	Json::Value &ds = settings["defaultSettings"],
				os  = settings["otherSettings"];
	if (!os.isMember(mctString)) {
		cout << "no such member as " << mctString << endl;
		return false;
	}
	if (os[mctString].asInt() < 10) {
		cout << mctString << " went nuts" << endl;
		return false;
	}
#endif // STARS_DEBUG_INFO

	// get the "right default settings"
	std::ifstream in(settingsFileName);
	if (!in.is_open())
		throw runtime_error("can't open settings file, match check aborted");
	Json::Value  rightDs;
	Json::Reader reader;
	if (!reader.parse(inFileSettings, os))
		throw logic_error(
			"can't parse in file settings file, match check aborted");
	rightDs = os["defaultSettings"];

	// check if default settings match
	members member = ds.getMemberNames();
	for (members::iterator i = member.begin(); i != member.end(); ++i) {
		if (!rightDs.isMember(*i))
			return false;
		members inset = ds[*i].getMemberNames();
		for (members::iterator j = inset.begin(); j != inset.end(); ++j) {
			if (!rightDs[*i].isMember(*j)) {
#ifdef STARS_DEBUG_INFO
				cout << *i << " has no member " << *j << endl;
#endif // STARS_DEBUG_INFO
				return false;
			}
		}
	}
	return true;
}

#ifdef STARS_LANG_CHINESE
string toChinese(const string& word) {
	if (word == "good")
		return "��";
	if (word == "bad")
		return "��";
	if (word == "free")
		return "����"; // ������ˣ�
	if (word == "debug")
		return "�˻�";
	if (word == "play")
		return "˫��";
	if (word == "board width")
		return "���̿��";
	if (word == "board height")
		return "���̸߶�";
	if (word == "win number")
		return "��ʤ����������С��";
	if (word == "changeBoard")
		return "��������";
	if (word == "askToSaveBoard")
		return "ѯ�������Ƿ�浵";
	if (word == "defaultSaveBoard")
		return "Ĭ�������Ƿ�浵";
	if (word == "gameIsOver")
		return "��Ϸ����";
	if (word == "askToReverse")
		return "ѯ�ʽ��볷��ģʽ";
	if (word == "defaultReverse")
		return "Ĭ�Ͻ��볷��ģʽ";
	if (word == "inCustomMode")
		return "�Զ���ģʽ";
	if (word == "inDebugMode")
		return "��ͨģʽ";
	if (word == "hintOn")
		return "����ʾ";
	if (word == "showCalculate")
		return "��ʾ����������";
	if (word == "showTime")
		return "��ʾ��ʱ";
	if (word == "starrySky")
		return "�ǿ�";
	if (word == "starsOn")
		return "����";
	if (word == "trackRoutes")
		return "׷��·��";
	if (word == "whenSaveGame")
		return "������Ϸ";
	if (word == "askGiveName")
		return "ѯ�������浵";
	if (word == "defaultGiveName")
		return "Ĭ�������浵";
	if (word == "maxcaltime")
		return "������ʱ��";
	cout << "Word = " << word << endl;
	throw logic_error("toChinese: wrong input");
}
string toChinese(const bool word) {
	if (word)
		return "��";
	return "��";
}
#endif // STARS_LANG_CHINESE
