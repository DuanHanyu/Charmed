/*-- encoding: GBK --*/
#include "boardInterface.h"

BoardInterface::BoardInterface() {
	generate(8, 8, 4);
	analyse->maxcaltime = record.getOtherSettings("maxcaltime").asInt();
}

BoardInterface::BoardInterface(BoardAnalyse& hb) {
	analyse				= new BoardAnalyse(hb);
	analyse->maxcaltime = record.getOtherSettings("maxcaltime").asInt();
}

BoardInterface::~BoardInterface() {
	record.getOtherSettings("maxcaltime") = analyse->maxcaltime;
	delete analyse;
}

bool BoardInterface::getStateFromInput() {
	short  rows = analyse->getRows(), cols = analyse->getCols();
	vector<string> input;
	string inputEater;
	analyse->state.printHead();

	for (int i = 0; i < rows; ++i) {
		getline(cin, inputEater);
		if (inputEater == "0" || inputEater == "exit")
			return false;
		if (inputEater.size() != cols * 2 + 1) {
#ifndef STARS_LANG_CHINESE
			cout << "Something wrong going on with the input board\n";
#else
			cout << "输入的棋盘出了点问题\n";
#endif
			return false;
		}
		input.push_back(inputEater);
	}

	// transform
	char** temp = new char*[cols];
	if (transformInput(temp, input, cols, rows)) {
		analyse->state.refreshBoard(temp);
		analyse->state.refreshTop();
		analyse->state.freeBoard(temp, rows);
		return true;
	}
	analyse->state.freeBoard(temp, rows);
	return false;
}

bool BoardInterface::transformInput(char** dest, vector<string>& src,
	const int cols, const int rows) {
	bool rax = true;
	for (short i = 0; i < cols; ++i) {
		dest[i] = new char[rows];
		for (short j = 0; j < rows && rax; ++j) {
			// check
			if (src[rows - j - 1][i * 2 + 1] == '+')
				src[rows - j - 1][i * 2 + 1] = ' ';
			if (src[rows - j - 1][i * 2 + 1] != ' ' &&
				src[rows - j - 1][i * 2 + 1] != 'X' &&
				src[rows - j - 1][i * 2 + 1] != '0') {
				cout << "Something wrong going on with the input board while "
					<< "transforming.\n";
				rax = false;
				break;
			}
			// real transform
			dest[i][j] = src[rows - j - 1][i * 2 + 1];
		}
	}
	return rax;
}

// mode = "reverse", "add"
string BoardInterface::getInput(const string mode) {
	while (true) {
#ifndef STARS_LANG_CHINESE
		if (mode == "reverse")
			printf("\nIn reverse mode\nEnter move> ");
		else if (mode == "add")
			printf("\nIn Add mode\nEnter player(blank space)move> ");
#else
		if (mode == "reverse")
			printf("\n撤回模式\n输入行数> ");
		else if (mode == "add")
			printf("\n摆子模式\n输入棋子-数字> ");
#endif // STARS_LANG_CHINESE
		char input[INTER_MAX_INPUT];
		cin.getline(input, INTER_MAX_INPUT);

		if (input[0] == '\0' || !strcmp(input, "0") || !strcmp(input, "exit"))
			return "exit";
		else if (!strcmp(input, "quit") || !strcmp(input, "q"))
			return "quit";
		else if (!strcmp(input, "r") || !strcmp(input, "reverse")) {
			if (mode != "add")
#ifndef STARS_LANG_CHINESE
				printf("We are already in reverse mode\n");
#else
				printf("已经进入撤回模式\n");
#endif	// STARS_LANG_CHINESE
			else
				return "reverse";
		}
		else if (!strcmp(input, "a") || !strcmp(input, "add")) {
			if (mode != "reverse")
#ifndef STARS_LANG_CHINESE
				printf("We are already in add mode\n");
#else
				printf("已经进入摆子模式\n");
#endif	// STARS_LANG_CHINESE
			else
				return "add";
		}
		else if (!strcmp(input, "S") || !strcmp(input, "show"))
			analyse->show();
		else if (!strcmp(input, "h") || !strcmp(input, "help"))
			cout << getHelp(mode) << endl;
		else if (!strcmp(input, "i") || !strcmp(input, "info") ||
			!strcmp(input, "story") || !strcmp(input, "t") ||
			!strcmp(input, "tips"))
			cout << getInfo(input) << endl;
		else if (!strcmp(input, "m") || !strcmp(input, "move"))
#ifndef STARS_LANG_CHINESE
			printf("Why don't we try this in debug mode to see what it does?\n");
#else
			printf("你可以在普通模式里输入这个指令看看会发生什么\n");
#endif	// STARS_LANG_CHINESE
		else if (mode != "add") {
			int num = atoi(input);
			if (num > 0 && num <= analyse->getCols() && !analyse->colIsEmpty(num))
				return input;
#ifndef STARS_LANG_CHINESE
			cout << "Invalid " << mode << " mode number input, let\'s try again\n";
#else
			cout << "这个输入有点小问题\n";
#endif
		}
		else if (mode != "reverse") {
			char temp[INTER_MAX_INPUT];
			strcpy(temp, input + 2);
			int num = atoi(temp);
			if ((input[0] == 'X' || input[0] == 'x' || input[0] == '0') && num > 0 &&
				num <= analyse->getCols() && !analyse->colIsFull(num))
				return input;
		}
	}
}

// mode = "debug"
string BoardInterface::getInput(char plr, double& inputTime) {
	int						 counter = 0;
	system_clock::time_point start;
	system_clock::time_point end;
	string					 getter;
	while (true) {
		if (counter > 32)
			cin.clear();
#ifndef STARS_LANG_CHINESE
		cout << "\nIn debug mode\nPlayer '" << plr << "' move> ";
#else
		cout << "\n普通模式\n玩家 '" << plr << "' > ";
#endif // STARS_LANG_CHINESE
		char input[INTER_MAX_INPUT];
		start = system_clock::now();
		cin.getline(input, INTER_MAX_INPUT);
		end			 = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start);
		inputTime	 = elapsed.count();

		int num = atoi(input);
		if (num > 0 && num <= analyse->getCols() && !analyse->colIsFull(num))
			return input;
		// excute
		if (!strcmp(input, "0") || !strcmp(input, "exit"))
			return "exit";
		else if (input[0] == '\0') {
			++counter;
#ifndef STARS_LANG_CHINESE
			cout << "Invalid empty input, let\'s try again\n";
#else
			cout << "输入无效，请重试\n";
#endif // STARS_LANG_CHINESE
		}
		else if (!strcmp(input, "a") || !strcmp(input, "add"))
			return "add";
		else if (!strcmp(input, "C") || !strcmp(input, "custom"))
			return "custom";
		else if (!strcmp(input, "H") || !strcmp(input, "hint"))
			return input;
		else if (!strcmp(input, "h") || !strcmp(input, "help")) {
			getter = getHelp("debug");
			if (getter != "quit")
				cout << getter << endl;
			else
				return getter;
		}
		else if (!strcmp(input, "i") || !strcmp(input, "info") ||
			!strcmp(input, "story") || !strcmp(input, "t") ||
			!strcmp(input, "tips") || !strcmp(input, "a song, please")) {
			getter = getInfo(input);
			if (getter != "quit")
				cout << getter << endl;
			else
				return getter;
		}
		else if (!strcmp(input, "m") || !strcmp(input, "move") ||
			!strcmp(input, "I") || !strcmp(input, "import") ||
			!strcmp(input, "c") || !strcmp(input, "change"))
			return input;
		else if (!strcmp(input, "P") || !strcmp(input, "play back") ||
			!strcmp(input, "playback"))
			return "playBack";
		else if (!strcmp(input, "p") || !strcmp(input, "play") ||
			!strcmp(input, "play"))
			return "play";
		else if (!strcmp(input, "q") || !strcmp(input, "quit"))
			return "quit";
		else if (!strcmp(input, "r") || !strcmp(input, "reverse"))
			return "reverse";
		else if (!strcmp(input, "S") || !strcmp(input, "show")) {
			if (record.getDefaultSettings("inDebugMode", "starrySky"))
				analyse->starShow();
			else
				analyse->show();
		}
		else if (!strcmp(input, "s") || !strcmp(input, "setting") ||
			!strcmp(input, "settings"))
			return "settings";
		else if (!strcmp(input, "sa") || !strcmp(input, "st") ||
			!strcmp(input, "show stars"))
			analyse->starShow();
		else if (!strcmp(input, "sp") || !strcmp(input, "self play")) {
			if(selfPlayMode() == "quit")
				return "quit";
			continue;
		}
		else if (!strcmp(input, "sv") || !strcmp(input, "save"))
			record.saveGame(analyse->state);
		else if (!strcmp(input, "sr") || !strcmp(input, "show routes")) {
			analyse->routes.crnt = analyse->routes.head;
			if (showRoutesMode() == "quit")
				return "quit";
		}
		else if (!strcmp(input, "w") || !strcmp(input, "winn"))
			cout << "winn = " << analyse->state.winn << endl;
		else
#ifndef STARS_LANG_CHINESE
			cout << "No worries, Let\'s try again?\n";
#else
			cout << "来咱再试一次\n";
#endif
	}
}

