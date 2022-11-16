#pragma once
#include "Options.h"

namespace FileManagerProperties {

	class Content abstract {
	protected:
		std::vector<STRING> items;
		Marker marker;
		HANDLE hConsole;
	public:
		Content() : marker(_T(""), _T(""), STRING(1, TCHAR(186)), _T(" ")), hConsole(GetStdHandle(STD_OUTPUT_HANDLE)) {}

		virtual void refresh(STRING path) = 0;
		virtual void moveMarker(int line) { marker.move(line); }

		virtual const size_t& getCount() const { return items.size(); }
		virtual const STRING& get(int index) const { return items.at(index); }
		virtual const std::vector<STRING>& getList() const { return items; }

		virtual void show(Options& options) {
			if (options.isSearchMode() && items.size() >= 5 || items.size() > options.getItems() * options.getColumns()) {
				showAlternativeView(options);
				return;
			}
			std::vector<int> indents = calculateIndents(options);
			for (size_t i = 0; i < options.getItems(); i++) {
				if (i < items.size()) {
					marker(i + 1);
					COUT << STRING(fs::path(items[i]).filename().string());
					if (options.isSearchMode()) {
						SetConsoleTextAttribute(hConsole, 8);
						COUT << _T("   [") << STRING(fs::path(items[i]).string()) << _T("]");
						SetConsoleTextAttribute(hConsole, 7);
					}
					for (size_t j = 0; j < indents.size(); j++) {
						if (i + (j + 1) * options.getItems() < items.size()) {
							for (int a = 0; a < indents[j] - items[i + j * options.getItems()].length(); a++)
								COUT << _T(' ');
							marker(i + (j + 1) * options.getItems() + 1);
							COUT << STRING(fs::path(items[i + (j + 1) * options.getItems()]).filename().string()); //_U
						}
					}
					COUT << std::endl;
				}
				else
					break;
			}
		}

		virtual void showAlternativeView(Options& options) {
			COUT << _T(" Quantity of items: ") << items.size() << _T("\t\tCurrent item: ") << marker.getPosition() << std::endl;
			COUT << _T("----------------------------------------------------") << std::endl;
			int item = marker.getPosition();
			SetConsoleTextAttribute(hConsole, 8);
			COUT << std::endl;
			COUT << _T("    ") << fs::path((item - 1 > 0 && item - 2 > 0) ? items[item - 3] : items[items.size() - 2]).filename().string() << std::endl;
			if (options.isSearchMode())
				COUT << _T("    [") << fs::path((item - 1 > 0 && item - 2 > 0) ? items[item - 3] : items[items.size() - 2]).string() << _T("]") << std::endl;
			COUT << std::endl;
			COUT << _T("   ") << fs::path(item - 1 > 0 ? items[item - 2] : items[items.size() - 1]).filename().string() << std::endl;
			if (options.isSearchMode())
				COUT << _T("   [") << fs::path(item - 1 > 0 ? items[item - 2] : items[items.size() - 1]).string() << _T("]") << std::endl;
			COUT << std::endl;
			SetConsoleTextAttribute(hConsole, 7);
			marker(item); COUT << fs::path(items[item - 1]).filename().string() << std::endl;
			SetConsoleTextAttribute(hConsole, 8);
			if (options.isSearchMode())
				COUT << _T(" [") << fs::path(items[item - 1]).string() << _T("]") << std::endl;
			COUT << std::endl;
			COUT << _T("   ") << fs::path(item + 1 <= items.size() ? items[item] : items[0]).filename().string() << std::endl;
			if (options.isSearchMode())
				COUT << _T("   [") << fs::path(item + 1 <= items.size() ? items[item] : items[0]).string() << _T("]") << std::endl;
			COUT << std::endl;
			COUT << _T("    ") << fs::path((item + 1 <= items.size() && item + 2 <= items.size()) ? items[item + 1] : items[1]).filename().string() << std::endl;
			if (options.isSearchMode())
				COUT << _T("    [") << fs::path((item + 1 <= items.size() && item + 2 <= items.size()) ? items[item + 1] : items[1]).string() << _T("]") << std::endl;
			SetConsoleTextAttribute(hConsole, 7);
		}

		virtual std::vector<int> calculateIndents(Options& options) {
			std::vector<int> indents;
			int maxLen;
			if (items.size() > 20) {
				for (int i = 0; i < items.size() / options.getItems(); i++) {
					maxLen = 0;
					std::for_each(begin(items) + options.getItems() * i, begin(items) + options.getItems() * (i + 1),
						[&maxLen](STRING& str) {
							if (str.length() > maxLen)
								maxLen = str.length();
						});
					indents.push_back(maxLen + 3);
				}
				indents.push_back(0);
			}
			return indents;
		}
	};
}