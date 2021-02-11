#include <iostream>
#include <algorithm>
#include "boardState.h"

using std::cout;

int BoardState::removeNumber;
int BoardState::addNumber;


oneMove::oneMove(const Json::Value& root)
	: mode(std::move(root["mode"].asString())),
	  word(std::move(root["word"].asString())),
	  list(std::move(MyJson::toVectorInt(root["list"]))),
	  time(root["time"].asDouble()),
	  move(root["move"].asInt()),
	  suggestion(root["suggestion"].asInt()),
	  player(root["player"].asInt()),
	  byComputer(root["byComputer"].asBool()),
	  hintOn(root["hintOn"].asBool()) {}

oneMove::operator Json::Value() {
	Json::Value root;
	root["byComputer"] = byComputer;
	root["hintOn"]     = hintOn;
	root["list"]       = MyJson::toValue(list);
	root["mode"]       = mode;
	root["move"]       = move;
	root["player"]     = player;
	root["suggestion"] = suggestion;
	root["time"]       = time;
	root["word"]       = word;
	return root;
}

BoardState::BoardState(const Json::Value& root)
	: board(vector<string>(root["column"].asInt())),
	  top(vector<int>(board.size())),
	  starArea(vector<int>(board.size())),
	  rows(root["row"].asInt()),
	  cols(board.size()),
	  winn(root["winn"].asInt()) {
	for (int i = 0; i < cols; ++i) {
		board[i] = std::move(root["board"][i].asString());
		top[i]   = root["top"][i].asInt();
	}
}

Json::Value BoardState::boardToJson() {
	Json::Value root;
	for (const string& str : board)
		root.append(str);
	return root;
}

Json::Value BoardState::topToJson() {
	Json::Value root;
	for (const int i : top)
		root.append(i);
	return root;
}

void BoardState::show() const {
	printHead();
	int i = 1;
	for (int i = rows - 1; ~i; --i) {
		for (int j = 0; j < cols; ++j)
			cout << '|' << board[j][i];
		cout << "|\n";
	}
}

void BoardState::printHead() const {
	for (int i = 1; i <= cols; ++i) {
		if (i <= 10)
			cout << ' ';
		cout << i;
	}
	cout << '\n';
}

bool BoardState::colCanAdd(const int col) {
	return col > 0 && col <= cols && top[col - 1] != rows;
}

bool BoardState::colCanRemove(const int col) {
	return col > 0 && col <= cols && top[col - 1] != 0;
}

char BoardState::getTopPiece(const int col) {
	if (!colCanRemove(col))
		throw std::logic_error("getTopPiece: call this on an empty column");
	return board[col - 1][top[col - 1] - 1];
}

bool BoardState::boardIsFull() {
	for (const int i : top)
		if (i != rows)
			return false;
	return true;
}

char BoardState::isOver() {
	for (int i = 0; i < cols; ++i)
		for (int j = 0; j < top[i]; ++j)
			if (winPieceNearBy(i, j))
				return board[i][j];
	return 'N';
}

void BoardState::nonFullColumn(shortv& nonFull) {
	nonFull.clear();
	for (int i = 0; i < (int)top.size(); ++i)
		if (top[i] != starArea[i])
			nonFull.push_back(i + 1);
}

void BoardState::sweepFullColumn(shortv& nonFull, int col) {
	if (top[col - 1] == starArea[col - 1])
		nonFull.erase(std::find(nonFull.begin(), nonFull.end(), col));
	shortv realNonFull;
	nonFullColumn(realNonFull);
#ifdef STARS_DEBUG_INFO
	if (nonFull != realNonFull)
		throw logic_error("this non-full and the real non-full didn't match");
#endif
}

int BoardState::pieceCount() {
	int count = 0;
	for (const int i : top)
		count += i;
	return count;
}

int BoardState::randomMove() {
	shortv list;
	nonFullColumn(list);
#ifdef STARS_DEBUG_INFO
	if (list.empty())
		throw logic_error("trying randomMove() in full board");
#endif // STARS_DEBUG_INFO
	return randomMove(list);
}