short BoardInterface::getCustomInput(const string item) {
	char  input[16];
	short customNumber;
	while (true) {
#ifndef STARS_LANG_CHINESE
		cout << item << " (2~" << SHORTV_LENGTH - 1 << ") = ";
#else
		cout << toChinese(item) << " (2~" << SHORTV_LENGTH - 1 << ") = ";
#endif // STARS_LANG_CHINESE
		cin.getline(input, 16);
		if (!strcmp(input, "0") || !strcmp(input, "exit"))
			return 0;
		customNumber = atoi(input);
		if (customNumber > 1 && customNumber < SHORTV_LENGTH - 1)
			return customNumber;
#ifndef STARS_LANG_CHINESE
		cout << "Let's try again\n";
#else
		cout << "来咱再试一次\n";
#endif
	}
}

string BoardInterface::addMode() {
	string	input;
	oneMove move;
	move.mode = "add";
	while (true) {
		input = getInput("add");
		if (input == "exit")
			return "debug";
		if (input == "quit" | input == "reverse")
			return input;
		if (input[0] == '0')
			move.player = input[0];
		else
			move.player = 'X';
		move.move = atoi(input.c_str() + 2);
#ifndef STARS_LANG_CHINESE
		printf("Player '%c' is added to '%d' as you like it:\n", move.player, move.move);
#else
		printf("棋子 '%c' 已被如愿添加到 '%d' 列:\n", move.player, move.move);
#endif // STARS_LANG_CHINESE
		analyse->go(move.player, move.move);
		analyse->show();
		record.push_back(move);
	}
}

string BoardInterface::reverseMode() {
	string	input;
	oneMove move;
	move.mode = "reverse";
	while (true) {
		input = getInput("reverse");
		if (input == "exit")
			return "debug";
		if (input == "quit" || input == "add")
			return input;
		move.move = atoi(input.c_str());
		// record which move is reversed
		move.player = analyse->state.board[move.move - 1][analyse->state.top[move.move - 1] - 1];
		analyse->reverse(move.move);
#ifndef STARS_LANG_CHINESE
		printf("Remove %d as you like it:\n", move.move);
#else
		printf("%d 列被撤回:\n", move.move);
#endif // STARS_LANG_CHINESE
		record.push_back(move);
		analyse->show();
	}
}

string BoardInterface::debugMode(oneMove& byPlayer) {
	// init
	oneMove byOpponent;
	string	input;
	byPlayer.mode		  = "debug";
	byPlayer.byComputer	  = false;
	byOpponent.mode		  = "debug";
	byOpponent.byComputer = true;
	byOpponent.player	  = analyse->rPlayer(byPlayer.player);
	analyse->show();

	// main loop
	while (true) {
		input = getInput(byPlayer.player, byPlayer.time);
		if (input == "exit" || input == "quit") {
#ifndef STARS_LANG_CHINESE
			printf("Exit from debug mode ...\n");
#else
			printf("退出普通模式 ...\n");
#endif // STARS_LANG_CHINESE
			return input;
		}
		if (input == "add" || input == "reverse" || input == "settings" ||
			input == "custom" || input == "play" || input == "playBack")
			return input;
		if (input == "c" || input == "change") {
			// change player
			std::swap(byPlayer.player, byOpponent.player);
			std::swap(byPlayer.word, byOpponent.word);
			std::swap(byPlayer.list, byOpponent.list);
			std::swap(byPlayer.suggestion, byOpponent.suggestion);

			// print hint
			if (byPlayer.mode == "debug" && record.getDefaultSettings("inDebugMode", "hintOn")) {
#ifndef STARS_LANG_CHINESE
				printf("Here was %c's word, list, and suggestion:\n", byPlayer.player);
				cout << "word = " << byPlayer.word << "\nlist = [ ";
#else
				printf("另一个玩家 %c 的状态，建议列表和建议如下:\n", byPlayer.player);
				cout << "玩家状态 = " << toChinese(byPlayer.word) << "\n建议列表 = [ ";
#endif // STARS_LANG_CHINESE
				for (short move : byPlayer.list)
					printf("%d ", move);
				printf("]\n");
			}
			continue;
		}
		else if (input == "I" || input == "import") {
			if (record.getDefaultSettings("changeBoard", "askToSaveBoard"))
				askToSaveBoard(record.getDefaultSettings("changeBoard", "defaultSaveBoard"));
			else if (record.getDefaultSettings("changeBoard", "defaultSaveBoard"))
				record.saveGame(analyse->state);
			// new board
			importNewBoard();
			continue;
		}
		else if (input == "H" || input == "hint") {
#ifndef STARS_LANG_CHINESE
			if (byPlayer.mode != "debug") {
				printf("The last move is not in debug move, therefore has no hint.\n");
				continue;
			}
			printf("this is the hint for the previous move:\n");
			cout << "word = " << byPlayer.word << "\nlist = [ ";
#else
			if (byPlayer.mode != "debug") {
				printf("上一步不在普通模式，所以没有提示\n");
				continue;
			}
			printf("上一步的提示：\n");
			cout << "玩家状态 = " << toChinese(byPlayer.word) << "\n建议列表 = [ ";
#endif // STARS_LANG_CHINESE
			for (short i : byPlayer.list)
				printf("%d ", i);
			printf("]\n");
			continue;
		}
		else if (input != "m" && input != "move") {
			// player goes
			byPlayer.move = atoi(input.c_str());
			analyse->go(byPlayer.player, byPlayer.move);
			record.push_back(byPlayer);	 // byPlayer end here
			if (isOver(byPlayer)) {
#ifndef STARS_LANG_CHINESE
				printf("Exit from debug mode ...\n");
#else
				printf("退出普通模式 ...\n");
#endif // STARS_LANG_CHINESE
				return "over";
			}
		}
		else if (isOver(byPlayer)) {
#ifndef STARS_LANG_CHINESE
			printf("Exit from debug mode ...\n");
#else
			printf("退出普通模式 ...\n");
#endif // STARS_LANG_CHINESE
			return "over";
		}

		// opp respond
		if (record.getDefaultSettings("inDebugMode", "showCalculate") &&
			record.getDefaultSettings("inDebugMode", "showTime"))
#ifndef STARS_LANG_CHINESE
			printf("Info for the computer:\n");
#else
			printf("电脑信息：\n");
#endif // STARS_LANG_CHINESE
		byOpponent.move = analyse->respond(byOpponent.player, byOpponent,
			record.getDefaultSettings("inDebugMode", "showCalculate"),
			record.getDefaultSettings("inDebugMode", "showTime"),
			record.getDefaultSettings("inDebugMode", "starsOn"),
			record.getDefaultSettings("inDebugMode", "trackRoutes"));
#ifdef STARS_DEBUG_INFO
		if (!byOpponent.list.empty() &&
			!MyShortList::inList(byOpponent.list, byOpponent.move))
			throw logic_error("suggestion not in safe list");
#endif
		analyse->go(byOpponent.player, byOpponent.move);
		byOpponent.suggestion = byOpponent.move;
		record.push_back(byOpponent);
#ifndef STARS_LANG_CHINESE
		if (record.getDefaultSettings("inDebugMode", "showTime"))
			cout << "    input time used: " << byPlayer.time << " ms\n";
		printf("    %c goes here %d\n", byOpponent.player, byOpponent.move);
		if (isOver(byOpponent)) {
			printf("Exit from debug mode ...\n");
			return "over";
		}

		byPlayer.hintOn	= record.getDefaultSettings("inDebugMode", "hintOn");
		if (byPlayer.hintOn)
			cout << "\nHere is hint provided for you\n";
#else
		if (record.getDefaultSettings("inDebugMode", "showTime"))
			cout << "    输入用时：" << byPlayer.time << " 毫秒\n";
		printf("    %c 走了这一步：%d\n", byOpponent.player, byOpponent.move);
		if (isOver(byOpponent)) {
			printf("退出普通模式 ...\n");
			return "over";
		}
		
		byPlayer.hintOn	= record.getDefaultSettings("inDebugMode", "hintOn");
		if (byPlayer.hintOn)
			cout << "\n给玩家的提示信息\n";
#endif // STARS_LANG_CHINESE

		// recommend
		byPlayer.suggestion = analyse->respond(byPlayer.player, byPlayer,
			byPlayer.hintOn,
			record.getDefaultSettings("inDebugMode", "showTime"),
			record.getDefaultSettings("inDebugMode", "starsOn"),
			false);
		showComment(byPlayer);
		if (byPlayer.hintOn && byPlayer.word != "bad")
#ifndef STARS_LANG_CHINESE
			printf("    %d is recommended for player %c\n", byPlayer.suggestion, byPlayer.player);
#else
			printf("    推荐 %c 走这一步：%d\n", byPlayer.player, byPlayer.suggestion);
#endif // STARS_LANG_CHINESE

		// show
		printf("\n");
		if (record.getDefaultSettings("inDebugMode", "starrySky"))
			analyse->starShow();
		else
			analyse->show();
		printf("\n");
	}
#ifdef STARS_DEBUG_INFO
	throw logic_error("control flow into the end of debug mode");
#endif
}

