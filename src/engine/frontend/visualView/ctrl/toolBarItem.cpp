#include "toolbar.h"

//***********************************************************************************
//*                           IMPLEMENT_DYNAMIC_CLASS                               *
//***********************************************************************************

wxIMPLEMENT_DYNAMIC_CLASS(CValueToolBarItem, IValueControl);
wxIMPLEMENT_DYNAMIC_CLASS(CValueToolBarSeparator, IValueControl);

//***********************************************************************************
//*                           CValueToolBarItem                               *
//***********************************************************************************

CValueToolBarItem::CValueToolBarItem() : IValueControl()
{
}

void CValueToolBarItem::OnCreated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost, bool first�reated)
{
    CAuiToolBar* auiToolWnd = dynamic_cast<CAuiToolBar*>(wxparent);
    wxASSERT(auiToolWnd);
    wxAuiToolBarItem* m_toolItem = auiToolWnd->AddTool(GetControlID(),
        m_propertyCaption->GetValueAsString(),
        m_propertyBitmap->GetValueAsBitmap(),
        wxNullBitmap,
        wxItemKind::wxITEM_NORMAL,
        m_properyTooltip->GetValueAsString(),
        wxEmptyString,
        wxobject
    );

#if defined(_LP64) || defined(__LP64__) || defined(__arch64__) || defined(_WIN64)
    m_toolItem->SetUserData((long long)wxobject);
#else 
    m_toolItem->SetUserData((long)wxobject);
#endif 

    auiToolWnd->EnableTool(m_toolItem->GetId(), m_propertyEnabled->GetValueAsBoolean());

    if (m_propertyContextMenu->GetValueAsBoolean() == true && !m_toolItem->HasDropDown())
        auiToolWnd->SetToolDropDown(m_toolItem->GetId(), true);
    else if (m_propertyContextMenu->GetValueAsBoolean() == false && m_toolItem->HasDropDown())
        auiToolWnd->SetToolDropDown(m_toolItem->GetId(), false);

    auiToolWnd->Realize();
    auiToolWnd->Refresh();
    auiToolWnd->Update();
}

void CValueToolBarItem::OnUpdated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost)
{
    CAuiToolBar* auiToolWnd = dynamic_cast<CAuiToolBar*>(wxparent);
    wxASSERT(auiToolWnd);

    wxAuiToolBarItem* toolItem = auiToolWnd->FindTool(GetControlID());
    IValueFrame* parentControl = GetParent(); int idx = wxNOT_FOUND;

    for (unsigned int i = 0; i < parentControl->GetChildCount(); i++) {
        IValueFrame* child = parentControl->GetChild(i);
        if (m_controlId == child->GetControlID()) {
            idx = i;
            break;
        }
    }

    if (toolItem != nullptr) {
        auiToolWnd->DeleteTool(GetControlID());
    }

    toolItem = auiToolWnd->InsertTool(idx, GetControlID(),
        m_propertyCaption->GetValueAsString(),
        m_propertyBitmap->GetValueAsBitmap(),
        wxNullBitmap,
        wxItemKind::wxITEM_NORMAL,
        m_properyTooltip->GetValueAsString(),
        wxEmptyString,
        wxobject
    );

#if defined(_LP64) || defined(__LP64__) || defined(__arch64__) || defined(_WIN64)
    toolItem->SetUserData((long long)wxobject);
#else 
    toolItem->SetUserData((long)wxobject);
#endif 

    auiToolWnd->EnableTool(toolItem->GetId(), m_propertyEnabled->GetValueAsBoolean());

    if (m_propertyContextMenu->GetValueAsBoolean() == true && !toolItem->HasDropDown())
        auiToolWnd->SetToolDropDown(toolItem->GetId(), true);
    else if (m_propertyContextMenu->GetValueAsBoolean() == false && toolItem->HasDropDown())
        auiToolWnd->SetToolDropDown(toolItem->GetId(), false);

    auiToolWnd->Realize();
    auiToolWnd->Refresh();
    auiToolWnd->Update();
}

