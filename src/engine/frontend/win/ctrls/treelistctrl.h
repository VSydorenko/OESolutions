/////////////////////////////////////////////////////////////////////////////
// Name:        treelistctrl.h
// Purpose:     wxTreeListExtCtrl class
// Created:     01/02/97
// Author:      Robert Roebling
// Maintainer:  Ronan Chartois (pgriddev)
// Version:     $Id: treelistctrl.h 3043 2012-07-31 19:28:14Z pgriddev $
// Copyright:   (c) 2004-2011 Robert Roebling, Julian Smart, Alberto Griggio,
//              Vadim Zeitlin, Otto Wyss, Ronan Chartois
// Licence:     wxWindows
/////////////////////////////////////////////////////////////////////////////

#ifndef TREELISTCTRL_H
#define TREELISTCTRL_H

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "treelistctrl.h"
#endif

#include <wx/treectrl.h>
#include <wx/control.h>
#include <wx/pen.h>
#include <wx/listctrl.h> // for wxListEvent
#if wxUSE_XRC
#include <wx/xrc/xmlres.h>
#endif

#include "frontend/frontend.h"

class FRONTEND_API wxTreeListItem;
class FRONTEND_API wxTreeListHeaderWindow;
class FRONTEND_API wxTreeListMainWindow;

#define wxTR_COLUMN_LINES 0x1000 // put border around items
#define wxTR_VIRTUAL      0x4000 // The application provides items text on demand.

// Using this typedef removes an ambiguity when calling Remove()
#ifdef __WXMSW__
#if !wxCHECK_VERSION(2, 5, 0)
typedef long wxTreeItemIdValue;
#endif
#endif

//-----------------------------------------------------------------------------
// wxTreeListColumnAttrs
//-----------------------------------------------------------------------------

enum {
	DEFAULT_COL_WIDTH = 100
};

class wxTreeListColumnInfo : public wxObject {

public:
	wxTreeListColumnInfo(const wxString &text = wxEmptyString,
		int width = DEFAULT_COL_WIDTH,
		int flag = wxALIGN_LEFT,
		int image = -1,
		bool shown = true,
		bool edit = false) : m_text(text)
	{
		m_width = width;
		m_flag = flag;
		m_image = image;
		m_selected_image = -1;
		m_shown = shown;
		m_edit = edit;
	}

	wxTreeListColumnInfo(const wxTreeListColumnInfo& other) : m_text(other.m_text)
	{
		m_width = other.m_width;
		m_flag = other.m_flag;
		m_image = other.m_image;
		m_selected_image = other.m_selected_image;
		m_shown = other.m_shown;
		m_edit = other.m_edit;
	}

	~wxTreeListColumnInfo() {}

	// get/set
	wxString GetText() const { return m_text; }
	wxTreeListColumnInfo& SetText(const wxString& text) { m_text = text; return *this; }

	int GetWidth() const { return m_shown ? m_width : 0; }
	wxTreeListColumnInfo& SetWidth(int width) { m_width = width; return *this; }

	int GetAlignment() const { return m_flag; }
	wxTreeListColumnInfo& SetAlignment(int flag) { m_flag = flag; return *this; }

	int GetImage() const { return m_image; }
	wxTreeListColumnInfo& SetImage(int image) { m_image = image; return *this; }

	int GetSelectedImage() const { return m_selected_image; }
	wxTreeListColumnInfo& SetSelectedImage(int image) { m_selected_image = image; return *this; }

	bool IsEditable() const { return m_edit; }
	wxTreeListColumnInfo& SetEditable(bool edit)
	{
		m_edit = edit; return *this;
	}

	bool IsShown() const { return m_shown; }
	wxTreeListColumnInfo& SetShown(bool shown) { m_shown = shown; return *this; }

private:
	wxString m_text;
	int m_width;
	int m_flag;
	int m_image;
	int m_selected_image;
	bool m_shown;
	bool m_edit;
};

//----------------------------------------------------------------------------
// wxTreeListExtCtrl - the multicolumn tree control
//----------------------------------------------------------------------------

// modes for navigation
const int wxTL_MODE_NAV_FULLTREE = 0x0000; // default
const int wxTL_MODE_NAV_EXPANDED = 0x0001;
const int wxTL_MODE_NAV_VISIBLE = 0x0002;
const int wxTL_MODE_NAV_LEVEL = 0x0004;

