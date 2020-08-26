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
			cout << "��������̳��˵�����\n";
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
			printf("\n����ģʽ\n��������> ");
		else if (mode == "add")
			printf("\n����ģʽ\n��������-����> ");
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
				printf("�Ѿ����볷��ģʽ\n");
#endif	// STARS_LANG_CHINESE
			else
				return "reverse";
		}
		else if (!strcmp(input, "a") || !strcmp(input, "add")) {
			if (mode != "reverse")
#ifndef STARS_LANG_CHINESE
				printf("We are already in add mode\n");
#else
				printf("�Ѿ��������ģʽ\n");
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
			printf("���������ͨģʽ���������ָ����ᷢ��ʲô\n");
#endif	// STARS_LANG_CHINESE
		else if (mode != "add") {
			int num = atoi(input);
			if (num > 0 && num <= analyse->getCols() && !analyse->colIsEmpty(num))
				return input;
#ifndef STARS_LANG_CHINESE
			cout << "Invalid " << mode << " mode number input, let\'s try again\n";
#else
			cout << "��������е�С����\n";
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
		cout << "\n��ͨģʽ\n��� '" << plr << "' > ";
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
			cout << "������Ч��������\n";
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
			cout << "��������һ��\n";
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
		cout << "��������һ��\n";
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
		printf("���� '%c' �ѱ���Ը��ӵ� '%d' ��:\n", move.player, move.move);
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
		printf("%d �б�����:\n", move.move);
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
			printf("�˳���ͨģʽ ...\n");
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
				printf("��һ����� %c ��״̬�������б�ͽ�������:\n", byPlayer.player);
				cout << "���״̬ = " << toChinese(byPlayer.word) << "\n�����б� = [ ";
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
				printf("��һ��������ͨģʽ������û����ʾ\n");
				continue;
			}
			printf("��һ������ʾ��\n");
			cout << "���״̬ = " << toChinese(byPlayer.word) << "\n�����б� = [ ";
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
				printf("�˳���ͨģʽ ...\n");
#endif // STARS_LANG_CHINESE
				return "over";
			}
		}
		else if (isOver(byPlayer)) {
#ifndef STARS_LANG_CHINESE
			printf("Exit from debug mode ...\n");
#else
			printf("�˳���ͨģʽ ...\n");
#endif // STARS_LANG_CHINESE
			return "over";
		}

		// opp respond
		if (record.getDefaultSettings("inDebugMode", "showCalculate") &&
			record.getDefaultSettings("inDebugMode", "showTime"))
#ifndef STARS_LANG_CHINESE
			printf("Info for the computer:\n");
