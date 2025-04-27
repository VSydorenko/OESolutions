#include "tableBox.h"
#include "form.h"

#include "frontend/visualView/visualHost.h"
#include "backend/system/value/valueTable.h"
#include "backend/metaCollection/partial/commonObject.h"
#include "backend/appData.h"

//***********************************************************************************
//*                           IMPLEMENT_DYNAMIC_CLASS                               *
//***********************************************************************************

wxIMPLEMENT_DYNAMIC_CLASS(CValueTableBox, IValueWindow);

//***********************************************************************************
//*                                 Special tablebox func                           *
//***********************************************************************************

bool CValueTableBox::GetControlValue(CValue& pvarControlVal) const
{
    if (m_tableModel == nullptr) {
        if (appData->DesignerMode()) {
            if (!m_propertySource->IsEmptyProperty()) {
                ISourceDataObject* srcObject = m_formOwner->GetSourceObject();
                if (srcObject != nullptr) {
                    IValueModel* tableModel = nullptr;
                    if (srcObject->GetModel(tableModel, m_propertySource->GetValueAsSource())) {
                        if (tableModel != m_tableModel) {
                            pvarControlVal = tableModel;
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
    pvarControlVal = m_tableModel;
    return true;
}

bool CValueTableBox::SetControlValue(const CValue& varControlVal)
{
    IValueModel* tableModel =
        varControlVal.ConvertToType<IValueModel>();

    if (m_tableModel) {
        m_tableModel->DecrRef();
        m_tableModel = nullptr;
    }

    if (tableModel != nullptr) {
        m_tableModel = tableModel;
        m_tableModel->IncrRef();
    }

    return true;
}

void CValueTableBox::AddColumn()
{
    wxASSERT(m_formOwner);

    CValueTableBoxColumn* columnTable = wxDynamicCast(m_formOwner->NewObject(g_controlTableBoxColumnCLSID, this), CValueTableBoxColumn);
    g_visualHostContext->InsertControl(columnTable, this);
    if (m_tableModel != nullptr) {
        IValueModel::IValueModelColumnCollection* columnData = m_tableModel->GetColumnCollection();
        wxASSERT(columnData);
        IValueModel::IValueModelColumnCollection::IValueModelColumnInfo* column_info = columnData->AddColumn(
            columnTable->GetControlName(),
            columnTable->GetTypeDesc(),
            columnTable->GetCaption(),
            columnTable->GetWidthColumn()
        ); 
        if (column_info != nullptr) column_info->SetColumnID(columnTable->GetControlID());
    }
    g_visualHostContext->RefreshEditor();
}

void CValueTableBox::CreateColumnCollection(wxDataViewCtrl* tableCtrl)
{
    if (appData->DesignerMode())
        return;
    wxDataViewCtrl* tc = tableCtrl ?
        tableCtrl : dynamic_cast<wxDataViewCtrl*>(GetWxObject());
    wxASSERT(tc);
    CVisualDocument* visualDocument = m_formOwner->GetVisualDocument();
    //clear all controls 
    for (unsigned int idx = 0; idx < GetChildCount(); idx++) {
        IValueFrame* childColumn = GetChild(idx);
        wxASSERT(childColumn);
        if (visualDocument != nullptr) {
            CVisualHost* visualView = visualDocument->GetVisualView();
            wxASSERT(visualView);
            visualView->RemoveControl(childColumn, this);
        }
        childColumn->SetParent(nullptr);
        childColumn->DecrRef();
    }
    //clear all children
    RemoveAllChildren();
    //clear all old columns
    tc->ClearColumns();
    //create new columns
    IValueModel::IValueModelColumnCollection* tableColumns = m_tableModel->GetColumnCollection();
    wxASSERT(tableColumns);
    for (unsigned int idx = 0; idx < tableColumns->GetColumnCount(); idx++) {

        IValueModel::IValueModelColumnCollection::IValueModelColumnInfo* columnInfo = tableColumns->GetColumnInfo(idx);
        CValueTableBoxColumn* newTableBoxColumn =
            m_formOwner->NewObject<CValueTableBoxColumn>(g_controlTableBoxColumnCLSID, this);

        const CTypeDescription& typeDescription = columnInfo->GetColumnType();
        if (typeDescription.IsOk())
            newTableBoxColumn->SetDefaultMetaType(typeDescription);
        else
            newTableBoxColumn->SetDefaultMetaType(eValueTypes::TYPE_STRING);

        newTableBoxColumn->SetCaption(columnInfo->GetColumnCaption());
        newTableBoxColumn->SetWidthColumn(columnInfo->GetColumnWidth());

        //newTableBoxColumn->m_dataSource = GetGuidByID(columnInfo->GetColumnID());
        newTableBoxColumn->SetSource(columnInfo->GetColumnID());

        if (visualDocument != nullptr) {
            CVisualHost* visualView = visualDocument->GetVisualView();
            wxASSERT(visualView);
            visualView->CreateControl(newTableBoxColumn, this);
        }
    }

    if (visualDocument != nullptr) {
        CVisualHost* visualView = visualDocument->GetVisualView();
        wxASSERT(visualView);
        //fix size in parent window 
        wxWindow* wndParent = visualView->GetParent();
        if (wndParent != nullptr) {
            wndParent->Layout();
        }
    }
}

void CValueTableBox::CreateTable(bool recreateModel)
{
    if (m_tableModel != nullptr) {
        if (recreateModel) {
            m_tableModel->DecrRef();
            m_tableModel = nullptr;
        }
    }

    if (m_tableModel == nullptr) {
        IValueModel* modelValue = ITypeControlFactory::CreateAndConvertValueRef<IValueModel>();
        //wxASSERT(modelValue);
        if (modelValue != nullptr) {
            for (unsigned int idx = 0; idx < GetChildCount(); idx++) {
                CValueTableBoxColumn* columnTable = wxDynamicCast(GetChild(idx), CValueTableBoxColumn);
                if (columnTable != nullptr) {
                    IValueModel::IValueModelColumnCollection* columnData = modelValue->GetColumnCollection();
                    wxASSERT(columnData);
                    IValueModel::IValueModelColumnCollection::IValueModelColumnInfo* column_info = columnData->AddColumn(
                        columnTable->GetControlName(),
                        columnTable->GetTypeDesc(),
                        columnTable->GetCaption(),
                        columnTable->GetWidthColumn()
                    );
                    if (column_info != nullptr) column_info->SetColumnID(columnTable->GetControlID());               
                }
            }
            m_tableModel = modelValue;
            m_tableModel->IncrRef();
        }
    }
}

void CValueTableBox::CreateModel(bool recreateModel)
{
    if (!m_propertySource->IsEmptyProperty()) {
        ISourceDataObject* srcObject = m_formOwner->GetSourceObject();
        if (srcObject != nullptr) {
            IValueModel* tableModel = nullptr;
            if (srcObject->GetModel(tableModel, m_propertySource->GetValueAsSource())) {
                if (tableModel != m_tableModel) {
                    if (m_tableModel != nullptr) {
                        m_tableModel->DecrRef();
                        m_tableModel = nullptr;
                    }
                    m_tableModel = tableModel;
                    m_tableModel->IncrRef();
                }
            }
        }
        CreateTable(false);
    }
    else if (m_tableModel != nullptr && m_propertySource->GetValueAsTypeDesc() != m_tableModel->GetSourceClassType()) {
        CreateTable(true);
    }
    else {
        CreateTable(recreateModel);
    }
}

void CValueTableBox::RefreshModel(bool recreateModel)
{
    CValueTableBox::CreateModel(recreateModel);
}

////////////////////////////////////////////////////////////////////////////////////

ISourceObject* CValueTableBox::GetSourceObject() const
{
    return m_formOwner ? m_formOwner->GetSourceObject() : nullptr;
}

IMetaObjectSourceData* CValueTableBox::GetSourceMetaObject() const
{
    wxASSERT(m_tableModel);
    if (m_tableModel == nullptr) return nullptr;
    return m_tableModel->GetSourceMetaObject();
}

class_identifier_t CValueTableBox::GetSourceClassType() const
{
    wxASSERT(m_tableModel);
    if (m_tableModel == nullptr) return 0;
    return m_tableModel->GetSourceClassType();
}

bool CValueTableBox::FilterSource(const CSourceExplorer& src, const meta_identifier_t& id)
{
    return src.IsTableSection();
}

//***********************************************************************************
//*                              CValueTableBox                                     *
//***********************************************************************************

CValueTableBox::CValueTableBox() : IValueWindow(), ITypeControlFactory(),
m_tableModel(nullptr), m_tableCurrentLine(nullptr), m_dataViewUpdated(false), m_dataViewSizeChanged(false)
{
    m_propertySource->SetValue(CTypeDescription(g_valueTableCLSID));

    //set default params
    m_propertyMinSize->SetValue(wxSize(300, 100));
    m_propertyBG->SetValue(wxColour(255, 255, 255));
}

CValueTableBox::~CValueTableBox()
{
    if (m_tableModel != nullptr)  m_tableModel->DecrRef();
    if (m_tableCurrentLine != nullptr) m_tableCurrentLine->DecrRef();
}

/////////////////////////////////////////////////////////////////////////////////////

wxObject* CValueTableBox::Create(wxWindow* wxparent, IVisualHost* visualHost)
{
    wxDataModelViewCtrl* tableCtrl = new wxDataModelViewCtrl(wxparent, wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxDV_SINGLE | wxDV_HORIZ_RULES | wxDV_VERT_RULES | wxDV_ROW_LINES | wxDV_VARIABLE_LINE_HEIGHT | wxBORDER_SIMPLE);

    if (!visualHost->IsDemonstration()) {
        tableCtrl->Bind(wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, &CValueTableBox::OnColumnClick, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_COLUMN_REORDERED, &CValueTableBox::OnColumnReordered, this);

        //system events:
        tableCtrl->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &CValueTableBox::OnSelectionChanged, this);

        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &CValueTableBox::OnItemActivated, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_COLLAPSED, &CValueTableBox::OnItemCollapsed, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_EXPANDED, &CValueTableBox::OnItemExpanded, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_COLLAPSING, &CValueTableBox::OnItemCollapsing, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_EXPANDING, &CValueTableBox::OnItemExpanding, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, &CValueTableBox::OnItemStartEditing, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_EDITING_STARTED, &CValueTableBox::OnItemEditingStarted, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &CValueTableBox::OnItemEditingDone, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, &CValueTableBox::OnItemValueChanged, this);

        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_START_INSERTING, &CValueTableBox::OnItemStartInserting, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_START_DELETING, &CValueTableBox::OnItemStartDeleting, this);

#if wxUSE_DRAG_AND_DROP 
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, &CValueTableBox::OnItemBeginDrag, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_DROP_POSSIBLE, &CValueTableBox::OnItemDropPossible, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_DROP, &CValueTableBox::OnItemDrop, this);
#endif // wxUSE_DRAG_AND_DROP

        tableCtrl->GenericGetHeader()->Bind(wxEVT_HEADER_RESIZING, &CValueTableBox::OnHeaderResizing, this);

        tableCtrl->Bind(wxEVT_SCROLLWIN_TOP, &CValueTableBox::HandleOnScroll, this);
        tableCtrl->Bind(wxEVT_SCROLLWIN_BOTTOM, &CValueTableBox::HandleOnScroll, this);
        tableCtrl->Bind(wxEVT_SCROLLWIN_LINEUP, &CValueTableBox::HandleOnScroll, this);
        tableCtrl->Bind(wxEVT_SCROLLWIN_LINEDOWN, &CValueTableBox::HandleOnScroll, this);
        tableCtrl->Bind(wxEVT_SCROLLWIN_PAGEUP, &CValueTableBox::HandleOnScroll, this);
        tableCtrl->Bind(wxEVT_SCROLLWIN_PAGEDOWN, &CValueTableBox::HandleOnScroll, this);

        tableCtrl->Bind(wxEVT_SCROLLWIN_THUMBTRACK, &CValueTableBox::HandleOnScroll, this);
        tableCtrl->Bind(wxEVT_SCROLLWIN_THUMBRELEASE, &CValueTableBox::HandleOnScroll, this);

        tableCtrl->GetMainWindow()->Bind(wxEVT_LEFT_DOWN, &CValueTableBox::OnMainWindowClick, this);

#if wxUSE_DRAG_AND_DROP && wxUSE_UNICODE
        tableCtrl->EnableDragSource(wxDF_UNICODETEXT);
        tableCtrl->EnableDropTarget(wxDF_UNICODETEXT);
#endif // wxUSE_DRAG_AND_DROP && wxUSE_UNICODE

        tableCtrl->Bind(wxEVT_SIZE, &CValueTableBox::OnSize, this);
        tableCtrl->Bind(wxEVT_IDLE, &CValueTableBox::OnIdle, this);

        tableCtrl->Bind(wxEVT_MENU, &CValueTableBox::OnCommandMenu, this);
        tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &CValueTableBox::OnContextMenu, this);
    }

    return tableCtrl;
}

void CValueTableBox::OnCreated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost, bool first�reated)
{
    wxDataModelViewCtrl* tableCtrl = dynamic_cast<wxDataModelViewCtrl*>(wxobject);

    if (tableCtrl != nullptr) CValueTableBox::CreateModel();

    if (visualHost->IsDesignerHost() && GetChildCount() == 0
        && first�reated) {
        CValueTableBox::AddColumn();
    }
}

