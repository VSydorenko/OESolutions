////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : value colour  
////////////////////////////////////////////////////////////////////////////

#include "valueColour.h"



//////////////////////////////////////////////////////////////////////
wxIMPLEMENT_DYNAMIC_CLASS(CValueColour, CValue);

CValue::CMethodHelper CValueColour::m_methodHelper;

CValueColour::CValueColour() :
	CValue(eValueTypes::TYPE_VALUE), m_colour()
{
}

CValueColour::CValueColour(const wxColour& colour) :
	CValue(eValueTypes::TYPE_VALUE), m_colour(colour)
{
}

bool CValueColour::Init(CValue** paParams, const long lSizeArray)
{
	if (paParams[0]->GetType() == eValueTypes::TYPE_STRING) {
		m_colour = typeConv::StringToColour(paParams[0]->GetString());
		return true;
	}
	else {
		m_colour = wxColour(
			paParams[0]->GetInteger(),
			paParams[1]->GetInteger(),
			paParams[2]->GetInteger()
		);
		return true;
	}
	return false;
}

enum
{
	enColorR,
	enColorG,
	enColorB
};

void CValueColour::PrepareNames() const
{
	m_methodHelper.ClearHelper();

	m_methodHelper.AppendProp(wxT("r"));
	m_methodHelper.AppendProp(wxT("g"));
	m_methodHelper.AppendProp(wxT("b"));
}

bool CValueColour::SetPropVal(const long lPropNum, const CValue& varPropVal)
{
	switch (lPropNum)
	{
	case enColorR:
		m_colour.Set((unsigned char)varPropVal.GetUInteger(), m_colour.Green(), m_colour.Blue());
		return true;
	case enColorG:
		m_colour.Set(m_colour.Red(), (unsigned char)varPropVal.GetUInteger(), m_colour.Blue());
		return true;
	case enColorB:
		m_colour.Set(m_colour.Red(), m_colour.Green(), (unsigned char)varPropVal.GetUInteger());
		return true;
	}
	return false;
}

bool CValueColour::GetPropVal(const long lPropNum, CValue& pvarPropVal)
{
	switch (lPropNum)
	{
	case enColorR:
		pvarPropVal = m_colour.Red();
		return true;
	case enColorG:
		pvarPropVal = m_colour.Green();
		return true;
	case enColorB:
		pvarPropVal = m_colour.Blue();
		return true;
	}
	return false;
}

//**********************************************************************
//*                       Runtime register                             *
//**********************************************************************

VALUE_TYPE_REGISTER(CValueColour, "colour", string_to_clsid("VL_COLOR"));