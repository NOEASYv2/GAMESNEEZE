#include "../includes.h"
#include "../utilities/render.h"

#include "menu_commands.h"


namespace MENU {
	namespace PPGUI_PP_GUI {
		WindowCommand::WindowCommand( std::string Name, Vector2D Position, Vector2D Size, int Scroll_Size ) {
			if ( menu.GetFont( ) == -1 )
				throw std::runtime_error( "No font provided" );

			info.object_type = OBJECT_TYPE::OBJECT_WINDOW;
			info.font = menu.GetFont( );
			info.name = Name;
			info.id_name = CreateIDName( );
			info.name = Menu::FilterName( Name );
			info.position = Position + menu.GetCommandData( GetID( ) ).window_data.offset;
			info.size = Size;

			scroll_size = Scroll_Size;
		}
		void WindowCommand::Draw( ) {
			auto position = info.position;
			auto size = info.size;

			// draws the window body
			RENDER::DrawGradientRectangle(
				Vector2D( position.x, position.y ),
				Vector2D( size.x, size.y ),
				CColor( 26, 26, 26 ), CColor( 26, 26, 26 ) );


			{// draws the window border (code looks gay as fuck but looks nice in-game)
				RENDER::DrawEmptyRect( position.x - 6, position.y +
					WINDOW_TITLE_BAR_SIZE - 26, position.x + size.x + 5, position.y
					+ info.size.y + 5, CColor( 0, 0, 0, 255 ) );

				RENDER::DrawEmptyRect( position.x - 5, position.y +
					WINDOW_TITLE_BAR_SIZE - 25, position.x + size.x + 4, position.y
					+ info.size.y + 4, CColor( 60, 60, 60, 255 ) );

				RENDER::DrawEmptyRect( position.x - 4, position.y +
					WINDOW_TITLE_BAR_SIZE - 24, position.x + size.x + 3, position.y
					+ info.size.y + 3, CColor( 40, 40, 40, 255 ) );

				RENDER::DrawEmptyRect( position.x - 3, position.y +
					WINDOW_TITLE_BAR_SIZE - 23, position.x + size.x + 2, position.y
					+ info.size.y + 2, CColor( 40, 40, 40, 255 ) );

				RENDER::DrawEmptyRect( position.x - 2, position.y +
					WINDOW_TITLE_BAR_SIZE - 22, position.x + size.x + 1, position.y
					+ info.size.y + 1, CColor( 40, 40, 40, 255 ) );

				RENDER::DrawEmptyRect( position.x - 1, position.y +
					WINDOW_TITLE_BAR_SIZE - 21, position.x + size.x, position.y
					+ info.size.y, CColor( 60, 60, 60, 255 ) );
			}


			// draws the window title bar
			RENDER::DrawFilledRectOutline( position.x + 1, position.y,
				position.x + size.x - 2, position.y + WINDOW_TITLE_BAR_SIZE,
				CColor( 17, 17, 17 ) );
			// draws the colored bar
			RENDER::DrawGradientRectangle(
				Vector2D( position.x + 1, position.y ),
				Vector2D( size.x - 2, 2 ),
				Vars::options.menu_col, Vars::options.menu_col );

			std::string name1 = "game";
			std::string name2 = "sneeze";

			Vector2D textsize = RENDER::GetTextSize( info.font, name1 );

			RENDER::DrawF( position.x + 22, position.y + ( WINDOW_TITLE_BAR_SIZE * 0.5f ) + 2,
				info.font, true, true, colors [ WINDOW_TEXT ], name1 );

			RENDER::DrawF( position.x + textsize.x + 28, position.y + ( WINDOW_TITLE_BAR_SIZE * 0.5f ) + 2,
				info.font, true, true, Vars::options.menu_col, name2 );

			RENDER::DrawLine( position.x, position.y + WINDOW_TITLE_BAR_SIZE,
				position.x + size.x, position.y + WINDOW_TITLE_BAR_SIZE,
				CColor( 48, 48, 48 ) );



			/* SCROLLBAR */
			if ( !scroll_size )
				return;

			auto scrollbar_position = Vector2D( ( position.x + size.x ) - ( OBJECT_SCROLLBAR_WIDTH + OBJECT_PADDING + OBJECT_PADDING ), position.y + WINDOW_TITLE_BAR_SIZE + FRAME_PADDING );
			auto scrollbar_size = Vector2D( OBJECT_SCROLLBAR_WIDTH, size.y - ( FRAME_PADDING + FRAME_PADDING + WINDOW_TITLE_BAR_SIZE ) );
			int scrollbar_fraction = OBJECT_SCROLLBAR_FRACTION * scrollbar_size.y;

			int scrollbar_offset = ( -menu.GetCommandData( GetID( ) ).window_data.scroll_offset ) * ( ( scrollbar_size.y - scrollbar_fraction ) / scroll_size );
			scrollbar_offset = UTILS::clamp<int>( scrollbar_offset, 0, scrollbar_size.y - scrollbar_fraction );

			// draw the scrollbar
			RENDER::DrawFilledRectOutline( scrollbar_position.x, scrollbar_position.y +
				scrollbar_offset, scrollbar_position.x + scrollbar_size.x,
				scrollbar_position.y + scrollbar_fraction + scrollbar_offset,
				colors [ SCROLLBAR_BODY ] );
		}
		void WindowCommand::ModifyScrollSize( int new_scroll ) {
			scroll_size = max( scroll_size, new_scroll );
		}
		void WindowCommand::TestInput( Input& input ) {
			// dragging
			if ( input.mouse.left_button & 1 && UTILS::is_point_in_range(
				input.mouse.position, info.position, Vector2D( info.size.x,
				WINDOW_TITLE_BAR_SIZE ) ) ) {
				menu.GetCommandData( GetID( ) ).window_data.is_dragging = true;
				input.mouse.left_button = 0;
			}
			else if ( !input.real_mouse.left_button )
				menu.GetCommandData( GetID( ) ).window_data.is_dragging = false;

			// add to the window offset when dragging
			if ( menu.GetCommandData( GetID( ) ).window_data.is_dragging )
				menu.GetCommandData( GetID( ) ).window_data.offset += input.mouse.position_delta;

			if ( !UTILS::is_point_in_range( input.mouse.position, info.position, info.size ) )
				return;

			menu.GetCommandData( GetID( ) ).window_data.scroll_offset = UTILS::clamp<int>(
				menu.GetCommandData( GetID( ) ).window_data.scroll_offset, -scroll_size, 0 );

			if ( !scroll_size )
				return;

			auto scrollbar_position = Vector2D( ( info.position.x + info.size.x ) - ( OBJECT_SCROLLBAR_WIDTH + OBJECT_PADDING + OBJECT_PADDING ), info.position.y + WINDOW_TITLE_BAR_SIZE + FRAME_PADDING );
			auto scrollbar_size = Vector2D( OBJECT_SCROLLBAR_WIDTH, info.size.y - ( FRAME_PADDING + FRAME_PADDING + WINDOW_TITLE_BAR_SIZE ) );
			int scrollbar_fraction = OBJECT_SCROLLBAR_FRACTION * scrollbar_size.y;

			int scrollbar_offset = ( -menu.GetCommandData( GetID( ) ).window_data.scroll_offset ) * ( ( scrollbar_size.y - scrollbar_fraction ) / scroll_size );
			scrollbar_offset = UTILS::clamp<int>( scrollbar_offset, 0, scrollbar_size.y - scrollbar_fraction );

			auto new_scrollbar_position = scrollbar_position + Vector2D( 0, scrollbar_offset );
			auto new_scrollbar_size = Vector2D( scrollbar_size.x, scrollbar_fraction );

			// if dragging scroll bar
			if ( UTILS::is_point_in_range( input.mouse.position, new_scrollbar_position, new_scrollbar_size ) && input.mouse.left_button & 1 ) {
				menu.GetCommandData( GetID( ) ).window_data.is_scrolling = true;
				input.mouse.left_button = 0;
			}
			else if ( input.real_mouse.left_button & 1 )
				menu.GetCommandData( GetID( ) ).window_data.is_scrolling = false;

			if ( menu.GetCommandData( GetID( ) ).window_data.is_scrolling && input.mouse.left_button ) {
				input.mouse.left_button = 0;
				menu.GetCommandData( GetID( ) ).window_data.scroll_offset -= input.mouse.position_delta.y * ( scroll_size / ( scrollbar_size.y - scrollbar_fraction ) );
			}


			// scrolling
			if ( input.mouse.scroll )
				menu.GetCommandData( GetID( ) ).window_data.scroll_offset +=
				input.mouse.scroll > 0 ? OBJECT_SCROLL_SPEED : -OBJECT_SCROLL_SPEED;

			// clamp scroll offset
			menu.GetCommandData( GetID( ) ).window_data.scroll_offset = UTILS::clamp<int>(
				menu.GetCommandData( GetID( ) ).window_data.scroll_offset, -scroll_size, 0 );
		}
		CommandInfo WindowCommand::Info( ) {
			return info;
		}

