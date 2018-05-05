#include "../includes.h"
#include "../utilities/render.h"

#include "menu_objects.h"


namespace MENU
{
	namespace PPGUI_PP_GUI
	{
		Menu menu;

		// leave blank 
		void Command::TestInput(Input&) {}; 
		void Command::ModifyScrollSize(int) {};

		uint32_t Command::GetID()
		{
			std::string text = Info().id_name;
			return Menu::Hash(text.data(), text.size());
		}
		std::string Command::CreateIDName()
		{
			if (!menu.StackFrameSize())
				return Info().name + std::to_string(Info().object_type);

			auto stack_frame = menu.GetStackFrame();
			auto stack_frame_info = stack_frame.command->Info();
			return std::to_string(stack_frame_info.object_type) + '_' + stack_frame_info.id_name + "->" + std::to_string(Info().object_type) + '_' + Info().name;
		}

		// CommandQueue
		CommandQueue::~CommandQueue()
		{
			for (auto &c : allocated)
				delete c;
		}
		std::vector<Command*> CommandQueue::Get()
		{
			return queue;
		}
		void CommandQueue::Clear()
		{
			queue.clear();

			for (auto &c : allocated)
				delete c;

			allocated.clear();
		}

		// CommandFrame
		CommandFrame::CommandFrame(Command* cmd, Vector2D pos, Vector2D sz, FrameBounds bounds)
		{
			command = cmd;
			positions.push_back(pos);
			frame_bounds = bounds;
			size = sz;
		}
		bool CommandFrame::AllocateSpace(Vector2D space, Vector2D &pos, bool scroll_enabled)
		{
			Vector2D free_space = size - Vector2D(FRAME_PADDING * 2.f, FRAME_PADDING * 2.f);

			// if object is a window account for columns
			bool is_window = command && command->Info().object_type == OBJECT_TYPE::OBJECT_WINDOW;
			int current_line = is_window ? menu.GetCurrentLine() : 0;
			if (is_window)
			{
				free_space.x -= FRAME_PADDING * (menu.GetCurrentColumns() - 1);
				free_space.x /= menu.GetCurrentColumns();
				pos = positions.at(current_line) + Vector2D(FRAME_PADDING, FRAME_PADDING);
				pos.x += (free_space.x + FRAME_PADDING) * current_line;
			}
			else
				pos = positions.at(0) + Vector2D(FRAME_PADDING, FRAME_PADDING);

			// if not enough room, increase scroll size
			if (pos.y + space.y + FRAME_PADDING > frame_bounds.max.y)
				command->ModifyScrollSize((pos.y + space.y + FRAME_PADDING) - frame_bounds.max.y);

			if (scroll_enabled)
				pos.y += menu.GetCommandData(command->GetID()).window_data.scroll_offset;

			positions.at(current_line).y += space.y + OBJECT_PADDING;

			count++;

			return true;
		}
		void CommandFrame::GetBounds(FrameBounds &bounds)
		{
			bounds = frame_bounds;
		}
		void CommandFrame::UpdateBounds(FrameBounds bounds)
		{
			frame_bounds = bounds;
		}
		void CommandFrame::SetColumns(int amount)
		{
			int delta = UTILS::clamp<int>(amount - positions.size(), 0, amount);
			
			for (int i = 0; i < delta; i++)
				positions.push_back(positions.back());
		}
		Vector2D CommandFrame::GetEffectivePosition(int clmn)
		{
			return positions.at(clmn) + Vector2D(FRAME_PADDING, FRAME_PADDING);
		}
		Vector2D CommandFrame::GetEffectiveSize()
		{
			return size - (Vector2D(FRAME_PADDING, FRAME_PADDING) * 2.f);
		}

