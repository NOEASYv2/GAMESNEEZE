#pragma once

namespace MENU
{
	namespace PPGUI_PP_GUI
	{
		enum OBJECT_TYPE
		{
			OBJECT_WINDOW, 
			OBJECT_GROUPBOX,
			OBJECT_SEPARATOR,
			OBJECT_CHECKBOX,
			OBJECT_BUTTON,
			OBJECT_COMBOBOX,
			OBJECT_SLIDER,
			OBJECT_TAB,
			OBJECT_VERTICAL_TAB,
			OBJECT_COLOR_PICKER
		};

		enum OBJECT_FLAGS
		{
			FLAG_NONE = 0,
			FLAG_SCROLL = 0b00000001,
			FLAG_DRAGGABLE = 0b00000010 // just for windows
		};

		// this is for the arrays of colors, so you can do shit like colors[OBJECT_INDEX]
		enum OBJECT_INDEX
		{
			WINDOW_BODY = 0,
			WINDOW_TITLE_BAR,
			WINDOW_TEXT,

			GROUPBOX_BODY,
			GROUPBOX_OUTLINE,
			GROUPBOX_TEXT,

			SCROLLBAR_BODY,

			SEPARATOR_TEXT,
			SEPARATOR_LINE,

			CHECKBOX_CLICKED,
			CHECKBOX_NOT_CLICKED,
			CHECKBOX_TEXT,

			BUTTON_BODY,
			BUTTON_TEXT,

			COMBOBOX_TEXT,
			COMBOBOX_SELECTED,
			COMBOBOX_SELECTED_TEXT,
			COMBOBOX_ITEM,
			COMBOBOX_ITEM_TEXT,

			SLIDER_BODY,
			SLIDER_VALUE,
			SLIDER_TEXT,

			TAB_BODY,
			TAB_TEXT,
			TAB_BODY_SELECTED,
			TAB_TEXT_SELECTED,

			VERTICAL_TAB_BODY,
			VERTICAL_TAB_OUTLINE,
			VERTICAL_TAB_BODY_SELECTED,
			VERTICAL_TAB_TEXT,

			COLOR_PICKER_BODY,
			COLOR_PICKER_TEXT,
			COLOR_PICKER_OUTLINE,

			OBJECT_INDEX_SIZE
		};

		constexpr int OBJECT_SCROLL_SPEED = 15; // how fast you scroll
		constexpr int OBJECT_PADDING = 8; // this is the padding between objects
		constexpr int OBJECT_HORIZONTAL_PADDING = 20; // this is the horizontal padding between objects on the same line

		constexpr int OBJECT_SCROLLBAR_WIDTH = 10; // width of scrollbars
		constexpr float OBJECT_SCROLLBAR_FRACTION = 0.2f; // the height of the scrollbar is the fraction * the object height

		constexpr int FRAME_PADDING = 15; // how far "deep" into the window objects are
		constexpr int WINDOW_TITLE_BAR_SIZE = 20; // title bar height

		constexpr int SEPARATOR_TEXT_OFFSET = 30; // how far away from the start of the line the text is

		constexpr int CHECKBOX_SIZE = 14;

		constexpr int COMBOBOX_HEIGHT = 14;
		constexpr int COMBOBOX_WIDTH = 200;
		constexpr int COMBOBOX_MAX_ITEMS_AT_ONCE = 8;

		constexpr int SLIDER_HEIGHT = 14;
		constexpr int SLIDER_WIDTH = 200;

		constexpr int TAB_PADDING = 10;
		constexpr int TAB_HEIGHT = 16;

		constexpr int VERTICAL_TAB_WIDTH = 250; // width
		constexpr int VERTICAL_TAB_HEIGHT = 16; // height of a single item
		constexpr int VERTICAL_TAB_PADDING = 0; // the padding between items
		constexpr int VERTICAL_TAB_MAX_ITEMS_AT_ONCE = 6;

		constexpr int COLOR_PICKER_PIXELATION = 3; // how pixelated the color picker is, to improve fps
		constexpr int COLOR_PICKER_PADDING = 6; // padding between shit
		constexpr int COLOR_PICKER_SLIDER_SIZE = 12; // size of alpha and hue sliders
		constexpr int COLOR_PICKER_WIDTH = 250; 
		constexpr int COLOR_PICKER_OPEN_HEIGHT = 200;
		constexpr int COLOR_PICKER_CLOSED_HEIGHT = 16;


		extern CColor colors[OBJECT_INDEX_SIZE];
	}
}