////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : output window
////////////////////////////////////////////////////////////////////////////

#include "outputWindow.h"
#include "frontend/mainFrame.h"
#include "frontend/settings/fontcolorsettings.h"

/** Enumeration of commands and child windows. */
enum
{
	idcmdUndo = 10,
	idcmdRedo = 11,
	idcmdCut = 12,
	idcmdCopy = 13,
	idcmdPaste = 14,
	idcmdDelete = 15,
	idcmdSelectAll = 16,

	idcmdClear = 17,
};

wxBEGIN_EVENT_TABLE(COutputWindow, wxStyledTextCtrl)
EVT_LEFT_DCLICK(COutputWindow::OnDoubleClick)
EVT_KEY_DOWN(COutputWindow::OnKeyDown)
EVT_CONTEXT_MENU(COutputWindow::OnContextMenu)
EVT_MENU(idcmdClear, COutputWindow::OnClearOutput)
wxEND_EVENT_TABLE()

#define DEF_LINENUMBER_ID 0
#define DEF_IMAGE_ID 1

COutputWindow::COutputWindow(wxWindow* parent, wxWindowID winid)
	: wxStyledTextCtrl(parent, winid, wxDefaultPosition, wxDefaultSize)
{
	// initialize styles
	StyleClearAll();

	//set Lexer to LEX_CONTAINER: This will trigger the styleneeded event so you can do your own highlighting
	SetLexer(wxSTC_LEX_CONTAINER);

	//Set margin cursor
	for (int margin = 0; margin < GetMarginCount(); margin++)
		SetMarginCursor(margin, wxSTC_CURSORARROW);

	MarkerDefine(eMessage, wxSTC_MARK_SHORTARROW, *wxWHITE, *wxBLACK);
	MarkerDefine(eWarning, wxSTC_MARK_SHORTARROW, *wxWHITE, *wxYELLOW);
	MarkerDefine(eError, wxSTC_MARK_SHORTARROW, *wxWHITE, *wxRED);

	wxAcceleratorEntry entries[2];
	entries[0].Set(wxACCEL_CTRL, (int) 'A', idcmdSelectAll);
	entries[1].Set(wxACCEL_CTRL, (int) 'C', idcmdCopy);

	wxAcceleratorTable accel(2, entries);
	SetAcceleratorTable(accel);

	SetFontColorSettings(mainFrame->GetFontColorSettings());
}

void COutputWindow::SetFontColorSettings(const FontColorSettings& settings)
{
	// For some reason StyleSetFont takes a (non-const) reference, so we need to make
	// a copy before passing it in.
	wxFont font = settings.GetFont();

	StyleClearAll();
	StyleSetFont(wxSTC_STYLE_DEFAULT, font);

	SetSelForeground(true, settings.GetColors(FontColorSettings::DisplayItem_Selection).foreColor);
	SetSelBackground(true, settings.GetColors(FontColorSettings::DisplayItem_Selection).backColor);

	font = settings.GetFont(FontColorSettings::DisplayItem_Default);

	StyleSetFont(wxSTC_C_DEFAULT, font);
	StyleSetFont(wxSTC_C_IDENTIFIER, font);

	SetMarginType(DEF_LINENUMBER_ID, wxSTC_MARGIN_NUMBER);
	SetMarginWidth(DEF_LINENUMBER_ID, 0);

	// set margin as unused
	SetMarginType(DEF_IMAGE_ID, wxSTC_MARGIN_SYMBOL);
	SetMarginMask(DEF_IMAGE_ID, ~(1024 | 256 | 512 | 128 | 64 | wxSTC_MASK_FOLDERS));
	StyleSetBackground(DEF_IMAGE_ID, *wxWHITE);

	SetMarginWidth(DEF_IMAGE_ID, FromDIP(16));
	SetMarginSensitive(DEF_IMAGE_ID, true);

	SetEditable(false);
}

void COutputWindow::OutputMessage(const wxString& message, 
	const wxString &fileName, const wxString &docPath,
	int currLine)
{
	SharedOutput(message, eStatusMessage::eMessage, 
		fileName, docPath, currLine);
}

void COutputWindow::OutputWarning(const wxString& message, 
	const wxString &fileName, const wxString &docPath,
	int currLine)
{
	SharedOutput(message, eStatusMessage::eWarning,
		fileName, docPath,
		currLine);
}

