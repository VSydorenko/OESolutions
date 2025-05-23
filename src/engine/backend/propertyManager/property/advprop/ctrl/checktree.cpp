#include "checktree.h"

#include "res/checked2.xpm"
#include "res/checked_d.xpm"
#include "res/checked_ld.xpm"
#include "res/checked_mo.xpm"
#include "res/unchecked2.xpm"
#include "res/unchecked_d.xpm"
#include "res/unchecked_ld.xpm"
#include "res/unchecked_mo.xpm"

#include <wx/icon.h>
#include <wx/imaglist.h>

wxDEFINE_EVENT(wxEVT_PROPERTY_CHECKTREE_FOCUS, wxTreeEvent);
wxDEFINE_EVENT(wxEVT_PROPERTY_CHECKTREE_CHOICE, wxTreeEvent);

wxIMPLEMENT_DYNAMIC_CLASS(wxPropertyCheckTree, wxTreeCtrl)

bool wxPropertyCheckTree::on_check_or_label(int flags)
{
	return flags & (wxTREE_HITTEST_ONITEMSTATEICON | wxTREE_HITTEST_ONITEMLABEL) ? true : false;
}

void wxPropertyCheckTree::unhighlight(const wxTreeItemId& id)
{
	if (!id.IsOk())
		return;

	int i = wxPropertyCheckTree::GetItemState(id);

	if (wxPropertyCheckTree::UNCHECKED <= i && i < wxPropertyCheckTree::UNCHECKED_DISABLED)
	{
		wxPropertyCheckTree::SetItemState(id, wxPropertyCheckTree::UNCHECKED);

	}
	else if (wxPropertyCheckTree::CHECKED <= i && i < wxPropertyCheckTree::CHECKED_DISABLED)
	{
		wxPropertyCheckTree::SetItemState(id, wxPropertyCheckTree::CHECKED);
	}
}

void wxPropertyCheckTree::mohighlight(const wxTreeItemId& id, bool toggle)
{
	if (!id.IsOk())
		return;

	int i = wxPropertyCheckTree::GetItemState(id);

	if (!id.IsOk() || i < 0) {
		return;
	}

	int checkedCount = 0;
	for (auto &i : m_checkedItems) {
		checkedCount += i.second ? 1 : 0;
	}

	bool is_checked;

	if (wxPropertyCheckTree::UNCHECKED <= i && i < wxPropertyCheckTree::UNCHECKED_DISABLED)
	{
		wxPropertyCheckTree::SetItemState(id, toggle ? wxPropertyCheckTree::CHECKED_MOUSE_OVER : wxPropertyCheckTree::UNCHECKED_MOUSE_OVER);
		is_checked = true;
	}
	else if (wxPropertyCheckTree::CHECKED <= i && i < wxPropertyCheckTree::CHECKED_DISABLED)
	{
		if (!m_allowEmpty && checkedCount <= 1)
			return;
		wxPropertyCheckTree::SetItemState(id, toggle ? wxPropertyCheckTree::UNCHECKED_MOUSE_OVER : wxPropertyCheckTree::CHECKED_MOUSE_OVER);
		is_checked = false;
	}

	if (toggle)
	{
		wxTreeEvent eventChoice(wxEVT_PROPERTY_CHECKTREE_CHOICE, this, id);
		eventChoice.SetExtraLong(is_checked ? 1 : 0);
		wxPropertyCheckTree::ProcessWindowEvent(eventChoice);

		if (m_singleCheck) {
			for (auto item : m_checkedItems) {
				if (item.second) {
					wxPropertyCheckTree::SetItemState(item.first, wxPropertyCheckTree::UNCHECKED);
				}
			}
			m_checkedItems.clear();
		}

		m_checkedItems.insert_or_assign(id, is_checked);
	}
}

void wxPropertyCheckTree::ldhighlight(const wxTreeItemId& id)
{
	if (!id.IsOk())
		return;

	int i = wxPropertyCheckTree::GetItemState(id);

	if (wxPropertyCheckTree::UNCHECKED <= i && i < wxPropertyCheckTree::UNCHECKED_DISABLED)
	{
		wxPropertyCheckTree::SetItemState(id, wxPropertyCheckTree::UNCHECKED_LEFT_DOWN);
	}
	else if (wxPropertyCheckTree::CHECKED <= i && i < wxPropertyCheckTree::CHECKED_DISABLED)
	{
		wxPropertyCheckTree::SetItemState(id, wxPropertyCheckTree::CHECKED_LEFT_DOWN);
	}
}

