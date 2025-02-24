#ifndef __META_CONTEXT_H__
#define __META_CONTEXT_H__

#include "backend/metaCollection/attribute/metaAttributeObject.h"

class BACKEND_API IMetaObjectContextData
	: public IMetaObject {
	wxDECLARE_ABSTRACT_CLASS(IMetaObjectContextData);
public:

	IMetaObjectContextData(
		const wxString& strName = wxEmptyString,
		const wxString& synonym = wxEmptyString,
		const wxString& comment = wxEmptyString
	) : IMetaObject(strName, synonym, comment) {
	}

	virtual ~IMetaObjectContextData() {}

	//override base objects 
	virtual std::vector<IMetaObjectAttribute*> GetGenericAttributes() const = 0;

protected:

	CMetaObjectAttributeDefault* CreateBoolean(const wxString& name, const wxString& synonym, const wxString& comment,
		eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, false, eValueTypes::TYPE_BOOLEAN, useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateBoolean(const wxString& name, const wxString& synonym, const wxString& comment,
		bool fillCheck, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, fillCheck, eValueTypes::TYPE_BOOLEAN, useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateBoolean(const wxString& name, const wxString& synonym, const wxString& comment,
		bool fillCheck, const bool& defValue, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, fillCheck, CValue(defValue), useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateNumber(const wxString& name, const wxString& synonym, const wxString& comment,
		unsigned char precision, unsigned char scale, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, qualifierNumber_t(precision, scale), false, eValueTypes::TYPE_NUMBER, useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateNumber(const wxString& name, const wxString& synonym, const wxString& comment,
		unsigned char precision, unsigned char scale, bool fillCheck, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, qualifierNumber_t(precision, scale), fillCheck, eValueTypes::TYPE_NUMBER, useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateNumber(const wxString& name, const wxString& synonym, const wxString& comment,
		unsigned char precision, unsigned char scale, bool fillCheck, const number_t& defValue, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, qualifierNumber_t(precision, scale), fillCheck, CValue(defValue), useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateDate(const wxString& name, const wxString& synonym, const wxString& comment,
		eDateFractions dateTime, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, qualifierDate_t(dateTime), false, eValueTypes::TYPE_DATE, useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateDate(const wxString& name, const wxString& synonym, const wxString& comment,
		eDateFractions dateTime, bool fillCheck, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, qualifierDate_t(dateTime), fillCheck, eValueTypes::TYPE_DATE, useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateDate(const wxString& name, const wxString& synonym, const wxString& comment,
		eDateFractions dateTime, bool fillCheck, const wxDateTime& defValue, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, qualifierDate_t(dateTime), fillCheck, CValue(defValue), useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateString(const wxString& name, const wxString& synonym, const wxString& comment,
		unsigned short length, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, qualifierString_t(length), false, eValueTypes::TYPE_STRING, useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateString(const wxString& name, const wxString& synonym, const wxString& comment,
		unsigned short length, bool fillCheck, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, qualifierString_t(length), fillCheck, eValueTypes::TYPE_STRING, useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateString(const wxString& name, const wxString& synonym, const wxString& comment,
		unsigned short length, bool fillCheck, const wxString& defValue, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, qualifierString_t(length), fillCheck, CValue(defValue), useItem, selectMode);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CMetaObjectAttributeDefault* CreateEmptyType(const wxString& name, const wxString& synonym, const wxString& comment,
		eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, false, useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateEmptyType(const wxString& name, const wxString& synonym, const wxString& comment,
		bool fillCheck, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, fillCheck, useItem, selectMode);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CMetaObjectAttributeDefault* CreateSpecialType(const wxString& name, const wxString& synonym, const wxString& comment,
		const class_identifier_t& clsid, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, clsid, false, CValue(), useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateSpecialType(const wxString& name, const wxString& synonym, const wxString& comment,
		const class_identifier_t& clsid, const CValue& defValue, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, clsid, false, defValue, useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateSpecialType(const wxString& name, const wxString& synonym, const wxString& comment,
		const class_identifier_t& clsid, bool fillCheck, const CValue& defValue, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, clsid, fillCheck, defValue, useItem, selectMode);
	}

	CMetaObjectAttributeDefault* CreateSpecialType(const wxString& name, const wxString& synonym, const wxString& comment,
		const class_identifier_t& clsid, const typeDescription_t::typeData_t& descr,
		bool fillCheck, const CValue& defValue, eItemMode useItem = eItemMode::eItemMode_Item, eSelectMode selectMode = eSelectMode::eSelectMode_Items) {
		return IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectAttributeDefault>(name, synonym, comment, clsid, descr, fillCheck, defValue, useItem, selectMode);
	}
};

#endif