int BoardState::randomMove(shortv& list) {
#ifdef STARS_DEBUG_INFO
	if (list.empty())
		throw logic_error("trying randomMove(shortv& list) in an empty list");
#endif // STARS_DEBUG_INFO
	srand((unsigned)time(NULL));
	return list[rand() % list.size()];
}

int BoardState::randomSuggestion(
	const char plr, shortv& list, const string& mode) {
	shortv opp2, opp1, plr2, plr1;
	char   opp = rPlayer(plr);
	srand((unsigned)time(NULL));
	for (const int col : list) {
		if (colIsFull(col) || colIsEmpty(col))
			continue;
		string& line = board[col - 1];
		int     head = top[col - 1] - 1;
		if (line[head] == plr) {
			if (head > 0 && line[head - 1] == plr)
				plr2.push_back(col);
			else
				plr1.push_back(col);
		}
		else {
			if (head > 0 && line[head - 1] == opp)
				opp2.push_back(col);
			else
				opp1.push_back(col);
		}
	}
	int ran = rand() % 100;
	if (mode == "progressive") {
		if (!plr2.empty() && ran < 72)
			return randomMove(plr2);
		ran = rand() % 100;
		if (!opp2.empty() && ran < 95)
			return randomMove(opp2);
		ran = rand() % 100;
		if (!plr1.empty() && ran < 95)
			return randomMove(plr1);
		ran = rand() % 100;
		if (!opp1.empty() && ran < 57)
			return randomMove(opp1);
	}
	else if (mode == "defensive") {
		if (!opp2.empty() && ran < 95)
			return randomMove(opp2);
		ran = rand() % 100;
		if (!plr2.empty() && ran < 72)
			return randomMove(plr2);
		ran = rand() % 100;
		if (!opp1.empty() && ran < 77)
			return randomMove(opp1);
		ran = rand() % 100;
		if (!plr1.empty() && ran < 65)
			return randomMove(plr1);
	}
#ifdef STARS_DEBUG_INFO
	else
		throw logic_error("no such mode");
	if (list.empty())
		throw logic_error("call randomSuggestion with empty list");
#endif // STARS_DEBUG_INFO
	return randomMove(list);
}

int BoardState::randomSuggestion(
	const char plr, shortv& list, shortv oppList, const string& mode) {
#ifdef STARS_DEBUG_INFO
	if (list.empty())
		throw logic_error("call randomSuggestion(4 args) with empty list");
#endif
	shortv intersectionList;
	// preference No.1: take what can bring me winn-1 in a row, and what can
	// interrupt opponent's three in a row
	// but some times it block itself which is rather stupid
	shortv plrTList = makeThreeCols(plr, list),
		   oppTList = makeThreeCols(rPlayer(plr), oppList);
	srand((unsigned)time(NULL));
	if (rand() % 100 < 85) {
		MyShortList::shortIntersection(intersectionList, plrTList, oppTList);
		if (intersectionList.empty()) {
			if (plrTList.empty()) {
				MyShortList::shortIntersection(
					intersectionList, list, oppTList);
				if (!intersectionList.empty())
					return randomSuggestion(plr, intersectionList, mode);
			}
			else
				return randomSuggestion(plr, plrTList, mode);
		}
		else
			return randomSuggestion(plr, intersectionList, mode);
	}

	// else if everything is empty
	// preference No.2: take the opponent's safe list
	MyShortList::shortIntersection(intersectionList, list, oppList);
	if (intersectionList.empty())
		return randomSuggestion(plr, list, mode);
	return randomSuggestion(plr, intersectionList, mode);
}

bool BoardState::valid() {
	for (int i = 0; i < cols; ++i) {
		int j = 0;
		while (j < rows && (board[i][j] == 'X' || board[i][j] == '0'))
			++j;
		if (top[i] != j)
			return false;
	}
	return true;
}