		void Menu::Begin()
		{
			UTILS::INPUT::input_handler.GetKeyboardState(input.keyboard);

			auto mouse_info = UTILS::INPUT::input_handler.GetMouseInfo();
			input.mouse.position_delta = mouse_info.position - input.mouse.position;
			input.mouse.position = mouse_info.position;
			input.mouse.scroll = mouse_info.scroll;
			input.mouse.left_button = mouse_info.left;
			input.mouse.right_button = mouse_info.right;

			input.real_mouse = input.mouse;

			input.valid = true;		
		}
		void Menu::End()
		{
			if (stack_frame.size())
				throw std::runtime_error("Begin() not ended with End()");
			
			if (!input.valid)
				throw std::runtime_error("Input not provided");

			auto sorted_commands = SortCommands(cmd_queue.Get());

			for (int i = sorted_commands.size() - 1; i >= 0; i--)
				sorted_commands[i]->TestInput(input);
			for (auto cmd : sorted_commands)
				cmd->Draw();

			static POINT Cursor;

			GetCursorPos(&Cursor);

			int x = Cursor.x;
			int y = Cursor.y;

			// if they called DrawMouse once this frame draw the mouse
			if (should_draw_mouse)
				DrawMouse(Vector2D(x, y));

			cmd_queue.Clear();
			last_command = nullptr;
			input.valid = false;
			should_draw_mouse = false;
		}
		void Menu::DrawMouse(Vector2D pos)
		{
			for (int i = 0; i <= 9; i++)
			{
				RENDER::DrawLine(pos.x, pos.y, pos.x + i, pos.y + 11, WHITE);
			}
			for (int i = 0; i <= 7; i++)
			{
				RENDER::DrawLine(pos.x, pos.y + 9 + i, pos.x + i, pos.y + 9, WHITE);
			}
			for (int i = 0; i <= 3; i++)
			{
				RENDER::DrawLine(pos.x + 6 + i, pos.y + 11, pos.x, pos.y + i, WHITE);
			}
			RENDER::DrawLine(pos.x + 5, pos.y + 11, pos.x + 8, pos.y + 18, WHITE);
			RENDER::DrawLine(pos.x + 4, pos.y + 11, pos.x + 7, pos.y + 18, WHITE);

			RENDER::DrawLine(pos.x, pos.y, pos.x, pos.y + 17, BLACK);
			RENDER::DrawLine(pos.x, pos.y + 17, pos.x + 3, pos.y + 14, BLACK);

			RENDER::DrawLine(pos.x + 4, pos.y + 14, pos.x + 7, pos.y + 19, BLACK);
			RENDER::DrawLine(pos.x + 5, pos.y + 14, pos.x + 7, pos.y + 19, BLACK);

			RENDER::DrawLine(pos.x + 7, pos.y + 18, pos.x + 9, pos.y + 18, BLACK);

			RENDER::DrawLine(pos.x + 10, pos.y + 18, pos.x + 7, pos.y + 12, BLACK);

			RENDER::DrawLine(pos.x + 7, pos.y + 12, pos.x + 12, pos.y + 12, BLACK);

			RENDER::DrawLine(pos.x + 12, pos.y + 12, pos.x, pos.y, BLACK);
		}
		void Menu::SetFont(uint32_t font)
		{
			current_font = font;
		}
		uint32_t Menu::GetFont()
		{
			return current_font;
		}
		uint32_t Menu::Hash(const void* const key, int len)
		{
			unsigned char* p = const_cast<unsigned char*>(reinterpret_cast<
				const unsigned char*>(key));
			unsigned int h = 2166136261;

			for (int i = 0; i < len; i++)
				h = (h * 16777619) ^ p[i];

			return h;
		}
		void Menu::PushbackStackFrame(Command* command)
		{
			auto position = command->Info().position;
			auto size = command->Info().size;

			// account for title bar
			bool is_window = command->Info().object_type == OBJECT_TYPE::OBJECT_WINDOW;
			if (is_window)
			{
				position.y += WINDOW_TITLE_BAR_SIZE;
				size.y -= WINDOW_TITLE_BAR_SIZE;
				size -= Vector2D(OBJECT_SCROLLBAR_WIDTH + OBJECT_PADDING + OBJECT_PADDING, 0);
			}

			auto frame = FrameBounds(position, position + size);

			// how bounds work, the stack frame's bounds have to be smaller or the same size as the stack frame's bounds its currently in
			if (stack_frame.size())
			{
				auto stack_frame = GetStackFrame();

				FrameBounds stack_frame_bounds;
				stack_frame.GetBounds(stack_frame_bounds);

				// do the clamping, just for the y axis since bounds currently dont effect the x axis
				if (stack_frame_bounds.min.y > frame.min.y)
					frame.min.y = stack_frame_bounds.min.y;
				if (stack_frame_bounds.max.y < frame.max.y)
					frame.max.y = stack_frame_bounds.max.y;
			}

			current_window = CommandFrame(command, position, size, frame);
			stack_frame.push(CommandFrame(command, position, size, frame));
		}
		void Menu::PopStackFrame()
		{
			auto top = GetStackFrame();
			if (top.command->Info().object_type == OBJECT_TYPE::OBJECT_WINDOW) 
				current_window.command = nullptr;

			stack_frame.pop();
		}
		CommandFrame& Menu::GetStackFrame()
		{
			if (!stack_frame.size())
				throw std::runtime_error("No stack frame found!");

			return stack_frame.top();
		}
		Command* Menu::GetLastCommand()
		{
			return last_command;
		}
		CommandFrame& Menu::GetCurrentWindow()
		{
			return current_window;
		}
		CommandData& Menu::GetCommandData(uint32_t hash)
		{
			auto data = command_data_map.find(hash);

			if (data == command_data_map.end())
				command_data_map[hash] = CommandData();

			return command_data_map[hash];
		}
		void Menu::SetCurrentLine(int value)
		{
			current_line = value;
		}
		std::vector<Command*> Menu::SortCommands(std::vector<Command*> commands)
		{
			auto sorted_commands = commands;

			for (int i = 0; i < commands.size(); i++)
			{
				auto command = commands[i];
				auto& command_data = GetCommandData(command->GetID());
				auto info = command->Info();
				if (info.object_type == OBJECT_TYPE::OBJECT_COMBOBOX &&
					command_data.combobox_data.combobox_items_open)
				{
					sorted_commands.erase(sorted_commands.begin() + i);
					sorted_commands.push_back(command);
				}
				else if (info.object_type == OBJECT_TYPE::OBJECT_COLOR_PICKER && 
					command_data.color_picker_data.is_selected)
				{
					sorted_commands.erase(sorted_commands.begin() + i);
					sorted_commands.push_back(command);
				}
			}

			return sorted_commands;
		}
		size_t Menu::StackFrameSize()
		{
			return stack_frame.size();
		}
		bool Menu::AllocateSpace(Vector2D size, Vector2D &position, bool enable_scroll)
		{
			if (!stack_frame.size())
				return false;

			bool ret = stack_frame.top().AllocateSpace(size, position, enable_scroll);

			// this updates the bounds' min to be the bottom of the object we just allocated for objects that dont scroll
			if (!enable_scroll)
			{
				FrameBounds bounds;
				stack_frame.top().GetBounds(bounds);

				bounds.min.y = position.y + size.y + OBJECT_PADDING;
				stack_frame.top().UpdateBounds(bounds);
			}

			return ret;
		}
		void Menu::GetBounds(FrameBounds& bounds)
		{
			GetStackFrame().GetBounds(bounds);
		}
		Vector2D Menu::PeekNextPosition()
		{
			if (!stack_frame.size())
				throw std::runtime_error("Cant get next position if not in frame!");

			return stack_frame.top().GetEffectivePosition();
		}
		Vector2D Menu::GetCurrentEffectiveSize()
		{
			if (!stack_frame.size())
				throw std::runtime_error("Cant get size if not in frame!");

			return stack_frame.top().GetEffectiveSize();
		}
		Vector2D Menu::GetCurrentWindowEffectiveSize()
		{
			if (!current_window.command)
				throw std::runtime_error("Can't get effective window size when not in window!");

			return current_window.GetEffectiveSize();
		}
		Vector2D Menu::GetCurrentWindowSize()
		{
			if (!current_window.command)
				throw std::runtime_error("Can't get window size when not in window!");

			return current_window.command->Info().size;
		}
		void Menu::SetCurrentColumns(int amount)
		{
			current_columns = amount;
		}
		int Menu::GetCurrentLine()
		{
			return current_line;
		}
		int Menu::GetCurrentColumns()
		{
			return current_columns;
		}
		std::string Menu::FilterName(std::string name)
		{
			std::string ret;
			bool was_last_letter_hash = false;
			for (auto letter : name)
			{
				ret.push_back(letter);

				if (letter == '#')
				{
					if (was_last_letter_hash)
					{
						ret.pop_back();
						ret.pop_back();
						break;
					}

					was_last_letter_hash = true;
				}
			}

			return ret;
		}
		void Menu::ShouldDrawMouse()
		{
			should_draw_mouse = true;
		}
	}
}