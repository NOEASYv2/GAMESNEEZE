#pragma once
#include "../includes.h"
#include "../utilities/render.h"

namespace Components
{
	POINT cursorPos;
	bool canClick;//mouse is fun!!!!!
				  //todo: replace with less gay method of checking if a combo is open or not
	bool comboStates[1000];
	int currentCombo;
	int lastComboX, lastComboY, lastComboHeight;

	void Start() {
		if (!GetAsyncKeyState(VK_LBUTTON)) {
			canClick = true;
		}
		currentCombo = 0;
		GetCursorPos(&cursorPos);
		lastComboX = 0;
		lastComboY = 0;
		lastComboHeight = 0;
	}

	bool Checkbox(const char *text, bool *toggle, int x, int y) {
		if (x >= lastComboX && x <= lastComboX + 145 && y >= lastComboY && y <= lastComboY + lastComboHeight) {
			RENDER::DrawF(x + 7, y, FONTS::numpad_menu_font, false, false, CColor(255, 255, 255, 255), text);
			return false;
		}
		RENDER::DrawF(x + 7, y, FONTS::numpad_menu_font, false, false, CColor(255, 255, 255, 255), text);
		if (*toggle) RENDER::DrawFilledRect(x + 293, y + 2, x + 293 + 8, y + 2 + 8, CColor(10, 145, 190));
		RENDER::DrawEmptyRect(x + 293, y + 2, x + 293 + 8, y + 2 + 8, CColor(27, 27, 27));
		if (GetAsyncKeyState(VK_LBUTTON) && canClick) {
			if (cursorPos.x >= x && cursorPos.x <= x + 400 && cursorPos.y >= y && cursorPos.y <= y + 12) {
				*toggle = !*toggle;
				canClick = false;
			}
		}
		return true;
	}

	bool Combo(const char *text, int *value, const char* texties[], int size, int x, int y) {
		if (x >= lastComboX && x <= lastComboX + 145 && y >= lastComboY && y <= lastComboY + lastComboHeight) {
			RENDER::DrawF(x + 7, y, FONTS::numpad_menu_font, false, false, CColor(255, 255, 255, 255), text);
			return false;
		}
		lastComboX = x - 10;
		lastComboY = y;
		lastComboHeight = comboStates[currentCombo] ? 17 + 12 * size : 14;
		RENDER::DrawFilledRect(x + 293, y - 1, x + 293 + 102, y - 1 + 14, CColor(50, 50, 50));
		RENDER::DrawFilledRect(x + 384, y + 4, x + 384 + 5, y + 4 + 1, CColor(0, 0, 0));
		RENDER::DrawFilledRect(x + 384, y + 5, x + 384 + 5, y + 5 + 1, CColor(255, 255, 255));
		RENDER::DrawFilledRect(x + 385, y + 6, x + 385 + 3, y + 6 + 1, CColor(255, 255, 255));
		RENDER::DrawFilledRect(x + 386, y + 7, x + 386 + 1, y + 7 + 1, CColor(255, 255, 255));
		RENDER::DrawEmptyRect(x + 293, y - 1, x + 293 + 102, y - 1 + 14, CColor(27, 27, 27));
		RENDER::DrawF(x + 7, y, FONTS::numpad_menu_font, false, false, CColor(255, 255, 255, 255), text);
		RENDER::DrawF(x + 296, y - 1, FONTS::numpad_menu_font, false, false, CColor(255, 255, 255, 255), texties[*value]);
		if (comboStates[currentCombo]) {
			RENDER::DrawFilledRect(x + 293, y + 15, x + 293 + 102, y + 15 + 12 * size + 2, CColor(50, 50, 50));
			RENDER::DrawEmptyRect(x + 293, y + 15, x + 293 + 102, y + 15 + 12 * size + 2, CColor(27, 27, 27));
			int yPos = y + 15;
			for (int i = 0; i < size; i++) {
				RENDER::DrawF(x + 296, yPos, FONTS::numpad_menu_font, false, false, CColor(255, 255, 255, 255), texties[i]);
				yPos += 12;
			}
		}
		if (GetAsyncKeyState(VK_LBUTTON) && canClick) {
			if (cursorPos.x >= x && cursorPos.x <= x + 395 && cursorPos.y >= y - 1 && cursorPos.y <= y + 13) {
				comboStates[currentCombo] = !comboStates[currentCombo];
				canClick = false;
			}
			if (comboStates[currentCombo]) {
				int yPos = y + 17;
				for (int i = 0; i < size; i++) {
					if (cursorPos.x >= x + 294 && cursorPos.x <= x + 395 && cursorPos.y >= yPos && cursorPos.y <= yPos + 12) {
						comboStates[currentCombo] = false;
						*value = i;
						canClick = false;
					}
					yPos += 12;
				}
			}
		}
		currentCombo++;
		return comboStates[currentCombo - 1];
	}
};