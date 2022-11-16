#pragma once
#include "Accessories.h"

namespace FileManagerProperties {

	class Options {
		const short numberOfoptions = 3;
		const short maxItemsInColumn = 50;
		const short maxAmountOfColumns = 7;
		const short maxAlterStep = 20;
		const short minItemsInColumn = 10;
		const short minAmountOfColumns = 1;
		const short minAlterStep = 2;
		const short defaultItems = 20;
		const short defaultColumns = 4;
		const short defaultAlterStep = 5;
		short items = 0;
		short columns = 0;
		short alterStep = 0;
		bool searchMode = false;
		bool cascadeDeletion = false;
		bool alterStepMode = false;
	public:
		Options() {
			std::ifstream file("Options.txt");
			if (file.is_open()) {
				try {
					std::vector<STRING> arr;
					arr.reserve(numberOfoptions);
					STRING buf;
					while (!file.eof()) {
						std::getline(file, buf);
						arr.push_back(buf);
					}
					if (arr.size() != numberOfoptions)
						throw std::exception("[Invalid data in the file]");
					setItems(stoi(arr[0]));
					setColumns(stoi(arr[1]));
					setAlterStep(stoi(arr[2]));
					COUT << _T("[Options loaded]");
					Sleep(600);
					return;
				}
				catch (std::exception& exc) {
					COUT << _T("[Loading process error]") << std::endl;
					COUT << exc.what() << std::endl;
					Sleep(800);
				}
				file.close();
			}
			COUT << _T("[Program loading with the default options]");
			Sleep(800);
			items = defaultItems;
			columns = defaultColumns;
			alterStep = defaultAlterStep;
		}

		void setItems(short number) {
			if (number < minItemsInColumn || maxItemsInColumn < number)
				throw std::exception("[Items: out of range]");
			items = number;
		}
		void setColumns(short number) {
			if (number < minAmountOfColumns || maxAmountOfColumns < number)
				throw std::exception("[Columns: out of range]");
			columns = number;
		}
		void setAlterStep(short number) {
			if (number < minAlterStep || maxAlterStep < number)
				throw std::exception("[Alternate length of the step: out of range]");
			alterStep = number;
		}

		short getNumberOfOptions() const { return numberOfoptions; }
		short getMaxItems() const { return maxItemsInColumn; }
		short getMaxColumns() const { return maxAmountOfColumns; }
		short getMaxAlterStep() const { return maxAlterStep; }
		short getMinItems() const { return minItemsInColumn; }
		short getMinColumns() const { return minAmountOfColumns; }
		short getMinAlterStep() const { return minAlterStep; }
		short getDefaultItems() const { return defaultItems; }
		short getDefaultColumns() const { return defaultColumns; }
		short getDefaultAlterStep() const { return defaultAlterStep; }
		short getItems() const { return items; }
		short getColumns() const { return columns; }
		short getAlterStep() const { return alterStep; }
		bool isSearchMode() const { return searchMode; }
		bool isCascadeDeletion() const { return cascadeDeletion; }
		bool isAlterStepMode() const { return alterStepMode; }

		void searchModeOn() { searchMode = true; }
		void searchModeOFF() { searchMode = false; }
		void cascadeDeletionOn() { cascadeDeletion = true; }
		void cascadeDeletionOff() { cascadeDeletion = false; }
		void alterStepModeOn() { alterStepMode = true; }
		void alterStepModeOFF() { alterStepMode = false; }
		void switchAlterStepMode() { alterStepMode = !alterStepMode; }

		void toDefault() {
			items = defaultItems;
			columns = defaultColumns;
			alterStep = defaultAlterStep;
			cascadeDeletion = false;
		}

		~Options() {
			std::ofstream file("Options.txt");
			if (file.is_open()) {
				try {
					file.clear();
					file << items << std::endl;
					file << columns << std::endl;
					file << alterStep;
					COUT << _T("[Options saved]");
					Sleep(800);
					return;
				}
				catch (std::exception& exc) {
					file.clear();
					COUT << _T("[Saving process error]") << std::endl;
					COUT << exc.what() << std::endl;
					Sleep(600);
				}
				file.close();
			}
			COUT << _T("[Options weren't saved due to an error]");
			Sleep(800);
		}
	};
}