#include <wx/itemattr.h>

void CValueTableBox::Update(wxObject* wxobject, IVisualHost* visualHost)
{
    wxDataModelViewCtrl* tableCtrl = dynamic_cast<wxDataModelViewCtrl*>(wxobject);

    UpdateWindow(tableCtrl);

    if (tableCtrl != nullptr) {
        wxItemAttr attr(
            tableCtrl->GetForegroundColour(),
            tableCtrl->GetBackgroundColour(),
            tableCtrl->GetFont()
        );
        tableCtrl->SetHeaderAttr(attr);
    }
}

void CValueTableBox::OnUpdated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost)
{
    wxDataModelViewCtrl* tableCtrl = dynamic_cast<wxDataModelViewCtrl*>(wxobject);

    if (tableCtrl != nullptr) {

        bool needRefresh = false;
        if (m_tableModel != nullptr && !appData->DesignerMode()) {
            needRefresh = true;
        }

        CValueTableBox::RefreshModel();

        if (m_tableModel != tableCtrl->GetModel()) {
            tableCtrl->AssociateModel(m_tableModel);
        }

        if (needRefresh) {

            try {
                m_tableModel->CallRefreshModel(tableCtrl->GetTopItem(), tableCtrl->GetCountPerPage());
            }
            catch (const CBackendException* err) {
                tableCtrl->AssociateModel(nullptr);
                throw(err);
            }

            if (m_tableModel != nullptr) {
                IValueFrame* ownerControl = m_formOwner->GetOwnerControl();
                if (ownerControl != nullptr && m_tableCurrentLine == nullptr) {
                    CValue retValue; ownerControl->GetControlValue(retValue);
                    const wxDataViewItem& currLine = m_tableModel->FindRowValue(retValue);
                    if (currLine.IsOk()) {
                        if (m_tableCurrentLine != nullptr)
                            m_tableCurrentLine->DecrRef();
                        m_tableCurrentLine = m_tableModel->GetRowAt(currLine);
                        m_tableCurrentLine->IncrRef();
                    }
                }
            }

            if (m_tableCurrentLine != nullptr && !m_tableModel->ValidateReturnLine(m_tableCurrentLine)) {
                const wxDataViewItem& currLine = m_tableModel->FindRowValue(m_tableCurrentLine);
                m_tableCurrentLine->DecrRef();
                m_tableCurrentLine = nullptr;
                if (currLine.IsOk()) {
                    m_tableCurrentLine = m_tableModel->GetRowAt(currLine);
                    m_tableCurrentLine->IncrRef();
                }
            }
        }

        if (m_tableCurrentLine != nullptr) {
            tableCtrl->Select(
                m_tableCurrentLine->GetLineItem()
            );
        }

        m_dataViewUpdated = true;
        m_dataViewSize = tableCtrl->GetSize();
    }
}