string BoardInterface::defaultSettings() {
#ifndef STARS_LANG_CHINESE
	cout << "We have " << record.getDefaultSettingsItemNum() << " situations and "
		 << "within them we have multiple items each marked with tags\n"
		 << "below, type in tags to change these settings, 0 to exit, S/show "
		 << "to show current\nsettings, h/help for help\n";
	// show
	record.showSettingsWithTags();
	cout << "\nCare to change one of them?\n";
#else
	cout << "一共有" << record.getDefaultSettingsItemNum() << "种情况，其中"
		 << "每个情况有多个项目，各个项目均有两个字母为标记，输入标记\n"
		 << "来改变相应的设定值输入0退出，输入S或show显示当前设置，输入h获得帮助\n";
	// show
	record.showSettingsWithTags();
#endif // STARS_LANG_CHINESE
	while (true) {
		printf("> ");
		char input[8];
		cin.getline(input, 8);
		if (input[0] == '\0' || !strcmp(input, "0") || !strcmp(input, "exit") ||
			!strcmp(input, "no")) {
			record.saveSettings();
			cout << "Exit from settings mode\n";
			return "debug";
		}
		if (!strcmp(input, "q") || !strcmp(input, "quit")) {
			cout << "Exit from settings mode\n";
			record.saveSettings();
			return "quit";
		}
		else if (!strcmp(input, "h") || !strcmp(input, "help"))
			cout << getHelp("settings");
		else if (!strcmp(input, "S") || !strcmp(input, "show"))
			record.showSettingsWithTags();
		else if (strlen(input) == 2) {
			int tag1 = input[0] - 'a', tag2 = input[1] - 'a';
			if (!(tag1 < 26 && tag1 > -1 && tag2 < 26 && tag2 > -1 &&
				record.changeSettingsUsingTags(tag1, tag2))) {
				cout << "Invalid input, let's try again.\n";
				cout << "And if you don't know what's going on, you can always"
						" type help, happy to help as always.\n";
			}
		}
		else {
			cout << "Invalid input, let's try again.\n";
			cout << "And if you don't know what's going on, you can always "
				"type help, happy to help as always.\n";
		}
	}
#ifdef STARS_DEBUG_INFO
	throw logic_error("control flow into the end of settings mode");
	return "quit";
#endif
}

string BoardInterface::otherSettings() {
	if (analyse->maxcaltime < 0)
		printf("analyse->maxcaltime went nuts, better fix it\n");
#ifndef STARS_LANG_CHINESE
	cout << "Here you can change computer's max calculation time settings, the\n"
		<< "current cal time is "
		<< analyse->maxcaltime
		<< "ms\n"
		<< "Enter nothing or 0 to exit, q to quit\n";
#else
	cout << "你可以在此处改变程序的最大计算时间，当前最大计算时间是\n"
		<< analyse->maxcaltime
		<< "毫秒\n直接回车或输入0返回，q退出游戏\n";
#endif // STARS_LANG_CHINESE
	char in[16];
	int	 trans = 0;
	do {
		printf("(10ms ~ 9999999ms)> ");
		cin.getline(in, 16);
		if (in[0] == '\0' || !strcmp(in, "0") || !strcmp(in, "exit"))
			return "debug";
		if (!strcmp(in, "q") || !strcmp(in, "quit"))
			return "quit";
		trans = atoi(in);
		if (trans > 9 && trans < 9999999) {
			analyse->maxcaltime = trans;
			return "debug";
		}
		printf("let's try again\n");
	} while (true);
}

string BoardInterface::settingsMode() {
#ifndef STARS_LANG_CHINESE
	printf("We are in settings mode now:\n");
	char input[8];
	printf("1. default settings\n2. other settings\n");
	do {
		printf("(enter number)> ");
#else
	printf("设置模式\n");
	char input[8];
	printf("1. 默认设置\n2. 其他设置\n");
	do {
		printf("(输入数字)> ");
#endif // STARS_LANG_CHINESE
		cin.getline(input, 8);
		if (input[0] == '\0' || !strcmp(input, "0") || !strcmp(input, "exit"))
			return "debug";
		if (!strcmp(input, "q") || !strcmp(input, "quit"))
			return "quit";
		if (!strcmp(input, "1"))
			return defaultSettings();
		if (!strcmp(input, "2"))
			return otherSettings();
		else
#ifndef STARS_LANG_CHINESE
			printf("let's try again\n");
	} while (true);
#else
			printf("我们来再试一次\n");
	} while (true);
#endif // STARS_LANG_CHINESE
}

string BoardInterface::playMode() {
	if (!record.getNumberOfSavedBoard()) {
		cout << "No games has been recorded yet. If you want to try this mode, "
				"save a game first\nTill next time.\nExit from play mode...\n";
		return "debug";
	}
#ifndef STARS_LANG_CHINESE
	printf("We are here in play mode.\n");
	printf("You can choose from %d saved games.\n", record.getNumberOfSavedBoard());
	printf("Hit 'Enter' to see more, type in index number to went into one of those games.\n");
	printf("Or type c to continue to play the board that is currently displayed. ");
	printf("Use d/rm/delete/remove to delete the current board, 0 to exit.\n");
	printf("Explore!\n");
#else
	printf("这里是读档模式\n");
	printf("共 %d 份存档\n", record.getNumberOfSavedBoard());
	printf("回车浏览下一个，输入索引号直接读取该存档，输入c读取当前显示的存档\n");
	printf("使用 d/rm/delete/remove 删除当前存档，0退出\n");
	printf("探索愉快!\n");
#endif // STARS_LANG_CHINESE
	Json::Value game;
	string result = record.showSavedGames(game);

	if (result == "exit") {
		printf("Exit from play mode...\n");
		return "debug";
	}
	if (result == "quit") {
		printf("Exit from play mode...\n");
		return "quit";
	}

	// set up a new game
	BoardAnalyse   analyser(game["state"]);
	BoardInterface interface(analyser);
	BoardRecord	   newRecord;
	newRecord.refreshHistoryMove(game["historyMove"]);
	interface.refreshRecord(newRecord);
#ifndef STARS_LANG_CHINESE
	printf("Into a new control mode:\n");
#else
	printf("进入一个新游戏：\n");
#endif // STARS_LANG_CHINESE

	if(!interface.controlMode())
		return "quit";
#ifndef STARS_LANG_CHINESE
	printf("Exit from play mode ...\n");
#else
	printf("退出读档模式 ...\n");
#endif // STARS_LANG_CHINESE
	return "debug";
}

