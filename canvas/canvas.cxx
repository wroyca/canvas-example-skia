#include <Windows.h>

#include <skia/core/SkCanvas.h>
#include <skia/core/SkColorSpace.h>
#include <skia/core/SkPaint.h>
#include <skia/core/SkRect.h>
#include <skia/core/SkSurface.h>
#include <skia/gpu/GrDirectContext.h>

#include <GL/gl.h>

using namespace std;

namespace canvas
{
  namespace
  {
    struct pixel
    {
      struct
      {
        int s;
        PIXELFORMATDESCRIPTOR descriptor;

        auto&
        operator= (const int v)
        {
          return s = v, *this;
        }

        operator int () const
        {
          return s;
        }
      } format;
    };

    struct context
    {
      unique_ptr<HGLRC> gl;
      unique_ptr<sk_sp<GrDirectContext>> skia;
    };

    void
    draw_frame (const HDC& h)
    {
      static auto [format] (pixel {});
      static auto ctx (context {});
      static auto flag (once_flag {});

      auto create_context = [&]
      {
        format.descriptor.nSize = sizeof format.descriptor;
        format.descriptor.nVersion = 1;
        format.descriptor.dwFlags = PFD_DRAW_TO_WINDOW
                                  | PFD_SUPPORT_OPENGL
                                  | PFD_DEPTH_DONTCARE;
        format.descriptor.iPixelType = PFD_TYPE_RGBA;
        format.descriptor.cColorBits = 24;
        format.descriptor.cAlphaBits = 8;
        format.descriptor.iLayerType = PFD_MAIN_PLANE;
        format = ChoosePixelFormat (h, &format.descriptor);
        DescribePixelFormat (h, format, sizeof (PIXELFORMATDESCRIPTOR),
                             &format.descriptor);
        SetPixelFormat (h, format, &format.descriptor);
        ctx.gl = make_unique <HGLRC> (wglCreateContext (h));
        wglMakeCurrent (h, *ctx.gl);
        ctx.skia = make_unique <sk_sp <GrDirectContext>> (
          GrDirectContext::MakeGL ());
      };

      call_once (flag, create_context);
      SetPixelFormat (h, format, &format.descriptor);

      const auto props = SkSurfaceProps {};
      const auto surf = SkSurface::MakeFromBackendRenderTarget (
        ctx.skia->get (),
        GrBackendRenderTarget (500, 500, 1, 0,
                               GrGLFramebufferInfo (0, GL_RGBA8)),
        kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType,
        SkColorSpace::MakeSRGB (), &props);
      {
        auto  pa = SkPaint {};
        auto& ca = *surf->getCanvas ();

        ca.save ();
        ca.clear (SK_ColorBLACK);
        pa.setColor (SK_ColorBLUE);
        pa.setStyle (SkPaint::kFill_Style);
        ca.drawRect (SkRect::MakeXYWH (100, 100, 200, 200), pa);
        ca.restore ();

        ctx.skia->get ()->flushAndSubmit (true);
        SwapBuffers (h);
      }
    }
  }
}

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
        canvas::draw_frame (BeginPaint (h, &ps));
        EndPaint (h, &ps);
        break;
      }

      case WM_SETCURSOR:
      {
        SetCursor (LoadCursor (nullptr, IDC_ARROW));
        break;
      }

      default:
      {
        break;
      }
    }
    return DefWindowProcW (h, u, w, l);
  };

  RegisterClassW (&wc);
  ShowWindow (CreateWindowExW (0, L"Sample", L"Sample", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                               CW_USEDEFAULT, nullptr, nullptr, in, nullptr), SW_SHOW);

  auto m (MSG {});
  {
    while (GetMessageW (&m, nullptr, 0, 0) > 0)
    {
      TranslateMessage (&m);
      DispatchMessageW (&m);
    }
  }
}