		GroupboxCommand::GroupboxCommand( std::string Name, Vector2D Position, Vector2D Size, FrameBounds Bounds, int Scroll_Size ) {
			if ( menu.GetFont( ) == -1 )
				throw std::runtime_error( "No font provided" );

			info.position = Position;
			info.size = Size;
			info.object_type = OBJECT_TYPE::OBJECT_GROUPBOX;
			info.font = FONTS::menu_combobox_font;
			info.name = Name;
			info.id_name = CreateIDName( );
			info.name = Menu::FilterName( Name );
			frame_bounds = Bounds;
			scroll_size = Scroll_Size;
		}
		void GroupboxCommand::Draw( ) {
			bool is_top_in_bounds = UTILS::is_point_in_range( info.position,
				frame_bounds.min, frame_bounds.max - frame_bounds.min );
			bool is_bottom_in_bounds = UTILS::is_point_in_range(
				info.position + info.size, frame_bounds.min, frame_bounds.max -
				frame_bounds.min );

			if ( !( is_top_in_bounds || is_bottom_in_bounds ) )
				return;

			auto position = info.position;
			auto size = info.size;

			if ( !is_top_in_bounds ) {
				position.y = UTILS::clamp<int>( position.y, frame_bounds.min.y, position.y + size.y );
				size.y -= position.y - info.position.y;
			}
			if ( !is_bottom_in_bounds )
				size.y = frame_bounds.max.y - position.y;

			position.x += 5;

			// draws the groupbox body
			RENDER::DrawFilledRect( position.x, position.y,
				position.x + size.x, position.y + size.y,
				CColor( 24, 24, 24, 255) );

			if ( is_top_in_bounds ) {
				// if its an unnamed groupbox
				if ( !info.name.size( ) ) {
					RENDER::DrawLine( position.x, position.y, position.x + size.x, position.y, colors [ GROUPBOX_OUTLINE ] );
					RENDER::DrawLine( position.x, position.y - 1, position.x + size.x, position.y - 1, CColor( 0, 0, 0, 255 ) );
				}
				else {
					// draw the top outline so that it doesn't touch the text
					RENDER::DrawLine( position.x, position.y, position.x
						+ 16, position.y, colors [ GROUPBOX_OUTLINE ] );
					RENDER::DrawLine( position.x + 24 + RENDER::GetTextSize( info.font,
						info.name ).x, position.y, position.x + size.x,
						position.y, colors [ GROUPBOX_OUTLINE ] );

					RENDER::DrawLine( position.x, position.y - 1, position.x
						+ 16, position.y - 1, CColor::CColor( 0, 0, 0, 255 ) );
					RENDER::DrawLine( position.x + 24 + RENDER::GetTextSize( info.font,
						info.name ).x, position.y - 1, position.x + size.x + 1,
						position.y - 1, CColor::CColor( 0, 0, 0, 255 ) );

					RENDER::DrawF( position.x + 20, position.y,
						info.font, false, true, colors [ GROUPBOX_TEXT ], info.name );
				}
			}

			// draw the left and right outline
			RENDER::DrawEmptyRect( position.x, position.y, position.x + size.x,
				position.y + size.y, colors [ GROUPBOX_OUTLINE ], 0b1 | 0b100 );

			RENDER::DrawEmptyRect( position.x - 1, position.y - 1, position.x + size.x + 1,
				position.y + size.y + 1, CColor::CColor( 0, 0, 0, 255 ), 0b1 | 0b100 );

			if ( is_bottom_in_bounds ) {
				RENDER::DrawLine( position.x + size.x, position.y +
					size.y, position.x, position.y + size.y,
					colors [ GROUPBOX_OUTLINE ] );

				RENDER::DrawLine( position.x + size.x, position.y +
					size.y + 1, position.x, position.y + size.y + 1,
					CColor::CColor( 0, 0, 0, 255 ) );
			}
		}
		void GroupboxCommand::ModifyScrollSize( int new_scroll ) {
			scroll_size = max( scroll_size, new_scroll );
		}
		void GroupboxCommand::TestInput( Input& input ) {
			if ( !UTILS::is_point_in_range( input.mouse.position, info.position, info.size ) )
				return;

			// clamp scrolling even if we aren't scrolling since it might get changed with ModifyScrollSize()
			menu.GetCommandData( GetID( ) ).window_data.scroll_offset = UTILS::clamp<int>(
				menu.GetCommandData( GetID( ) ).window_data.scroll_offset, -scroll_size, 0 );
			if ( !input.mouse.scroll )
				return;

			// scrolling
			menu.GetCommandData( GetID( ) ).window_data.scroll_offset +=
				input.mouse.scroll > 0 ? OBJECT_SCROLL_SPEED : -OBJECT_SCROLL_SPEED;
			menu.GetCommandData( GetID( ) ).window_data.scroll_offset = UTILS::clamp<int>(
				menu.GetCommandData( GetID( ) ).window_data.scroll_offset, -scroll_size, 0 );

			input.mouse.scroll = 0;
		}

		CommandInfo GroupboxCommand::Info( ) {
			return info;
		}

		SeparatorCommand::SeparatorCommand( std::string name, Vector2D position, Vector2D size, FrameBounds bounds ) {
			if ( menu.GetFont( ) == -1 )
				throw std::runtime_error( "No font provided" );

			info.position = position;
			info.size = size;
			info.object_type = OBJECT_TYPE::OBJECT_SEPARATOR;
			info.font = menu.GetFont( );
			info.name = name;
			info.id_name = CreateIDName( );
			info.name = Menu::FilterName( name );
			frame_bounds = bounds;
		}
		void SeparatorCommand::Draw( ) {
			auto text_size = RENDER::GetTextSize( info.font, info.name );
			auto size = info.size;
			size.y = text_size.y;
			auto position = info.position;
			auto center = position + ( size * 0.5f );

			// make sure in bounds
			if ( !UTILS::is_point_in_range( info.position, frame_bounds.min, frame_bounds.max - frame_bounds.min ) ||
				!UTILS::is_point_in_range( info.position + size, frame_bounds.min, frame_bounds.max - frame_bounds.min ) )
				return;

			RENDER::DrawF( position.x + SEPARATOR_TEXT_OFFSET, position.y + ( size.y * 0.5f ), info.font, false, true, colors [ SEPARATOR_TEXT ], info.name );

			RENDER::DrawLine( position.x, center.y, position.x + ( SEPARATOR_TEXT_OFFSET - 5 ), center.y, colors [ SEPARATOR_LINE ] );
			RENDER::DrawLine( position.x + SEPARATOR_TEXT_OFFSET + 5 + text_size.x, center.y, position.x + size.x, center.y, colors [ SEPARATOR_LINE ] );
		}
		CommandInfo SeparatorCommand::Info( ) {
			return info;
		}

