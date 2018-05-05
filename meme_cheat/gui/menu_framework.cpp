#include "../includes.h"
#include "../utilities/render.h"

#include "menu_framework.h"

namespace MENU {
	namespace PPGUI_PP_GUI {
		CColor colors [ OBJECT_INDEX_SIZE ];

		void DrawMouse( ) {
			menu.ShouldDrawMouse( );
		}

		void SetFont( uint32_t font ) {
			menu.SetFont( font );
		}

		void SetLine( int line ) {
			menu.SetCurrentLine( line - 1 );
		}
		void SplitWindow( int splits ) {
			menu.SetCurrentColumns( splits );
			menu.GetCurrentWindow( ).SetColumns( splits );

			if ( menu.GetCurrentWindow( ).command == menu.GetStackFrame( ).command )
				menu.GetStackFrame( ).SetColumns( splits );
		}

		Vector2D PeekNextPosition( ) {
			return menu.PeekNextPosition( );
		}
		Vector2D GetWindowSize( ) {
			return menu.GetCurrentWindowSize( );
		}
		Vector2D GetEffectiveWindowSize( ) {
			return menu.GetCurrentEffectiveSize( );
		}
		FrameBounds GetWindowBounds( ) {
			FrameBounds bounds;
			menu.GetCurrentWindow( ).GetBounds( bounds );
			return bounds;
		}

		void Begin( ) {
			menu.Begin( );
		}
		void End( ) {
			menu.End( );
		}

		// windows
		void WindowBegin( std::string name, Vector2D position, Vector2D size, int scroll_size ) {
			menu.PushbackStackFrame( menu.PushbackCommand( WindowCommand( name,
				position, size, scroll_size ) ) );
		}
		void WindowEnd( ) {
			if ( !menu.StackFrameSize( ) )
				throw std::runtime_error( "Problem getting stack frame!" );

			auto stack_frame = menu.GetStackFrame( );
			if ( stack_frame.command->Info( ).object_type != OBJECT_TYPE::OBJECT_WINDOW )
				throw std::runtime_error( "Problem ending window frame!" );

			menu.PopStackFrame( );
			menu.SetCurrentLine( 0 );
			menu.SetCurrentColumns( 1 );
		}

		void VerGroupboxBegin( std::string name, int height, int scroll_size ) {
			int columns = menu.GetCurrentColumns( );
			int width = ( menu.GetCurrentEffectiveSize( ).x + 15 );

			Vector2D size( width, height );

			Vector2D position;
			FrameBounds bounds;
			if ( !menu.AllocateSpace( size, Vector2D( position.x + 100, position.y ) ) )
				return;

			menu.GetBounds( bounds );
			menu.PushbackStackFrame( menu.PushbackCommand( GroupboxCommand( name,
				position, size, bounds, scroll_size ) ) );
		}

		// groupboxes, basically same functionality as a window
		void GroupboxBegin( std::string name, int height, int scroll_size ) {
			int columns = menu.GetCurrentColumns( );
			int width = ( menu.GetCurrentEffectiveSize( ).x + 15 );

			Vector2D size( width, height );

			Vector2D position;
			FrameBounds bounds;
			if ( !menu.AllocateSpace( size, position ) )
				return;

			menu.GetBounds( bounds );
			menu.PushbackStackFrame( menu.PushbackCommand( GroupboxCommand( name,
				position, size, bounds, scroll_size ) ) );
		}
		void GroupboxEnd( ) {
			auto stack_frame = menu.GetStackFrame( );
			if ( stack_frame.command->Info( ).object_type != OBJECT_TYPE::OBJECT_GROUPBOX )
				throw std::runtime_error( "Problem ending groupbox frame!" );

			menu.PopStackFrame( );
		}

		// separators
		void Separator( std::string name ) {
			int columns = menu.GetCurrentColumns( );
			int text_height = RENDER::GetTextSize( menu.GetFont( ), name ).y;
			Vector2D size( menu.GetCurrentEffectiveSize( ).x, text_height );

			Vector2D position;
			if ( !menu.AllocateSpace( size, position ) )
				return;

			FrameBounds bounds;
			menu.GetBounds( bounds );

			menu.PushbackCommand( SeparatorCommand( name, position, size, bounds ) );
		}
		void Separator( ) {

		}

		// checkboxes
		bool Checkbox( std::string name, bool& variable ) {
			Vector2D size( CHECKBOX_SIZE, CHECKBOX_SIZE );

			Vector2D position;
			if ( !menu.AllocateSpace( size, position ) )
				return false;

			FrameBounds bounds;
			menu.GetBounds( bounds );

			menu.PushbackCommand( CheckboxCommand( name, position, size, bounds, variable ) );

			return variable;
		}
		bool Checkbox( std::string name ) {
			Vector2D size( CHECKBOX_SIZE, CHECKBOX_SIZE );

			Vector2D position;
			if ( !menu.AllocateSpace( size, position ) )
				return false;

			FrameBounds bounds;
			menu.GetBounds( bounds );

			auto command = menu.PushbackCommand( CheckboxCommand( name, position, size, bounds ) );
			return menu.GetCommandData( command->GetID( ) ).checkbox_value;
		}