string BoardInterface::playBackMode() {
#ifndef STARS_LANG_CHINESE
	if (!record.getNumberOfSavedBoard()) {
		printf("No games has been recorded yet. If you want to try this mode,");
		cout << " save a game first\nTill next time.\n";
		printf("Exit from play back mode...\n");
		return "debug";
	}
	printf("We are here in play back mode.\n");
	printf("You can choose from %d saved games.\n", record.getNumberOfSavedBoard());
	printf("Hit 'Enter' to see more, type in index number to went into one of those games.\n");
	printf("Or type c to continue to play the board that is currently displayed. ");
	printf("Use d/rm/delete/remove to delete the current board, 0 to exit.\n");
	printf("Explore!\n");
	Json::Value game;
	string result = record.showSavedGames(game);
	if (result == "exit") {
		printf("Exit from play back mode...\n");
		return "debug";
	}
	if (result == "quit") {
		printf("Exit from play back mode...\n");
		return "quit";
	}
#else
	if (!record.getNumberOfSavedBoard()) {
		printf("还没有任何存档，如果你想使用此模式，请先保存游戏");
		printf("退出读档回放模式 ...\n");
		return "debug";
	}
	printf("读档回放模式\n");
	printf("共 %d 份存档\n", record.getNumberOfSavedBoard());
	printf("回车浏览下一个，输入索引号直接读取该存档，输入c读取当前显示的存档\n");
	printf("使用 d/rm/delete/remove 删除当前存档，0退出\n");
	printf("探索愉快!\n");
	Json::Value game;
	string result = record.showSavedGames(game);
	if (result == "exit") {
		printf("退出读档回放模式 ...\n");
		return "debug";
	}
	if (result == "quit") {
		printf("退出读档回放模式 ...\n");
		return "quit";
	}
#endif // STARS_LANG_CHINESE

	// set up a new game and play back the moves
	BoardState	 state = game["state"];
	BoardRecord	 newRecord, tempRecord;
	newRecord.refreshHistoryMove(game["historyMove"]);
	// return to the initial state
	state.retInit(newRecord.historyMove);
	BoardAnalyse analyser(state);
#ifndef STARS_LANG_CHINESE
	printf("Here's the play back of an old game:\n");
	printf("Use c to cut in and play, h for help, b to go back to the previous step, 0\nto exit.\n");
#else
	printf("这是存档游戏的回放：\n");
	printf("使用c打断回放并进入游戏模式，b回到上一步，h帮助，0退出\n");
#endif // STARS_LANG_CHINESE
	char					  input[16];
	bool					  wentBack		= false;
	char					  reversePlayer = ' ';
	vector<oneMove>::iterator iter			= newRecord.historyMove.begin();
	while (iter != newRecord.historyMove.end()) {
		// show
#ifndef STARS_LANG_CHINESE
		printf("This is step %zu/%zu\n", tempRecord.historyMove.size() + 1, newRecord.historyMove.size());
#else
		printf("此步/总步数 %zu/%zu\n", tempRecord.historyMove.size() + 1, newRecord.historyMove.size());
#endif // STARS_LANG_CHINESE
		if (wentBack)
			wentBack = false;
		else {
			cout << *iter;
			tempRecord.push_back(*iter);
			if (iter->mode != "reverse")
				analyser.go(iter->player, iter->move);
			else {
				reversePlayer = analyser.state.board[iter->move - 1][analyser.state.top[iter->move - 1] - 1];
				analyser.reverse(iter->move);
			}
		}
		analyser.show();

		// get input
		printf("> ");
		cin.getline(input, 16);
		if (!strcmp(input, "0") || !strcmp(input, "exit"))
			break;
		if (!strcmp(input, "q") || !strcmp(input, "quit"))
			return "quit";
		else if (!strcmp(input, "c") || !strcmp(input, "cut") || !strcmp(input, "cut in")) {
			BoardAnalyse   tempAnalyser = analyser;
			BoardInterface interface(tempAnalyser);
			interface.refreshRecord(tempRecord);
#ifndef STARS_LANG_CHINESE
			printf("Go into a new control mode:\n");
			if(!interface.controlMode())
				return "quit";
			printf("We're back in play back mode again\n");
#else
			printf("进入新的游戏模式\n");
			if(!interface.controlMode())
				return "quit";
			printf("回到读档回放模式\n");
#endif // STARS_LANG_CHINESE
		}
		else if (!strcmp(input, "b") || !strcmp(input, "back") || !strcmp(input, "go back")) {
			if (tempRecord.historyMove.empty())
#ifndef STARS_LANG_CHINESE
				printf("This is the beginning, there is no going back.\n");
#else
				printf("已经回到开头，没有上一步了\n");
#endif // STARS_LANG_CHINESE
			else {
				tempRecord.pop_back();
				if (iter->mode != "reverse")
					analyser.reverse(iter->move);
				else
					analyser.go(reversePlayer, iter->move);
				wentBack = true;
				--iter;
				continue;
			}
		}
		else if (!strcmp(input, "h") || !strcmp(input, "help"))
#ifndef STARS_LANG_CHINESE
			printf("b to go back to the previous move, c to cut in and play, q to quit\n");
			printf("the whole game, 0 to exit this mode\n");
#else
			printf("使用c打断回放并进入游戏模式，b回到上一步，h帮助，0退出\n");
#endif // STARS_LANG_CHINESE
		++iter;
	}
#ifndef STARS_LANG_CHINESE
	printf("Exit from play back mode ...\n");
#else
	printf("退出读档回放模式 ...\n");
#endif // STARS_LANG_CHINESE
	return "debug";
}

string BoardInterface::customMode() {
#ifndef STARS_LANG_CHINESE
	printf("In custom mode ...\n");
	printf("Note that the more width you set, the clumsier the computer will be\n");
	printf("It is recommended to set the width lesser than 16\n");
#else
	printf("自定义模式 ...\n");
	printf("注意设置的宽度越大，程序的计算量就越大，可能会导致程序变慢，不推荐\n");
	printf("将宽度设置到16以上\n");
#endif // STARS_LANG_CHINESE
	if (record.getDefaultSettings("inCustomMode", "askToSaveBoard"))
		askToSaveBoard(record.getDefaultSettings("inCustomMode", "defaultSaveBoard"));
	else if (record.getDefaultSettings("inCustomMode", "defaultSaveBoard"))
		record.saveGame(analyse->state);
	short column, row, winn;
	column = getCustomInput("board width");
	if (!column)
		return "debug";
	row = getCustomInput("board height");
	if (!row)
		return "debug";
	winn = getCustomInput("win number");
	if (!winn)
		return "debug";
	analyse->customBoard(column, row, winn);
	record.clearHistoryMove();
#ifndef STARS_LANG_CHINESE
	printf("Exit from custom mode ...\n");
#else
	printf("退出自定义模式 ...\n");
#endif // STARS_LANG_CHINESE
	return "debug";
}