void CValueTableBox::Cleanup(wxObject* obj, IVisualHost* visualHost)
{
    wxDataModelViewCtrl* tableCtrl = dynamic_cast<wxDataModelViewCtrl*>(obj);
    if (tableCtrl != nullptr) tableCtrl->AssociateModel(nullptr);
}

//***********************************************************************************
//*                                  Property                                       *
//***********************************************************************************

bool CValueTableBox::LoadData(CMemoryReader& reader)
{
    if (!m_propertySource->LoadData(reader))
        return false;

    return IValueWindow::LoadData(reader);
}

bool CValueTableBox::SaveData(CMemoryWriter& writer)
{
    if (!m_propertySource->SaveData(writer))
        return false;

    return IValueWindow::SaveData(writer);
}

//***********************************************************************************

enum prop {
    eTableValue,
    eCurrentRow,
};

IMetaData* CValueTableBox::GetMetaData() const
{
    return m_formOwner ?
        m_formOwner->GetMetaData() : nullptr;
}

void CValueTableBox::PrepareNames() const
{
    IValueFrame::PrepareNames();

    m_methodHelper->AppendProp("tableValue", eTableValue, eControl);
    m_methodHelper->AppendProp("currentRow", eCurrentRow, eControl);
}

bool CValueTableBox::SetPropVal(const long lPropNum, const CValue& varPropVal)
{
    const long lPropAlias = m_methodHelper->GetPropAlias(lPropNum); bool refreshColumn = false;
    if (lPropAlias == eControl) {
        const long lPropData = m_methodHelper->GetPropData(lPropNum);
        if (lPropData == eTableValue) {
            refreshColumn = true;
            if (m_tableCurrentLine != nullptr)
                m_tableCurrentLine->DecrRef();
            m_tableCurrentLine = nullptr;
            if (m_tableModel != nullptr)
                m_tableModel->DecrRef();
            m_tableModel = varPropVal.ConvertToType<IValueTable>();
            m_tableModel->IncrRef();
        }
        else if (lPropData == eCurrentRow) {
            if (m_tableCurrentLine != nullptr)
                m_tableCurrentLine->DecrRef();
            m_tableCurrentLine = nullptr;
            IValueTable::IValueModelReturnLine* tableReturnLine = nullptr;
            if (varPropVal.ConvertToValue(tableReturnLine)) {
                if (m_tableModel == tableReturnLine->GetOwnerModel()) {
                    m_tableCurrentLine = tableReturnLine;
                    m_tableCurrentLine->IncrRef();
                }
            }
        }
    }

    bool result = IValueFrame::SetPropVal(lPropNum, varPropVal);

    if (refreshColumn && m_tableModel != nullptr && m_tableModel->AutoCreateColumn()) {
        CValueTableBox::CreateColumnCollection();
    }

    return result;
}

bool CValueTableBox::GetPropVal(const long lPropNum, CValue& pvarPropVal)
{
    const long lPropAlias = m_methodHelper->GetPropAlias(lPropNum);
    if (lPropAlias == eControl) {
        const long lPropData = m_methodHelper->GetPropData(lPropNum);
        if (lPropData == eTableValue) {
            pvarPropVal = m_tableModel;
            return true;
        }
        else if (lPropData == eCurrentRow) {
            pvarPropVal = m_tableCurrentLine;
            return true;
        }
    }
    return IValueFrame::GetPropVal(lPropNum, pvarPropVal);
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

CONTROL_TYPE_REGISTER(CValueTableBox, "tablebox", "container", g_controlTableBoxCLSID);