#include "pch.h"
#include "framework.h"
#include "MFCSample.h"
#include "MFCSampleDlg.h"
#include "afxdialogex.h"

#include <vector>
#include <cmath>
#include <cstring>
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <afxwin.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void ClickFunc(int x, int y)
{
    // 그림판 프로그램의 윈도우 핸들 가져오기
    HWND hWndPaint = ::FindWindow(_T("MSPaintApp"), NULL);
    if (!hWndPaint)
        return;

    // 그림판 프로그램의 클라이언트 좌표를 스크린 좌표로 변환
    POINT pt = { x, y };
    ::ClientToScreen(hWndPaint, &pt);

    // 마우스 클릭 이벤트를 발생시키기 위해 좌표를 정규화
    static const int normalizedValue = 65535;

    HMONITOR monitor = MonitorFromWindow(hWndPaint, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi;
    std::memset(&mi, 0, sizeof(MONITORINFO));
    mi.cbSize = sizeof(MONITORINFO);

    if (!GetMonitorInfo(monitor, &mi))
        return;

    double ratioX = (pt.x - mi.rcMonitor.left) / static_cast<double>(mi.rcMonitor.right - mi.rcMonitor.left);
    double ratioY = (pt.y - mi.rcMonitor.top) / static_cast<double>(mi.rcMonitor.bottom - mi.rcMonitor.top);
    int normalizedX = static_cast<int>(std::round(ratioX * normalizedValue));
    int normalizedY = static_cast<int>(std::round(ratioY * normalizedValue));

    // 마우스 클릭 이벤트 생성
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = normalizedX;
    input.mi.dy = normalizedY;
    input.mi.mouseData = 0;
    input.mi.dwExtraInfo = NULL;
    input.mi.time = 0;
    input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));

    Sleep(100); // 클릭 유지 시간

    input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));

}

void KeyFunc(WORD key)
{
    HWND hWndPaint = ::FindWindow(_T("Gersang"), NULL);

    if (SetForegroundWindow(hWndPaint)) {
        AfxMessageBox(_T("창 활성화 성공"));
    }
    else {
        AfxMessageBox(_T("창 활성화 실패"));
    }

    INPUT input[2];

    // 키 누르기 이벤트
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wScan = MapVirtualKey(key, MAPVK_VK_TO_VSC);
    input[0].ki.time = 0;
    input[0].ki.dwExtraInfo = 0;
    input[0].ki.wVk = key;
    input[0].ki.dwFlags = 0; // Key down

    // 키 떼기 이벤트
    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wScan = MapVirtualKey(key, MAPVK_VK_TO_VSC);
    input[1].ki.time = 0;
    input[1].ki.dwExtraInfo = 0;
    input[1].ki.wVk = key;
    input[1].ki.dwFlags = KEYEVENTF_KEYUP; // Key up

    // 입력 이벤트 전송
    UINT uSent = SendInput(2, input, sizeof(INPUT));
    if (uSent != 2) {
        AfxMessageBox(_T("키보드 입력 실패"));
        return;
    }

    // 입력 후 잠시 대기
    Sleep(100);

    AfxMessageBox(_T("키보드 입력 성공"));
}

void CMFCSampleDlg::OnBnClickPixcelSearch()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    // 픽셀 서치 시작
    AfxMessageBox(_T("픽셀 서치 시작"));

    // 그림판 프로그램의 윈도우 핸들 가져오기
    HWND hWndPaint = ::FindWindow(_T("Gersang"), NULL);
    if (hWndPaint == NULL) {
        AfxMessageBox(_T("프로그램을 찾을 수 없습니다."));
        return;
    }
    else {
        AfxMessageBox(_T("프로그램 FIND 성공."));
    }

    // 그림 그리는 영역의 DC 가져오기
    HDC hdc = ::GetDC(hWndPaint);
    if (hdc == NULL) {
        AfxMessageBox(_T("디바이스 컨텍스트를 가져오지 못했습니다."));
        return;
    }
    else {
        AfxMessageBox(_T("DC 정보 호출 성공."));
        KeyFunc(VK_SPACE);
    }

    // 그림판 클라이언트 영역의 크기 가져오기
    RECT rect;
    ::GetClientRect(hWndPaint, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // 비트맵 정보를 설정
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // top-down 비트맵
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 32비트 비트맵
    bmi.bmiHeader.biCompression = BI_RGB;

    BYTE* pPixels = new BYTE[width * height * 4]; // 픽셀 데이터를 저장할 메모리

    // 영역의 비트맵 데이터를 가져옴
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbm = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(hdcMem, hbm);
    BitBlt(hdcMem, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
    GetDIBits(hdcMem, hbm, 0, height, pPixels, &bmi, DIB_RGB_COLORS);

    // 리소스 해제
    DeleteObject(hbm);
    DeleteDC(hdcMem);
    ::ReleaseDC(hWndPaint, hdc);

    // 색상 정의
    COLORREF blueColor = RGB(0, 0, 255); // 파랑색
    COLORREF redColor = RGB(255, 0, 0); // 빨간색

    POINT bluePoint = { -1, -1 }; // 기준점 위치
    std::vector<POINT> redPoints; // 빨간색 위치 저장

    // 픽셀 색상 및 위치를 저장
    for (int i = 0; i < width * height; i++) {
        BYTE b = pPixels[i * 4];
        BYTE g = pPixels[i * 4 + 1];
        BYTE r = pPixels[i * 4 + 2];
        COLORREF color = RGB(r, g, b);

        int x = i % width;
        int y = i / width;

        if (color == blueColor) {
            bluePoint = POINT{ x, y };
        }
        else if (color == redColor) {
            redPoints.push_back(POINT{ x, y });
        }
    }

    if (bluePoint.x == -1 || bluePoint.y == -1) {
        AfxMessageBox(_T("기준점 색(파랑색)을 찾을 수 없습니다."));
        delete[] pPixels;
        return;
    }

    // 가장 가까운 빨간색 위치 찾기
    POINT closestRedPoint = { -1, -1 };
    double minDistance = DBL_MAX; // 초기값은 무한대

    for (const POINT& redPoint : redPoints) {
        double distance = sqrt(pow((redPoint.x - bluePoint.x), 2) + pow((redPoint.y - bluePoint.y), 2));
        if (distance < minDistance) {
            minDistance = distance;
            closestRedPoint = redPoint;
        }
    }

    if (closestRedPoint.x != -1 && closestRedPoint.y != -1) {
        CString msg;
        msg.Format(_T("가장 가까운 빨간색 위치: (%d, %d)"), closestRedPoint.x, closestRedPoint.y);
        AfxMessageBox(msg);
        ClickFunc(closestRedPoint.x, closestRedPoint.y);
        KeyFunc(VK_SPACE); // 필요 시 여기에 추가 작업
    }
    else {
        AfxMessageBox(_T("빨간색을 찾을 수 없습니다."));
    }

    delete[] pPixels;

    // 픽셀 서치 성공, 검색된 서치가 다수 인 경우 그중 하나 찍음 (이대로 그냥 테스트를 해봐도 될듯)
    // 그림판이 화면이 띄워져 있는게 아니면 픽셀 서치 안됨 OnBnClickPixcelSearch() 픽셀 서치중일땐 화면 띄워놔야 할듯 
    // 위에 조건 해결 못하면 한 피씨에서 다클라 많이는 못 땡길듯
    // 디스 플레이 1, 2 있는 경우 이것도 따로 설정해야함 디스 플레이 2로 창을 옮기고 실행 시 클릭 을 디스플레이 1에다가 함
}