wxPropertyCheckTree::wxPropertyCheckTree()
	:wxTreeCtrl()
	, last_mo(), last_ld(), last_kf(), mouse_entered_tree_with_left_down(false), m_singleCheck(false), m_allowEmpty(false)
{
}

wxPropertyCheckTree::wxPropertyCheckTree(wxWindow* parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: wxTreeCtrl(parent, id, pos, size, style & ~wxPR_MULTIPLE_CHECK & ~wxPR_SINGLE_CHECK & ~wxPR_EMPTY_CHECK)
	, last_mo(), last_ld(), last_kf(), mouse_entered_tree_with_left_down(false), m_singleCheck((style& wxPR_SINGLE_CHECK) == wxPR_SINGLE_CHECK), m_allowEmpty((style& wxPR_EMPTY_CHECK) == wxPR_EMPTY_CHECK)
{
	Init();
}

wxPropertyCheckTree::~wxPropertyCheckTree()
{
	wxTreeCtrl::UnselectAll();
	m_colors.clear();
}

void wxPropertyCheckTree::Init()
{
	wxImageList* states;

	wxIcon icons[8];
	icons[0] = wxIcon(unchecked2_xpm);
	icons[1] = wxIcon(unchecked_mo_xpm);
	icons[2] = wxIcon(unchecked_ld_xpm);
	icons[3] = wxIcon(unchecked_d_xpm);
	icons[4] = wxIcon(checked2_xpm);
	icons[5] = wxIcon(checked_mo_xpm);
	icons[6] = wxIcon(checked_ld_xpm);
	icons[7] = wxIcon(checked_d_xpm);

	int width = icons[0].GetWidth(),
		height = icons[0].GetHeight();

	// Make an state image list containing small icons
	states = new wxImageList(width, height, true);

	for (size_t i = 0; i < WXSIZEOF(icons); i++)
		states->Add(icons[i]);

	AssignStateImageList(states);

	Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(wxPropertyCheckTree::On_Tree_Sel_Changed), nullptr, this);

	Connect(wxEVT_CHAR, wxKeyEventHandler(wxPropertyCheckTree::On_Char), nullptr, this);
	Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(wxPropertyCheckTree::On_KeyDown), nullptr, this);
	Connect(wxEVT_KEY_UP, wxKeyEventHandler(wxPropertyCheckTree::On_KeyUp), nullptr, this);

	Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(wxPropertyCheckTree::On_Mouse_Enter_Tree), nullptr, this);
	Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(wxPropertyCheckTree::On_Mouse_Leave_Tree), nullptr, this);
	Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(wxPropertyCheckTree::On_Left_DClick), nullptr, this);
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(wxPropertyCheckTree::On_Left_Down), nullptr, this);
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(wxPropertyCheckTree::On_Left_Up), nullptr, this);
	Connect(wxEVT_MOTION, wxMouseEventHandler(wxPropertyCheckTree::On_Mouse_Motion), nullptr, this);
	Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(wxPropertyCheckTree::On_Mouse_Wheel), nullptr, this);

	Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(wxPropertyCheckTree::On_Tree_Focus_Set), nullptr, this);
	Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(wxPropertyCheckTree::On_Tree_Focus_Lost), nullptr, this);
}

void wxPropertyCheckTree::SetItemTextColour(const wxTreeItemId& item, const wxColour& col)
{
	std::map<wxTreeItemId, wxColor>::iterator it = m_colors.find(item);

	if (it == m_colors.end())
	{
		m_colors.insert(std::pair<wxTreeItemId, wxColor>(item, col));
	}
	else
	{
		m_colors[item] = col;
	}

	wxTreeCtrl::SetItemTextColour(item, col);
}

bool wxPropertyCheckTree::EnableCheckBox(const wxTreeItemId& item, bool enable)
{
	if (!item.IsOk())
	{
		return false;
	}

	int i = GetItemState(item);

	if (i<0 || i>CHECKED_DISABLED)
	{
		return false;
	}
	else if (enable)
	{
		if (i == UNCHECKED_DISABLED)
		{
			SetItemState(item, UNCHECKED);
		}
		else if (i == CHECKED_DISABLED)
		{
			SetItemState(item, CHECKED);
		}

		std::map<wxTreeItemId, wxColor>::iterator it = m_colors.find(item);

		if (it != m_colors.end())
		{
			SetItemTextColour(item, it->second);
			m_colors.erase(it);
		}

		return true;
	}
	else
	{
		if (i == UNCHECKED_DISABLED || i == CHECKED_DISABLED)
		{
			//don't disable a second time or we'll lose the
			//text color information.
			return true;
		}

		if (i == UNCHECKED || i == UNCHECKED_MOUSE_OVER || i == UNCHECKED_LEFT_DOWN)
		{
			SetItemState(item, UNCHECKED_DISABLED);
		}
		else if (i == CHECKED || i == CHECKED_MOUSE_OVER || i == CHECKED_LEFT_DOWN)
		{
			SetItemState(item, CHECKED_DISABLED);
		}

		wxColor col = GetItemTextColour(item);

		SetItemTextColour(item, wxColour(161, 161, 146));
		m_colors[item] = col;
		return true;
	}
}

