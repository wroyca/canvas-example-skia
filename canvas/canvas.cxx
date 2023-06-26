#include <Windows.h>

#pragma comment(lib, "Gdi32.lib");

using namespace std;

int
main (int argc, char* argv[])
{
  auto wc (WNDCLASSW {});
  auto in (GetModuleHandleW (nullptr));

  wc.hInstance = in;
  wc.lpszClassName = L"Sample";
  wc.lpfnWndProc = [] (const HWND h,
                       const UINT u,
                       const WPARAM w,
                       const LPARAM l)
  {
    switch (u)
    {
      case WM_DESTROY:
      {
        PostQuitMessage (0);
        break;
      }

      case WM_PAINT:
      {
        PAINTSTRUCT ps;

        FillRect (BeginPaint (h, &ps), &ps.rcPaint,
                  static_cast <HBRUSH> (GetStockObject (BLACK_BRUSH)));

        EndPaint (h, &ps);
        break;
      }
      default: ;
    }
    return DefWindowProcW (h, u, w, l);
  };

  RegisterClassW (&wc);
  ShowWindow (CreateWindowExW (0, L"Sample", L"Sample", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                               CW_USEDEFAULT, nullptr, nullptr, in, nullptr), SW_SHOW);

  MSG msg = {};
  {
    while (GetMessageW (&msg, nullptr, 0, 0) > 0)
    {
      TranslateMessage (&msg);
      DispatchMessageW (&msg);
    }
  }
}