bool BoardInterface::controlMode() {
	oneMove byPlayer;
	byPlayer.player = 'X';
	string	advice	= debugMode(byPlayer);
	int		i		= 0;
	while (i < 100) {
		if (advice == "quit" || advice == "exit")
			break;
		else if (advice == "add")
			advice = addMode();
		else if (advice == "reverse")
			advice = reverseMode();
		else if (advice == "debug")
			advice = debugMode(byPlayer);
		else if (advice == "settings")
			advice = settingsMode();
		else if (advice == "play")
			advice = playMode();
		else if (advice == "playBack")
			advice = playBackMode();
		else if (advice == "custom")
			advice = customMode();
		else if (advice == "over") {
			if (record.getDefaultSettings("gameIsOver", "askToReverse") &&
				askToReverse(record.getDefaultSettings("gameIsOver", "defaultReverse"))) {
				advice = reverseMode();
				continue;
			}
			else if (record.getDefaultSettings("gameIsOver", "defaultReverse")) {
				advice = reverseMode();
				continue;
			}
			if (record.getDefaultSettings("gameIsOver", "askToSaveBoard"))
				askToSaveBoard(record.getDefaultSettings("gameIsOver", "defaultSaveBoard"));
			else if (record.getDefaultSettings("gameIsOver", "defaultSaveBoard"))
				record.saveGame(analyse->state);
			break;
		}
		++i;
	}
	if (!record.match())
		throw runtime_error("current settings var in BoardRecord doesn't match with the default settings");
	if (i == 100)
		throw runtime_error("too much unhandled advice");
	if (advice == "quit")
		return false;
#ifndef STARS_LANG_CHINESE
	cout << "Exit from controller ...\n";
#else
	cout << "退出控制模式 ...\n";
#endif // STARS_LANG_CHINESE
	return true;
}

string BoardInterface::showRoutesMode() {
	RouteTree&			routes		  = analyse->routes;
	vector<RouteNode*>& next		  = routes.crnt->next;
	bool				nearFlag	  = false;
	long long routeBranches = routes.getBranches();
#ifndef STARS_LANG_CHINESE
	cout << "The program examined " << routeBranches
		 << " possibilities in this node\n" << routes.getBranches(-2)
		 << " of them is free, " << routes.getBranches(-1)
		 << " of them is good and " << routes.getBranches(0)
		 << " of them is bad for the program\n";
	cout << "We'll be showing:\na. free routes\nb. good routes\nc. bad "
		 << "routes\nd. all routes\n";
	if (next.size() == 1 && next[0]->next.empty() &&
		(next[0]->data == routes.goodNode || next[0]->data == routes.badNode ||
		next[0]->data == routes.freeNode))
		nearFlag = true; // next is a flag
	else {
		cout << "or see less in the next node choosing from:\n[ ";
		vRi iter = next.begin();
		for (; iter != next.end(); ++iter)
			cout << (*iter)->data << " ";
		cout << "]\n";
	}
	if (routes.crnt->prev)
		cout << "Enter B or back to go back to the formal node\n";
	string in;
	string tryAgain = "No such move, let's try again";
#else
	cout << "程序为自己检验了 " << routeBranches
		 << "种路径，其中有 " << routes.getBranches(-2)
		 << " 个自由 " << routes.getBranches(-1)
		 << " 个好和 " << routes.getBranches(0)
		 << " 个坏\n";
	cout << "通过输入相应字母，本程序将展示：\na. 自由的路径\nb. 好的路径\nc. 坏的路径\nd. 所有路径\n";
	if (next.size() == 1 && next[0]->next.empty() &&
		(next[0]->data == routes.goodNode || next[0]->data == routes.badNode ||
		next[0]->data == routes.freeNode))
		nearFlag = true; // next is a flag
	else {
		cout << "或者输入相应数字选择进入如下路径，查看该路径下的种种可能\n[ ";
		vRi iter = next.begin();
		for (; iter != next.end(); ++iter)
			cout << (*iter)->data << " ";
		cout << "]\n";
	}
	if (routes.crnt->prev)
		cout << "输入B或back返回上一个路径\n";
	string in;
	string tryAgain = "没有这个选项，我们再试一次";
#endif // STARS_LANG_CHINESE
	int num = 0;
	do {
		cout << "> ";
		getline(cin, in);
		if (in == "q" || in == "quit")
			return "quit";
		if (in == "0" || in == "exit" || in.empty())
			return "debug";
		if (in == "a") {
			num = 1;
			break;
		}
		else if (in == "b") {
			num = 2;
			break;
		}
		else if (in == "c") {
			num = 3;
			break;
		}
		else if (in == "d") {
			num = 4;
			break;
		}
		else if (in == "B" || in == "back") {
			if (routes.crnt->prev) {
				routes.backward();
				// 新时代的老递归怪
				return showRoutesMode();
			}
#ifndef STARS_LANG_CHINESE
			cout << "There's no going back, let's try again.\n";
#else
			cout << "没有回头路了，我们再试一次\n";
#endif // STARS_LANG_CHINESE
			continue;
		}
		else if (!nearFlag) {
			num = atoi(in.c_str());
			// find_if by me
			vRi iter = next.begin();
			for (; iter != next.end() &&
				(*iter)->data != num;++iter);
			if (iter == next.end()) {
				cout << tryAgain << endl;
				continue;
			}
			routes.forward(num);
			// 老递归怪了
			return showRoutesMode();
		}
		else
			cout << tryAgain << endl;
	} while (true);


	// print
	routeBranches = routes.getBranches(num - 3);
	if (routeBranches > 1024) {
#ifndef STARS_LANG_CHINESE
		cout << "There are " << routeBranches
			<< " branches, sure you want to print them all?\n"
			<< "(Yes/no)> ";
#else
		cout << "一共有 " << routeBranches
			<< " 种路径，确定要全部输出？\n"
			<< "(Yes/no)> ";
#endif // STARS_LANG_CHINESE
		getline(cin, in);
		if (in != "no" && in != "n")
			routes.showRoute(num - 3);
	}
	else
		routes.showRoute(num - 3);
	return "debug";
}

string BoardInterface::selfPlayMode() {
	cout << "With hit or without hit (w/Wo)>";
	string input;
	bool   with = false;
	//getline(cin, input);
	if (input == "w")
		with = true;
	else if (input == "q" || input == "quit")
		return input;
	else if (input == "0" || input == "exit")
		return "debug";
	
	// show time
	oneMove byOpponent, byPlayer;
	// string	input;
	// byPlayer.mode		  = "selfPlay";
	byPlayer.player		  = 'X';
	// byPlayer.byComputer	  = true;
	byPlayer.move		  = 5;
	// byOpponent.mode		  = "selfPlay";
	// byOpponent.byComputer = true;
	byOpponent.player	  = '0';
	
	printf("We are in self play mode now\n");
	analyse->show();

	// main loop
	while (true) {
		if (with)
			break;
		// player goes
		analyse->go(byPlayer.player, byPlayer.move);
		printf("    %c goes here %d\n", byPlayer.player, byPlayer.move);
		// record.push_back(byPlayer);	 // byPlayer end here
		if (isOver(byPlayer)) {
			printf("Exit from self play mode ...\n");
			return "quit";
		}

		// opp respond
		// printf("Info for the computer:\n");
		byOpponent.move = analyse->respond(byOpponent.player, byOpponent,
			false, false,
			// record.getDefaultSettings("inDebugMode", "showCalculate"),
			// record.getDefaultSettings("inDebugMode", "showTime"),
			record.getDefaultSettings("inDebugMode", "starsOn"),
			false
			// record.getDefaultSettings("inDebugMode", "trackRoutes")
			);
#ifdef STARS_DEBUG_INFO
		if (!byOpponent.list.empty() &&
			!MyShortList::inList(byOpponent.list, byOpponent.move))
			throw logic_error("suggestion not in safe list");
#endif
		// analyse->go(byOpponent.player, byOpponent.move);
		// record.push_back(byOpponent);
		// cout << "    input time used: " << byPlayer.time << "ms\n";
		// printf("    %c goes here %d\n", byOpponent.player, byOpponent.move);
		// if (isOver(byOpponent)) {
		// 	printf("Exit from self play mode ...\n");
		// 	return "quit";
		// }

		// recommend
		// byPlayer.hintOn	= record.getDefaultSettings("inDebugMode", "hintOn");
		// if (byPlayer.hintOn)
		// 	cout << "\nHere is hint provided for you\n";
		// byPlayer.suggestion = analyse->respond(byPlayer.player, byPlayer,
			// byPlayer.hintOn,
			// record.getDefaultSettings("inDebugMode", "showTime"),
			// record.getDefaultSettings("inDebugMode", "starsOn"),
			// false);
		// showComment(byPlayer);
		// if (byPlayer.hintOn && byPlayer.word != "bad")
		// 	printf("    %d is recommended\n", byPlayer.suggestion);

		// show
		printf("\n");
		if (record.getDefaultSettings("inDebugMode", "starrySky"))
			analyse->starShow();
		else
			analyse->show();
		printf("\n");

		// change the player
		std::swap(byPlayer.player, byOpponent.player);
		// std::swap(byPlayer.word, byOpponent.word);
		// std::swap(byPlayer.list, byOpponent.list);
		std::swap(byPlayer.move, byOpponent.move);
	}

	return "debug";
}