#else
			printf("������Ϣ��\n");
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
			cout << "    ������ʱ��" << byPlayer.time << " ����\n";
		printf("    %c ������һ����%d\n", byOpponent.player, byOpponent.move);
		if (isOver(byOpponent)) {
			printf("�˳���ͨģʽ ...\n");
			return "over";
		}
		
		byPlayer.hintOn	= record.getDefaultSettings("inDebugMode", "hintOn");
		if (byPlayer.hintOn)
			cout << "\n����ҵ���ʾ��Ϣ\n";
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
			printf("    �Ƽ� %c ����һ����%d\n", byPlayer.player, byPlayer.suggestion);
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
	cout << "һ����" << record.getDefaultSettingsItemNum() << "�����������"
		 << "ÿ������ж����Ŀ��������Ŀ����������ĸΪ��ǣ�������\n"
		 << "���ı���Ӧ���趨ֵ����0�˳�������S��show��ʾ��ǰ���ã�����h��ð���\n";
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
	cout << "������ڴ˴��ı�����������ʱ�䣬��ǰ������ʱ����\n"
		<< analyse->maxcaltime
		<< "����\nֱ�ӻس�������0���أ�q�˳���Ϸ\n";
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
	printf("����ģʽ\n");
	char input[8];
	printf("1. Ĭ������\n2. ��������\n");
	do {
		printf("(��������)> ");
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
			printf("����������һ��\n");
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
	printf("�����Ƕ���ģʽ\n");
	printf("�� %d �ݴ浵\n", record.getNumberOfSavedBoard());
	printf("�س������һ��������������ֱ�Ӷ�ȡ�ô浵������c��ȡ��ǰ��ʾ�Ĵ浵\n");
	printf("ʹ�� d/rm/delete/remove ɾ����ǰ�浵��0�˳�\n");
	printf("̽�����!\n");
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
	printf("����һ������Ϸ��\n");
#endif // STARS_LANG_CHINESE

	if(!interface.controlMode())
		return "quit";
#ifndef STARS_LANG_CHINESE
	printf("Exit from play mode ...\n");
#else
	printf("�˳�����ģʽ ...\n");
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
		printf("��û���κδ浵���������ʹ�ô�ģʽ�����ȱ�����Ϸ");
		printf("�˳������ط�ģʽ ...\n");
		return "debug";
	}
	printf("�����ط�ģʽ\n");
	printf("�� %d �ݴ浵\n", record.getNumberOfSavedBoard());
	printf("�س������һ��������������ֱ�Ӷ�ȡ�ô浵������c��ȡ��ǰ��ʾ�Ĵ浵\n");
	printf("ʹ�� d/rm/delete/remove ɾ����ǰ�浵��0�˳�\n");
	printf("̽�����!\n");
	Json::Value game;
	string result = record.showSavedGames(game);
	if (result == "exit") {
		printf("�˳������ط�ģʽ ...\n");
		return "debug";
	}
	if (result == "quit") {
		printf("�˳������ط�ģʽ ...\n");
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
	printf("���Ǵ浵��Ϸ�Ļطţ�\n");
	printf("ʹ��c��ϻطŲ�������Ϸģʽ��b�ص���һ����h������0�˳�\n");
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
		printf("�˲�/�ܲ��� %zu/%zu\n", tempRecord.historyMove.size() + 1, newRecord.historyMove.size());
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
			printf("�����µ���Ϸģʽ\n");
			if(!interface.controlMode())
				return "quit";
			printf("�ص������ط�ģʽ\n");
#endif // STARS_LANG_CHINESE
		}
		else if (!strcmp(input, "b") || !strcmp(input, "back") || !strcmp(input, "go back")) {
			if (tempRecord.historyMove.empty())
#ifndef STARS_LANG_CHINESE
				printf("This is the beginning, there is no going back.\n");
#else
				printf("�Ѿ��ص���ͷ��û����һ����\n");
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
			printf("ʹ��c��ϻطŲ�������Ϸģʽ��b�ص���һ����h������0�˳�\n");
#endif // STARS_LANG_CHINESE
		++iter;
	}
#ifndef STARS_LANG_CHINESE
	printf("Exit from play back mode ...\n");
#else
	printf("�˳������ط�ģʽ ...\n");
#endif // STARS_LANG_CHINESE
	return "debug";
}

string BoardInterface::customMode() {
#ifndef STARS_LANG_CHINESE
	printf("In custom mode ...\n");
	printf("Note that the more width you set, the clumsier the computer will be\n");
	printf("It is recommended to set the width lesser than 16\n");
#else
	printf("�Զ���ģʽ ...\n");
	printf("ע�����õĿ��Խ�󣬳���ļ�������Խ�󣬿��ܻᵼ�³�����������Ƽ�\n");
	printf("��������õ�16����\n");
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
	printf("�˳��Զ���ģʽ ...\n");
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
	cout << "�˳�����ģʽ ...\n";
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
	cout << "����Ϊ�Լ������� " << routeBranches
		 << "��·���������� " << routes.getBranches(-2)
		 << " ������ " << routes.getBranches(-1)
		 << " ���ú� " << routes.getBranches(0)
		 << " ����\n";
	cout << "ͨ��������Ӧ��ĸ��������չʾ��\na. ���ɵ�·��\nb. �õ�·��\nc. ����·��\nd. ����·��\n";
	if (next.size() == 1 && next[0]->next.empty() &&
		(next[0]->data == routes.goodNode || next[0]->data == routes.badNode ||
		next[0]->data == routes.freeNode))
		nearFlag = true; // next is a flag
	else {
		cout << "����������Ӧ����ѡ���������·�����鿴��·���µ����ֿ���\n[ ";
		vRi iter = next.begin();
		for (; iter != next.end(); ++iter)
			cout << (*iter)->data << " ";
		cout << "]\n";
	}
	if (routes.crnt->prev)
		cout << "����B��back������һ��·��\n";
	string in;
	string tryAgain = "û�����ѡ���������һ��";
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
				// ��ʱ�����ϵݹ��
				return showRoutesMode();
			}
