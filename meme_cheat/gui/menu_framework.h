#pragma once
#include "menu_commands.h"

/*
Steps for making a menu:

somewhere during initilization -
 - call your defined MENU::InitColors() function, this is where you pick colors
   for stuff like windows, checkboxes etc
 - you can change colors on the fly but there needs to be defined colors before 
   drawing, there are NO default colors provided

every frame -
 - call MENU::PPGUI_PP_GUI::Begin();
 - call your defined MENU::Do() function, this is where you do the menu stuff in;
 - call MENU::PPGUI_PP_GUI::End();

in your MENU::Do() function -
 - to create a window just call WindowBegin(), draw checkboxes and stuff in between,
   then call WindowEnd()
 - to create a groupbox just call GroupboxBegin() and GroupboxEnd(), similar to a window except it does not require
   you to give a position, only height and name

*/

namespace MENU
{
	// this is where you make the menu
	void InitColors();
	void Do();

	namespace PPGUI_PP_GUI
	{
		void DrawMouse();

		void SetFont(uint32_t); // sets the font

		void SetLine(int = 1); // kinda shit function name but basically if you have the frame_bounds split in 'x' amount of pieces,
						       // this sets which piece to draw on, need to call SetLine(1) after ur done for shit to go back to normal
		void SplitWindow(int); // splits the window into specified pieces, doing SplitWindow(2) 
		                       // would split the window into 2 even parts, to be used with SameLine()



		Vector2D PeekNextPosition(); // this is the position the next object will be placed at
		Vector2D GetWindowSize(); // get the size of the window
		Vector2D GetEffectiveWindowSize(); // this is the space where objects can be placed, includes the invisible border
		FrameBounds GetWindowBounds(); //bounds of the current window

		void Begin();
		void End();

		// windows
		void WindowBegin(std::string name, Vector2D position, Vector2D size, int scroll_size = 0);
		void WindowEnd();

		// groupboxes, basically same functionality as a window
		void GroupboxBegin(std::string name, int height, int scroll_size = 0);
		void GroupboxEnd();

		// makes a horizontal line
		void Separator(std::string name);
		void Separator();

		// checkboxes
		bool Checkbox(std::string, bool&);
		bool Checkbox(std::string);

		// buttons
		bool Button(std::string);

		// comboboxes
		int Combobox(std::string, std::vector<std::string>, int&);

		// sliders
		float Slider(std::string, float, float, float&, int decimal_places = 1);

		int MainTab(std::string name, std::vector<std::string> tabs, OBJECT_FLAGS flags);

		// tabs
		int Tab(std::string, std::vector<std::string>, OBJECT_FLAGS = OBJECT_FLAGS::FLAG_SCROLL);
		int VerticalTab(std::string, std::vector<std::string>, OBJECT_FLAGS = OBJECT_FLAGS::FLAG_SCROLL);

		// color picker
		CColor ColorPicker(std::string name, CColor &variable);
	}
}