string BoardInterface::getHelp(string mode) {
#ifndef STARS_LANG_CHINESE
	string enjoy  = "Enjoy!\n";
	string addon  = "";
	string end	  = "------------------------------------ The End ----------------------------------\n";
	string enterForMore =
		"------------------------- 'Enter' for more, 0 to quit -------------------------\n";
	string add = addon + "Enter player (X or 0) and number to place a piece, for example, enter\n" +
		"X 3\nwill place a pice X in column 3, you can also type x-7 if you want. Just make\n" +
		"sure there's one and only one character between player and its move.\n" +
		"Hit 'Enter' only to exit add mode.\n";
	string reverse = "Type in column number to reverse that action. Hit 'Enter' to exit\n";
	string moreAddOrReverse =
		addon + "If you don't know what's going on, why are you here. Why don't you exit from here\n" +
		"and see what the help information in debug mode have to say?\n" + enjoy + end;
	string normal =
		addon + "Type in number to play, one who place four piece in a row first wins\n\n" +
		"0/exit             exit from a certain mode\n" +
		"q/quit             quit the whole game\n" +
		"C/custom           custom board height, width and win number (4 by default)\n" +
		"h/help             show help message of the current mode\n" +
		"p/play             into play mode\n" +
		"P/play back        into play back mode\n" +
		"S/show             show the current board\n" +
		"s/settings         view and change the settings\n" +
		"st/show stars      show stars\n" +
		"sv/save            save the current game\n" +
		"sr/show routes     show routes that the computer has examined\n" +
		"t/tips             tips I wrote to help other player (you) to play the game\n" +
		"w/winn             show win number (4 by default) in case you forgot\n" +
		"i/info             information about the game\n\n" + enterForMore;
	vector<string> moreDebug = {
		addon +
			"If hintOn is true, then when the computer says your word = good, then you'll\n" +
			"win in a few steps if you chose to take the step within the list that follows.\n" +
			"Note that if starsOn is true however, then there is a 15% chance that the\n" +
			"computer will make one mistake in every 30 rounds, which means that the rules\n" +
			"above won\'t apply ;-)\n" + enterForMore,
		addon + "If word=free, list=[1, 5] but you can see that there are plenty of column that\n" +
			"is not full but out side of that [1, 5] list, it is recommended that you take\n" +
			"the step within the list for every Move outside the safe list is risky.\n" +
			"Note that area that's covered by the stars cannot be accessed by the computer,\n" +
			"therefore might contain surprise or danger.\n" + enterForMore,
		addon +
			"a/add        into add mode: add some moves\n" +
			"c/change     change (swap) the player\n" +
			"H/hint       show hint for the previous step\n"
			"I/import     import a new board from input, which must fit in the current board\n" +
			"m/move       force the computer to take a move now\n" +
			"r/reverse    into reverse mode: reverse some moves\n" +
			enjoy + end};
#else
	string enjoy  = "玩的开心!\n";
	string addon  = "";
	string end	  = "------------------------------------ 没了 ----------------------------------\n";
	string enterForMore =
		"------------------------- '回车'显示余下信息， 0返回 -------------------------\n";
	string add = addon + "输入棋子(X或0)和数字来指定放置棋子的种类和位置。比如：输入\n" +
		"X 3\n就是把棋子X放在第三列，也可以输入'x-7'，只需要保证棋子和数字之间有且只有一个字符即可\n" +
		"不作输入直接回车回导致推出添加模式\n";
	string reverse = "输入列数撤回该列最顶端棋子，按回车退出\n";
	string moreAddOrReverse =
		addon + "如果你不知道发生了什么，最好回到普通模式看那里的帮助文档\n" + enjoy + end;
	string normal =
		addon + "输入非0数字下棋，四子连成一条线则获胜，注意棋子只能从下往上叠起来\n\n" +
		"选项               效果\n" +
		"0/exit             退出所在模式\n" +
		"q/quit             退出游戏\n" +
		"C/custom           自定义棋盘长宽和获胜所需最小成排棋子数（默认4）\n" +
		"h/help             显示当前模式帮助文档\n" +
		"p/play             进入读档模式\n" +
		"P/play back        进入读档回放模式\n" +
		"S/show             显示当前棋盘\n" +
		"s/settings         进入设置模式\n" +
		"st/show stars      显示星星\n" +
		"sv/save            存档\n" +
		"sr/show routes     显示电脑计算过的路径\n" +
		"t/tips             显示游戏玩法提示\n" +
		"w/winn             显示获胜所需最小成排棋子数\n" +
		"i/info             显示游戏信息\n\n" + enterForMore;
	vector<string> moreDebug = {
		addon +
			"    如果打开提示（hintOn）为真，那么当电脑显示玩家状态为好时，如果玩家选择遵\n" +
			"循推荐列表中的下法，那么玩家将在几步之内获胜，反之如果显示玩家状态为坏\n" +
			"则电脑将在几步之内获胜\n" +
			"    不过如果设置中的晴天（starsOn）为真，那么电脑有大概15%概率计算错误 ;-)\n" + enterForMore,
		addon + "    当玩家状态显示为自由时，推荐列表中的下法一般可以随便下，推荐列表之外\n" +
			"就不好说了，最好不要下。\n" +
			"    注意有星星的地方电脑是看不到的，所以不会推荐，但这并不说明那儿是好或者坏\n" +
			"只能说明那儿远\n" + enterForMore,
		addon +
			"a/add        进入摆子模式\n" +
			"c/change     人机玩家身份互换\n" +
			"H/hint       显示上一步的提示\n"
			"I/import     导入新棋盘，注意新旧棋盘的大小必须相同\n" +
			"m/move       使电脑立刻走子（就不等你下了）\n" +
			"r/reverse    进入撤回模式\n" +
			enjoy + end
		};
#endif // STARS_LANG_CHINESE
	if (mode == "debug") {
		cout << normal;
		char dis[16];
		int	 wrongInput = 0;
		int	 i			= 0;
		while (i < 3) {
			printf("> ");
			cin.getline(dis, 16);
			if (!strcmp(dis, "0") || !strcmp(dis, "exit"))
				break;
			if (!strcmp(dis, "quit") || !strcmp(dis, "q"))
				return "quit";
			else if (i < 3 && (!strlen(dis) || !strcmp(dis, "m") || !strcmp(dis, "more"))) {
				cout << moreDebug[i];
				++i;
			}
			else {
#ifndef STARS_LANG_CHINESE
				cout << "Pardon?\n";
#else
				cout << "啥?\n";
#endif // STARS_LANG_CHINESE
				++wrongInput;
			}
			if (wrongInput > 8) {
				cout << "There might be an overflow here, please don't enter more than 15 characters at "
					"a\ntime\n";
				cin.clear();
				wrongInput = 0;
			}
		}
		return addon;
	}
	if (mode == "add")
		return add + moreAddOrReverse;
	if (mode == "reverse")
		return reverse + moreAddOrReverse;
#ifndef STARS_LANG_CHINESE
	if (mode == "settings") {
		string settings = addon +
			"default settings:\n" +
			"other settings:\n" +
			"Customize the program's behaviour in certain situations\n" +
			"Each situation represent a situation where questions might be asked, you can\n" +
			"decide whether it will be asked, and dispite whether it IS asked, the default\n" +
			"answer would be. Notice that, for example, in situation gameIsOver, if\n" +
			"askToReverse is false but defaultReverse is true, then when game is over, we will\n" +
			"went into reverse mode immediately.\n" +
			"If starsOn is true, then stars will fall down from the sky then make sure the\n" +
			"computer think fast and reckless.\n";
		return settings;
	}
#else
	if (mode == "settings") {
		string settings = addon +
			"默认设定：\n" +
			"    设定程序在特定情况下的行为，比如情况：游戏结束 下的项目：询问撤回如果为\n" +
			"真，程序就会在游戏结束时询问是否进入撤回模式，注意如果询问撤回为假而项目：默认\n" +
			"撤回为真，则程序会跳过询问直接进入撤回模式\n" +
			"    如果晴天为真，星星就会从天上落下来，进而导致程序的计算迅速而鲁莽\n";
		return settings;
	}
#endif // STARS_LANG_CHINESE
	return reverse;
}