		// buttons
		bool Button( std::string name ) {
			Vector2D size( RENDER::GetTextSize( menu.GetFont( ), name ) + Vector2D( 8, 2 ) );

			Vector2D position;
			if ( !menu.AllocateSpace( size, position ) )
				return false;

			FrameBounds bounds;
			menu.GetBounds( bounds );

			auto command = menu.PushbackCommand( ButtonCommand( name, position, size, bounds ) );
			return menu.GetCommandData( command->GetID( ) ).button_value;
		}

		// comboboxes
		int Combobox( std::string name, std::vector<std::string> items, int& variable ) // variable is in range [0, items_size)
		{
			Vector2D size( COMBOBOX_WIDTH, COMBOBOX_HEIGHT );

			Vector2D position;
			if ( !menu.AllocateSpace( size, position ) )
				return false;

			FrameBounds bounds;
			menu.GetBounds( bounds );



			auto command = menu.PushbackCommand( ComboboxCommand( name, position, size, bounds, items, variable ) );

			return variable;
		}

		// sliders
		float Slider( std::string name, float min, float max, float& variable, int decimal_places ) {
			Vector2D size( SLIDER_WIDTH, SLIDER_HEIGHT );

			Vector2D position;
			if ( !menu.AllocateSpace( size, position ) )
				return false;

			FrameBounds bounds;
			menu.GetBounds( bounds );

			auto command = menu.PushbackCommand( SliderCommand( name, position, size, bounds, min, max, variable, decimal_places ) );
			return variable;
		}

		// tabs
		int MainTab( std::string name, std::vector<std::string> tabs, OBJECT_FLAGS flags ) {
			Vector2D size( menu.GetCurrentEffectiveSize( ).x, TAB_HEIGHT );

			FrameBounds bounds;
			menu.GetBounds( bounds );
			auto offset = Vector2D( 0, -6 );
			Vector2D position;
			if ( !menu.AllocateSpace( size, position, flags & OBJECT_FLAGS::FLAG_SCROLL ) )
				return false;

			position.x += 5;
			position.y += 5;

			auto command = menu.PushbackCommand( TabCommand( name, position, size, bounds, tabs, offset ) );

			return menu.GetCommandData( command->GetID( ) ).tab_data.tab_index;
		}
		int Tab( std::string name, std::vector<std::string> tabs, OBJECT_FLAGS flags ) {

			Vector2D size( menu.GetCurrentEffectiveSize( ).x, TAB_HEIGHT );

			FrameBounds bounds;
			menu.GetBounds( bounds );
			auto offset = Vector2D( 0, 0 );
			Vector2D position;
			if ( !menu.AllocateSpace( size, position, flags & OBJECT_FLAGS::FLAG_SCROLL ) )
				return false;

			position.x -= 2;

			auto command = menu.PushbackCommand( TabCommand( name, position, size, bounds, tabs, offset ) );

			return menu.GetCommandData( command->GetID( ) ).tab_data.tab_index;
		}
		int VerticalTab( std::string name, std::vector<std::string> tabs, OBJECT_FLAGS flags ) {
			if ( !tabs.size( ) )
				return -1;

			FrameBounds bounds;
			menu.GetBounds( bounds );

			Vector2D used_size( VERTICAL_TAB_WIDTH, static_cast<int>( (
				VERTICAL_TAB_HEIGHT * min( VERTICAL_TAB_MAX_ITEMS_AT_ONCE,
				tabs.size( ) ) ) + ( VERTICAL_TAB_PADDING * ( min( VERTICAL_TAB_MAX_ITEMS_AT_ONCE,
				tabs.size( ) ) - 1 ) ) ) );

			Vector2D position;
			if ( !menu.AllocateSpace( used_size, position, flags & OBJECT_FLAGS::FLAG_SCROLL ) )
				return false;

			Vector2D size( VERTICAL_TAB_WIDTH, static_cast<int>( ( VERTICAL_TAB_HEIGHT *
				tabs.size( ) ) + ( VERTICAL_TAB_PADDING * ( tabs.size( ) - 1 ) ) ) );
			auto command = menu.PushbackCommand( VerticalTabCommand( name, position, size, bounds, tabs ) );

			return menu.GetCommandData( command->GetID( ) ).tab_data.tab_index;
		}

		// color picker
		CColor ColorPicker( std::string name, CColor &variable ) {
			Vector2D size( COLOR_PICKER_WIDTH, COLOR_PICKER_CLOSED_HEIGHT );
			Vector2D position;
			if ( !menu.AllocateSpace( size, position ) )
				return CColor( );

			FrameBounds bounds;
			menu.GetBounds( bounds );

			size.y = COLOR_PICKER_OPEN_HEIGHT;
			auto command = menu.PushbackCommand( ColorPickerCommand( name, position, size, bounds, variable ) );
			return variable;
		}
	}
}