#include "widgets.h"

/* XPM */
static char* s_button_xpm[] = {
	/* columns rows colors chars-per-pixel */
	"22 22 20 1",
	"  c black",
	". c #003399",
	"X c magenta",
	"o c #CFD9EC",
	"O c #C7EAFF",
	"+ c #CBEBFF",
	"@ c #D0EDFF",
	"# c #D2EEFF",
	"$ c #D9F0FF",
	"% c #E0F3FF",
	"& c #E3F4FF",
	"* c #E8F6FF",
	"= c #ECF7FF",
	"- c #F0F9FF",
	"; c #F4FAFF",
	": c #F4FBFF",
	"> c #F6FCFF",
	", c #FAFDFF",
	"< c #FBFDFF",
	"1 c None",
	/* pixels */
	"1111111111111111111111",
	"1111111111111111111111",
	"1111111111111111111111",
	"1111111111111111111111",
	"1111111111111111111111",
	"1111111111111111111111",
	"o....................o",
	".,,,,,,,,,,,,,,,,,,,,.",
	".::::::  :: :::::::::.",
	".===== == = == =====-.",
	".&&&&& && & & &&&&&&*.",
	".$$$$$ $$ $  $$$$$$$&.",
	".@@@@@ @@ @ @ @@+@@@$.",
	".OOOOOO  OO OO OO++O@.",
	".++++++++++++++++++++.",
	"o....................o",
	"1111111111111111111111",
	"1111111111111111111111",
	"1111111111111111111111",
	"1111111111111111111111",
	"1111111111111111111111",
	"1111111111111111111111"
};

wxIcon CValueButton::GetIcon() const
{
	return wxIcon(s_button_xpm);
}

wxIcon CValueButton::GetIconGroup()
{
	return wxIcon(s_button_xpm);
}