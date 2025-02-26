#include "backend_mainFrame.h"

IBackendDocMDIFrame* IBackendDocMDIFrame::ms_mainFrame = nullptr;

///////////////////////////////////////////////////////////////////////////

IBackendDocMDIFrame::IBackendDocMDIFrame()
{
	wxASSERT(ms_mainFrame == nullptr);

	if (ms_mainFrame == nullptr) {
		ms_mainFrame = this;
	}
}

IBackendDocMDIFrame* IBackendDocMDIFrame::GetDocMDIFrame()
{
	return ms_mainFrame;
}

///////////////////////////////////////////////////////////////////////////

IBackendDocMDIFrame::~IBackendDocMDIFrame() {

	wxASSERT(ms_mainFrame != nullptr);

	if (ms_mainFrame != nullptr) {
		ms_mainFrame = nullptr;
	}
}

void IBackendDocMDIFrame::RaiseFrame() {
	wxFrame* const mainFrame = GetFrameHandler();
	if (mainFrame != nullptr && mainFrame->IsFocusable()) {
		mainFrame->Iconize(false); // restore the window if minimized
		mainFrame->SetFocus();     // focus on my window
		mainFrame->Raise();        // bring window to front
	}
}