bool wxPropertyCheckTree::DisableCheckBox(const wxTreeItemId& item)
{
	return EnableCheckBox(item, false);
}

void wxPropertyCheckTree::MakeCheckable(const wxTreeItemId& item, bool state)
{
	if (item.IsOk())
	{
		int i = GetItemState(item);

		if (i<0 || i>CHECKED_DISABLED)
		{
			SetItemState(item, state ? CHECKED : UNCHECKED);
		}
	}
}

void wxPropertyCheckTree::Check(const wxTreeItemId& item, bool state)
{
	if (item.IsOk())
	{
		int old_state = GetItemState(item);

		if (UNCHECKED <= old_state && old_state <= CHECKED_DISABLED)
		{
			bool enab = (old_state == UNCHECKED_DISABLED || old_state == CHECKED_DISABLED) ? false : true;
			int ch = enab ? CHECKED : CHECKED_DISABLED;
			int unch = enab ? UNCHECKED : UNCHECKED_DISABLED;
			int new_state = state ? ch : unch;

			int checkedCount = 0;
			for (auto i : m_checkedItems) {
				checkedCount += i.second ? 1 : 0;
			}

			if (m_singleCheck) {
				if (checkedCount > 1) {
					new_state = unch;
				}
			}

			if (!m_allowEmpty && !state && checkedCount <= 1)
				return;

			if (new_state != old_state) {
				wxPropertyCheckTree::SetItemState(item, new_state);
			}

			if (state) {
				wxPropertyCheckTree::SelectItem(item);
			}

			m_checkedItems.insert_or_assign(item, state);
		}
	}
}

void wxPropertyCheckTree::Uncheck(const wxTreeItemId& item)
{
	Check(item, false);
}

void wxPropertyCheckTree::On_Tree_Sel_Changed(wxTreeEvent& event)
{
	wxTreeItemId id = event.GetItem();

	unhighlight(last_kf);
	mohighlight(id, false);
	last_kf = id;

	event.Skip();
}

void wxPropertyCheckTree::On_Char(wxKeyEvent& event)
{
	if (!GetSelection().IsOk()) {
		//If there is no selection, any keypress should just select the first item
		wxTreeItemIdValue cookie;
		SelectItem(
			HasFlag(wxTR_HIDE_ROOT) ?
			GetFirstChild(GetRootItem(), cookie) :
			GetRootItem()
		);
		return;
	}

	event.Skip();
}

void wxPropertyCheckTree::On_KeyDown(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_SPACE) {
		ldhighlight(last_kf);
	}

	event.Skip();
}

void wxPropertyCheckTree::On_KeyUp(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_SPACE) {
		mohighlight(last_kf, true);
	}
	else if (event.GetKeyCode() == WXK_ESCAPE) {
		unhighlight(last_kf);
		last_kf = wxTreeItemId();
		Unselect();
	}

	event.Skip();
}


void wxPropertyCheckTree::On_Mouse_Enter_Tree(wxMouseEvent& event)
{
	if (event.LeftIsDown())
	{
		mouse_entered_tree_with_left_down = true;
	}
}

void wxPropertyCheckTree::On_Mouse_Leave_Tree(wxMouseEvent& event)
{
	unhighlight(last_mo);
	unhighlight(last_ld);
	last_mo = wxTreeItemId();
	last_ld = wxTreeItemId();
}

void wxPropertyCheckTree::On_Left_DClick(wxMouseEvent& event)
{
	int flags;

	HitTest(event.GetPosition(), flags);

	//double clicks on buttons can be annoying, so we'll ignore those
	//but all other double clicks will just have 1 more click added to them

	//without this, the check boxes are not as responsive as they should be
	if (!(flags & wxTREE_HITTEST_ONITEMBUTTON))
	{
		On_Left_Down(event);
		On_Left_Up(event);
	}
}

void wxPropertyCheckTree::On_Left_Down(wxMouseEvent& event)
{
	int flags;
	wxTreeItemId id = HitTest(event.GetPosition(), flags);
	if (!id.IsOk())
		return;
	int i = GetItemState(id);

	if (id.IsOk() && i >= 0 && on_check_or_label(flags))
	{
		last_ld = id;
		ldhighlight(id);

		wxPropertyCheckTree::SelectItem(id);
	}
}

