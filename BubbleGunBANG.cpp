#include <windows.h>
#include <cmath>
#include <stdio.h>
#include <tchar.h>
#include <ctime>
//#include <vector> doesn't work!

HDC hdc = 0, hdcMem = 0;
HBITMAP buffer = 0;
RECT screen_rect, paint_rect = {0, 0, 800, 600}, rect = {0, 0, paint_rect.right+16, paint_rect.bottom+38};
HBRUSH whiteBrush = CreateSolidBrush(RGB(255,255,255)), brush;
int g_ArrowX, g_ArrowY, g_CentX = 100, g_CentY = rect.bottom-100, g_Rad = 5, g_TargX, g_TargY;
TCHAR line[10];
const int constSz = 100;
double g_PackAlpha[constSz], g_Alpha = 0, g_PackPow[constSz], g_Pow = 4;
int g_PackStartX[constSz], g_PackStartY[constSz], g_PackTime[constSz], g_X[constSz], g_Y[constSz], g_PackSize = 0;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
void ReDrawBuff(HWND hWnd);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
  LPCTSTR lpzClass = TEXT("My Window Class!");
  if (!RegMyWindowClass(hInstance, lpzClass))
    return 1;
  GetWindowRect(GetDesktopWindow(),&screen_rect);
  HWND hWnd = CreateWindow(lpzClass, TEXT("Bubble Gun BANG! v0.1"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    (screen_rect.right - rect.right) / 2, (screen_rect.bottom - rect.bottom) / 2,
    rect.right, rect.bottom, NULL, NULL, hInstance, NULL);
  if(!hWnd) return 2;
  MSG msg = {0};
  int iGetOk = 0;
  while ((iGetOk = GetMessage(&msg, NULL, 0, 0 )) != 0){
    if (iGetOk == -1) return 3;
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}

ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName){
  WNDCLASS wcWindowClass = {0};
  wcWindowClass.lpfnWndProc = (WNDPROC)WndProc;
  wcWindowClass.style = CS_HREDRAW|CS_VREDRAW;
  wcWindowClass.hInstance = hInst;
  wcWindowClass.lpszClassName = lpzClassName;
  wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
  return RegisterClass(&wcWindowClass);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
  PAINTSTRUCT paintStruct;
  static bool is_w_psd, is_s_psd, is_a_psd, is_d_psd, is_spc_psd;
  switch (message){
  case WM_CREATE:
    srand(time(0));
    g_TargX = 100+rand()%800;
    g_TargY = 100+rand()%600;
    hdc = BeginPaint(hWnd, &paintStruct);
    hdcMem = CreateCompatibleDC(hdc);
    buffer = CreateCompatibleBitmap(hdc, paint_rect.right, paint_rect.bottom);
    SelectObject(hdcMem, buffer);
    EndPaint(hWnd, &paintStruct);
    ReDrawBuff(hWnd);
    SetTimer(hWnd, 0, 1, NULL);
    break;
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &paintStruct);
    BitBlt(hdc, paintStruct.rcPaint.left, paintStruct.rcPaint.top, paintStruct.rcPaint.right - paintStruct.rcPaint.left, paintStruct.rcPaint.bottom - paintStruct.rcPaint.top,
           hdcMem, paintStruct.rcPaint.left, paintStruct.rcPaint.top, SRCCOPY);
    EndPaint(hWnd, &paintStruct);
    break;
  case WM_TIMER:
    static int tSpeed=5;
    switch(wParam)
    case 0:
      if(is_w_psd && g_CentY > 0) g_CentY-=tSpeed;
      if(is_s_psd && g_CentY < paint_rect.bottom) g_CentY+=tSpeed;
      if(is_a_psd && g_CentX > 0) g_CentX-=tSpeed;
      if(is_d_psd && g_CentX < paint_rect.right) g_CentX+=tSpeed;
      if(is_spc_psd)
        if(g_Pow > 8);
        else g_Pow+=0.1;//strange bug +d
      else if(g_Pow > 4) g_Pow-=0.1;
      ReDrawBuff(hWnd);
      break;
    break;
  case WM_LBUTTONDOWN:
    if(g_PackSize<constSz){
      g_PackStartX[g_PackSize] = g_CentX;
      g_PackStartY[g_PackSize] = g_CentY;
      g_PackTime[g_PackSize] = 0;
      g_PackAlpha[g_PackSize] = g_Alpha;
      g_PackPow[g_PackSize] = g_Pow;
      g_PackSize++;
      brush = CreateSolidBrush(RGB(rand()%255,rand()%255,rand()%255));
      SelectObject(hdcMem, brush);
    }
    break;
  case WM_MOUSEMOVE:
    g_ArrowX = LOWORD(lParam);
    g_ArrowY = HIWORD(lParam);
    if(g_ArrowX != g_CentX)
      g_Alpha = atan((double)(g_ArrowY-g_CentY)/(g_ArrowX-g_CentX));
    else if(g_ArrowY < g_CentY)g_Alpha = -M_PI/2;
    else g_Alpha = M_PI/2;
    if(g_ArrowX < g_CentX)g_Alpha+=M_PI;
  break;
  case WM_KEYDOWN:
    switch(wParam){
    case 0x57: is_w_psd = true; break;
    case 0x53: is_s_psd = true; break;
    case 0x41: is_a_psd = true; break;
    case 0x44: is_d_psd = true; break;
    case 0x20: is_spc_psd = true; break;
    }
  break;
  case WM_KEYUP:
    switch(wParam){
    case 0x57: is_w_psd = false; break;
    case 0x53: is_s_psd = false; break;
    case 0x41: is_a_psd = false; break;
    case 0x44: is_d_psd = false; break;
    case 0x20: is_spc_psd = false; break;
    }
  break;
  case WM_DESTROY:
    DeleteObject(buffer);
    ReleaseDC(hWnd, hdc);
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

void ReDrawBuff(HWND hWnd){
  static int tRad=15;
  FillRect(hdcMem, &paint_rect, whiteBrush);
  Ellipse(hdcMem, g_CentX-tRad, g_CentY-tRad, g_CentX+tRad, g_CentY+tRad);
  MoveToEx(hdcMem, g_CentX, g_CentY, NULL);
  LineTo(hdcMem, g_CentX+tRad*cos(g_Alpha), g_CentY+tRad*sin(g_Alpha));
  static double a = 0.03;
  for(int i=0; i<g_PackSize; i++){
    g_PackTime[i]++;
    g_X[i] = round(g_PackStartX[i]+cos(g_PackAlpha[i])*g_PackPow[i]*g_PackTime[i]);
    g_Y[i] = round(g_PackStartY[i]+sin(g_PackAlpha[i])*g_PackPow[i]*g_PackTime[i]+a*g_PackTime[i]*g_PackTime[i]/2.0);
    Ellipse(hdcMem, g_X[i]-g_Rad, g_Y[i]-g_Rad, g_X[i]+g_Rad, g_Y[i]+g_Rad);
  }
  for(int i=0; i<g_PackSize; i++){
    if(g_Y[i]>paint_rect.bottom){
      g_PackStartX[i] = g_PackStartX[g_PackSize-1];
      g_PackStartY[i] = g_PackStartY[g_PackSize-1];
      g_PackTime[i] = g_PackTime[g_PackSize-1];
      g_PackAlpha[i] = g_PackAlpha[g_PackSize-1];
      g_PackPow[i] = g_PackPow[g_PackSize-1];
      g_PackSize--;
    }
    if(sqrt((g_X[i]-g_TargX)*(g_X[i]-g_TargX)+(g_Y[i]-g_TargY)*(g_Y[i]-g_TargY))<10+g_Rad){
      g_TargX = rand()%paint_rect.right;
      g_TargY = 50+rand()%(paint_rect.bottom-50);
    }
  }
  Ellipse(hdcMem, g_TargX-10, g_TargY-10, g_TargX+10, g_TargY+10);
  _stprintf(line, TEXT("Y: %d %d %d %d"), g_TargX, g_TargY, g_CentX, g_CentY);
  TextOut(hdcMem, 0, 0, line, strlen(line));
  InvalidateRect(hWnd, &paint_rect, false);
}