﻿#include "form.h"
#include "frontend/visualView/ctrl/sizers.h"

inline wxString GetClassType(const wxString& className)
{
	IControlTypeCtor* objectSingle =
		dynamic_cast<IControlTypeCtor*>(CValue::GetAvailableCtor(className));
	wxASSERT(objectSingle);
	return objectSingle->GetTypeControlName();
}

inline void SetDefaultLayoutProperties(CValueSizerItem* sizerItem)
{
	IValueFrame* child = sizerItem->GetChild(0);
	const wxString& obj_type = child->GetObjectTypeName();

	if (obj_type == wxT("sizer")) {
		sizerItem->SetBorder(0);
		sizerItem->SetFlagBorder(wxALL);
		sizerItem->SetFlagState(wxEXPAND);
	}
	else if (child->GetClassName() == wxT("splitter") ||
		child->GetClassName() == wxT("spacer")) {
		sizerItem->SetProportion(1);
		sizerItem->SetFlagState(wxEXPAND);
	}
	else if (child->GetClassName() == wxT("staticline")) {
		sizerItem->SetFlagBorder(wxALL);
		sizerItem->SetFlagState(wxEXPAND);
	}
	else if (child->GetClassName() == wxT("toolbar")) {
		sizerItem->SetBorder(0);
		sizerItem->SetFlagBorder(wxALL);
		sizerItem->SetFlagState(wxEXPAND);
	}
	else if (child->GetClassName() == wxT("notebook")) {
		sizerItem->SetProportion(1);
		sizerItem->SetFlagBorder(wxALL);
		sizerItem->SetFlagState(wxEXPAND);
	}
	else if (obj_type == wxT("widget") ||
		obj_type == wxT("statusbar")) {
		sizerItem->SetProportion(0);
		sizerItem->SetFlagBorder(wxALL);
		sizerItem->SetFlagState(wxSHRINK);
	}
	else if (obj_type == wxT("container")) {
		sizerItem->SetProportion(1);
		sizerItem->SetFlagBorder(wxALL);
		sizerItem->SetFlagState(wxEXPAND);
	}
}

IValueFrame* CValueForm::NewObject(const class_identifier_t& clsid, IValueFrame* controlParent, const CValue& generateId)
{
	if (CValue::IsRegisterCtor(clsid)) {
		IValueFrame* newControl = nullptr;
		CValue* ppParams[] = {this, controlParent, const_cast<CValue*>(&generateId)};
		try {
			newControl = CValue::CreateAndConvertObjectRef< IValueFrame>(clsid, ppParams, 3);
			newControl->IncrRef();
		}
		catch (...) {
			return nullptr;
		}
		return newControl;
	}
	return nullptr;
}

#include "frontend/visualView/ctrl/toolBar.h"

void CValueForm::ResolveNameConflict(IValueFrame* control)
{
	// Save the original name for use later.
	wxString originalName = control->GetControlName();

	if (originalName.IsEmpty()) {
		const IValueFrame* parentControl = control->GetParent();
		if (parentControl != nullptr && g_controlToolBarItemCLSID == control->GetClassType()) {
			originalName = parentControl->GetControlName() + wxT("_") + control->GetClassName();
		}
		else if (parentControl != nullptr && g_controlToolBarSeparatorCLSID == control->GetClassType()) {
			originalName = parentControl->GetControlName() + wxT("_") + control->GetClassName();
		}
		else {
			originalName = control->GetClassName();
		}
	}

	// el nombre no puede estar repetido dentro del mismo form
	IValueFrame* top = control->GetOwnerForm();
	wxASSERT(top);

	// construimos el conjunto de nombres
	std::set<wxString> name_set;

	std::function<void(IValueFrame*, IValueFrame*, std::set< wxString >&)> buildNameSet = [&buildNameSet](IValueFrame* object, IValueFrame* top, std::set< wxString >& name_set)
	{
		wxASSERT(object);
		if (object != top) {
			name_set.emplace(top->GetControlName());
		}
		for (unsigned int i = 0; i < top->GetChildCount(); i++) {
			buildNameSet(object, top->GetChild(i), name_set);
		}
	};

	buildNameSet(control, top, name_set);

	// comprobamos si hay conflicto
	std::set<wxString>::iterator it = name_set.find(originalName);

	int i = 0;

	wxString name = originalName; // The name that gets incremented.

	while (it != name_set.end()) {
		i++;
		name = wxString::Format(wxT("%s%i"), originalName.c_str(), i);
		it = name_set.find(name);
	}

	control->SetControlName(name);
}

