#include "Cursor.h"
#include "ContentSubClasses.h"

namespace FileManagerProperties {

	typedef std::unique_ptr<Content> Ptr;

	class FileManager {
		fs::path currentPath;
		fs::path searchPath;
		fs::path searchRoot;
		std::vector<bool> access;
		State state;
		Ptr content;
		Marker marker;
		Cursor cursor;
		HANDLE hConsole;
		STRING buffer;
		STRING mask;
		Options options;
		bool isExit = false;
		int depth = 0;
		int searchModeDepth = 0;

	public:
		FileManager() :
			state(State::DISKS),
			content(new Disks(currentPath.string())),
			marker(_T(""), _T(""), STRING(1, TCHAR(186)), _T(" ")),
			cursor(1, content->getCount(), 1, 12),
			hConsole(GetStdHandle(STD_OUTPUT_HANDLE)) {

			SetConsoleCP(1251);
			SetConsoleOutputCP(1251);
		}

		void startProgram() {
			while (!isExit) {
				setAccess();
				moveMarkers();
				showMenu();
				react();
			}
		}

	private:
		void moveMarkers() {
			if (cursor.isOptionsMenu())
				marker.move(cursor.vPos());
			else {
				marker.move(cursor.hPos());
				content->moveMarker(cursor.vPos());
			}
		}

		void showMenu() {
			try {
				if (cursor.isOptionsMenu()) {
					cursor.setVerticalBounds(1, 6);
					optionsMenu();
				}
				else {
					cursor.setBounds(1, content->getCount(), 1, 12);
					buttons();
					content->show(options);
					if (content->getCount() == 0) {
						marker.move(1);
						marker(1);
						COUT << _T("[Empty]") << std::endl;
					}
				}
			}
			catch (std::exception& exc) {
				system("cls");
				COUT << _T("Menu display error") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
				if (!cursor.isOptionsMenu())
					reset();
			}
		}

		void react() {
			try {
				cursor(options);
				if (cursor.isOptionsMenu())
					optionsSwitch();
				else
					mainSwitch();
			}
			catch (std::exception& exc) {
				system("cls");
				COUT << _T("Reaction process error") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
				if (!cursor.isOptionsMenu())
					reset();
			}
		}

		void reset() {
			currentPath.clear();
			content = Ptr(new Disks(currentPath.string()));
			cursor.setBounds(1, content->getCount(), 1, 12);
			cursor.resetVPos();
			depth = 0;
		}

		void setAccess() {
			if (!options.isSearchMode() && depth == 0)
				access = { 1,0,0,0,0,0,0,0,0,1,1,1 };
			else if (!options.isSearchMode() && currentPath == _T("C:\\"))
				access = { 1,1,1,0,1,1,1,1,1,1,1,1 };
			else if (options.isSearchMode() && searchModeDepth == 0)
				access = { 1,1,0,0,1,1,1,1,0,1,1,1 };
			else
				access = { 1,1,1,1,1,1,1,1,1,1,1,1 };

			if (!options.isSearchMode() && depth == 0)
				state = State::DISKS;
			else if (!options.isSearchMode())
				state = State::FOLDER;
			else if (searchModeDepth == 0)
				state = State::SEARCH_ROOT;
			else
				state = State::SEARCH_MODE_FOLDER;
		}

