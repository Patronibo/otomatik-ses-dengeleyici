#include "TrayIcon.h"
#include "resource.h"

TrayIcon::TrayIcon()
    : m_hwnd(nullptr)
    , m_hInstance(nullptr)
    , m_isActive(true)
{
    ZeroMemory(&m_nid, sizeof(NOTIFYICONDATA));
}

TrayIcon::~TrayIcon() {
    if (m_hwnd) {
        Shell_NotifyIcon(NIM_DELETE, &m_nid);
        DestroyWindow(m_hwnd);
    }
}

bool TrayIcon::Initialize(HINSTANCE hInstance, const std::wstring& tooltip) {
    m_hInstance = hInstance;

    // Gizli pencere sınıfı oluştur
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SesDengeleyiciTrayClass";
    
    if (!RegisterClassEx(&wc)) {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
    }

    // Gizli pencere oluştur
    m_hwnd = CreateWindowEx(
        0,
        L"SesDengeleyiciTrayClass",
        L"Ses Dengeleyici",
        0,
        0, 0, 0, 0,
        nullptr, nullptr, hInstance, this);

    if (!m_hwnd) {
        return false;
    }

    // this pointer'ı window data'ya kaydet
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    // Tray icon'u ayarla
    m_nid.cbSize = sizeof(NOTIFYICONDATA);
    m_nid.hWnd = m_hwnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_TRAYICON;
    
    // İkon yükle (varsayılan application icon kullan)
    m_nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    
    wcsncpy_s(m_nid.szTip, tooltip.c_str(), _TRUNCATE);

    if (!Shell_NotifyIcon(NIM_ADD, &m_nid)) {
        return false;
    }

    return true;
}

void TrayIcon::SetTooltip(const std::wstring& tooltip) {
    wcsncpy_s(m_nid.szTip, tooltip.c_str(), _TRUNCATE);
    Shell_NotifyIcon(NIM_MODIFY, &m_nid);
}

void TrayIcon::ShowNotification(const std::wstring& title, const std::wstring& message) {
    m_nid.uFlags |= NIF_INFO;
    wcsncpy_s(m_nid.szInfoTitle, title.c_str(), _TRUNCATE);
    wcsncpy_s(m_nid.szInfo, message.c_str(), _TRUNCATE);
    m_nid.dwInfoFlags = NIIF_INFO;
    
    Shell_NotifyIcon(NIM_MODIFY, &m_nid);
    
    m_nid.uFlags &= ~NIF_INFO;
}

void TrayIcon::ProcessMessages() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK TrayIcon::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    TrayIcon* pThis = nullptr;

    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<TrayIcon*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    } else {
        pThis = reinterpret_cast<TrayIcon*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (!pThis) {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    switch (uMsg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            pThis->CreateContextMenu(pt);
        } else if (lParam == WM_LBUTTONDBLCLK) {
            if (pThis->m_onToggle) {
                pThis->m_onToggle();
            }
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_EXIT:
            if (pThis->m_onExit) {
                pThis->m_onExit();
            }
            PostQuitMessage(0);
            break;

        case IDM_TOGGLE:
            if (pThis->m_onToggle) {
                pThis->m_onToggle();
            }
            break;

        case IDM_ABOUT:
            MessageBoxW(hwnd,
                L"Ses Dengeleyici v1.0\n\n"
                L"Bilgisayardaki sesleri otomatik olarak dengeler.\n"
                L"Yüksek sesleri düşürür, düşük sesleri yükseltir.\n\n"
                L"Çift tıklayarak açıp-kapatabilirsiniz.",
                L"Hakkında",
                MB_OK | MB_ICONINFORMATION);
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void TrayIcon::CreateContextMenu(POINT pt) {
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) {
        return;
    }

    AppendMenuW(hMenu, MF_STRING, IDM_TOGGLE, 
        m_isActive ? L"Devre Dışı Bırak" : L"Etkinleştir");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_ABOUT, L"Hakkında");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"Çıkış");

    // Menüyü göstermek için pencereyi öne getir
    SetForegroundWindow(m_hwnd);

    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN,
        pt.x, pt.y, 0, m_hwnd, nullptr);

    DestroyMenu(hMenu);
}
