#pragma once
#include "Options.h"

namespace FileManagerProperties {

	class Cursor {
		int upperBound, lowerBound, leftBound, rightBound;
		int verticalPos, horizontalPos;
		bool pressed;
		bool showControls;
		bool optionsMenu;
	public:
		Cursor(int upper, int lower, int left, int right) :
			upperBound(upper), lowerBound(lower), leftBound(left), rightBound(right),
			verticalPos(1), horizontalPos(1),
			pressed(false), showControls(false), optionsMenu(false) {
		}

		void setBounds(int upper, int lower, int left, int right) {
			upperBound = upper;
			lowerBound = lower;
			leftBound = left;
			rightBound = right;
		}
		void setVerticalBounds(int upper, int lower) {
			upperBound = upper;
			lowerBound = lower;
		}
		void setHorizontalBounds(int left, int right) {
			leftBound = left;
			rightBound = right;
		}
		void setVerticalPosition(int pos) { verticalPos = pos; }
		void setHorizontalPosition(int pos) { horizontalPos = pos; }

		bool isShowControls() const { return showControls; }
		bool isPressed() const { return pressed; }
		bool isOptionsMenu() const { return optionsMenu; }
		int vPos() const { return verticalPos; }
		int& vPos() { return verticalPos; }
		int hPos() const { return horizontalPos; }
		int& hPos() { return horizontalPos; }
		int upBound() const { return upperBound; }
		int lowBound() const { return lowerBound; }
		int lBound() const { return leftBound; }
		int rBound() const { return rightBound; }

		void resetVPos() { verticalPos = upperBound; }
		void resetHPos() { horizontalPos = leftBound; }
		void switchShowControls() { showControls = !showControls; }
		void showControlsOn() { showControls = true; }
		void showControlsOff() { showControls = false; }
		void switchOptionsMenu() { optionsMenu = !optionsMenu; }
		void press() { pressed = true; }
		void release() { pressed = false; }

		void operator()(Options& options) {
			int item = _getch();
			if (toupper(item) == 'C' || item == 241 || item == 209) {
				switchShowControls();
				return;
			}
			if (!optionsMenu) {
				if (toupper(item) == 'A' || toupper(item) == 244 || item == 212) {
					options.switchAlterStepMode();
					return;
				}
				if (toupper(item) == 'O' || toupper(item) == 249 || item == 217) {
					resetVPos();
					setHorizontalPosition(options.getItems());
					showControlsOff();
					switchOptionsMenu();
					return;
				}
			}
			if (item == 0 || item == 224)
				switch (_getch()) {
				case UP:
					if (!optionsMenu && options.isAlterStepMode()) {
						verticalPos -= options.getAlterStep();
						while (true) {
							if (verticalPos < upperBound)
								verticalPos = lowerBound + verticalPos;
							else
								break;
						}
					}
					else {
						--verticalPos;
						if (verticalPos < upperBound)
							verticalPos = lowerBound;
					}
					if (optionsMenu)
						optionsSwitch(verticalPos, options);
					break;
				case DOWN:
					if (!optionsMenu && options.isAlterStepMode()) {
						verticalPos += options.getAlterStep();
						while (true) {
							if (verticalPos > lowerBound)
								verticalPos = upperBound + (verticalPos - lowerBound - 1);
							else
								break;
						}
					}
					else {
						++verticalPos;
						if (verticalPos > lowerBound)
							verticalPos = upperBound;
					}
					if (optionsMenu)
						optionsSwitch(verticalPos, options);
					break;
				case LEFT:
					--horizontalPos;
					if (horizontalPos < leftBound)
						horizontalPos = rightBound;
					break;
				case RIGHT:
					++horizontalPos;
					if (horizontalPos > rightBound)
						horizontalPos = leftBound;
					break;
				default:
					break;
				}
			else
				if (item == 13)
					pressed = true;
		}

		void optionsSwitch(int num, Options& options) {
			switch (num) {
			case 1:
				setHorizontalPosition(options.getItems());
				break;
			case 2:
				setHorizontalPosition(options.getColumns());
				break;
			case 3:
				setHorizontalPosition(options.getAlterStep());
				break;
			case 4:
				setHorizontalPosition(options.isCascadeDeletion() ? 1 : 0);
				break;
			default:
				break;
			}
		}
	};
}