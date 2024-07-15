#ifndef HEADER__FILE__INPUTMANAGER
#define HEADER__FILE__INPUTMANAGER

#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>
#include <tuple>

#include <afxwin.h>

/* 
코드에서 사용법은 밑과 같음
InputManager::get().Click(_T("Gersang"), 100, 700);
InputManager::get().InputKey(_T("Gersang"), 'I');
*/
class InputManager
{
    InputManager() : m_normalizedValue{ 65535 } { }
    ~InputManager() {}

    std::map<CString, HWND> m_hWnds;
    const int m_normalizedValue;

public:
    static InputManager& get()
    {
        static InputManager singleton;
        return singleton;
    }

    InputManager(InputManager const&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(InputManager const&) = delete;
    InputManager& operator=(InputManager&&) = delete;

    // wndName: 윈도우 이름
    // x, y: 마우스 좌표 (거상 윈도우 기준)
    bool Click(CString wndName, int x, int y)
    {
        HWND hWnd = nullptr;
        if (m_hWnds.find(wndName) == m_hWnds.end())
        {
            hWnd = FindWindow(NULL, wndName);
            if (!hWnd)
                return false;

            m_hWnds[wndName] = hWnd;
        }
        else
            hWnd = m_hWnds[wndName];

        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi;
        RECT rect;
        ZeroMemory(&mi, sizeof(MONITORINFO));
        mi.cbSize = sizeof(MONITORINFO);

        if (!GetMonitorInfo(monitor, &mi) || !::GetWindowRect(hWnd, &rect))
            return false;

        double ratioX = (static_cast<double>(rect.left) + static_cast<double>(x)) / (static_cast<double>(mi.rcMonitor.right) - static_cast<double>(mi.rcMonitor.left));
        double ratioY = (static_cast<double>(rect.top) + static_cast<double>(y)) / (static_cast<double>(mi.rcMonitor.bottom) - static_cast<double>(mi.rcMonitor.top));
        int normalizedX = static_cast<int>(std::round(ratioX * m_normalizedValue));
        int normalizedY = static_cast<int>(std::round(ratioY * m_normalizedValue));

        INPUT input;
        input.type = INPUT_MOUSE;
        input.mi.dx = normalizedX;
        input.mi.dy = normalizedY;
        input.mi.mouseData = 0;
        input.mi.dwExtraInfo = NULL;
        input.mi.time = 0;
        input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN);
        SendInput(1, &input, sizeof(INPUT));

        Sleep(100);

        input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTUP);
        SendInput(1, &input, sizeof(INPUT));

        Sleep(100);

        return true;
    }

    // wndName: 윈도우 이름
    // key: 키보드 입력 (ex. S키는 'S', F11키는 VK_F11, 즉 알파벳을 제외한 특수키들은 `VK_`로 시작하는 매크로임)
    bool InputKey(CString wndName, WORD key)
    {
        HWND hWnd = nullptr;
        if (m_hWnds.find(wndName) == m_hWnds.end())
        {
            hWnd = FindWindow(NULL, wndName);
            if (!hWnd)
                return false;

            m_hWnds[wndName] = hWnd;
        }
        else
            hWnd = m_hWnds[wndName];

        INPUT input;
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = MapVirtualKey(key, MAPVK_VK_TO_VSC);
        input.ki.time = 0;
        input.ki.dwExtraInfo = 0;
        input.ki.wVk = key;
        input.ki.dwFlags = 0;
        SendInput(1, &input, sizeof(INPUT));

        Sleep(100);

        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));

        Sleep(100);

        return true;
    }
};

#endif /* HEADER__FILE__INPUTMANAGER */