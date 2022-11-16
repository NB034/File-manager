#pragma once
#include "ContentSuperClass.h"

namespace FileManagerProperties {

	class Disks : public Content {
	public:
		Disks(STRING path) {
			refresh(path);
		}

		void refresh(STRING path) override {
			items.clear();
			DWORD length = 40;
			TCHAR* disksStr = new TCHAR[length];
			length = GetLogicalDriveStrings(length, disksStr);
			items.reserve(length / 4);
			for (size_t i = 0; i < length; i += 4)
				items.push_back(STRING(disksStr + i, disksStr + i + 3));
			delete[] disksStr;
		}

		void show(Options& options) override {
			for (size_t i = 0; i < items.size(); i++) {
				marker(i + 1);
				COUT << items[i] << std::endl;
			}
		}
	};



	class Folder : public Content {
	public:
		Folder(STRING path) {
			refresh(path);
		}

		void refresh(STRING path) override {
			fs::path cPath(path);
			items.clear();
			int count = 0;
			for (const auto& entry : fs::directory_iterator(cPath.c_str()))
				++count;
			items.reserve(count);
			for (const auto& entry : fs::directory_iterator(cPath.c_str()))
				items.push_back(entry.path().string());
		}
	};



	class SearchResults : public Content {
		STRING mask;
	public:
		SearchResults(STRING searchMask, STRING searchRoot) {
			mask = searchMask;
			toLower(mask);
			refresh(searchRoot);
		}

		void refresh(STRING path) override {
			items.clear();
			size_t passed = 0;
			size_t found = 0;
			STRING temp;
			std::list<STRING> list;
			for (auto entry : RecursiveIt(path, fs::directory_options::skip_permission_denied)) {
				system("cls");
				if (_kbhit()) {
					system("cls");
					COUT << _T("[Search wasn't fully completed]\n");
					pause(false);
					Sleep(700);
					break;
				}
				temp = fs::path(entry).filename().string();
				toLower(temp);
				if (temp.find(mask) != STRING::npos) {
					list.push_back(fs::path(entry).string());
					++found;
				}
				COUT << _T("Items passed: ") << ++passed << std::endl;
				COUT << _T("Items found:  ") << found;
			}
			if (list.size() == 0)
				throw(std::exception("[Nothing was found]"));
			items.reserve(list.size());
			for (const auto str : list)
				items.push_back(str);
			std::sort(begin(items), end(items));
		}

		void toLower(STRING& str) {
			std::transform(begin(mask), end(mask), begin(mask),
				[](char ch) {
					return std::tolower(ch);
				});
		}
	};
}