string BoardInterface::getInfo(string input) {
	string addon = "";
#ifdef STARS_VERSION_DEBUG
	string version = STARS_VERSION_DEBUG;
#else
#ifdef STARS_VERSION_RELEASE
	string version = STARS_VERSION_RELEASE;
#endif // STARS_VERSION_RELEASE
#endif // STARS_VERSION_DEBUG
#ifndef STARS_LANG_CHINESE
	string enjoy = "Enjoy!\n";
	string end	 = "----------------------------------- The End -----------------------------------\n";
	string tips	 = addon + "Tips from CharmedPython:\n" +
		"So the trick is to build yourself as high as possible, but don't make hugh\n" +
		"chunks, leave some room between them. Here are some good examples:\n" +
		"1 2 3 4 5 6 7 8\n" + "| | | | | | | | |\n" + "| | | | | | | | |\n" +
		"| | | | | |0| | |\n" + "|0| | | | |X| | |\n" + "|X| |X|0| |X| | |\n" +
		"|0| |X|X| |X| | |\n" + "|0| |X|X| |0|X| |\n" + "|0|0|0|X|0|X|0| |\n" + "\nor\n" +
		"1 2 3 4 5 6 7 8\n" + "| | | | | | | | |\n" + "| |0| | | | | | |\n" +
		"| |0| | | | | | |\n" + "| |X| | |0| | | |\n" + "| |X| |0|X| | | |\n" +
		"| |X| |X|X| | | |\n" + "| |0| |X|X| | | |\n" + "|0|0| |X|0|X|0|0|\n" +
		"------------------------- 'Enter' for more, 0 to exit -------------------------\n";
	string moreTips = addon +
		"This type of layout will give you a lot of advantage. Trying to build any kind\n" +
		"of three piece in a row is helpful.\nGood luck!\n" + end;
	string info = addon +
		"\nA 1v1 & 8x8 command line based board game\n" +
#ifndef STARS_DEBUG_INFO
		"\n------------------------------- version " + version + " ---------------------------------\n" +
#endif
#ifdef STARS_DEBUG_INFO
		"\n---------------------------- version " + version + " -------------------------------\n" +
#endif
		"                                                                  by Duan Hanyu\n" +
		"                                                                      2020-8-25";
#else
	string enjoy = "玩的开心！\n";
	string end	 = "----------------------------------- 没了 -----------------------------------\n";
	string tips	 = addon + "从上一个python版游戏复制来的游戏:\n" +
		"这个要诀就是尽量造出连续的，对方不能下的点，可以从盖连续高楼开始\n" +
		"但中间留点空，别弄成一大堆\n" +
		"1 2 3 4 5 6 7 8\n" + "| | | | | | | | |\n" + "| | | | | | | | |\n" +
		"| | | | | |0| | |\n" + "|0| | | | |X| | |\n" + "|X| |X|0| |X| | |\n" +
		"|0| |X|X| |X| | |\n" + "|0| |X|X| |0|X| |\n" + "|0|0|0|X|0|X|0| |\n" + "\nor\n" +
		"1 2 3 4 5 6 7 8\n" + "| | | | | | | | |\n" + "| |0| | | | | | |\n" +
		"| |0| | | | | | |\n" + "| |X| | |0| | | |\n" + "| |X| |0|X| | | |\n" +
		"| |X| |X|X| | | |\n" + "| |0| |X|X| | | |\n" + "|0|0| |X|0|X|0|0|\n" +
		"------------------------- '回车'显示余下信息， 0返回 -------------------------\n";
	string moreTips = addon +
		"这样就能提升赢的概率。实际上，所有形式的三个一行都能达到提高获胜概率的效果" +
		"\n祝好运!\n" + end;
	string info = addon +
		"\n一个一对一，8x8 基于命令行的四子棋游戏\n" +
#ifndef STARS_DEBUG_INFO
		"\n------------------------------- version " + version + " ---------------------------------\n" +
#endif
#ifdef STARS_DEBUG_INFO
		"\n---------------------------- version " + version + " -------------------------------\n" +
#endif
		"                                                                      by 段晗宇\n" +
		"                                                                      2020-8-25";
#endif // STARS_LANG_CHINESE
	vector<string> story = {
		addon +
		"Out of a few random try, you finally get yourself an Easter Egg! But I'm afraid\n" +
		"this is actually wrote for myself, so it's quite boring for other players. Not\n" +
		"too late to quite now.\n" +
		"------------------------- 'Enter' for more, q to quit --------------------------\n",
		addon +
		"Anyway, I have just finished an interview when I see my roommate\n" +
		"coding. He told me it was his homework, I got interested. Then I begin to work\n"
		"on this silly thing till I found that it's taking up all of my time and I'm not\n" +
		"even satisfied? Like it's a weird addictive activity that does even provide\n" +
		"satisfaction like another other decent addictive activity will. Watching Game of\n" +
		"Thrones all day might indicate that someone's life is decaying, but at least\n" +
		"that \"feels good\". Now I'm just coding for almost no reason at all, there's\n" +
		"not much technique here, doesn't require any brain work. Brick by brick, anyone\n" +
		"can build this. I mean, what? I didn't go to school so that I can sit here and\n" +
		"type all day. And there's always new features to develop and new bugs to fix.\n" +
		"Now I really get it when my cousin say you can never finish a project, it's a\n" + "disaster.\n" +
		"------------------------- 'Enter' for more, q to quit --------------------------\n",
		addon +
		"But when I say end a project instead of stop coding, I suddenly realized that it\n" +
		"IS my fault that the project never ends 'cause I never said WHEN it ends. When?\n" +
		"Do I stop once the computer can play at least five moves? Do I stop once I've\n" +
		"developed a recursive analyse function? Do I stop once play back feature is\n" +
		"developed? Maybe I should have set a goal, a point, where once it was met, I\n" +
		"stop. Maybe I should do that whenever I start to do anything?\n" + "So... where should I stop?\n" +
		"\n" + "Well, one have to admit, typing that IS refreshing. The developer is OK now.\n" +
		"Sorry you have to see all these irrelevant crap. Have a nice day!\n" + end
};
	if (input == "t" || input == "tips") {
		cout << tips;
		char dis[8];
		int	 wrongInput = 0;
		while (true) {
			printf("> ");
			cin.getline(dis, 8);
			if (!strcmp(dis, "0") || !strcmp(dis, "exit"))
				break;
			if (!strcmp(dis, "quit") || !strcmp(dis, "q"))
				return "quit";
			else if (!strlen(dis) || !strcmp(dis, "m") || !strcmp(dis, "more")) {
				cout << moreTips;
				break;
			}
			printf("Pardon?\n");
			++wrongInput;
			if (wrongInput > 8) {
				cout << "There might be an overflow here, please don't enter "
					 << "more than 15 characters at a\ntime\n";
				cin.clear();
				wrongInput = 0;
			}
		}
		return addon;
	}
	if (input == "story") {
		char  dis[16];
		int	  wrongInput = 0;
		short i			 = 1;
		cout << story[0] << "> ";
		cin.getline(dis, 16);
		if (!strcmp(dis, "q") || !strcmp(dis, "0") || !strcmp(dis, "quit") || !strcmp(dis, "exit"))
			return addon;
		cout << story[1] << "> ";
		cin.getline(dis, 16);
		if (!strcmp(dis, "q") || !strcmp(dis, "0") || !strcmp(dis, "quit") || !strcmp(dis, "exit"))
			return addon;
		cout << story[2];
		return addon;
	}
	if (input == "a song, please") {
		string aSong = addon +
		"\n    Oh my darling,\n" +
		"    oh my darling,\n" +
		"    oh my darling Clementine\n\n" +
		"    You are lost and\n" +
		"    gone forever,\n" +
		"    dreadful sorry Clementine\n\n" +
		"    How I missed her,\n" +
		"    how I missed her,\n" +
		"    how I missed my Clementine\n\n" +
		"    Until I kissed her\n" +
		"    little sister\n" +
		"    and forgot my Clementine";
		return aSong;
	}
	return info;
}