		void buttons() {
			system("cls");
			COUT << _T("----------------------------------------------------") << std::endl;
			if (cursor.isShowControls()) {
				COUT << _T(" Press [C] to hide the controls") << std::endl;
				COUT << _T("----------------------------------------------------") << std::endl;
				COUT << _T(" Use [Arrows] and [Enter] to navigate") << std::endl;
				COUT << _T(" Use [O] to open [Options]") << std::endl;
				COUT << _T(" Use [A] to turn on/off [Alternative step mode]") << std::endl;
			}
			else
				COUT << _T(" Press [C] to show the controls") << std::endl;
			COUT << _T("----------------------------------------------------") << std::endl;
			marker(1);    print(_T("Open"), 1);           marker(1);         COUT << _T("     ");
			marker(2);    print(_T("Back"), 2);           marker(2);         COUT << _T("     ");
			marker(3);    print(_T("Create folder"), 3);  marker(3);         COUT << _T("   ");
			marker(4);    print(_T("Create file"), 4);    marker(4);         COUT << std::endl;
			marker(5);    print(_T("Delete"), 5);         marker(5);         COUT << _T("   ");
			marker(6);    print(_T("Rename"), 6);         marker(6);         COUT << _T("   ");
			marker(7);    print(_T("Copy"), 7);           marker(7);         COUT << _T("            ");
			marker(8);    print(_T("Cut"), 8);            marker(8);         COUT << std::endl;
			marker(9);    print(_T("Insert"), 9);         marker(9);         COUT << _T("   ");
			marker(10);   print(_T("Size"), 10);          marker(10);        COUT << _T("     ");
			marker(11);   print(_T("Search"), 11);        marker(11);        COUT << _T("          ");
			marker(12);   print(_T("Exit"), 12);          marker(12);        COUT << std::endl;
			COUT << _T("----------------------------------------------------") << std::endl;
			if (!options.isSearchMode()) {
				COUT << _T(" ") << currentPath.string() << std::endl;
				COUT << _T("----------------------------------------------------") << std::endl;
			}
			else if (options.isSearchMode() && searchModeDepth != 0) {
				COUT << _T(" ") << searchPath.string() << std::endl;
				COUT << _T("----------------------------------------------------") << std::endl;
			}
		}

		void print(const TCHAR* str, short number) {
			if (!access[number - 1])
				SetConsoleTextAttribute(hConsole, 8);
			COUT << _T(str);
			if (!access[number - 1])
				SetConsoleTextAttribute(hConsole, 7);
		}

		void mainSwitch() {
			if (cursor.isPressed()) {
				switch (Items(cursor.hPos())) {
				case Items::OPEN:
					tryToOpen();
					break;
				case Items::BACK:
					tryToMoveBack();
					break;
				case Items::CREATE_FOLDER:
					tryToCreateFolder();
					break;
				case Items::CREATE_FILE:
					tryToCreateFile();
					break;
				case Items::DELETE_:
					tryToDelete();
					break;
				case Items::RENAME:
					tryToRename();
					break;
				case Items::COPY:
					tryToCopy();
					break;
				case Items::CUT:
					tryToCut();
					break;
				case Items::INSERT:
					tryToInsert();
					break;
				case Items::SIZE:
					tryToShowSize();
					break;
				case Items::SEARCH:
					tryToSearch();
					break;
				case Items::EXIT:
					exit();
					break;
				default:
					break;
				}
				cursor.release();
			}
		}