IValueFrame* CValueForm::CreateObject(const wxString& className, IValueFrame* controlParent)
{
	IValueFrame* object = nullptr;
	wxString classType = ::GetClassType(className);

	if (controlParent) {
		bool sizer = false;

		if (classType == wxT("form")) sizer = true;
		else if (classType == wxT("sizer")) sizer = controlParent->GetObjectTypeName() == wxT("sizer") || controlParent->GetObjectTypeName() == wxT("form") ? false : true;

		//FIXME! Esto es un parche para evitar crear los tipos menubar,statusbar y
		//toolbar en un form que no sea wxFrame.
		//Hay que modificar el conjunto de tipos para permitir tener varios tipos
		//de forms (como childType de project), pero hay mucho código no válido
		//para forms que no sean de tipo "form". Dicho de otra manera, hay
		//código que dependen del nombre del tipo, cosa que hay que evitar.
		if (controlParent->GetObjectTypeName() == wxT("form") && controlParent->GetClassName() != wxT("frontendForm") &&
			(classType == wxT("statusbar") ||
				classType == wxT("menubar") ||
				classType == wxT("ribbonbar") ||
				classType == wxT("toolbar")))

			return nullptr; // tipo no válido

		// No menu dropdown for wxToolBar until wx 2.9 :(
		if (controlParent->GetObjectTypeName() == wxT("tool"))
		{
			IValueFrame* gParent = controlParent->GetParent();

			if (
				(gParent->GetClassName() == wxT("toolbar")) &&
				(className == wxT("menu"))
				)
				return nullptr; // not a valid type
		}

		if (controlParent->GetObjectTypeName() == wxT("toolbar"))
		{
			if (classType == wxT("tool") /*|| classType == wxT("widget")*/)
			{
				object = NewObject(className, controlParent);
				//set enabled item
				object->SetReadOnly(controlParent->IsEditable());
			}
		}
		else if (controlParent->GetObjectTypeName() == wxT("notebook"))
		{
			if (classType == wxT("notebookPage")) {
				object = NewObject(className, controlParent);
				//set enabled item
				object->SetReadOnly(controlParent->IsEditable());
			}
		}
		else if (controlParent->GetObjectTypeName() == wxT("container")
			&& controlParent->GetClassName() == wxT("tablebox"))
		{
			if (classType == wxT("tableboxColumn"))
			{
				object = NewObject(className, controlParent);
				//set enabled item
				object->SetReadOnly(controlParent->IsEditable());
			}
		}
		else if (controlParent->GetObjectTypeName() == wxT("notebookPage"))
		{
			CValueSizerItem* sizerItem = NewObject<CValueSizerItem>("sizerItem", controlParent);
			IValueFrame* obj = NewObject(className, sizerItem);

			if (controlParent) {
				sizerItem->SetReadOnly(controlParent->IsEditable());
			}

			// la siguiente condición debe cumplirse siempre
			// ya que un item debe siempre contener a otro objeto
			if (obj) {
				//set enabled item
				obj->SetReadOnly(sizerItem->IsEditable());

				// sizerItem es un tipo de objeto reservado, para que el uso sea
				// más práctico se asignan unos valores por defecto en función
				// del tipo de objeto creado
				if (sizerItem->IsSubclassOf(wxT("sizerItem"))) {
					SetDefaultLayoutProperties(sizerItem);
				}

				object = sizerItem;
			}
		}
		else if (controlParent->GetComponentType() == COMPONENT_TYPE_FRAME ||
			controlParent->GetComponentType() == COMPONENT_TYPE_SIZER ||
			controlParent->GetComponentType() == COMPONENT_TYPE_SIZERITEM)
		{
			CValueSizerItem* sizerItem = NewObject< CValueSizerItem>("sizerItem", controlParent);
			IValueFrame* obj = NewObject(className, sizerItem);

			if (controlParent) {
				sizerItem->SetReadOnly(controlParent->IsEditable());
			}

			// la siguiente condición debe cumplirse siempre
			// ya que un item debe siempre contener a otro objeto
			if (obj) {
				//set enabled item
				obj->SetReadOnly(sizerItem->IsEditable());
				// sizerItem es un tipo de objeto reservado, para que el uso sea
				// más práctico se asignan unos valores por defecto en función
				// del tipo de objeto creado
				if (sizerItem->IsSubclassOf(wxT("sizerItem"))) {
					SetDefaultLayoutProperties(sizerItem);
				}

				object = sizerItem;
			}
		}
	}
	else // controlParent == nullptr;
	{
		object = NewObject(className, nullptr);
	}

	return object;
}

IValueFrame* CValueForm::CopyObject(IValueFrame* srcControl)
{
	if (srcControl->GetComponentType() == COMPONENT_TYPE_FRAME)
		return nullptr;

	assert(srcControl);
	IValueFrame* copyObj = NewObject(srcControl->GetClassName(), srcControl->GetParent(), false); // creamos la copia
	assert(copyObj);

	// copiamos las propiedades
	unsigned int i;
	unsigned int count = srcControl->GetPropertyCount();
	for (i = 0; i < count; i++) {
		IProperty* objProp = srcControl->GetProperty(i);
		assert(objProp);

		if (objProp->GetName() == wxT("name"))
			continue;

		IProperty* copyProp = copyObj->GetProperty(objProp->GetName());
		assert(copyProp);

		wxString propValue = objProp->GetValue();
		copyProp->SetValue(propValue);
	}

	// ...and the event handlers
	count = srcControl->GetEventCount();
	for (i = 0; i < count; i++) {
		IEvent* event = srcControl->GetEvent(i);
		IEvent* copyEvent = copyObj->GetEvent(event->GetName());
		copyEvent->SetValue(event->GetValue());
	}

	copyObj->SetParent(srcControl->GetParent());

	copyObj->GenerateGuid();
	copyObj->GenerateNewID();

	// creamos recursivamente los hijos
	count = srcControl->GetChildCount();
	for (i = 0; i < count; i++) {
		IValueFrame* childCopy =
			CopyObject(srcControl->GetChild(i));
		wxASSERT(childCopy);
		if (copyObj) {
			copyObj->AddChild(childCopy);
			childCopy->SetParent(copyObj);
		}
	}

	return copyObj;
}