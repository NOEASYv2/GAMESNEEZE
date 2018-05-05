#pragma once
#include "imgui.h"

class ColorVar {
public:
	ImColor color;
	bool rainbow;
	float rainbowSpeed;

	ColorVar( ) { }

	ColorVar( ImColor color ) {
		this->color = color;
		this->rainbow = false;
		this->rainbowSpeed = 0.5f;
	}

	ImColor Color( ) {
		ImColor result = this->color;
		result.Value.w = this->color.Value.w;
		return result;
	}
};