		void optionsMenu() {
			system("cls");
			COUT << _T("-----------------------------------------------------") << std::endl;
			if (cursor.isShowControls()) {
				COUT << _T(" Press [C] to hide the controls and the descriptions") << std::endl;
				COUT << _T("-----------------------------------------------------") << std::endl;
				COUT << _T(" Use [Arrows] and [Enter] to navigate") << std::endl;
				COUT << _T(" Use [O] to open [Options]") << std::endl;
				COUT << _T(" Use [A] to turn on/off [Alternative step mode]") << std::endl;
			}
			else
				COUT << _T(" Press [C] to show the controls and the descriptions") << std::endl;
			COUT << _T("-----------------------------------------------------") << std::endl;
			marker(1);
			COUT << _T("Number of items:   ");
			colorize(std::to_string(options.getItems()));
			COUT << std::endl;
			if (cursor.isShowControls()) {
				SetConsoleTextAttribute(hConsole, 8);
				COUT << _T(" Maximum quantity of items, that can content one column.\n") << std::endl;
				SetConsoleTextAttribute(hConsole, 7);
			}
			marker(2);
			COUT << _T("Number of columns: ");
			colorize(std::to_string(options.getColumns()));
			COUT << std::endl;
			if (cursor.isShowControls()) {
				SetConsoleTextAttribute(hConsole, 8);
				COUT << _T(" If exceeded, the alternative display mode is enabled.\n") << std::endl;
				SetConsoleTextAttribute(hConsole, 7);
			}
			marker(3);
			COUT << _T("Alternative step:  ");
			colorize(std::to_string(options.getAlterStep()));
			COUT << std::endl;
			if (cursor.isShowControls()) {
				SetConsoleTextAttribute(hConsole, 8);
				COUT << _T(" The number of items that are skipped in the \"Alternative step mode\".\n") << std::endl;
				SetConsoleTextAttribute(hConsole, 7);
			}
			marker(4);
			COUT << _T("Cascade deletion:  ");
			colorize(options.isCascadeDeletion() ? "On" : "Off");
			COUT << std::endl;
			if (cursor.isShowControls()) {
				SetConsoleTextAttribute(hConsole, 8);
				COUT << _T(" Allows to delete non-empty directories (including when inserting after cutting).") << std::endl;
				SetConsoleTextAttribute(hConsole, 7);
			}
			COUT << _T("-----------------------------------------------------") << std::endl;
			marker(5); COUT << _T("Reset to default"); marker(5); COUT << std::endl;
			marker(6); COUT << _T("Close"); marker(6); COUT << std::endl;
		}

		void colorize(const STRING str) {
			SetConsoleTextAttribute(hConsole, 4);
			COUT << _T(str);
			SetConsoleTextAttribute(hConsole, 7);
		}

		void optionsSwitch() {
			switch (cursor.vPos()) {
			case 1:
				cursor.setHorizontalBounds(options.getMinItems(), options.getMaxItems());
				options.setItems(cursor.hPos());
				break;
			case 2:
				cursor.setHorizontalBounds(options.getMinColumns(), options.getMaxColumns());
				options.setColumns(cursor.hPos());
				break;
			case 3:
				cursor.setHorizontalBounds(options.getMinAlterStep(), options.getMaxAlterStep());
				options.setAlterStep(cursor.hPos());
				break;
			case 4:
				cursor.setHorizontalBounds(0, 1);
				cursor.hPos() == 0 ? options.cascadeDeletionOff() : options.cascadeDeletionOn();
				break;
			case 5:
				cursor.setHorizontalBounds(1, 1);
				if (cursor.isPressed())
					options.toDefault();
				break;
			case 6:
				cursor.setHorizontalBounds(1, 1);
				if (cursor.isPressed()) {
					cursor.showControlsOff();
					cursor.switchOptionsMenu();
					cursor.resetVPos();
					cursor.resetHPos();
				}
				break;
			}
			cursor.release();
		}

		void tryToOpen() {
			if (content->getCount() == 0)
				return;
			try {
				open();
			}
			catch (std::exception& exc) {
				cursor.release();
				system("cls");
				COUT << _T("\"Open\" process failed") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
				if (!options.isSearchMode() && depth == 0)
					currentPath.clear();
				else if (options.isSearchMode() && searchModeDepth == 0)
					searchPath.clear();
				else if (options.isSearchMode()) {
					searchPath = searchPath.parent_path();
					content->refresh(searchPath.string());
				}
				else {
					currentPath = currentPath.parent_path();
					content->refresh(currentPath.string());
				}
				cursor.setBounds(1, content->getCount(), 1, 12);
				cursor.resetVPos();
			}
		}