// modes for FindItem
const int wxTL_MODE_FIND_EXACT = 0x0000; // default
const int wxTL_MODE_FIND_PARTIAL = 0x0010;
const int wxTL_MODE_FIND_NOCASE = 0x0020;

// additional flag for HitTest
const int wxTREE_HITTEST_ONITEMCOLUMN = 0x2000;
extern const wxChar* wxTreeListExtCtrlNameStr;

class FRONTEND_API wxTreeListExtCtrl : public wxControl
{
	friend class wxTreeListHeaderWindow;
	friend class wxTreeListMainWindow;
	friend class wxTreeListItem;
public:

	// ---------- creation ----------

	wxTreeListExtCtrl()
		: m_header_win(0), m_main_win(0), m_headerHeight(0)
	{}

	wxTreeListExtCtrl(wxWindow *parent, wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTR_DEFAULT_STYLE,
		const wxValidator &validator = wxDefaultValidator,
		const wxString& name = wxTreeListExtCtrlNameStr)
		: m_header_win(0), m_main_win(0), m_headerHeight(0)
	{
		Create(parent, id, pos, size, style, validator, name);
	}

	virtual ~wxTreeListExtCtrl() {}

	bool Create(wxWindow *parent, wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTR_DEFAULT_STYLE,
		const wxValidator &validator = wxDefaultValidator,
		const wxString& name = wxTreeListExtCtrlNameStr);

	void Refresh(bool erase = TRUE, const wxRect* rect = nullptr);
	void SetFocus();


	// ---------- general methods ----------

	// get the total number of items in the control
	unsigned int GetCount() const;

	// indent is the number of pixels the children are indented relative to
	// the parents position. SetIndent() also redraws the control
	// immediately.
	unsigned int GetIndent() const;
	void SetIndent(unsigned int indent);

	// line spacing is the space above and below the text on each line
	unsigned int GetLineSpacing() const;
	void SetLineSpacing(unsigned int spacing);

	// image list: these functions allow to associate an image list with
	// the control and retrieve it. Note that when assigned with
	// SetImageList, the control does _not_ delete
	// the associated image list when it's deleted in order to allow image
	// lists to be shared between different controls. If you use
	// AssignImageList, the control _does_ delete the image list.
	//
	// The normal image list is for the icons which correspond to the
	// normal tree item state (whether it is selected or not).
	// Additionally, the application might choose to show a state icon
	// which corresponds to an app-defined item state (for example,
	// checked/unchecked) which are taken from the state image list.
	wxImageList *GetImageList() const;
	wxImageList *GetStateImageList() const;
	wxImageList *GetButtonsImageList() const;

	void SetImageList(wxImageList *imageList);
	void SetStateImageList(wxImageList *imageList);
	void SetButtonsImageList(wxImageList *imageList);
	void AssignImageList(wxImageList *imageList);
	void AssignStateImageList(wxImageList *imageList);
	void AssignButtonsImageList(wxImageList *imageList);

	void SetToolTip(const wxString& tip);
	void SetToolTip(wxToolTip *tip);
	void SetItemToolTip(const wxTreeItemId& item, const wxString &tip);

	// ---------- Functions to work with columns ----------

	// adds a column
	void AddColumn(const wxString& text,
		int width = DEFAULT_COL_WIDTH,
		int flag = wxALIGN_LEFT,
		int image = -1,
		bool shown = true,
		bool edit = false) {
		AddColumn(wxTreeListColumnInfo(text, width, flag, image, shown, edit));
	}
	void AddColumn(const wxTreeListColumnInfo& colInfo);

	// inserts a column before the given one
	void InsertColumn(int before,
		const wxString& text,
		int width = DEFAULT_COL_WIDTH,
		int flag = wxALIGN_LEFT,
		int image = -1,
		bool shown = true,
		bool edit = false) {
		InsertColumn(before,
			wxTreeListColumnInfo(text, width, flag, image, shown, edit));
	}
	void InsertColumn(int before, const wxTreeListColumnInfo& colInfo);

	// deletes the given column - does not delete the corresponding column
	void RemoveColumn(int column);

	// returns the number of columns in the ctrl
	int GetColumnCount() const;

	// tells which column is the "main" one, i.e. the "threaded" one
	void SetMainColumn(int column);
	int GetMainColumn() const;

	void SetColumn(int column, const wxTreeListColumnInfo& colInfo);
	wxTreeListColumnInfo GetColumn(int column);
	const wxTreeListColumnInfo& GetColumn(int column) const;