		CheckboxCommand::CheckboxCommand( std::string Name, Vector2D Position, Vector2D Size, FrameBounds bounds, bool& variable ) {
			if ( menu.GetFont( ) == -1 )
				throw std::runtime_error( "No font provided" );

			value = &variable;

			info.position = Position;
			info.size = Size;
			info.object_type = OBJECT_TYPE::OBJECT_CHECKBOX;
			info.font = menu.GetFont( );
			info.name = Name;
			info.id_name = CreateIDName( );
			info.name = Menu::FilterName( Name );

			frame_bounds = bounds;

			menu.GetCommandData( GetID( ) ).checkbox_value = variable;
		}
		CheckboxCommand::CheckboxCommand( std::string Name, Vector2D Position, Vector2D Size, FrameBounds bounds ) {
			if ( menu.GetFont( ) == -1 )
				throw std::runtime_error( "No font provided" );

			info.position = Position;
			info.size = Size;
			info.object_type = OBJECT_TYPE::OBJECT_CHECKBOX;
			info.font = menu.GetFont( );
			info.name = Name;
			info.id_name = CreateIDName( );
			info.name = Menu::FilterName( Name );

			frame_bounds = bounds;
		}
		void CheckboxCommand::Draw( ) {
			bool is_top_in_bounds = UTILS::is_point_in_range( info.position,
				frame_bounds.min, frame_bounds.max - frame_bounds.min );
			bool is_bottom_in_bounds = UTILS::is_point_in_range(
				info.position + info.size, frame_bounds.min, frame_bounds.max -
				frame_bounds.min );

			// dont draw if not in window
			if ( !( is_top_in_bounds || is_bottom_in_bounds ) )
				return;
			auto position = info.position;
			auto size = info.size;
			if ( !is_top_in_bounds ) {
				position.y = UTILS::clamp<int>( position.y, frame_bounds.min.y, position.y + size.y );
				size.y -= position.y - info.position.y;
			}
			if ( !is_bottom_in_bounds )
				size.y = frame_bounds.max.y - position.y;

			RENDER::DrawGradientRectangle( Vector2D( position.x + 4, position.y + 4 ),
				Vector2D( size.x - 8, size.x - 8 ),
				CColor( 76, 76, 76 ), CColor( 51, 51, 51, 255 ) );

			RENDER::DrawEmptyRect( position.x + 3, position.y + 3,
				position.x + size.x - 4, position.y + size.y - 4,
				CColor( 0, 0, 0, 255 ) );

			// if clicked
			if ( menu.GetCommandData( GetID( ) ).checkbox_value ) {
				RENDER::DrawGradientRectangle( Vector2D( position.x + 4, position.y + 4 ),
					Vector2D( info.size.x - 8, info.size.y - 8 ),
					Vars::options.menu_col, CColor( 0, 0, 0, 200 ) );
			}

			// only draw if entire checkbox is in view, ghetto replacement for clipping
			if ( !( is_top_in_bounds && is_bottom_in_bounds ) )
				return;

			auto center = info.position + ( info.size * 0.5f );
			RENDER::DrawF( info.position.x + info.size.x + 5, center.y, info.font,
				false, true, colors [ CHECKBOX_TEXT ], info.name );
		}
		void CheckboxCommand::TestInput( Input& input ) {
			if ( !UTILS::is_point_in_range( input.mouse.position, frame_bounds.min,
				frame_bounds.max - frame_bounds.min ) )
				return;

			bool mouse_over_checkbox = UTILS::is_point_in_range( input.mouse.position,
				info.position, info.size );

			if ( mouse_over_checkbox && input.mouse.left_button & 1 ) {
				menu.GetCommandData( GetID( ) ).checkbox_value = !menu.GetCommandData( GetID( ) ).checkbox_value;
				if ( value )
					*value = menu.GetCommandData( GetID( ) ).checkbox_value;

				input.mouse.left_button = 0; // clear the button so that no other objects recieve input
			}
		}
		CommandInfo CheckboxCommand::Info( ) {
			return info;
		}

		ButtonCommand::ButtonCommand( std::string name, Vector2D position, Vector2D size, FrameBounds bounds ) {
			if ( menu.GetFont( ) == -1 )
				throw std::runtime_error( "No font provided" );

			info.position = position;
			info.object_type = OBJECT_TYPE::OBJECT_BUTTON;
			info.font = menu.GetFont( );
			info.size = size;
			info.name = name;
			info.id_name = CreateIDName( );
			info.name = Menu::FilterName( name );

			frame_bounds = bounds;
		}
		void ButtonCommand::Draw( ) {
			bool is_top_in_bounds = UTILS::is_point_in_range( info.position,
				frame_bounds.min, frame_bounds.max - frame_bounds.min );
			bool is_bottom_in_bounds = UTILS::is_point_in_range(
				info.position + info.size, frame_bounds.min, frame_bounds.max -
				frame_bounds.min );

			// dont draw if not in window
			if ( !( is_top_in_bounds || is_bottom_in_bounds ) )
				return;

			auto position = info.position;
			auto size = info.size;
			if ( !is_top_in_bounds ) {
				position.y = UTILS::clamp<int>( position.y, frame_bounds.min.y, position.y + size.y );
				size.y -= position.y - info.position.y;
			}
			if ( !is_bottom_in_bounds )
				size.y = frame_bounds.max.y - position.y;

			RENDER::DrawFilledRect( position.x, position.y,
				position.x + size.x, position.y + size.y,
				colors [ BUTTON_BODY ] );
			RENDER::DrawEmptyRect( position.x - 1, position.y - 1,
				position.x + size.x, position.y + size.y,
				CColor( 0, 0, 0, 100 ) );

			// only draw if entire button is in view
			if ( !( is_top_in_bounds && is_bottom_in_bounds ) )
				return;

			auto center = info.position + ( info.size * 0.5f );
			RENDER::DrawF( center.x, center.y, info.font, true, true,
				colors [ BUTTON_TEXT ], info.name );
		}
		void ButtonCommand::TestInput( Input& input ) {
			menu.GetCommandData( GetID( ) ).button_value = false;

			if ( !UTILS::is_point_in_range( input.mouse.position, frame_bounds.min,
				frame_bounds.max - frame_bounds.min ) )
				return;

			bool mouse_over_button = UTILS::is_point_in_range( input.mouse.position,
				info.position, info.size );

			if ( mouse_over_button && input.mouse.left_button & 1 ) {
				menu.GetCommandData( GetID( ) ).button_value = true;
				input.mouse.left_button = 0;
			}
		}
		CommandInfo ButtonCommand::Info( ) {
			return info;
		}

