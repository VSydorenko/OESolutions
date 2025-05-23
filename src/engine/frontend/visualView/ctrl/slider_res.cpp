#include "widgets.h"

/* XPM */
static char* s_slider_xpm[] = {
	/* columns rows colors chars-per-pixel */
	"22 22 13 1",
	"  c #716F64",
	". c #66CC33",
	"X c magenta",
	"o c #BBB9AD",
	"O c #D4D1C2",
	"+ c #D4D1C3",
	"@ c #E2DFCF",
	"# c #E3DFCF",
	"$ c #E3DFD0",
	"% c #E2E0CF",
	"& c #E2E0D0",
	"* c #ECE9D8",
	"= c None",
	/* pixels */
	"======================",
	"======================",
	"======================",
	"======================",
	"======================",
	"=====   ==============",
	"==== ... =============",
	"==== *$+ =============",
	"==== *@+ =============",
	"=ooo *@+ oooooooooooo=",
	"o*** *@+ ************o",
	"=ooo *%+ oooooooooooo=",
	"==== *@+ =============",
	"==== *&+ =============",
	"==== ... =============",
	"=====   ==============",
	"======================",
	"======================",
	"======================",
	"======================",
	"======================",
	"======================"
};

wxIcon CValueSlider::GetIcon() const
{
	return wxIcon(s_slider_xpm);
}

wxIcon CValueSlider::GetIconGroup()
{
	return wxIcon(s_slider_xpm);
}