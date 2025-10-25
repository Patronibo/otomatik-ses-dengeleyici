#pragma once

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <functional>

class TrayIcon {
public:
    TrayIcon();
    ~TrayIcon();

    bool Initialize(HINSTANCE hInstance, const std::wstring& tooltip);
    void SetTooltip(const std::wstring& tooltip);
    void ShowNotification(const std::wstring& title, const std::wstring& message);
    
    // Callback'ler
    using OnExitCallback = std::function<void()>;
    using OnToggleCallback = std::function<void()>;
    
    void SetOnExit(OnExitCallback callback) { m_onExit = callback; }
    void SetOnToggle(OnToggleCallback callback) { m_onToggle = callback; }

    HWND GetHwnd() const { return m_hwnd; }
    void ProcessMessages();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void CreateContextMenu(POINT pt);
    
    HWND m_hwnd;
    NOTIFYICONDATA m_nid;
    HINSTANCE m_hInstance;
    
    OnExitCallback m_onExit;
    OnToggleCallback m_onToggle;
    
    bool m_isActive;
    
    static constexpr UINT WM_TRAYICON = WM_USER + 1;
    static constexpr UINT IDM_EXIT = 1001;
    static constexpr UINT IDM_TOGGLE = 1002;
    static constexpr UINT IDM_ABOUT = 1003;
};