void COutputWindow::OutputError(const wxString& message,
	const wxString &fileName, const wxString &docPath,
	int currLine)
{
	SharedOutput(message, eStatusMessage::eError,
		fileName, docPath,
		currLine);
}

void COutputWindow::SharedOutput(const wxString &message, eStatusMessage status, 
	const wxString &fileName, const wxString &docPath, 
	int currLine)
{
	int beforeAppendPosition = GetInsertionPoint();
	int beforeAppendLastPosition = GetLastPosition();

	Freeze();

	unsigned int lastLine = GetLineCount();

	if (currLine != wxNOT_FOUND) {
		m_aCodeInfo.insert_or_assign(lastLine, code_info { fileName, docPath,
			currLine });
	}

	SetEditable(true);
	AppendText(message + '\n');
	SetEditable(false);

	unsigned int newLine = GetLineCount();

	MarkerAdd(
		lastLine - 1,
		status);

	Thaw();

	SetInsertionPoint(beforeAppendPosition);

	if (beforeAppendPosition == beforeAppendLastPosition)
	{
		SetInsertionPoint(GetLastPosition());
		ShowPosition(GetLastPosition());
		ScrollLines(-1);
	}

	// update output window 
	mainFrame->Update();
}

int COutputWindow::GetCurrentLine() const
{
	long pos = GetInsertionPoint();

	long x, y;
	PositionToXY(pos, &x, &y);

	return y;
}

#include "common/docManager.h"
#include "metadata/metadata.h"
#include "frontend/metatree/metatreeWnd.h"

void COutputWindow::OnDoubleClick(wxMouseEvent& event)
{
	wxTextCoord col, row;
	HitTest(event.GetPosition(), &col, &row);

	for (auto pair : m_aCodeInfo) {

		if (pair.first >= row) {

			auto code = pair.second; 

			if (code.m_fileName.IsEmpty()) {
				IMetadataTree *metaTree = metadata->GetMetaTree();
				wxASSERT(metaTree);
				metaTree->EditModule(code.m_docPath, code.m_currLine, false);
			}

			if (!code.m_fileName.IsEmpty()) {
				IMetaDocument *foundedDoc = dynamic_cast<IMetaDocument *>(
					docManager->FindDocumentByPath(code.m_fileName)
					);

				if (!foundedDoc) {
					foundedDoc = dynamic_cast<IMetaDocument *>(
						docManager->CreateDocument(code.m_fileName, wxDOC_SILENT)
						);
				}

				if (foundedDoc) {
					IMetadata *metaData = foundedDoc->GetMetadata();
					wxASSERT(metaData);
					IMetadataTree *metaTree = metaData->GetMetaTree();
					wxASSERT(metaTree);
					metaTree->EditModule(code.m_docPath, code.m_currLine, false);
				}
			}
			break;
		}
	}

	event.Skip();
}

void COutputWindow::OnContextMenu(wxContextMenuEvent &event)
{
	wxPoint pt = event.GetPosition();
	ScreenToClient(&pt.x, &pt.y);

	/*
	  Show context menu at event point if it's within the window,
	  or at caret location if not
	*/
	wxHitTest ht = wxStyledTextCtrl::HitTest(pt);
	if (ht != wxHT_WINDOW_INSIDE) {
		pt = this->PointFromPosition(this->GetCurrentPos());
	}

	wxMenu *popupMenu = new wxMenu;

	wxMenuItem *menuItemCopy = popupMenu->Append(idcmdCopy, wxT("Copy"));
	menuItemCopy->Enable(wxStyledTextCtrl::CanCopy());
	wxMenuItem *menuItemClear = popupMenu->Append(idcmdClear, wxT("Clear"));

	wxStyledTextCtrl::PopupMenu(popupMenu, pt);
	//event.Skip();
}

void COutputWindow::OnClearOutput(wxCommandEvent & event)
{
	m_aCodeInfo.clear(); 

	SetEditable(true);
	wxStyledTextCtrl::ClearAll();
	SetEditable(false);

	event.Skip();
}

void COutputWindow::OnKeyDown(wxKeyEvent &event)
{
	event.Skip();
}