		ComboboxCommand::ComboboxCommand( std::string name, Vector2D position, Vector2D size, FrameBounds bounds, std::vector<std::string> Items, int& var ) {
			if ( menu.GetFont( ) == -1 )
				throw std::runtime_error( "No font provided" );

			var = UTILS::clamp<int>( var, 0, Items.size( ) - 1 );
			variable = &var;

			items = Items;

			info.position = position;
			info.object_type = OBJECT_TYPE::OBJECT_COMBOBOX;
			info.font = menu.GetFont( );
			info.size = size;
			info.name = name;
			info.id_name = CreateIDName( );
			info.name = Menu::FilterName( name );

			frame_bounds = bounds;
		}
		void ComboboxCommand::Draw( ) {
			auto position = info.position;
			auto size = info.size;
			auto center = position + ( size * 0.5f );
			bool items_open = menu.GetCommandData( GetID( ) ).combobox_data.combobox_items_open;

			// check if in bounds
			bool top_in_bounds = UTILS::is_point_in_range( position, frame_bounds.min,
				frame_bounds.max - frame_bounds.min );
			bool bottom_in_bounds = UTILS::is_point_in_range( position + size,
				frame_bounds.min, frame_bounds.max - frame_bounds.min );

			if ( !( top_in_bounds || bottom_in_bounds ) )
				return;

			auto clamped_position = position;
			auto clamped_size = size;
			if ( !top_in_bounds ) {
				clamped_size.y -= frame_bounds.min.y - position.y;
				clamped_position.y = frame_bounds.min.y;
			}
			if ( !bottom_in_bounds )
				clamped_size.y = frame_bounds.max.y - position.y;

			// draw the selected item rectangle
			RENDER::DrawGradientRectangle( Vector2D( clamped_position.x, clamped_position.y ),
				Vector2D( clamped_size.x, clamped_size.y ), colors [ COMBOBOX_SELECTED ], CColor( 0, 0, 0, 150 ) );
			/*

			clamped_position.x, clamped_position.y, clamped_position.x +
			clamped_size.x, clamped_position.y + clamped_size.y, colors [ COMBOBOX_SELECTED ]
			*/
			// manually draw the outline cuz phat
			RENDER::DrawEmptyRect( clamped_position.x - 1, clamped_position.y - 1, clamped_position.x +
				clamped_size.x, clamped_position.y + clamped_size.y, CColor( 0, 0, 0, 100 ) );

			if ( top_in_bounds && bottom_in_bounds ) {
				// draw the selected item
				RENDER::DrawF( position.x + 5, center.y, info.font, false,
					true, colors [ COMBOBOX_SELECTED_TEXT ], items [ *variable ] );

				// draw the name of the combo
				RENDER::DrawF( position.x + size.x + 5, center.y, info.font, false,
					true, colors [ COMBOBOX_TEXT ], info.name );
			}

			if ( !items_open || !bottom_in_bounds )
				return;

			int current_item = max( 0, menu.GetCommandData( GetID( ) ).combobox_data.scroll_offset / size.y );

			// draw the items
			for ( int i = current_item; i < min( COMBOBOX_MAX_ITEMS_AT_ONCE + current_item, items.size( ) ); i++ ) {
				position.y += info.size.y;

				RENDER::DrawFilledRect( position.x, position.y, position.x
					+ size.x, position.y + size.y, colors [ COMBOBOX_ITEM ] );

				center.y += info.size.y;
				RENDER::DrawF( position.x + 5, center.y, info.font, false,
					true, colors [ COMBOBOX_ITEM_TEXT ], items [ i ] );
			}
			// draw an outline around the items
			RENDER::DrawEmptyRect( info.position.x - 1, info.position.y + info.size.y - 1,
				info.position.x + info.size.x, info.position.y + ( info.size.y *
				( min( COMBOBOX_MAX_ITEMS_AT_ONCE, items.size( ) ) + 1 ) ), CColor( 0, 0, 0, 100 ), 0b1 );
		}
		void ComboboxCommand::TestInput( Input& input ) {
			int current_item = max( 0, menu.GetCommandData( GetID( ) ).combobox_data.scroll_offset / info.size.y );
			int item_count = min( COMBOBOX_MAX_ITEMS_AT_ONCE + current_item, items.size( ) );

			bool combobox_open = menu.GetCommandData( GetID( ) ).combobox_data.combobox_items_open;
			bool mouse_over_combobox = UTILS::is_point_in_range( input.mouse.position,
				info.position, info.size );
			bool mouse_over_item = UTILS::is_point_in_range( input.mouse.position,
				info.position + Vector2D( 0, info.size.y ), Vector2D( info.size.x, info.size.y
				* min( items.size( ), COMBOBOX_MAX_ITEMS_AT_ONCE ) ) );

			// half assed the clipping for comboboxes cuz FUCK math and FUCK you
			bool combobox_within_bounds = UTILS::is_point_in_range( info.position,
				frame_bounds.min, frame_bounds.max - frame_bounds.min );
			if ( !combobox_within_bounds ) {
				menu.GetCommandData( GetID( ) ).combobox_data.scroll_offset = 0;
				menu.GetCommandData( GetID( ) ).combobox_data.combobox_items_open = false;
				return;
			}

			if ( mouse_over_combobox && input.mouse.left_button & 1 ) {
				if ( combobox_open ) {
					// if double click the combobox, accept text input and search for the closest string in items
				}
				menu.GetCommandData( GetID( ) ).combobox_data.combobox_items_open = true;
				input.mouse.left_button = 0;
			}
			else if ( mouse_over_item && input.mouse.left_button & 1 &&
				combobox_open ) {
				int item_selected = current_item + UTILS::clamp<int>( floorf( ( input.mouse.position.y -
					( info.position.y + info.size.y ) ) / info.size.y ), 0, item_count - 1 );
				*variable = item_selected;

				input.mouse.left_button = 0;
				menu.GetCommandData( GetID( ) ).combobox_data.scroll_offset = 0;
				menu.GetCommandData( GetID( ) ).combobox_data.combobox_items_open = false;
			}
			else if ( input.real_mouse.left_button & 1 ) // if clicked somewhere else on the screen close the combobox
			{
				menu.GetCommandData( GetID( ) ).combobox_data.scroll_offset = 0;
				menu.GetCommandData( GetID( ) ).combobox_data.combobox_items_open = false;
			}


			if ( !input.mouse.scroll || !( mouse_over_combobox || mouse_over_item ) || !combobox_open )
				return;

			// scrolling
			menu.GetCommandData( GetID( ) ).combobox_data.scroll_offset +=
				input.mouse.scroll < 0 ? OBJECT_SCROLL_SPEED : -OBJECT_SCROLL_SPEED;
			menu.GetCommandData( GetID( ) ).combobox_data.scroll_offset = UTILS::clamp<int>(
				menu.GetCommandData( GetID( ) ).combobox_data.scroll_offset, 0,
				info.size.y * ( max( 0, items.size( ) - COMBOBOX_MAX_ITEMS_AT_ONCE ) ) );
			input.mouse.scroll = 0;
		}
		CommandInfo ComboboxCommand::Info( ) {
			return info;
		}