void BoardInterface::showComment(oneMove& move) {
#ifndef STARS_LANG_CHINESE
	// comment
	if ((move.word == "good" && move.byComputer) || (move.word == "bad" && !move.byComputer))
		cout << "    not a promising future I'm afraid.\n";
	else if ((move.word == "bad" && move.byComputer) || (move.word == "good" && !move.byComputer))
		cout << "    this is going really well~\n";
	else {
		shortv non;
		analyse->state.nonFullColumn(non);
		if (move.list.size() == 1 && move.byComputer)
			cout << "    one move left, where can I go then?\n";
		else if (move.list.size() == 1 && !move.byComputer)
			cout << "    one move left, where can you go then?\n";
		else if (non.size() > 5 && move.list.size() <= non.size() / 2)
			cout << "    such broad space, yet so few of them is available.\n";
		else if (non.size() < 4)
			cout << "    not much space to chose form.\n";
	}
#else
	if ((move.word == "good" && move.byComputer) || (move.word == "bad" && !move.byComputer))
		cout << "    似乎不妙\n";
	else if ((move.word == "bad" && move.byComputer) || (move.word == "good" && !move.byComputer))
		cout << "    妙呀~\n";
	else {
		shortv non;
		analyse->state.nonFullColumn(non);
		if (move.list.size() == 1 && move.byComputer)
			cout << "    只有一步可走，我能走哪儿呢？\n";
		else if (move.list.size() == 1 && !move.byComputer)
			cout << "    只有一步可走，你能走哪儿呢？\n";
		else if (non.size() < 4)
			cout << "    剩下的空间不多了\n";
	}
#endif // STARS_LANG_CHINESE
}

bool BoardInterface::askToReverse(bool yes) {
	char input[8];
#ifndef STARS_LANG_CHINESE
	if (yes)
		printf("Care for reverse mode? (default yes) (Yes/no)> ");
	else
		printf("Care for reverse mode? (default no) (yes/No)> ");
	cin.getline(input, 8);
	if ((yes && !strlen(input)) || !strcmp(input, "Y") || !strcmp(input, "yes")
		|| !strcmp(input, "y")) {
		printf("As you wish\n");
		return true;
	}
	else
		printf("No it is.\n");
#else
	if (yes)
		printf("进入撤回模式? (默认 yes) (Yes/no)> ");
	else
		printf("进入撤回模式? (默认 no) (yes/No)> ");
	cin.getline(input, 8);
	if ((yes && !strlen(input)) || !strcmp(input, "Y") || !strcmp(input, "yes")
		|| !strcmp(input, "y")) {
		printf("如你所愿\n");
		return true;
	}
	else
		printf("罢了\n");
#endif // STARS_LANG_CHINESE
	return false;
}

void BoardInterface::askToSaveBoard(bool yes) {
	char input[8];
#ifndef STARS_LANG_CHINESE
	if (yes)
		printf("Save the old game? (yes as default) (Yes/no)> ");
	else
		printf("Save the old game? (no as default) (yes/No)> ");
#else
	if (yes)
		printf("将游戏存档? (默认 yes) (Yes/no)> ");
	else
		printf("将游戏存档? (默认 no) (yes/No)> ");
#endif // STARS_LANG_CHINESE
	cin.getline(input, 8);
	if ((yes && !strlen(input)) || !strcmp(input, "Y") || !strcmp(input, "yes")
		|| !strcmp(input, "y"))
		record.saveGame(analyse->state);
}

void BoardInterface::importNewBoard() {
#ifndef STARS_LANG_CHINESE
	printf("Paste a board down below>\n");
#else
	printf("粘贴一个棋盘>\n");
#endif // STARS_LANG_CHINESE
	if (!getStateFromInput())
		return;
	record.clearHistoryMove();
}

bool BoardInterface::isOver(const oneMove& move) {
#ifndef STARS_LANG_CHINESE
	string gameOver		   = "Game is over, stars are gone.\n";
	string congratulations = "Congratulations, you win!\n";
	string boardFull	   = "board is full, game is over, lura is gone.\n";
#else
	string gameOver		   = "游戏结束\n";
	string congratulations = "玩家获胜！\n";
	string boardFull	   = "棋盘满了\n";
#endif // STARS_LANG_CHINESE
	if (analyse->gameIsOver() == move.player) {
		analyse->show();
		if (move.byComputer)
			cout << gameOver;
		else
			cout << congratulations;
		return true;
	}
	if (analyse->gameIsOver() == analyse->rPlayer(move.player)) {
		analyse->show();
		if (move.byComputer)
			cout << congratulations;
		else
			cout << gameOver;
		return true;
	}
	if (analyse->boardIsFull()) {
		cout << boardFull;
		return true;
	}
	return false;
}

#ifdef STARS_DEBUG_INFO
int BoardInterface::autoTestMode(int startMove) {
	int errCount = 0, stepCount = 1;
	char expectedWinner = '1';
	// show time
	oneMove byOpponent, byPlayer;
	byPlayer.player	  = 'X';
	byPlayer.move	  = startMove;
	byOpponent.player = '0';

	printf("We are in auto test mode now\n");

	// main loop
	while (true) {
		// player goes
		analyse->go(byPlayer.player, byPlayer.move);
		printf("%d:'%c' goes '%d'\t", stepCount, byPlayer.player, byPlayer.move);
		if (isOver(byPlayer)) {
			if (analyse->gameIsOver() != expectedWinner)
				cout << "What? why?\n";
			printf("Exit from auto test mode ...\n");
			return errCount;
		}

		// opp respond
		try {
			byOpponent.move = analyse->respond(byOpponent.player, byOpponent,
				false, false,
				record.getDefaultSettings("inDebugMode", "starsOn"),
				false);
			
			// see if expectedWinner is changed
			if (byOpponent.word == "good" && stepCount > 3) {
				if (expectedWinner != byOpponent.player && expectedWinner != '1') {
					++errCount;
					cout << "Expected winner changed from "
						<< expectedWinner << " to "
						<< byOpponent.player << ", why?\n";
					cout << "player " << byPlayer.player << " goes "
						 << byPlayer.move << ", then opp respond with the board:\n";
					analyse->starShow();
					printf("\n\n\n\n");
					expectedWinner = byOpponent.player;
				}
				else
					expectedWinner = byOpponent.player;
			}
			else if (byOpponent.word == "bad" && stepCount > 3) {
				if (expectedWinner != byPlayer.player && expectedWinner != '1') {
					++errCount;
					cout << "Expected winner changed from "
						<< expectedWinner << " to "
						<< byPlayer.player << ", why?\n";
					cout << "player " << byPlayer.player << " goes "
						 << byPlayer.move << ", then opp respond with the board:\n";
					analyse->starShow();
					printf("\n\n\n\n");
					expectedWinner = byPlayer.player;
					//cout << "Hit anything to continue ...";
					//cin.get();
				}
				else
					expectedWinner = byPlayer.player;
			}
		}
		catch(const std::exception& e) {
			errCount++;
			std::cerr << e.what() << '\n';
			cout << "Error occurred after player '" << byPlayer.player
				 << "' goes " << byPlayer.move
				 << "\nWhen another player is responding to the following board:\n";
			analyse->starShow();

			// if in Visual Studio
			cout << "Let's try again and let VS catch the error>\n";
			byOpponent.move = analyse->respond(byOpponent.player, byOpponent,
				false, false,
				record.getDefaultSettings("inDebugMode", "starsOn"), false);
		}
#ifdef STARS_DEBUG_INFO
		if (!byOpponent.list.empty() &&
			!MyShortList::inList(byOpponent.list, byOpponent.move))
			throw logic_error("suggestion not in safe list");
#endif // STARS_DEBUG_INFO

		// change the player
		std::swap(byPlayer.player, byOpponent.player);
		std::swap(byPlayer.move, byOpponent.move);
		++stepCount;
	}
	return errCount;
}
#endif // STARS_DEBUG_INFO