		void open() {
			if (state == State::DISKS) {
				currentPath = content->get(cursor.vPos() - 1);
				content = Ptr(new Folder(currentPath.string()));
				cursor.setBounds(1, content->getCount(), 1, 12);
				cursor.resetVPos();
				++depth;
				return;
			}
			if (fs::is_directory(content->get(cursor.vPos() - 1))) {
				if (state == State::FOLDER) {
					currentPath = content->get(cursor.vPos() - 1);
					content->refresh(currentPath.string());
					++depth;
				}
				else {
					searchPath = content->get(cursor.vPos() - 1);
					if (state == State::SEARCH_ROOT)
						content = Ptr(new Folder(searchPath.string()));
					else
						content->refresh(searchPath.string());
					++searchModeDepth;
				}
				cursor.setBounds(1, content->getCount(), 1, 12);
				cursor.resetVPos();
				return;
			}
			if (fs::path(content->get(cursor.vPos() - 1)).extension() == ".txt") {
				STRING query = "Notepad " + content->get(cursor.vPos() - 1);
				system(query.c_str());
				return;
			}
			system("cls");
			COUT << _T("[Unsupported format]") << std::endl << std::endl;
			pause();
		}

		void tryToMoveBack() {
			if (state == State::DISKS)
				return;
			try {
				back();
			}
			catch (std::exception& exc) {
				cursor.release();
				system("cls");
				COUT << _T("\"Back\" process failed") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
				reset();
				resetSearchInfo();
			}
		}

		void back() {
			if (state == State::FOLDER) {
				if (depth == 1) {
					currentPath.clear();
					content = Ptr(new Disks(currentPath.string()));
				}
				else {
					currentPath = currentPath.parent_path();
					content->refresh(currentPath.string());
				}
				--depth;
			}
			else {
				if (state == State::SEARCH_ROOT) {
					options.searchModeOFF();
					if (depth == 0)
						reset();
					else
						content = Ptr(new Folder(currentPath.string()));
					resetSearchInfo();
				}
				else if (searchModeDepth == 1) {
					searchPath.clear();
					content = Ptr(new SearchResults(mask, searchRoot.string()));
				}
				else {
					searchPath = searchPath.parent_path();
					content->refresh(searchPath.string());
				}
				--searchModeDepth;
			}
			cursor.setBounds(1, content->getCount(), 1, 12);
			cursor.resetVPos();
		}

		void tryToCreateFolder() {
			if (state == State::DISKS || state == State::SEARCH_ROOT)
				return;
			try {
				createFolder();
			}
			catch (std::exception& exc) {
				cursor.release();
				system("cls");
				COUT << _T("\"Create folder\" process failed") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
			}
		}

		void createFolder() {
			system("cls");
			COUT << _T("Enter the new folder name: ");
			STRING str;
			std::getline(CIN, str);
			if (state == State::FOLDER)
				str = currentPath.string() + "\\" + str;
			else
				str = searchPath.string() + "\\" + str;
			if (std::find(begin(content->getList()), end(content->getList()), str) != end(content->getList()))
				throw std::exception("[Folder with this name is already exist in the directory]");
			if (fs::create_directory(str)) {
				system("cls");
				COUT << _T("[Success]") << std::endl << std::endl;
				if (state == State::SEARCH_MODE_FOLDER)
					content->refresh(searchPath.string());
				else
					content->refresh(currentPath.string());
				cursor.setBounds(1, content->getCount(), 1, 12);
				pause();
			}
			else
				throw std::exception("[Operation failed for the unknown reason]");
		}

		void tryToCreateFile() {
			if (state == State::DISKS || state == State::SEARCH_ROOT)
				return;
			if (currentPath == _T("C:\\"))
				return;
			try {
				createFile();
			}
			catch (std::exception& exc) {
				cursor.release();
				system("cls");
				COUT << _T("\"Create file\" process failed") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
			}
		}

		void createFile() {
			system("cls");
			COUT << _T("Enter the new file name: ");
			STRING str;
			std::getline(CIN, str);
			if (state == State::FOLDER)
				str = currentPath.string() + "\\" + str + ".txt";
			else
				str = searchPath.string() + "\\" + str + ".txt";
			if (std::find(begin(content->getList()), end(content->getList()), str) != end(content->getList()))
				throw std::exception("[File with this name is already exist in the directory]");
			std::ofstream nFile(str);
			if (nFile.is_open()) {
				nFile.close();
				system("cls");
				COUT << _T("[Success]") << std::endl << std::endl;
				if (state == State::SEARCH_MODE_FOLDER)
					content->refresh(searchPath.string());
				else
					content->refresh(currentPath.string());
				cursor.setBounds(1, content->getCount(), 1, 12);
				pause();
			}
			else
				throw std::exception("[Operation failed for the unknown reason]");
		}