void CValueToolBarItem::Cleanup(wxObject* obj, IVisualHost* visualHost)
{
    CAuiToolBar* auiToolWnd = dynamic_cast<CAuiToolBar*>(visualHost->GetWxObject(GetParent()));

    auiToolWnd->DeleteTool(GetControlID());

    auiToolWnd->Realize();
    auiToolWnd->Refresh();
    auiToolWnd->Update();
}

bool CValueToolBarItem::CanDeleteControl() const
{
    unsigned int toolCount = 0;
    for (unsigned int idx = 0; idx < m_parent->GetChildCount(); idx++) {
        auto child = m_parent->GetChild(idx);
        if (wxT("tool") == child->GetClassName()) {
            toolCount++;
        }
    }

    return toolCount > 1;
}

//***********************************************************************************
//*                           CValueToolBarSeparator                                *
//***********************************************************************************

CValueToolBarSeparator::CValueToolBarSeparator() : IValueControl()
{
}

void CValueToolBarSeparator::OnCreated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost, bool first�reated)
{
    CAuiToolBar* auiToolWnd = dynamic_cast<CAuiToolBar*>(visualHost->GetWxObject(GetParent()));
    wxASSERT(auiToolWnd);
    wxAuiToolBarItem* m_toolItem = auiToolWnd->AddSeparator();
    m_toolItem->SetId(GetControlID());

    auiToolWnd->Realize();
    auiToolWnd->Refresh();
    auiToolWnd->Update();
}

void CValueToolBarSeparator::OnUpdated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost)
{
    CAuiToolBar* auiToolWnd = dynamic_cast<CAuiToolBar*>(visualHost->GetWxObject(GetParent()));
    wxASSERT(auiToolWnd);

    wxAuiToolBarItem* m_toolItem = auiToolWnd->FindTool(GetControlID());
    IValueFrame* m_parentControl = GetParent(); int idx = wxNOT_FOUND;

    for (unsigned int i = 0; i < m_parentControl->GetChildCount(); i++)
    {
        IValueFrame* child = m_parentControl->GetChild(i);
        if (m_controlId == child->GetControlID()) { idx = i; break; }
    }

    if (m_toolItem) { auiToolWnd->DeleteTool(GetControlID()); }
    auiToolWnd->InsertSeparator(idx, GetControlID());

    auiToolWnd->Realize();
    auiToolWnd->Refresh();
    auiToolWnd->Update();
}

void CValueToolBarSeparator::Cleanup(wxObject* obj, IVisualHost* visualHost)
{
    CAuiToolBar* auiToolWnd = dynamic_cast<CAuiToolBar*>(visualHost->GetWxObject(GetParent()));
    if (auiToolWnd) auiToolWnd->DeleteTool(GetControlID());
}

bool CValueToolBarSeparator::CanDeleteControl() const
{
    return m_parent->GetChildCount() > 1;
}

//**********************************************************************************
//*                                  Data	                                       *
//**********************************************************************************

bool CValueToolBarItem::LoadData(CMemoryReader& reader)
{
    m_propertyCaption->LoadData(reader);
    m_propertyBitmap->LoadData(reader);
    m_propertyContextMenu->LoadData(reader);
    m_properyTooltip->LoadData(reader);
    m_propertyEnabled->LoadData(reader);
    m_eventAction->LoadData(reader);

    return IValueControl::LoadData(reader);
}

bool CValueToolBarItem::SaveData(CMemoryWriter& writer)
{
    m_propertyCaption->SaveData(writer);
    m_propertyBitmap->SaveData(writer);
    m_propertyContextMenu->SaveData(writer);
    m_properyTooltip->SaveData(writer);
    m_propertyEnabled->SaveData(writer);
    m_eventAction->SaveData(writer);

    return IValueControl::SaveData(writer);
}

bool CValueToolBarSeparator::LoadData(CMemoryReader& reader)
{
    return IValueControl::LoadData(reader);
}

bool CValueToolBarSeparator::SaveData(CMemoryWriter& writer)
{
    return IValueControl::SaveData(writer);
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

S_CONTROL_TYPE_REGISTER(CValueToolBarItem, "tool", "tool", g_controlToolBarItemCLSID);
S_CONTROL_TYPE_REGISTER(CValueToolBarSeparator, "toolSeparator", "tool", g_controlToolBarSeparatorCLSID);