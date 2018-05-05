#pragma once
#include "menu_objects.h"

namespace MENU
{
	namespace PPGUI_PP_GUI
	{
		class WindowCommand : public Command
		{
		public:
			WindowCommand(std::string, Vector2D, Vector2D, int);

			void Draw();
			void ModifyScrollSize(int);
			void TestInput(Input&);

			CommandInfo Info();

		private:
			CommandInfo info;
			int scroll_size;
		};

		class GroupboxCommand : public Command
		{
		public:
			GroupboxCommand(std::string, Vector2D, Vector2D, FrameBounds, int);

			void Draw();
			void ModifyScrollSize(int);
			void TestInput(Input&);

			CommandInfo Info();

		private:
			CommandInfo info;
			FrameBounds frame_bounds;
			int scroll_size;
		};

		class SeparatorCommand : public Command
		{
		public:
			SeparatorCommand(std::string name, Vector2D position, Vector2D size, FrameBounds bounds);

			void Draw();

			CommandInfo Info();

		private:
			CommandInfo info;
			FrameBounds frame_bounds;
		};

		class CheckboxCommand : public Command
		{
		public:
			CheckboxCommand(std::string, Vector2D, Vector2D, FrameBounds, bool&);
			CheckboxCommand(std::string, Vector2D, Vector2D, FrameBounds);

			void Draw();
			void TestInput(Input&);

			CommandInfo Info();

		private:
			CommandInfo info;
			FrameBounds frame_bounds;
			bool* value = nullptr;
		};

		class ButtonCommand : public Command
		{
		public:
			ButtonCommand(std::string, Vector2D, Vector2D, FrameBounds);

			void Draw();
			void TestInput(Input&);

			CommandInfo Info();

		private:
			CommandInfo info;
			FrameBounds frame_bounds;
		};

		class ComboboxCommand : public Command
		{
		public:
			ComboboxCommand(std::string, Vector2D, Vector2D, FrameBounds, std::vector<std::string>, int&);

			void Draw();
			void TestInput(Input&);

			CommandInfo Info();

		private:
			CommandInfo info;
			FrameBounds frame_bounds;
			std::vector<std::string> items;
			int* variable;
		};

		class SliderCommand : public Command
		{
		public:
			SliderCommand(std::string, Vector2D, Vector2D, FrameBounds, float, float, float&, int decimal_places);

			void Draw();
			void TestInput(Input&);

			CommandInfo Info();

		private:
			CommandInfo info;
			FrameBounds frame_bounds;
			int decimal_place;
			float* variable;
			float slider_min;
			float slider_max;
		};

		class TabCommand : public Command
		{
		public:
			TabCommand(std::string, Vector2D, Vector2D, FrameBounds, std::vector<std::string>, Vector2D offset);

			void Draw();
			void TestInput(Input&);

			CommandInfo Info();

		private:
			CommandInfo info;
			FrameBounds frame_bounds;
			std::vector<std::string> items;
		};

		class VerticalTabCommand : public Command
		{
		public:
			VerticalTabCommand(std::string, Vector2D, Vector2D, FrameBounds, std::vector<std::string>);

			void Draw();
			void TestInput(Input&);

			CommandInfo Info();

		private:
			CommandInfo info;
			FrameBounds frame_bounds;
			std::vector<std::string> items;
		};

		class ColorPickerCommand : public Command
		{
		public:
			ColorPickerCommand(std::string name, Vector2D position, Vector2D size, FrameBounds bounds, CColor &var);

			void Draw();
			void TestInput(Input&);

			CommandInfo Info();

		private:
			CommandInfo info;
			FrameBounds frame_bounds;
			CColor* variable;
		};
	}
}