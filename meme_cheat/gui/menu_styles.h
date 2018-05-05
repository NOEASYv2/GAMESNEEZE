#pragma once
#include "../UTILS/render.h"


namespace MENU
{
	// shit for flags
	constexpr unsigned char OBJECT_ACTIVE = 0b00000001;
	constexpr unsigned char OBJECT_HOVERED_OVER = 0b00000010;
	constexpr unsigned char OBJECT_CLICKED_ON = 0b00000100;

	// shit for objects
	enum OBJECT_CLASS
	{
		WINDOW_OBJECT,
		TAB_OBJECT,
		GROUPBOX_OBJECT,
		CHECKBOX_OBJECT
	};

	struct BasicObjectInfo
	{
		void* parent = nullptr;

		Vector2D position;
		Vector2D size;

		Vector2D virtual_position;

		OBJECT_CLASS object_class;
		unsigned char flags;
		int depth; // this is how far down the tree the object is, with the main menu class being 0
	}; 
	struct WindowInfo
	{
		BasicObjectInfo basic_object_info;

		std::string title;
	};
	struct TabInfo
	{
		std::string name;
		bool is_selected = false;

		BasicObjectInfo basic_object_info;
	};
	struct GroupboxInfo
	{
		BasicObjectInfo basic_object_info;

		std::string name;
	};
	struct CheckboxInfo
	{
		BasicObjectInfo basic_object_info;

		std::string name;

		bool value;
	};

	typedef void(*WindowDrawingFunc)(WindowInfo);
	typedef void(*TabDrawingFunc)(TabInfo);
	typedef void(*GroupboxDrawingFunc)(GroupboxInfo);
	typedef void(*CheckboxDrawingFunc)(CheckboxInfo);

	struct WindowStyle
	{
		Vector2D size;    // for determining whether clicked on or not

		WindowDrawingFunc drawing_func = nullptr; // function that draws
	};
	struct TabStyle
	{
		Vector2D size;

		TabDrawingFunc drawing_func = nullptr;
	};
	struct GroupboxStyle
	{
		GroupboxDrawingFunc drawing_func;
	};
	struct CheckboxStyle
	{
		Vector2D size;
		
		// the bbmin and bbmax dictate where the object is allowed in, it's only
		// applicable to the y axis, up and down, it will NOT draw if it's not within
		// this range
		int virtual_min; 
		int virtual_max;
		int virtual_size; // how much ***extra space*** there is
		                  // keep it at 0 for no scrolling

		CheckboxDrawingFunc drawing_func = nullptr;
	};
}