HDC hdc = GetDC(hWnd); // hWnd는 대상 윈도우의 핸들

int x = 100, y = 100, width = 200, height = 200; // 검색할 영역의 좌표와 크기

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
BitBlt(hdcMem, 0, 0, width, height, hdc, x, y, SRCCOPY);
GetDIBits(hdcMem, hbm, 0, height, pPixels, &bmi, DIB_RGB_COLORS);

// 리소스 해제
DeleteObject(hbm);
DeleteDC(hdcMem);
ReleaseDC(hWnd, hdc);

COLORREF targetColor = RGB(255, 0, 0); // 검색할 색 (빨간색)

for (int i = 0; i < width * height; i++) {
    BYTE b = pPixels[i * 4];
    BYTE g = pPixels[i * 4 + 1];
    BYTE r = pPixels[i * 4 + 2];
    COLORREF color = RGB(r, g, b);

    if (color == targetColor) {
        int foundX = i % width;
        int foundY = i / width;
        // 검색된 위치 (foundX, foundY)
        break;
    }
}

delete[] pPixels;