bool BoardState::winPieceNearBy(const int col, const int ro) {
	// grow up, right, upright, downright
	int  i       = 1;
	char present = board[col][ro];
	bool canUp = ro <= top[col] - winn, canRight = col <= cols - winn,
		 canDown = ro >= winn - 1;
	if (canRight) {
		// right
		for (i = 1; i < winn; ++i)
			if (board[col + i][ro] != present)
				break;
		if (i == winn)
			return true;
		// up & right
		for (i = 1; i < winn; ++i)
			if (board[col + i][ro + i] != present)
				break;
		if (i == winn)
			return true;
		if (canDown) {
			// down & right
			for (i = 1; i < winn; ++i)
				if (board[col + i][ro - i] != present)
					break;
			if (i == winn)
				return true;
		}
	}
	if (canUp) {
		// up
		for (i = 1; i < winn; ++i)
			if (board[col][ro + i] != present)
				return false;
		return true;
	}
	return false;
}

bool BoardState::winPieceButOne(const int col, const int ro, const int win) {
	// grow right, upright, downright
	int  i         = 1;
	bool butOneMet = false;
	char present   = board[col][ro];
	bool canUp = ro <= top[col] - win, canRight = col <= cols - win,
		 canDown = ro >= win - 1;
	if (!canRight)
		return false;
	// right
	for (i = 1; i < win; ++i)
		if (board[col + i][ro] != present && butOneMet)
			break;
		else if (board[col + i][ro] == ' ')
			butOneMet = true;
		else
			break;
	if (i == win)
		return true;
	if (canDown) {
		// down & right
		for (butOneMet = false, i = 1; i < win; ++i)
			if (board[col + i][ro - i] != present && butOneMet)
				break;
			else if (board[col + i][ro - i] == ' ')
				butOneMet = true;
			else
				break;
		if (i == win)
			return true;
	}
	// up & right
	for (butOneMet = false, i = 1; i < win; ++i)
		if (board[col + i][ro + i] != present && butOneMet)
			return false;
		else if (board[col + i][ro + i] == ' ')
			butOneMet = true;
		else
			return false;
	if (i == win)
		return true;
	return false;
}

void BoardState::customBoard(const int cl, const int ro, const int wi) {
	cols     = cl;
	rows     = ro;
	winn     = wi;
	board    = std::move(vector<string>(cols, string(rows, ' ')));
	top      = std::move(vector<int>(cols, 0));
	starArea = std::move(vector<int>(cols, rows));
}

void BoardState::areaTopTransform() {
	for (int i = 0; i < cols; ++i)
		starArea[i] = 0;
	for (int i = 0; i < cols; ++i)
		setATopWithTop(i, top[i]);

	// check for bulks of piece
	for (int i = 1; i < cols; ++i) {
		if (top[i] && top[i - 1]) {
			setATopWithNumber(i + 2, 1);
			setATopWithNumber(i - 3, 1);
		}
	}

	// then check for overflow
	for (int& i : starArea)
		if (i > rows)
			i = rows;

	// check if stars are even
	int stars = starNumber();
	if (stars % 2 == 0)
		return;
	// randomly chose a non-full column and add it
	shortv starNotZero, starNotFull, inter;
	for (int i = 0; i < cols; ++i) {
		// not full of stars(maybe doesn't have stars)
		if (starArea[i])
			starNotFull.push_back(i + 1);
		// have stars but may be full of stars
		if (rows != starArea[i])
			starNotZero.push_back(i + 1);
	}
	MyShortList::shortIntersection(inter, starNotFull, starNotZero);
	if (inter.empty())
		++starArea[randomMove(starNotZero) - 1];
	else
		++starArea[randomMove(inter) - 1];
}

void BoardState::areaTopRestore() {
	for (int& i : starArea)
		i = rows;
}

void BoardState::starShow() {
	areaTopTransform();
	printHead();

	for (int i = rows - 1; ~i; --i) {
		for (int j = 0; j < cols; ++j) {
			if (i < starArea[j])
				cout << '|' << board[j][i];
			else
				cout << "|+";
		}
		cout << "|\n";
	}
	areaTopRestore();
}

void BoardState::setATopWithTop(const int i, const int t) {
	// important numbers here!
	if (t == 0)
		return;
	setATopWithNumber(i - 2, t / 2 + 1);
	setATopWithNumber(i - 1, t + 1 > 4 ? t + 1 : 4);
	setATopWithNumber(i, t + 2);
	setATopWithNumber(i + 1, t + 1 > 4 ? t + 1 : 4);
	setATopWithNumber(i + 2, t / 2 + 1);
}