		void tryToDelete() {
			if (state == State::DISKS)
				return;
			try {
				delete_(content->get(cursor.vPos() - 1));
				system("cls");
				COUT << _T("[Success]") << std::endl;
				if (state == State::FOLDER)
					content->refresh(currentPath.string());
				else if (state == State::SEARCH_ROOT)
					content->refresh(searchRoot.string());
				else
					content->refresh(searchPath.string());
				cursor.setBounds(1, content->getCount(), 1, 12);
				cursor.resetVPos();
				COUT << std::endl;
				pause();
			}
			catch (std::exception& exc) {
				cursor.release();
				system("cls");
				COUT << _T("\"Delete\" process failed") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
			}
		}

		void delete_(STRING path) {
			if (options.isCascadeDeletion()) {
				if (!fs::remove_all(path))
					throw std::exception("[Cascade deletion wasn't accomplished]");
			}
			else {
				if (!fs::remove(path))
					throw std::exception("[Single item deletion wasn't accomplished]");
			}
		}

		void tryToRename() {
			if (state == State::DISKS)
				return;
			try {
				rename();
			}
			catch (std::exception& exc) {
				cursor.release();
				system("cls");
				COUT << _T("\"Rename\" process failed") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
			}
		}

		void rename() {
			system("cls");
			COUT << _T("Enter the new name: ");
			STRING str;
			std::getline(CIN, str);
			if (state == State::FOLDER)
				str = currentPath.string() + "\\" + str + fs::path(content->get(cursor.vPos() - 1)).extension().string();
			else if (state == State::SEARCH_MODE_FOLDER)
				str = searchPath.string() + "\\" + str + fs::path(content->get(cursor.vPos() - 1)).extension().string();
			else
				str = fs::path(content->get(cursor.vPos() - 1)).parent_path().string() + "\\" + str + fs::path(content->get(cursor.vPos() - 1)).extension().string();
			if (std::find(begin(content->getList()), end(content->getList()), str) != end(content->getList()))
				throw std::exception("[Item with this name is already exist int the directory]");
			fs::rename(content->get(cursor.vPos() - 1), str);
			system("cls");
			COUT << _T("[Success]") << std::endl;
			if (state == State::FOLDER)
				content->refresh(currentPath.string());
			else if (state == State::SEARCH_ROOT)
				content->refresh(searchRoot.string());
			else
				content->refresh(searchPath.string());
			COUT << std::endl;
			pause();
		}

		void tryToCopy() {
			if (state == State::DISKS)
				return;
			try {
				copy();
			}
			catch (std::exception& exc) {
				cursor.release();
				system("cls");
				COUT << _T("\"Copy\" process failed") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
			}
		}

		void copy() {
			buffer = '0' + content->get(cursor.vPos() - 1);
			system("cls");
			COUT << _T("[Item copied]") << std::endl << std::endl;
			pause();
		}

		void tryToCut() {
			if (state == State::DISKS)
				return;
			try {
				cut();
			}
			catch (std::exception& exc) {
				cursor.release();
				system("cls");
				COUT << _T("\"Cut\" process failed") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
			}
		}

		void cut() {
			buffer = '1' + content->get(cursor.vPos() - 1);
			system("cls");
			COUT << _T("[Item copied and ready to be deleted]") << std::endl << std::endl;
			pause();
		}

		void tryToInsert() {
			if (state == State::DISKS || state == State::SEARCH_ROOT)
				return;
			try {
				insert();
			}
			catch (std::exception& exc) {
				cursor.release();
				system("cls");
				COUT << _T("\"Insert\" process failed") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
			}
		}