	void SetColumnText(int column, const wxString& text);
	wxString GetColumnText(int column) const;

	void SetColumnWidth(int column, int width);
	int GetColumnWidth(int column) const;

	void SetColumnAlignment(int column, int flag);
	int GetColumnAlignment(int column) const;

	void SetColumnImage(int column, int image);
	int GetColumnImage(int column) const;

	void SetColumnShown(int column, bool shown = true);
	bool IsColumnShown(int column) const;

	void SetColumnEditable(int column, bool edit = true);
	bool IsColumnEditable(int column) const;

	// ----------  Functions to work with items. ----------

	// accessors (most properties have a default at row/item level)
	// ---------

	wxString GetItemText(const wxTreeItemId& item)             const { return GetItemText(item, GetMainColumn()); }
	wxString GetItemText(const wxTreeItemId& item, int column) const;

	int GetItemImage(const wxTreeItemId& item, wxTreeItemIcon which = wxTreeItemIcon_Normal) const;
	int GetItemImage(const wxTreeItemId& item, int column) const;

	wxTreeItemData *GetItemData(const wxTreeItemId& item)             const;
	wxTreeItemData *GetItemData(const wxTreeItemId& item, int column) const;

	bool GetItemBold(const wxTreeItemId& item)             const;
	bool GetItemBold(const wxTreeItemId& item, int column) const;

	wxColour GetItemTextColour(const wxTreeItemId& item)             const;
	wxColour GetItemTextColour(const wxTreeItemId& item, int column) const;

	wxColour GetItemBackgroundColour(const wxTreeItemId& item)             const;
	wxColour GetItemBackgroundColour(const wxTreeItemId& item, int column) const;

	wxFont GetItemFont(const wxTreeItemId& item)             const;
	wxFont GetItemFont(const wxTreeItemId& item, int column) const;

	// modifiers (most properties have a default at row/item level)
	// ---------

	void SetItemText(const wxTreeItemId& item, const wxString& text)
	{
		SetItemText(item, GetMainColumn(), text);
	}
	void SetItemText(const wxTreeItemId& item, int column, const wxString& text);

	// the which parameter is ignored for all columns but the main one
	void SetItemImage(const wxTreeItemId& item, int image, wxTreeItemIcon which = wxTreeItemIcon_Normal);
	void SetItemImage(const wxTreeItemId& item, int column, int image);

	void SetItemData(const wxTreeItemId& item, wxTreeItemData *data);
	void SetItemData(const wxTreeItemId& item, int column, wxTreeItemData *data);

	void SetItemBold(const wxTreeItemId& item, bool bold = true);
	void SetItemBold(const wxTreeItemId& item, int column, bool bold = true);

	void SetItemTextColour(const wxTreeItemId& item, const wxColour& colour);
	void SetItemTextColour(const wxTreeItemId& item, int column, const wxColour& colour);

	void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& colour);
	void SetItemBackgroundColour(const wxTreeItemId& item, int column, const wxColour& colour);

	// font should be of the same height for all items
	void SetItemFont(const wxTreeItemId& item, const wxFont& font);
	void SetItemFont(const wxTreeItemId& item, int column, const wxFont& font);

	// force appearance of [+] button near the item. This is useful to
	// allow the user to expand the items which don't have any children now
	// - but instead add them only when needed, thus minimizing memory
	// usage and loading time.
	void SetItemHasChildren(const wxTreeItemId& item, bool has = true);

	// item status inquiries
	// ---------------------

	// is the item visible (it might be outside the view or not expanded)?
	bool IsVisible(const wxTreeItemId& item, bool fullRow = false, bool within = true) const;
	// does the item has any children?
	bool HasChildren(const wxTreeItemId& item) const;
	// is the item expanded (only makes sense if HasChildren())?
	bool IsExpanded(const wxTreeItemId& item) const;
	// is this item currently selected (the same as has focus)?
	bool IsSelected(const wxTreeItemId& item) const;
	// is item text in bold font?
	bool IsBold(const wxTreeItemId& item)             const { return IsBold(item, GetMainColumn()); }
	bool IsBold(const wxTreeItemId& item, int column) const { return GetItemBold(item, column); }
	// does the layout include space for a button?


	// set the window font
	virtual bool SetFont(const wxFont &font);

	// set the styles.
	void SetWindowStyle(const long styles);
	long GetWindowStyle() const;
	long GetWindowStyleFlag() const { return GetWindowStyle(); }

	// number of children
	// ------------------

	// if 'recursively' is FALSE, only immediate children count, otherwise
	// the returned number is the number of all items in this branch
	unsigned int GetChildrenCount(const wxTreeItemId& item, bool recursively = true);

	// navigation
	// ----------

	// wxTreeItemId.IsOk() will return FALSE if there is no such item

	// get the root tree item
	wxTreeItemId GetRootItem() const;

	// get the item currently selected (may return nullptr if no selection)
	wxTreeItemId GetSelection() const;

	// get the items currently selected, return the number of such item
	unsigned int GetSelections(wxArrayTreeItemIds&) const;

	// get the parent of this item (may return nullptr if root)
	wxTreeItemId GetItemParent(const wxTreeItemId& item) const;

	// for this enumeration function you must pass in a "cookie" parameter
	// which is opaque for the application but is necessary for the library
	// to make these functions reentrant (i.e. allow more than one
	// enumeration on one and the same object simultaneously). Of course,
	// the "cookie" passed to GetFirstChild() and GetNextChild() should be
	// the same!

	// get child of this item