		SliderCommand::SliderCommand( std::string name, Vector2D position, Vector2D size, FrameBounds bounds, float min, float max, float& var, int decimal_places ) {
			if ( menu.GetFont( ) == -1 )
				throw std::runtime_error( "No font provided" );

			var = UTILS::clamp<float>( var, min, max );
			variable = &var;

			info.position = position;
			info.object_type = OBJECT_TYPE::OBJECT_SLIDER;
			info.font = menu.GetFont( );
			info.size = size;
			info.name = name;
			info.id_name = CreateIDName( );
			info.name = Menu::FilterName( name );
			decimal_place = decimal_places;

			slider_min = min;
			slider_max = max;

			frame_bounds = bounds;
		}
		void SliderCommand::Draw( ) {
			// determine whether top and bottom of slider is in frame bounds
			bool is_top_in_bounds = UTILS::is_point_in_range( info.position,
				frame_bounds.min, frame_bounds.max - frame_bounds.min );
			bool is_bottom_in_bounds = UTILS::is_point_in_range(
				info.position + info.size, frame_bounds.min, frame_bounds.max -
				frame_bounds.min );

			// dont draw if not in window
			if ( !( is_top_in_bounds || is_bottom_in_bounds ) )
				return;

			// clamp position and size to frame bounds
			auto position = info.position;
			auto size = info.size;
			if ( !is_top_in_bounds ) {
				position.y = UTILS::clamp<int>( position.y, frame_bounds.min.y, position.y + size.y );
				size.y -= position.y - info.position.y;
			}
			if ( !is_bottom_in_bounds )
				size.y = frame_bounds.max.y - position.y;

			{ //plus on the right side of the slider
				RENDER::DrawLine( position.x + size.x + 4, position.y + 6,
					position.x + size.x + 2, position.y + 6, CColor( 100, 100, 100, 255 ) );

				RENDER::DrawLine( position.x + size.x + 3, position.y + 5,
					position.x + size.x + 3, position.y + 8, CColor( 100, 100, 100, 255 ) );
			}

			{//minus on the left side of the slider
				RENDER::DrawLine( position.x - 5, position.y + 6,
					position.x - 2, position.y + 6, CColor( 100, 100, 100, 255 ) );
			}

			// draw the "body" of the slider, the background so to speak
			RENDER::DrawFilledRect( position.x, position.y + 4, position.x
				+ size.x, position.y + size.y - 4, colors [ SLIDER_BODY ] );

			RENDER::DrawGradientRectangle( Vector2D( position.x, position.y + 4 ),
				Vector2D( size.x, size.y - 8 ), CColor( 52, 52, 52, 255 ), CColor( 68, 68, 68, 255 ) );

			RENDER::DrawEmptyRect( position.x - 1, position.y + 3, position.x
				+ size.x, position.y + size.y - 4, CColor( 0, 0, 0, 255 ) );

			// translate value to pixels
			float fraction = UTILS::GetFraction( *variable, slider_min, slider_max );
			float filled_x_position = UTILS::GetValueFromFraction( fraction, info.position.x, info.position.x + info.size.x );

			if ( *variable > slider_min ) {
				// draw the filled part of the slider
				RENDER::DrawGradientRectangle( Vector2D( position.x, position.y + 4 ),
					Vector2D( filled_x_position - position.x, 6 ), Vars::options.menu_col, CColor( 0, 0, 0, 200 ) );

				/*
				position.x, position.y + 4, filled_x_position,
				position.y + size.y - 4, Vars::options.menu_col
				*/
			}


			// only draw the text if the entire slider is in view
			if ( is_bottom_in_bounds && is_top_in_bounds ) {
				auto center = position + ( size * 0.5f );

				// draw the name of the slider on the left
				RENDER::DrawF( position.x - 1, center.y - 9, info.font, false, true,
					colors [ SLIDER_TEXT ], info.name );

				// draw the value of the slider on the right
				auto value = UTILS::FloatToString( *variable, decimal_place );
				RENDER::DrawF( position.x + size.x - ( RENDER::GetTextSize( info.font,
					value ).x - 1 ), center.y - 9, info.font, false, true, colors [ SLIDER_TEXT ], value );
			}

		}
		void SliderCommand::TestInput( Input& input ) {
			if ( !UTILS::is_point_in_range( input.mouse.position, frame_bounds.min,
				frame_bounds.max - frame_bounds.min ) )
				return;

			bool mouse_over_slider = UTILS::is_point_in_range( input.mouse.position,
				info.position, info.size );

			// set whether slider is selected 
			if ( input.mouse.left_button & 1 && mouse_over_slider )
				menu.GetCommandData( GetID( ) ).slider_selected = true;
			else if ( input.real_mouse.left_button & 1 )
				menu.GetCommandData( GetID( ) ).slider_selected = false;

			if ( input.mouse.left_button && menu.GetCommandData( GetID( ) ).slider_selected ) {
				float fraction = UTILS::GetFraction( input.mouse.position.x, info.position.x, info.position.x + info.size.x );
				float value = UTILS::GetValueFromFraction( fraction, slider_min, slider_max );

				*variable = UTILS::clamp<float>( value, slider_min, slider_max );
			}
		}
		CommandInfo SliderCommand::Info( ) {
			return info;
		}

		TabCommand::TabCommand( std::string name, Vector2D position, Vector2D size, FrameBounds bounds, std::vector<std::string> Items, Vector2D offset ) {
			if ( menu.GetFont( ) == -1 )
				throw std::runtime_error( "No font provided" );

			items = Items;

			info.position = position;
			info.position += offset;
			info.object_type = OBJECT_TYPE::OBJECT_TAB;
			info.font = menu.GetFont( );
			info.size = size;
			info.name = name;
			info.id_name = CreateIDName( );
			info.name = Menu::FilterName( name );

			frame_bounds = bounds;
		}
		void TabCommand::Draw( ) {
			const auto size = info.size + Vector2D( TAB_PADDING, 0 );
			const auto size_per_item = Vector2D( size.x / items.size( ) - TAB_PADDING, size.y );

			for ( int i = 0; i < items.size( ); i++ ) {
				auto position = info.position + ( Vector2D( size.x / items.size( ), 0 ) * i );
				auto center = position + ( Vector2D( size_per_item.x, size.y ) * 0.5f );

				bool is_top_in_bounds = UTILS::is_point_in_range( position,
					frame_bounds.min, frame_bounds.max - frame_bounds.min );
				bool is_bottom_in_bounds = UTILS::is_point_in_range(
					position + size_per_item, frame_bounds.min, frame_bounds.max -
					frame_bounds.min );

				// dont draw if not in window         
				if ( !( is_top_in_bounds || is_bottom_in_bounds ) )
					continue;

				// clamp the position and size to the window bounds
				auto clamped_size = size_per_item;
				if ( !is_top_in_bounds ) {
					auto original_pos = position;
					position.y = UTILS::clamp<int>( position.y, frame_bounds.min.y, position.y + size.y );
					clamped_size.y -= position.y - original_pos.y;
				}
				if ( !is_bottom_in_bounds )
					clamped_size.y = frame_bounds.max.y - position.y;


				// draw the tab body
				RENDER::DrawGradientRectangle(
					Vector2D( position.x + 5, position.y ),
					Vector2D( clamped_size.x + 4, clamped_size.y ),
					colors [ TAB_BODY ], CColor( 26, 26, 26, 255 ) );

				// draw the left and right outline
				unsigned char outline_flags = 0;
				if ( !is_top_in_bounds )
					outline_flags = 0b1;
				else if ( !is_bottom_in_bounds )
					outline_flags = 0b100;

				// draw outline men
				RENDER::DrawEmptyRect( position.x + 6, position.y - 1, position.x + clamped_size.x + 9,
					position.y + clamped_size.y - 1, CColor( 50, 50, 50, 255 ), outline_flags );
				RENDER::DrawEmptyRect( position.x + 5, position.y - 2, position.x + clamped_size.x + 10,
					position.y + clamped_size.y, CColor( 0, 0, 0, 255 ), outline_flags );


				// draw the text but only if the entire tab is in view
				if ( is_top_in_bounds && is_bottom_in_bounds )
					RENDER::DrawF( center.x + 8, center.y, info.font, true, true,
					colors [ TAB_TEXT ], items [ i ] );

				// if selected
				if ( menu.GetCommandData( GetID( ) ).tab_data.tab_index == i ) {

					RENDER::DrawGradientRectangle(
						Vector2D( position.x + 5, position.y ),
						Vector2D( clamped_size.x + 4, clamped_size.y ),
						CColor( 0, 0, 0, 125 ), colors [ TAB_BODY ] );

					if ( is_top_in_bounds && is_bottom_in_bounds )
						RENDER::DrawF( center.x + 8, center.y, info.font, true, true,
						colors [ TAB_TEXT_SELECTED ], items [ i ] );
				}
			}
		}
		void TabCommand::TestInput( Input& input ) {
			if ( !UTILS::is_point_in_range( input.mouse.position, frame_bounds.min,
				frame_bounds.max - frame_bounds.min ) )
				return;

			auto size = info.size + Vector2D( TAB_PADDING, 0 );
			auto size_per_item = Vector2D( size.x / items.size( ) - TAB_PADDING, 0 );

			for ( int i = 0; i < items.size( ); i++ ) {
				auto position = info.position + ( Vector2D( size.x / items.size( ), 0 ) * i );
				auto center = position + ( Vector2D( size_per_item.x, size.y ) * 0.5f );

				bool mouse_over_item = UTILS::is_point_in_range( input.mouse.position,
					position, Vector2D( size_per_item.x, size.y ) );

				if ( mouse_over_item && input.mouse.left_button & 1 ) {
					menu.GetCommandData( GetID( ) ).tab_data.tab_index = i;
					input.mouse.left_button = 0;
					break;
				}
			}
		}
		CommandInfo TabCommand::Info( ) {
			return info;
		}