#ifndef STARS_LANG_CHINESE
			cout << "There's no going back, let's try again.\n";
#else
			cout << "û�л�ͷ·�ˣ���������һ��\n";
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
			// �ϵݹ����
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
		cout << "һ���� " << routeBranches
			<< " ��·����ȷ��Ҫȫ�������\n"
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
	string enjoy  = "��Ŀ���!\n";
	string addon  = "";
	string end	  = "------------------------------------ û�� ----------------------------------\n";
	string enterForMore =
		"------------------------- '�س�'��ʾ������Ϣ�� 0���� -------------------------\n";
	string add = addon + "��������(X��0)��������ָ���������ӵ������λ�á����磺����\n" +
		"X 3\n���ǰ�����X���ڵ����У�Ҳ��������'x-7'��ֻ��Ҫ��֤���Ӻ�����֮������ֻ��һ���ַ�����\n" +
		"��������ֱ�ӻس��ص����Ƴ����ģʽ\n";
	string reverse = "�����������ظ���������ӣ����س��˳�\n";
	string moreAddOrReverse =
		addon + "����㲻֪��������ʲô����ûص���ͨģʽ������İ����ĵ�\n" + enjoy + end;
	string normal =
		addon + "�����0�������壬��������һ�������ʤ��ע������ֻ�ܴ������ϵ�����\n\n" +
		"ѡ��               Ч��\n" +
		"0/exit             �˳�����ģʽ\n" +
		"q/quit             �˳���Ϸ\n" +
		"C/custom           �Զ������̳���ͻ�ʤ������С������������Ĭ��4��\n" +
		"h/help             ��ʾ��ǰģʽ�����ĵ�\n" +
		"p/play             �������ģʽ\n" +
		"P/play back        ��������ط�ģʽ\n" +
		"S/show             ��ʾ��ǰ����\n" +
		"s/settings         ��������ģʽ\n" +
		"st/show stars      ��ʾ����\n" +
		"sv/save            �浵\n" +
		"sr/show routes     ��ʾ���Լ������·��\n" +
		"t/tips             ��ʾ��Ϸ�淨��ʾ\n" +
		"w/winn             ��ʾ��ʤ������С����������\n" +
		"i/info             ��ʾ��Ϸ��Ϣ\n\n" + enterForMore;
	vector<string> moreDebug = {
		addon +
			"    �������ʾ��hintOn��Ϊ�棬��ô��������ʾ���״̬Ϊ��ʱ��������ѡ����\n" +
			"ѭ�Ƽ��б��е��·�����ô��ҽ��ڼ���֮�ڻ�ʤ����֮�����ʾ���״̬Ϊ��\n" +
			"����Խ��ڼ���֮�ڻ�ʤ\n" +
			"    ������������е����죨starsOn��Ϊ�棬��ô�����д��15%���ʼ������ ;-)\n" + enterForMore,
		addon + "    �����״̬��ʾΪ����ʱ���Ƽ��б��е��·�һ���������£��Ƽ��б�֮��\n" +
			"�Ͳ���˵�ˣ���ò�Ҫ�¡�\n" +
			"    ע�������ǵĵط������ǿ������ģ����Բ����Ƽ������Ⲣ��˵���Ƕ��Ǻû��߻�\n" +
			"ֻ��˵���Ƕ�Զ\n" + enterForMore,
		addon +
			"a/add        �������ģʽ\n" +
			"c/change     �˻������ݻ���\n" +
			"H/hint       ��ʾ��һ������ʾ\n"
			"I/import     ���������̣�ע���¾����̵Ĵ�С������ͬ\n" +
			"m/move       ʹ�����������ӣ��Ͳ��������ˣ�\n" +
			"r/reverse    ���볷��ģʽ\n" +
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
				cout << "ɶ?\n";
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
			"Ĭ���趨��\n" +
			"    �趨�������ض�����µ���Ϊ�������������Ϸ���� �µ���Ŀ��ѯ�ʳ������Ϊ\n" +
			"�棬����ͻ�����Ϸ����ʱѯ���Ƿ���볷��ģʽ��ע�����ѯ�ʳ���Ϊ�ٶ���Ŀ��Ĭ��\n" +
			"����Ϊ�棬����������ѯ��ֱ�ӽ��볷��ģʽ\n" +
			"    �������Ϊ�棬���Ǿͻ���������������������³���ļ���Ѹ�ٶ�³ç\n";
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
	string enjoy = "��Ŀ��ģ�\n";
	string end	 = "----------------------------------- û�� -----------------------------------\n";
	string tips	 = addon + "����һ��python����Ϸ����������Ϸ:\n" +
		"���Ҫ�����Ǿ�����������ģ��Է������µĵ㣬���ԴӸ�������¥��ʼ\n" +
		"���м�����գ���Ū��һ���\n" +
		"1 2 3 4 5 6 7 8\n" + "| | | | | | | | |\n" + "| | | | | | | | |\n" +
		"| | | | | |0| | |\n" + "|0| | | | |X| | |\n" + "|X| |X|0| |X| | |\n" +
		"|0| |X|X| |X| | |\n" + "|0| |X|X| |0|X| |\n" + "|0|0|0|X|0|X|0| |\n" + "\nor\n" +
		"1 2 3 4 5 6 7 8\n" + "| | | | | | | | |\n" + "| |0| | | | | | |\n" +
		"| |0| | | | | | |\n" + "| |X| | |0| | | |\n" + "| |X| |0|X| | | |\n" +
		"| |X| |X|X| | | |\n" + "| |0| |X|X| | | |\n" + "|0|0| |X|0|X|0|0|\n" +
		"------------------------- '�س�'��ʾ������Ϣ�� 0���� -------------------------\n";
	string moreTips = addon +
		"������������Ӯ�ĸ��ʡ�ʵ���ϣ�������ʽ������һ�ж��ܴﵽ��߻�ʤ���ʵ�Ч��" +
		"\nף����!\n" + end;
	string info = addon +
		"\nһ��һ��һ��8x8 ���������е���������Ϸ\n" +
#ifndef STARS_DEBUG_INFO
		"\n------------------------------- version " + version + " ---------------------------------\n" +
#endif
#ifdef STARS_DEBUG_INFO
		"\n---------------------------- version " + version + " -------------------------------\n" +
#endif
		"                                                                      by ������\n" +
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
		cout << "    �ƺ�����\n";
	else if ((move.word == "bad" && move.byComputer) || (move.word == "good" && !move.byComputer))
		cout << "    ��ѽ~\n";
	else {
		shortv non;
		analyse->state.nonFullColumn(non);
		if (move.list.size() == 1 && move.byComputer)
			cout << "    ֻ��һ�����ߣ��������Ķ��أ�\n";
		else if (move.list.size() == 1 && !move.byComputer)
			cout << "    ֻ��һ�����ߣ��������Ķ��أ�\n";
		else if (non.size() < 4)
			cout << "    ʣ�µĿռ䲻����\n";
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
		printf("���볷��ģʽ? (Ĭ�� yes) (Yes/no)> ");
	else
		printf("���볷��ģʽ? (Ĭ�� no) (yes/No)> ");
	cin.getline(input, 8);
	if ((yes && !strlen(input)) || !strcmp(input, "Y") || !strcmp(input, "yes")
		|| !strcmp(input, "y")) {
		printf("������Ը\n");
		return true;
	}
	else
		printf("����\n");
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
		printf("����Ϸ�浵? (Ĭ�� yes) (Yes/no)> ");
	else
		printf("����Ϸ�浵? (Ĭ�� no) (yes/No)> ");
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
	printf("ճ��һ������>\n");
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
	string gameOver		   = "��Ϸ����\n";
	string congratulations = "��һ�ʤ��\n";
	string boardFull	   = "��������\n";
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