void BoardState::setATopWithNumber(const int i, const int n)  {
	if (i >= 0 && i < cols && starArea[i] < n)
		starArea[i] = n;
}

shortv BoardState::aTopFullColumn() {
	areaTopTransform();
	shortv list;
	for (int i = 0; i < cols; ++i)
		if (starArea[i] == 0)
			list.push_back(i + 1);
	return list;
}

int BoardState::starNumber() {
	int sum = 0;
	for (int i = 0; i < cols; ++i)
		sum += rows - starArea[i];
	return sum;
}

// count how many three in a row or winn but one in a row is there in the board,
// just like isOver with the winn set to winn - 1
int BoardState::threeRowCount(const char plr, shortv& safeList) {
	int rax = 0;
	winn--;
	for (int i = 0; i < cols; ++i)
		for (int j = 0; j < top[i]; ++j)
			if (specialPiece(i, j) || winPieceButOne(i, j, winn + 1))
				++rax;
	winn++;
	return rax;
}

shortv BoardState::makeThreeCols(const char plr, shortv& safeList) {
	shortv rax;
	int    counter;
	for (int i : safeList) {
		counter = threeRowCount(plr, safeList);
		add(plr, i);
		counter -= threeRowCount(plr, safeList);
		remove(i);
		if (counter < 0)
			rax.push_back(i);
	}
	return rax;
}

bool BoardState::specialPiece(const int col, const int ro) {
	// grow up, right, upright, downright
	int  i       = 1;
	char present = board[col][ro];
	bool canUp = ro <= top[col] - winn, canRight = col <= cols - winn,
		 canDown = ro >= winn - 1;
	if (canRight) {
		// right
		for (i = 1; i < winn; ++i)
			if (board[col + i][ro] != present)
				break;
		if (i == winn && ((col + i < cols && board[col + i][ro] == ' ') ||
						  (col > 0 && board[col - 1][ro] == ' ')))
			return true;
		// up & right
		for (i = 1; i < winn; ++i)
			if (board[col + i][ro + i] != present)
				break;
		if (i == winn && ((col + i < cols && ro + i < rows &&
						   board[col + i][ro + i] == ' ') ||
						  (col > 0 && ro > 0 && board[col - 1][ro - 1] == ' ')))
			return true;
		if (canDown) {
			// down & right
			for (i = 1; i < winn; ++i)
				if (board[col + i][ro - i] != present)
					break;
			if (i == winn &&
				((col + i < cols && ro - i >= 0 &&
				  board[col + i][ro - i] == ' ') ||
				 (col > 0 && ro + 1 <= rows && board[col - 1][ro + 1] == ' ')))
				return true;
		}
	}
	if (canUp) {
		// up
		for (i = 1; i < winn; ++i)
			if (board[col][ro + i] != present)
				return false;
		if ((ro > 0 && board[col][ro - 1] == ' ') ||
			(ro + i < rows && board[col][ro + i] == ' '))
			return true;
	}
	return false;
}

void BoardState::refreshTop() noexcept {
	top = vector<int>(cols, 0);
	for (int i = 0; i < cols; ++i)
		top[i] = find(board[i].begin(), board[i].end(), ' ') - board[i].begin();
}

void BoardState::retInit(const vector<oneMove>& history) {
	// return the state to the 'initial state' accroding to the history move
	for (auto riter = history.rbegin(); riter != history.rend(); ++riter) {
		const string& currentMode = riter->mode;
		if (currentMode == "debug" || currentMode == "add" ||
			currentMode == "normal" || currentMode == "test-debug" ||
			currentMode == "test-play")
			remove(riter->move);
		else if (currentMode == "reverse")
			add(riter->player, riter->move);
#ifdef STARS_DEBUG_INFO
		else
			throw logic_error(
				"unexpected, unhandled mode in retInit in boardState.cpp");
#else
		else
			remove(riter->move);
#endif // STARS_DEBUG_INFO
	}
}