		void insert() {
			if (buffer.empty())
				throw std::exception("[Buffer is empty]");
			fs::path temp(begin(buffer) + 1, end(buffer));
			if ((state == State::FOLDER || state == State::SEARCH_MODE_FOLDER) && temp.parent_path() == currentPath)
				throw std::exception("[Directory hasn't been changed]");
			fs::copy_options copyOptions = fs::copy_options::update_existing | fs::copy_options::recursive;
			fs::copy(temp, (options.isSearchMode() ? searchPath.string() : currentPath.string()) + '\\' + temp.filename().string(), copyOptions);
			content->refresh(options.isSearchMode() ? searchPath.string() : currentPath.string());
			cursor.setBounds(1, content->getCount(), 1, 12);
			system("cls");
			COUT << _T("[Item inserted]") << std::endl << std::endl;
			pause();
			if (buffer.at(0) == '1') {
				buffer.clear();
				delete_(temp.string());
				system("cls");
				COUT << _T("[Item successfully deleted from the previous location]") << std::endl << std::endl;
				pause();
			}
		}

		void tryToShowSize() {
			try {
				size();
			}
			catch (std::exception& exc) {
				cursor.release();
				system("cls");
				COUT << _T("\"Size\" process failed") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
			}
		}

		void size() {
			fs::path temp(content->get(cursor.vPos() - 1));
			size_t size = 0;
			bool wasSkip = false;
			bool wasInterrupted = false;
			if (temp.has_extension())
				size += fs::file_size(temp);
			else {
				fs::directory_options dirOptions(fs::directory_options::skip_permission_denied);
				size_t count = 0;
				RecursiveIt testIt(temp);
				for (const auto& entry : RecursiveIt(temp, dirOptions)) {
					if (_kbhit()) {
						wasInterrupted = true;
						system("cls");
						COUT << _T("[The process was forcibly interrupted]\n\n");
						pause(false);
						pause();
						break;
					}
					if (!wasSkip) {
						try {
							testIt++;
						}
						catch (std::exception& exc) {
							wasSkip = true;
						}
					}
					size += entry.file_size();
					system("cls");
					COUT << _T("Items passed: ") << count++;
				}
			}
			system("cls");
			if (wasInterrupted)
				COUT << _T("[Calculated by this point] ");
			COUT << _T("[");
			COUT << (options.isSearchMode() || depth == 0 ? content->get(cursor.vPos() - 1) : fs::path(content->get(cursor.vPos() - 1)).filename().string());
			COUT << _T("] - ");
			if (size < 1024)
				COUT << size << _T(" B");
			else if (size / 1024 < 1024)
				COUT << round(size / (float)1024 * 100) / 100 << _T(" KB");
			else
				COUT << round(size / ((float)1024 * 1024) * 100) / 100 << _T(" MB");
			if (wasSkip)
				COUT << std::endl << _T("[Result might be inaccurate due to the denial of access to some files]");
			COUT << std::endl << std::endl;
			pause();
		}

		void tryToSearch() {
			try {
				search();
			}
			catch (std::exception& exc) {
				cursor.release();
				system("cls");
				COUT << _T("\"Search\" process failed") << std::endl;
				COUT << exc.what() << std::endl << std::endl;
				pause();
			}
		}

		void search() {
			resetSearchInfo();
			system("cls");
			COUT << _T("Enter the mask: ");
			getline(CIN, mask);
			content = Ptr(new SearchResults(mask, content->get(cursor.vPos() - 1)));
			searchRoot = fs::path(content->get(cursor.vPos() - 1)).parent_path();
			options.searchModeOn();
			cursor.resetVPos();
		}

		void resetSearchInfo() {
			searchRoot.clear();
			searchPath.clear();
			mask.clear();
			searchModeDepth = 0;
		}

		void exit() {
			isExit = true;
			system("cls");
		}
	};
}

void main() {
	FileManagerProperties::FileManager fm;
	fm.startProgram();
}