		VerticalTabCommand::VerticalTabCommand( std::string name, Vector2D position, Vector2D size, FrameBounds bounds, std::vector<std::string> Items ) {
			if ( menu.GetFont( ) == -1 )
				throw std::runtime_error( "No font provided" );

			items = Items;

			info.position = position;
			info.object_type = OBJECT_TYPE::OBJECT_VERTICAL_TAB;
			info.font = menu.GetFont( );
			info.size = size;
			info.name = name;
			info.id_name = CreateIDName( );
			info.name = Menu::FilterName( name );

			frame_bounds = bounds;

			if ( menu.GetCommandData( GetID( ) ).tab_data.tab_index < 0 ||
				menu.GetCommandData( GetID( ) ).tab_data.tab_index >= items.size( ) )
				menu.GetCommandData( GetID( ) ).tab_data.tab_index = 0;
		}
		void VerticalTabCommand::Draw( ) {
			// calculate size per item
			const auto size = info.size;
			const auto size_per_item = Vector2D( size.x, ( ( size.y - ( ( items.size( ) - 1 ) *
				VERTICAL_TAB_PADDING ) ) / items.size( ) ) );

			// increment this by size_per_item for every item
			auto current_position = info.position;

			// for scrolling
			int current_line = max( 0, min( static_cast<int>( items.size( ) ) - VERTICAL_TAB_MAX_ITEMS_AT_ONCE, menu.GetCommandData( GetID( ) ).tab_data.scroll_offset / size_per_item.y ) );
			int items_count = min( VERTICAL_TAB_MAX_ITEMS_AT_ONCE + current_line, items.size( ) );
			for ( int i = current_line; i < items_count; i++ ) {
				bool is_item_selected = ( i == menu.GetCommandData( GetID( ) ).tab_data.tab_index );

				auto draw_position = current_position;
				auto draw_size = size_per_item;

				current_position.y += size_per_item.y + VERTICAL_TAB_PADDING;

				// check if in bounds
				bool top_in_bounds = UTILS::is_point_in_range( draw_position,
					frame_bounds.min, frame_bounds.max - frame_bounds.min );
				bool bottom_in_bounds = UTILS::is_point_in_range( draw_position +
					draw_size, frame_bounds.min, frame_bounds.max - frame_bounds.min );

				if ( !bottom_in_bounds && !top_in_bounds )
					continue;

				// handle clipping
				if ( !top_in_bounds ) {
					draw_size.y -= frame_bounds.min.y - draw_position.y;
					draw_position.y = frame_bounds.min.y;
				}
				else if ( !bottom_in_bounds )
					draw_size.y = frame_bounds.max.y - draw_position.y;

				// draw the item body
				RENDER::DrawFilledRect( draw_position.x, draw_position.y,
					draw_position.x + draw_size.x, draw_position.y +
					draw_size.y, is_item_selected ? colors [ VERTICAL_TAB_BODY_SELECTED ] :
					colors [ VERTICAL_TAB_BODY ] );

				// draw the item name
				if ( top_in_bounds && bottom_in_bounds )
					RENDER::DrawF( draw_position.x + 5, draw_position.y +
					( size_per_item.y * 0.5f ), info.font, false, true,
					colors [ VERTICAL_TAB_TEXT ], items [ i ] );
			}

			bool top_in_bounds = UTILS::is_point_in_range( info.position, frame_bounds.min,
				frame_bounds.max - frame_bounds.min );
			bool bottom_in_bounds = UTILS::is_point_in_range(
				Vector2D( current_position.x + size_per_item.x, current_position.y ),
				frame_bounds.min, frame_bounds.max - frame_bounds.min );

			// dont draw outline if no part is visible
			if ( !( top_in_bounds || bottom_in_bounds ) )
				return;

			// clip position and size
			auto clipped_position = info.position;
			auto clipped_size = Vector2D( info.size.x, current_position.y - info.position.y );
			if ( !top_in_bounds ) {
				clipped_size.y -= frame_bounds.min.y - clipped_position.y;
				clipped_position.y = frame_bounds.min.y;
			}
			else if ( !bottom_in_bounds )
				clipped_size.y = frame_bounds.max.y - clipped_position.y;

			// draw outline
			RENDER::DrawEmptyRect( clipped_position.x - 1, clipped_position.y - 1,
				clipped_position.x + clipped_size.x, clipped_position.y + clipped_size.y,
				colors [ VERTICAL_TAB_OUTLINE ] );
		}
		void VerticalTabCommand::TestInput( Input& input ) {
			if ( !UTILS::is_point_in_range( input.mouse.position, frame_bounds.min,
				frame_bounds.max - frame_bounds.min ) )
				return;

			// calculate size per item
			const auto size = info.size;
			const auto size_per_item = Vector2D( size.x, ( ( size.y - ( ( items.size( ) - 1 ) *
				VERTICAL_TAB_PADDING ) ) / items.size( ) ) );

			if ( !UTILS::is_point_in_range( input.mouse.position,
				info.position, Vector2D( size.x, size_per_item.y * VERTICAL_TAB_MAX_ITEMS_AT_ONCE ) ) )
				return;

			// increment this by size_per_item for every item
			auto current_position = info.position;

			// for when there's many items
			int current_line = max( 0, min( static_cast<int>( items.size( ) ) - VERTICAL_TAB_MAX_ITEMS_AT_ONCE, menu.GetCommandData( GetID( ) ).tab_data.scroll_offset / size_per_item.y ) );
			int items_count = min( VERTICAL_TAB_MAX_ITEMS_AT_ONCE + current_line, items.size( ) );
			for ( int i = current_line; i < items_count; i++ ) {
				bool is_item_selected = ( i == menu.GetCommandData( GetID( ) ).tab_data.tab_index );

				auto draw_position = current_position;
				auto draw_size = size_per_item;

				current_position.y += size_per_item.y + VERTICAL_TAB_PADDING;

				// check if in bounds
				bool top_in_bounds = UTILS::is_point_in_range( draw_position,
					frame_bounds.min, frame_bounds.max - frame_bounds.min );
				bool bottom_in_bounds = UTILS::is_point_in_range( draw_position +
					draw_size, frame_bounds.min, frame_bounds.max - frame_bounds.min );

				if ( !bottom_in_bounds && !top_in_bounds )
					continue;

				// handle clipping
				if ( !top_in_bounds ) {
					draw_size.y -= frame_bounds.min.y - draw_position.y;
					draw_position.y = frame_bounds.min.y;
				}
				else if ( !bottom_in_bounds )
					draw_size.y = frame_bounds.max.y - draw_position.y;

				if ( UTILS::is_point_in_range( input.mouse.position, draw_position, draw_size ) &&
					input.mouse.left_button & 1 ) {
					input.mouse.left_button = 0;
					menu.GetCommandData( GetID( ) ).tab_data.tab_index = i;
				}
			}

			if ( !input.mouse.scroll )
				return;

			// scrolling
			menu.GetCommandData( GetID( ) ).tab_data.scroll_offset +=
				input.mouse.scroll < 0 ? OBJECT_SCROLL_SPEED : -OBJECT_SCROLL_SPEED;
			menu.GetCommandData( GetID( ) ).tab_data.scroll_offset = UTILS::clamp<int>(
				menu.GetCommandData( GetID( ) ).tab_data.scroll_offset, 0, min(
				VERTICAL_TAB_MAX_ITEMS_AT_ONCE, items.size( ) -
				VERTICAL_TAB_MAX_ITEMS_AT_ONCE ) * size_per_item.y );

			input.mouse.scroll = 0;
		}
		CommandInfo VerticalTabCommand::Info( ) {
			return info;
		}

