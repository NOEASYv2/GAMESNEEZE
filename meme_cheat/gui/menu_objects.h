#pragma once
#include "menu_defines.h"

namespace MENU
{
	namespace PPGUI_PP_GUI
	{
		struct MouseInfo
		{
			Vector2D position;
			Vector2D position_delta;
			int scroll;
			int right_button;
			int left_button;
		};
		struct Input
		{
			MouseInfo mouse;
			MouseInfo real_mouse;
			int keyboard[255];

			bool valid = false;
		};

		struct FrameBounds
		{
			FrameBounds() {};
			FrameBounds(Vector2D bound_min, Vector2D bound_max)
			{
				min = bound_min;
				max = bound_max;
			}
			Vector2D min;
			Vector2D max;
		};

		struct CommandInfo
		{
			std::string name;
			std::string id_name;
			uint32_t id;
			uint32_t font;

			OBJECT_TYPE object_type;
			unsigned char flags; // this varys depending on the command

			Vector2D position;
			Vector2D size;
		};
		class Command
		{
		public:
			virtual void Draw() = 0;
			virtual void TestInput(Input&);

			virtual CommandInfo Info() = 0;
			virtual void ModifyScrollSize(int);

			uint32_t GetID();
			std::string CreateIDName();

		private:
		};
		class CommandQueue
		{
		public:
			~CommandQueue();

			std::vector<Command*> Get();

			void Clear();

			template <class T>
			Command* Insert(T command)
			{
				T* copy = new T(command);

				allocated.insert(allocated.begin(), copy);
				queue.insert(queue.begin(), copy);

				return copy;
			}
			template <class T>
			Command* Pushback(T command)
			{
				T* copy = new T(command);

				allocated.push_back(copy);
				queue.push_back(copy);

				return copy;
			}

		private:
			std::vector<Command*> queue;
			std::vector<Command*> allocated;
		};

		class CommandFrame
		{
		public:
			Command* command = nullptr;

			CommandFrame() {};
			CommandFrame(Command*, Vector2D, Vector2D, FrameBounds);

			bool AllocateSpace(Vector2D, Vector2D &, bool = true);
			void GetBounds(FrameBounds&);
			void UpdateBounds(FrameBounds);
			void SetColumns(int); // starts at 1, not 0


			Vector2D GetEffectivePosition(int = 0); // this is where the next object would be placed
			Vector2D GetEffectiveSize(); // this returns the amount of space that objects can go in

		private:
			FrameBounds frame_bounds;
			std::vector<Vector2D> positions;
			Vector2D size;
			int count = 0;
		};

		struct GroupboxData
		{
			int scroll_offset;
		};
		struct VerticalTabData
		{
			int scroll_offset;
			int tab_index;
		};
		struct ComboboxData
		{
			int scroll_offset;
			bool combobox_items_open;
		};
		struct WindowData
		{
			WindowData()
			{
				scroll_offset = 0;
				is_dragging = false;
				is_scrolling = false;
				offset = Vector2D();
			}
			float scroll_offset;
			bool is_dragging;
			Vector2D offset;
			bool is_scrolling;
		};
		struct ColorPickerData
		{
			bool is_selected;
		};
		union CommandData
		{
			CommandData() {};
			bool checkbox_value;
			bool button_value;
			bool slider_selected;
			GroupboxData groupbox_data;
			VerticalTabData tab_data;
			ComboboxData combobox_data;
			WindowData window_data = WindowData();
			ColorPickerData color_picker_data;
		};

		class Menu
		{
		public:
			void Begin();
			void End();

			void ShouldDrawMouse();

			void SetFont(uint32_t);
			uint32_t GetFont();

			void SetCurrentLine(int);
			void SetCurrentColumns(int);
			int GetCurrentLine();
			int GetCurrentColumns();

			template <class T>
			Command* PushbackCommand(T command)
			{
				last_command = cmd_queue.Pushback<T>(command);
				return last_command;
			}
			template <class T>
			Command* InsertCommand(T command)
			{
				last_command = cmd_queue.Insert<T>(command);
				return last_command;
			}

			Command* GetLastCommand();
			CommandFrame& GetCurrentWindow();
			CommandFrame& GetStackFrame();

			size_t StackFrameSize();
			void PushbackStackFrame(Command*);
			void PopStackFrame();

			bool AllocateSpace(Vector2D, Vector2D &, bool = true);
			void GetBounds(FrameBounds&);
			Vector2D PeekNextPosition();
			Vector2D GetCurrentEffectiveSize();
			Vector2D GetCurrentWindowEffectiveSize();
			Vector2D GetCurrentWindowSize();

			static std::string FilterName(std::string);
			static uint32_t Hash(const void* const, int);
			CommandData& GetCommandData(uint32_t);

		private:
			CommandQueue cmd_queue;
			Command* last_command = nullptr;
			Input input;

			uint32_t current_font = -1;
			int current_line = 0;
			int current_columns = 1;
			bool should_draw_mouse = false;

			CommandFrame current_window;
			std::stack<CommandFrame> stack_frame;
			std::unordered_map<uint32_t, CommandData> command_data_map;


			std::vector<Command*> SortCommands(std::vector<Command*>); // this sorts objects by what order they should be drawn 
			void DrawMouse(Vector2D);

		}; extern Menu menu;
	}
}