#if !wxCHECK_VERSION(2, 5, 0)
	wxTreeItemId GetFirstChild(const wxTreeItemId& item, long& cookie) const;
	wxTreeItemId GetNextChild(const wxTreeItemId& item, long& cookie) const;
	wxTreeItemId GetPrevChild(const wxTreeItemId& item, long& cookie) const;
	wxTreeItemId GetLastChild(const wxTreeItemId& item, long& cookie) const;
#else
	wxTreeItemId GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
	wxTreeItemId GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
	wxTreeItemId GetPrevChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
	wxTreeItemId GetLastChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
#endif

	// get sibling of this item
	wxTreeItemId GetNextSibling(const wxTreeItemId& item) const;
	wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const;

	// get item in the full tree (currently only for internal use)
	wxTreeItemId GetNext(const wxTreeItemId& item) const;
	wxTreeItemId GetPrev(const wxTreeItemId& item) const;

	// get expanded item, see IsExpanded()
	wxTreeItemId GetFirstExpandedItem() const;
	wxTreeItemId GetNextExpanded(const wxTreeItemId& item) const;
	wxTreeItemId GetPrevExpanded(const wxTreeItemId& item) const;

	// get visible item, see IsVisible()
	wxTreeItemId GetFirstVisibleItem(bool fullRow = false) const;
	wxTreeItemId GetFirstVisible(bool fullRow = false, bool within = true) const;
	wxTreeItemId GetNextVisible(const wxTreeItemId& item, bool fullRow = false, bool within = true) const;
	wxTreeItemId GetPrevVisible(const wxTreeItemId& item, bool fullRow = false, bool within = true) const;
	wxTreeItemId GetLastVisible(bool fullRow = false, bool within = true) const;

	// operations
	// ----------

	// add the root node to the tree
	wxTreeItemId AddRoot(const wxString& text,
		int image = -1, int selectedImage = -1,
		wxTreeItemData *data = nullptr);

	// insert a new item in as the first child of the parent
	wxTreeItemId PrependItem(const wxTreeItemId& parent,
		const wxString& text,
		int image = -1, int selectedImage = -1,
		wxTreeItemData *data = nullptr);

	// insert a new item after a given one
	wxTreeItemId InsertItem(const wxTreeItemId& parent,
		const wxTreeItemId& idPrevious,
		const wxString& text,
		int image = -1, int selectedImage = -1,
		wxTreeItemData *data = nullptr);

	// insert a new item before the one with the given index
	wxTreeItemId InsertItem(const wxTreeItemId& parent,
		unsigned int index,
		const wxString& text,
		int image = -1, int selectedImage = -1,
		wxTreeItemData *data = nullptr);

	// insert a new item in as the last child of the parent
	wxTreeItemId AppendItem(const wxTreeItemId& parent,
		const wxString& text,
		int image = -1, int selectedImage = -1,
		wxTreeItemData *data = nullptr);

	// delete this item (except root) + children and associated data if any
	void Delete(const wxTreeItemId& item);
	// delete all children (but don't delete the item itself)
	void DeleteChildren(const wxTreeItemId& item);
	// delete the root and all its children from the tree
	void DeleteRoot();

	//  change item's parent (return previous parent)
	void SetItemParent(const wxTreeItemId& parent, const wxTreeItemId& item);

	// expand this item
	void Expand(const wxTreeItemId& item);
	// expand this item and all subitems recursively
	void ExpandAll(const wxTreeItemId& item);
	// collapse the item without removing its children
	void Collapse(const wxTreeItemId& item);
	// collapse the item and remove all children
	void CollapseAndReset(const wxTreeItemId& item); //? TODO ???
	// toggles the current state
	void Toggle(const wxTreeItemId& item);

	// set cursor item (indicated by black rectangle)
	void SetCurrentItem(const wxTreeItemId& item = (wxTreeItemId*)nullptr);

	// remove the selection from currently selected item (if any)
	void Unselect();
	void UnselectAll();
	// select this item - return true if selection was allowed (no veto)
	bool SelectItem(const wxTreeItemId& item,
		const wxTreeItemId& last = (wxTreeItemId*)nullptr,
		bool unselect_others = true);
	// select all items in the expanded tree
	void SelectAll();
	// make sure this item is visible (expanding the parent item and/or
	// scrolling to this item if necessary)
	void EnsureVisible(const wxTreeItemId& item);
	// scroll to this item (but don't expand its parent)
	void ScrollTo(const wxTreeItemId& item);

	// The first function is more portable (because easier to implement
	// on other platforms), but the second one returns some extra info.
	wxTreeItemId HitTest(const wxPoint& point)
	{
		int flags; int column; return HitTest(point, flags, column);
	}
	wxTreeItemId HitTest(const wxPoint& point, int& flags)
	{
		int column; return HitTest(point, flags, column);
	}
	wxTreeItemId HitTest(const wxPoint& point, int& flags, int& column);

	// get the bounding rectangle of the item (or of its label only)
	bool GetBoundingRect(const wxTreeItemId& item, wxRect& rect,
		bool textOnly = false) const;

	// Start editing the item label: this (temporarily) replaces the item
	// with a one line edit control. The item will be selected if it hadn't
	// been before.
	void EditLabel(const wxTreeItemId& item)
	{
		EditLabel(item, GetMainColumn());
	}
	// edit item's label of the given column
	void EditLabel(const wxTreeItemId& item, int column);
	void EndEdit(bool isCancelled);

	// virtual mode
	virtual wxString OnGetItemText(wxTreeItemData* item, long column) const;

	// sorting
	// this function is called to compare 2 items and should return -1, 0
	// or +1 if the first item is less than, equal to or greater than the
	// second one. The base class version performs alphabetic comparaison
	// of item labels (GetText)
	virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
	virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2, int column);
	// sort the children of this item using OnCompareItems
	// NB: this function is not reentrant and not MT-safe (TODO)!
	void SortChildren(const wxTreeItemId& item, int column = -1, bool reverseOrder = false);

	// searching (by column only)
	wxTreeItemId FindItem(const wxTreeItemId& item, const wxString& str, int mode = 0) { return FindItem(item, -1, str, mode); }
	wxTreeItemId FindItem(const wxTreeItemId& item, int column, const wxString& str, int mode = 0);

	// overridden base class virtuals
	virtual bool SetBackgroundColour(const wxColour& colour);
	virtual bool SetForegroundColour(const wxColour& colour);

	// drop over item
	void SetDragItem(const wxTreeItemId& item = (wxTreeItemId*)nullptr);

	virtual void DoSetSize(int x, int y,
		int width, int height,
		int sizeFlags = wxSIZE_AUTO);

	virtual wxSize DoGetBestClientSize() const;
	virtual wxSize DoGetBestSize() const;

protected:
	// header window, responsible for column visualization and manipulation
	wxTreeListHeaderWindow* GetHeaderWindow() const
	{
		return m_header_win;
	}
	wxTreeListHeaderWindow* m_header_win;  // future cleanup: make private or remove GetHeaderWindow()

	// main window, the "true" tree ctrl
	wxTreeListMainWindow* GetMainWindow() const
	{
		return m_main_win;
	}
	wxTreeListMainWindow* m_main_win;  // future cleanup: make private or remove GetMainWindow()

	int GetHeaderHeight() const { return m_headerHeight; }

	void CalculateAndSetHeaderHeight();
	void DoHeaderLayout();
	void OnSize(wxSizeEvent& event);

private:
	int m_headerHeight;

	DECLARE_EVENT_TABLE()
	DECLARE_DYNAMIC_CLASS(wxTreeListExtCtrl)
};

/////////////////////////////////////////////////////////////////////////////
#endif /* TREELISTCTRL_H */
