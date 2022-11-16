#pragma once
#include <algorithm>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <Windows.h>
#include <tchar.h>

namespace FileManagerProperties {

#ifdef _UNICODE
	typedef std::wstring STRING;
	typedef std::wostream& OSTREAM;
	std::wostream& COUT = std::wcout;
	std::wistream& CIN = std::wcin;
#else
	typedef std::string STRING;
	typedef std::ostream& OSTREAM;
	std::ostream& COUT = std::cout;
	std::istream& CIN = std::cin;
#endif

#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77

	namespace fs = std::filesystem;
	typedef std::filesystem::recursive_directory_iterator RecursiveIt;



	enum class Items {
		OPEN = 1, BACK, CREATE_FOLDER, CREATE_FILE,
		DELETE_, RENAME, COPY, CUT,
		INSERT, SIZE, SEARCH, EXIT
	};



	enum class State {
		DISKS = 1,
		FOLDER,
		SEARCH_ROOT,
		SEARCH_MODE_FOLDER
	};



	class Marker {
		STRING leftIndent;
		STRING rightIndent;
		STRING mainBlock;
		STRING alterBlock;
		HANDLE hConsole;
		int position;
	public:
		Marker(STRING lIndent, STRING rIndent, STRING mBlock, STRING aBlock) :
			leftIndent(lIndent), rightIndent(rIndent),
			mainBlock(mBlock), alterBlock(aBlock),
			position(1), hConsole(GetStdHandle(STD_OUTPUT_HANDLE)) {
		}

		void move(int line) { position = line; }

		void operator()(int line) {
			if (position == line) {
				SetConsoleOutputCP(866);
				SetConsoleTextAttribute(hConsole, 4);
				COUT << leftIndent << mainBlock << rightIndent;
				SetConsoleTextAttribute(hConsole, 7);
				SetConsoleOutputCP(1251);
			}
			else COUT << leftIndent << alterBlock << rightIndent;
		}

		STRING& getLeftIndent() { return leftIndent; }
		STRING& getRightIndent() { return rightIndent; }
		STRING& getMainBlock() { return mainBlock; }
		STRING& getAlterBlock() { return alterBlock; }
		int getPosition() const { return position; }
	};



	void pause(bool showMessage = true) {
		if (showMessage)
			COUT << _T("[Press any key to continue]");
		int item = _getch();
		if (item == 0 || item == 224)
			item = _getch();
	}
}