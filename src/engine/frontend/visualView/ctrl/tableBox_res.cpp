#include "tableBox.h"

/* XPM */
static char* s_dataviewlist_ctrl_xpm[] = {
"21 21 6 1",
" 	c None",
".	c #3E9ADE",
"+	c #858585",
"@	c #2C2C2C",
"#	c #FFFFFF",
"$	c #B8B8B8",
".....................",
".++@++@++@++@++@++@+.",
".+@+@@++@+@@++@+@@++.",
".+#####$#####$######.",
".+#####$#####$######.",
".+#####$#####$######.",
".+$$$$$$$$$$$$$$$$$$.",
".+#####$#####$######.",
".+#####$#####$######.",
".+#####$#####$######.",
".+$$$$$$$$$$$$$$$$$$.",
".+#####$#####$######.",
".+#####$#####$######.",
".+#####$#####$######.",
".+$$$$$$$$$$$$$$$$$$.",
".+#####$#####$######.",
".+#####$#####$######.",
".+#####$#####$######.",
".+$$$$$$$$$$$$$$$$$$.",
".+#####$#####$######.",
"....................." };


wxIcon CValueTableBox::GetIcon() const
{
	return wxIcon(s_dataviewlist_ctrl_xpm);
}

wxIcon CValueTableBox::GetIconGroup()
{
	return wxIcon(s_dataviewlist_ctrl_xpm);
}