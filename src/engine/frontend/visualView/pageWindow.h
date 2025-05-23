#ifndef _PAGE_WINDOW_H__
#define _PAGE_WINDOW_H__

#include <wx/panel.h>
#include <wx/sizer.h>

class CPanelPage : public wxPanel
{
	wxDECLARE_DYNAMIC_CLASS(CPanelPage);

public:

	CPanelPage();
	CPanelPage(wxWindow *parent,
		wxWindowID winid = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL | wxNO_BORDER,
		const wxString& name = wxPanelNameStr);

	~CPanelPage();

	void SetOrientation(int orient) { m_boxSizer->SetOrientation(orient); }
	wxBoxSizer *GetSizer() { return m_boxSizer; }

protected:

	wxBoxSizer *m_boxSizer;
};

#endif // !_PAGE_WINDOW_H__
