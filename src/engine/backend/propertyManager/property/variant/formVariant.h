#ifndef __FORM_VARIANT_H__
#define __FORM_VARIANT_H__

#include "moduleVariant.h"

class BACKEND_API wxVariantDataForm : public wxVariantDataModule {
	wxString MakeString() const;
public:

	//set form code 
	void SetFormData(const wxMemoryBuffer& formData) { m_formData = formData; }
	wxMemoryBuffer& GetFormData() { return m_formData; }

	wxVariantDataForm(
		const wxMemoryBuffer& memory = wxMemoryBuffer(),
		const wxString& moduleData = wxEmptyString
	) : wxVariantDataModule(moduleData), m_formData(memory) {
	}

	virtual bool Eq(wxVariantData& data) const {
		wxVariantDataForm* srcData = dynamic_cast<wxVariantDataForm*>(&data);
		if (srcData != nullptr) {
			return m_moduleData == srcData->m_moduleData &&
				m_formData == srcData->m_formData;
		}
		return false;
	}

#if wxUSE_STD_IOSTREAM
	virtual bool Write(wxSTD ostream& str) const {
		str << MakeString();
		return true;
	}
#endif

	virtual bool Write(wxString& str) const {
		str = MakeString();
		return true;
	}

	virtual wxString GetType() const { return wxT("wxVariantDataForm"); }

private:
	wxMemoryBuffer m_formData;
};

#endif