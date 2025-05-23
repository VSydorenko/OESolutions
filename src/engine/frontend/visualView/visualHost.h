#ifndef __VISUAL_EDITOR_VIEW_H__
#define __VISUAL_EDITOR_VIEW_H__

#include "visual.h"

#include "frontend/visualView/ctrl/form.h"
#include "frontend/visualView/ctrl/sizers.h"
#include "frontend/visualView/ctrl/widgets.h"

class CVisualHost : public IVisualHost {
	bool			m_formDemonstration;
	CValueForm* m_valueForm;
	CMetaDocument* m_document;
	bool m_dataViewSizeChanged;
	wxSize m_dataViewSize;
public:

	// ctor
	CVisualHost(CMetaDocument* document, CValueForm* valueForm, wxWindow* parent, bool demonstration = false);
	virtual ~CVisualHost();

	void CreateFrame();
	void UpdateFrame();

	virtual bool IsDemonstration() const { return m_formDemonstration; }
	virtual CValueForm* GetValueForm() const { return m_valueForm; }
	virtual void SetValueForm(CValueForm* valueForm) { m_valueForm = valueForm; }

	virtual wxWindow* GetParentBackgroundWindow() const { return const_cast<CVisualHost*>(this); }

	virtual wxWindow* GetBackgroundWindow() const { return const_cast<CVisualHost*>(this); }

	void ShowForm();
	void ActivateForm();
	void UpdateForm();
	bool CloseForm();

protected:
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);
protected:
	friend class CValueForm;
};

#endif