void wxPropertyCheckTree::On_Left_Up(wxMouseEvent& event)
{
	SetFocus();

	int flags;
	wxTreeItemId id = HitTest(event.GetPosition(), flags);
	if (!id.IsOk())
		return;
	int i = GetItemState(id);

	if (mouse_entered_tree_with_left_down)
	{
		mouse_entered_tree_with_left_down = false;

		if (i >= 0 && on_check_or_label(flags))
		{
			mohighlight(id, false);
			last_mo = id;
		}
	}
	else if (id.IsOk())
	{
		if (flags & wxTREE_HITTEST_ONITEMBUTTON && ItemHasChildren(id))
		{
			if (IsExpanded(id))
			{
				Collapse(id);
			}
			else
			{
				Expand(id);
			}
		}
		else if (i >= 0 && on_check_or_label(flags))
		{
			if (id != last_ld)
			{
				unhighlight(last_ld);
				mohighlight(id, false);
			}
			else
			{
				mohighlight(id, true);
			}

			last_ld = wxTreeItemId();
			last_mo = id;
		}
		else
		{
			unhighlight(last_ld);
			unhighlight(last_mo);
			last_ld = wxTreeItemId();
			last_mo = wxTreeItemId();
		}
	}
	else
	{
		//id is not ok
		unhighlight(last_ld);
		unhighlight(last_mo);
		last_ld = wxTreeItemId();
		last_mo = wxTreeItemId();
	}
}

void wxPropertyCheckTree::On_Mouse_Motion(wxMouseEvent& event)
{
	if (mouse_entered_tree_with_left_down)
	{
		//just ignore everything until the left button is released
		return;
	}

	int flags;
	wxTreeItemId id = HitTest(event.GetPosition(), flags);

	if (event.LeftIsDown())
	{
		//to match the behavior of ordinary check boxes,
		//if we've moved to a new item while holding the mouse button down
		//we want to set the item where the left down click occured to have
		//mouse over highlight.  And if we return to the box where the
		//left down occured, we want to return it to the having the left down highlight

		//I don't understand why this is the behavior
		//of ordinary check boxes, but I'm goin to match it anyway.

		if (id == last_ld)
		{
			if (!last_ld.IsOk())
				return;
			int i = GetItemState(last_ld);
			if (i != UNCHECKED_LEFT_DOWN && i != CHECKED_LEFT_DOWN)
			{
				ldhighlight(last_ld);
			}
		}
		else
		{
			mohighlight(last_ld, false);
		}
	}
	else if (!id.IsOk())
	{
		unhighlight(last_mo);
		last_mo = wxTreeItemId();
	}
	else
	{
		if (!id.IsOk())
			return;

		//4 cases 1 we're still on the same item, but we've moved off the state icon or label
		//        2 we're still on the same item and on the state icon or label - do nothing
		//        3 we're on a new item but not on its state icon or label (or the new item has no state)
		//        4 we're on a new item, it has a state icon, and we're on the state icon or label

		int i = GetItemState(id);

		if (id == last_mo)
		{
			if (i < 0 || !on_check_or_label(flags))
			{
				unhighlight(last_mo);
				last_mo = wxTreeItemId();
			}
			else
			{
				//nothing
			}
		}
		else
		{
			if (i < 0 || !on_check_or_label(flags))
			{
				unhighlight(last_mo);
				last_mo = wxTreeItemId();
			}
			else
			{
				unhighlight(last_mo);
				mohighlight(id, false);
				last_mo = id;
			}
		}
	}
}

void wxPropertyCheckTree::On_Mouse_Wheel(wxMouseEvent& event)
{
	event.Skip();
}

void wxPropertyCheckTree::On_Tree_Focus_Set(wxFocusEvent& event)
{
	//event.Skip();

	//skipping this event will set the last selected item
	//to be highlighted and I want the tree items to only be
	//highlighted by keyboard actionData.
}

void wxPropertyCheckTree::On_Tree_Focus_Lost(wxFocusEvent& event)
{
	unhighlight(last_kf);
	Unselect();

	event.Skip();
}

void wxPropertyCheckTree::SetFocusFromKbd()
{
	if (last_kf.IsOk()) {
		SelectItem(last_kf);
	}

	wxTreeEvent eventChoice(wxEVT_PROPERTY_CHECKTREE_FOCUS, this, wxTreeItemId());
	ProcessWindowEvent(eventChoice);

	wxWindow::SetFocusFromKbd();
}
