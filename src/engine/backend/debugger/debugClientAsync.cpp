#include "debugClient.h"

void CDebuggerClient::CDebuggerAdapterClient::OnSessionStart(wxSocketClient* sock)
{
	if (m_debugBridge != nullptr) {
		m_debugBridge->OnSessionStart(sock);
	}
}

void CDebuggerClient::CDebuggerAdapterClient::OnSessionEnd(wxSocketClient* sock)
{
	if (m_debugBridge != nullptr) {
		m_debugBridge->OnSessionEnd(sock);
	}
}

void CDebuggerClient::CDebuggerAdapterClient::OnEnterLoop(wxSocketClient* sock, const debugLineData_t& data)
{
	if (m_debugBridge != nullptr) {
		m_debugBridge->OnEnterLoop(sock, data);
	}
}

void CDebuggerClient::CDebuggerAdapterClient::OnLeaveLoop(wxSocketClient* sock, const debugLineData_t& data)
{
	if (m_debugBridge != nullptr) {
		m_debugBridge->OnLeaveLoop(sock, data);
	}
}

void CDebuggerClient::CDebuggerAdapterClient::OnAutoComplete(const debugAutoCompleteData_t& data)
{
	if (m_debugBridge != nullptr) {
		m_debugBridge->OnAutoComplete(data);
	}
}

void CDebuggerClient::CDebuggerAdapterClient::OnMessageFromServer(const debugLineData_t& data, const wxString& message)
{
	if (m_debugBridge != nullptr) {
		m_debugBridge->OnMessageFromServer(data, message);
	}
}

void CDebuggerClient::CDebuggerAdapterClient::OnSetToolTip(const debugExpressionData_t& data, const wxString& strResult)
{
	if (m_debugBridge != nullptr) {
		m_debugBridge->OnSetToolTip(data, strResult);
	}
}

void CDebuggerClient::CDebuggerAdapterClient::OnSetStack(const stackData_t& data)
{
	if (m_debugBridge != nullptr) {
		m_debugBridge->OnSetStack(data);
	}
}

void CDebuggerClient::CDebuggerAdapterClient::OnSetLocalVariable(const localWindowData_t& data)
{
	if (m_debugBridge != nullptr) {
		m_debugBridge->OnSetLocalVariable(data);
	}
}

void CDebuggerClient::CDebuggerAdapterClient::OnSetVariable(const watchWindowData_t& data)
{
	if (m_debugBridge != nullptr) {
		m_debugBridge->OnSetVariable(data);
	}
}

void CDebuggerClient::CDebuggerAdapterClient::OnSetExpanded(const watchWindowData_t& data)
{
	if (m_debugBridge != nullptr) {
		m_debugBridge->OnSetExpanded(data);
	}
}