		ColorPickerCommand::ColorPickerCommand( std::string name, Vector2D position, Vector2D size, FrameBounds bounds, CColor &var ) {
			if ( menu.GetFont( ) == -1 )
				throw std::runtime_error( "No font provided" );

			variable = &var;

			info.position = position;
			info.object_type = OBJECT_TYPE::OBJECT_COLOR_PICKER;
			info.font = menu.GetFont( );
			info.size = size;
			info.name = name;
			info.id_name = CreateIDName( );
			info.name = Menu::FilterName( name );

			frame_bounds = bounds;
		}
		void ColorPickerCommand::Draw( ) {
			auto& command_data = menu.GetCommandData( GetID( ) ).color_picker_data;

			// ignore alpha for drawing the preview color
			CColor color_max_alpha = *variable;
			color_max_alpha.RGBA [ 3 ] = 255;

			auto position = info.position;
			auto size = Vector2D( info.size.x, COLOR_PICKER_CLOSED_HEIGHT );

			bool is_top_in_bounds = UTILS::is_point_in_range( position, frame_bounds.min, frame_bounds.max - frame_bounds.min );
			bool is_bottom_in_bounds = UTILS::is_point_in_range( position + size, frame_bounds.min, frame_bounds.max - frame_bounds.min );

			if ( !( is_top_in_bounds || is_bottom_in_bounds ) )
				return;

			// handle clamping the position and size into the framebounds, 
			// TODO: does not work when opened, and fix outlines
			if ( !is_top_in_bounds ) {
				size.y -= frame_bounds.min.y - position.y;
				position.y = frame_bounds.min.y;
			}
			if ( !is_bottom_in_bounds )
				size.y = frame_bounds.max.y - position.y;

			// the position for the small colored rectangle that showcases what color is currently chosen
			auto color_showcase_position = Vector2D( position.x + size.x - 30, position.y + 3 );
			auto color_showcase_size = Vector2D( 27, size.y - 6 );

			// draw the background rectangle that holds the name of the color picker
			//	RENDER::DrawFilledRect(position.x, position.y, position.x + size.x, position.y + size.y, colors[COLOR_PICKER_BODY]);
			//		RENDER::DrawEmptyRect(position.x - 1, position.y - 1, position.x + size.x, position.y + size.y, CColor(0, 0, 0, 100));

			// draw the color part inside of the rectangle
			RENDER::DrawFilledRect( color_showcase_position.x, color_showcase_position.y, color_showcase_position.x + color_showcase_size.x, color_showcase_position.y + color_showcase_size.y, color_max_alpha );
			RENDER::DrawEmptyRect( color_showcase_position.x - 1, color_showcase_position.y - 1, color_showcase_position.x + color_showcase_size.x, color_showcase_position.y + color_showcase_size.y, CColor( 0, 0, 0, 100 ) );

			// draw the name of the color picker
			if ( is_top_in_bounds && is_bottom_in_bounds )
				RENDER::DrawF( position.x + 5, position.y + ( size.y * 0.5f ), info.font, false, true, colors [ COLOR_PICKER_TEXT ], info.name );

			if ( command_data.is_selected ) {
				position = info.position + Vector2D( 0, COLOR_PICKER_CLOSED_HEIGHT + COLOR_PICKER_PADDING );
				size = info.size - Vector2D( 0, COLOR_PICKER_CLOSED_HEIGHT + COLOR_PICKER_PADDING );

				// draw the background of the actual color picker
				RENDER::DrawFilledRect( position.x, position.y, position.x + size.x, position.y + size.y, colors [ COLOR_PICKER_BODY ] );
				RENDER::DrawEmptyRect( position.x - 1, position.y - 1, position.x + size.x, position.y + size.y, CColor( 0, 0, 0, 100 ) );

				// this is where you pick the black/white/grey part, the biggest rectangle in the color picker
				auto color_picker_position = position + Vector2D( COLOR_PICKER_PADDING, COLOR_PICKER_PADDING );
				auto color_picker_size = Vector2D( size.x - ( COLOR_PICKER_PADDING * 3 + COLOR_PICKER_SLIDER_SIZE ), size.y - ( COLOR_PICKER_PADDING * 3 + COLOR_PICKER_SLIDER_SIZE ) );

				{
					// draw every pixel in the color picker
					for ( int x = 0; x < color_picker_size.x; x += COLOR_PICKER_PIXELATION ) {
						for ( int y = 0; y < color_picker_size.y; y += COLOR_PICKER_PIXELATION ) {
							CColor current_color = CColor::HSBtoRGB( ( *variable ).Hue( ), UTILS::GetFraction( x, 0, color_picker_size.x ), 1.f - UTILS::GetFraction( y, 0, color_picker_size.y ), 255 );
							RENDER::DrawFilledRect( color_picker_position.x + x, color_picker_position.y + y,
								color_picker_position.x + x + COLOR_PICKER_PIXELATION,
								color_picker_position.y + y + COLOR_PICKER_PIXELATION,
								current_color );
						}
					}
					// outline
					RENDER::DrawEmptyRect( color_picker_position.x - 1, color_picker_position.y - 1, color_picker_position.x + color_picker_size.x, color_picker_position.y + color_picker_size.y, CColor( 0, 0, 0, 100 ) );

					float saturation = ( *variable ).Saturation( );
					float brightness = 1.f - ( *variable ).Brightness( );
					Vector2D selected_position = color_picker_position + Vector2D( saturation * color_picker_size.x, brightness * color_picker_size.y );

					RENDER::DrawEmptyRect( selected_position.x - 2, selected_position.y - 2, selected_position.x + 2, selected_position.y + 2, WHITE );
				}
				{
					// alpha slider on the bottom
					auto alpha_slider_position = Vector2D( color_picker_position.x, color_picker_position.y + color_picker_size.y + COLOR_PICKER_PADDING );
					auto alpha_slider_size = Vector2D( size.x - ( COLOR_PICKER_PADDING * 2 ), COLOR_PICKER_SLIDER_SIZE );

					// white background
					RENDER::DrawFilledRect( alpha_slider_position.x, alpha_slider_position.y, alpha_slider_position.x + alpha_slider_size.x, alpha_slider_position.y + alpha_slider_size.y, WHITE );

					// draw the fading alpha from left to right
					for ( int i = 0; i <= alpha_slider_size.x; i++ ) {
						CColor current_color = CColor::HSBtoRGB( ( *variable ).Hue( ), 1.f, 1.f, 255 - ( ( 255.f / alpha_slider_size.x ) * i ) );
						RENDER::DrawFilledRect( alpha_slider_position.x + i, alpha_slider_position.y, alpha_slider_position.x + i + 1, alpha_slider_position.y + alpha_slider_size.y, current_color );
					}

					// outline
					RENDER::DrawEmptyRect( alpha_slider_position.x - 1, alpha_slider_position.y - 1, alpha_slider_position.x + alpha_slider_size.x, alpha_slider_position.y + alpha_slider_size.y, CColor( 0, 0, 0, 100 ) );

					// mark where we currently are in the slider
					int offset = ( ( 255.f - ( *variable ).RGBA [ 3 ] ) / 255.f ) * alpha_slider_size.x;
					RENDER::DrawLine( alpha_slider_position.x + offset, alpha_slider_position.y - 2, alpha_slider_position.x + offset, alpha_slider_position.y + alpha_slider_size.y + 2, WHITE );
				}
				{
					// hue slider on the right
					auto hue_slider_position = Vector2D( color_picker_position.x + color_picker_size.x + COLOR_PICKER_PADDING, color_picker_position.y );
					auto hue_slider_size = Vector2D( COLOR_PICKER_SLIDER_SIZE, color_picker_size.y );

					// draw the hue slider
					for ( int i = 0; i < hue_slider_size.y; i++ ) {
						float hue = UTILS::GetFraction( i, 0, hue_slider_size.y );
						CColor current_color = CColor::HSBtoRGB( hue, 1, 1, 255 );
						RENDER::DrawFilledRect( hue_slider_position.x, hue_slider_position.y + i, hue_slider_position.x + hue_slider_size.x, hue_slider_position.y + i + 1, current_color );
					}
					// draw outline for hue slider
					RENDER::DrawEmptyRect( hue_slider_position.x - 1, hue_slider_position.y - 1, hue_slider_position.x + hue_slider_size.x, hue_slider_position.y + hue_slider_size.y, CColor( 0, 0, 0, 100 ) );
					// draw where the hue is selected on with a vertical line
					int offset = UTILS::GetValueFromFraction( ( *variable ).Hue( ), 0, hue_slider_size.y );
					RENDER::DrawFilledRect( hue_slider_position.x - 2, hue_slider_position.y + offset, hue_slider_position.x + hue_slider_size.x + 2, hue_slider_position.y + offset + 1, WHITE );
				}
			}
		}
		void ColorPickerCommand::TestInput( Input& input ) {
			if ( !( UTILS::is_point_in_range( info.position, frame_bounds.min, frame_bounds.max - frame_bounds.min ) ||
				UTILS::is_point_in_range( info.position + info.size, frame_bounds.min, frame_bounds.max - frame_bounds.min ) ) )
				return;

			auto& command_data = menu.GetCommandData( GetID( ) ).color_picker_data;

			auto position = info.position;
			auto size = Vector2D( info.size.x, COLOR_PICKER_CLOSED_HEIGHT );

			// the position for the small colored rectangle that showcases what color is currently chosen
			auto color_showcase_position = Vector2D( position.x + size.x - 30, position.y + 3 );
			auto color_showcase_size = Vector2D( 27, size.y - 6 );

			// if clicked
			if ( input.mouse.left_button ) {
				position = info.position + Vector2D( 0, COLOR_PICKER_CLOSED_HEIGHT + COLOR_PICKER_PADDING );
				size = info.size - Vector2D( 0, COLOR_PICKER_CLOSED_HEIGHT + COLOR_PICKER_PADDING );

				bool is_hovered_over = UTILS::is_point_in_range( input.mouse.position, color_showcase_position, color_showcase_size );
				if ( command_data.is_selected )
					is_hovered_over = UTILS::is_point_in_range( input.mouse.position, position, size );

				// if clicked on the open rectangle
				if ( is_hovered_over ) {
					// account for padding
					position = info.position + Vector2D( 0, COLOR_PICKER_CLOSED_HEIGHT + COLOR_PICKER_PADDING );
					size = info.size - Vector2D( 0, COLOR_PICKER_CLOSED_HEIGHT + COLOR_PICKER_PADDING );

					// this is where you pick the black/white/grey part, the biggest rectangle in the color picker
					auto color_picker_position = position + Vector2D( COLOR_PICKER_PADDING, COLOR_PICKER_PADDING );
					auto color_picker_size = Vector2D( size.x - ( COLOR_PICKER_PADDING * 3 + COLOR_PICKER_SLIDER_SIZE ), size.y - ( COLOR_PICKER_PADDING * 3 + COLOR_PICKER_SLIDER_SIZE ) );

					// alpha slider on the bottom
					auto alpha_slider_position = Vector2D( color_picker_position.x, color_picker_position.y + color_picker_size.y + COLOR_PICKER_PADDING );
					auto alpha_slider_size = Vector2D( size.x - ( COLOR_PICKER_PADDING * 2 ), COLOR_PICKER_SLIDER_SIZE );

					// hue slider on the right
					auto hue_slider_position = Vector2D( color_picker_position.x + color_picker_size.x + COLOR_PICKER_PADDING, color_picker_position.y );
					auto hue_slider_size = Vector2D( COLOR_PICKER_SLIDER_SIZE, color_picker_size.y );


					bool is_over_alpha_slider = UTILS::is_point_in_range( input.mouse.position, alpha_slider_position, alpha_slider_size );
					bool is_over_hue_slider = UTILS::is_point_in_range( input.mouse.position, hue_slider_position, hue_slider_size );
					bool is_over_color_picker = UTILS::is_point_in_range( input.mouse.position, color_picker_position, color_picker_size );


					if ( is_over_color_picker ) {
						float hue = ( *variable ).Hue( );
						float brightness = UTILS::GetFraction( input.mouse.position.y, color_picker_position.y, color_picker_position.y + color_picker_size.y );
						float saturation = UTILS::GetFraction( input.mouse.position.x, color_picker_position.x, color_picker_position.x + color_picker_size.x );

						*variable = CColor::HSBtoRGB( hue, saturation, 1.f - brightness, ( *variable ).RGBA [ 3 ] );
					}
					else if ( is_over_hue_slider ) {
						float hue = UTILS::clamp( UTILS::GetFraction( input.mouse.position.y, hue_slider_position.y, hue_slider_position.y + hue_slider_size.y ), 0.f, 1.f );
						float brightness = ( *variable ).Brightness( );
						float saturation = ( *variable ).Saturation( );

						*variable = CColor::HSBtoRGB( hue, saturation, brightness, ( *variable ).RGBA [ 3 ] );
					}
					else if ( is_over_alpha_slider ) {
						int alpha = 255 - ( UTILS::GetFraction( input.mouse.position.x, alpha_slider_position.x, alpha_slider_position.x + alpha_slider_size.x ) * 255.f );

						CColor nigger = *variable;
						nigger.RGBA [ 3 ] = alpha;
						*variable = nigger;
					}

					// do smol shitz
					command_data.is_selected = true;
					input.mouse.left_button = 0;
				}
				else if ( input.mouse.left_button & 1 )
					command_data.is_selected = false;
			}
		}
		CommandInfo ColorPickerCommand::Info( ) {
			return info;
		}
	}
}