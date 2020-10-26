extern "C" IMAGE_DOS_HEADER __ImageBase;

using namespace winrt;

auto CreateDispatcherQueueController()
{
    namespace abi = ABI::Windows::System;

    DispatcherQueueOptions options
    {
        sizeof(DispatcherQueueOptions),
        DQTYPE_THREAD_CURRENT,
        DQTAT_COM_STA
    };

    Windows::System::DispatcherQueueController controller{ nullptr };
    check_hresult(CreateDispatcherQueueController(options, reinterpret_cast<abi::IDispatcherQueueController**>(put_abi(controller))));
    return controller;
}

template <typename T>
struct DesktopWindow
{
    static T* GetThisFromHandle(HWND const window) noexcept
    {
        return reinterpret_cast<T*>(GetWindowLongPtr(window, GWLP_USERDATA));
    }

    static LRESULT __stdcall WndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
    {
        WINRT_ASSERT(window);

        if (WM_NCCREATE == message)
        {
            auto cs = reinterpret_cast<CREATESTRUCT*>(lparam);
            T* that = static_cast<T*>(cs->lpCreateParams);
            WINRT_ASSERT(that);
            WINRT_ASSERT(!that->m_window);
            that->m_window = window;
            SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(that));
        }
        else if (T* that = GetThisFromHandle(window))
        {
            return that->MessageHandler(message, wparam, lparam);
        }

        return DefWindowProc(window, message, wparam, lparam);
    }

    LRESULT MessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
    {
        if (WM_DESTROY == message)
        {
            PostQuitMessage(0);
            return 0;
        }
        else if (WM_CREATE == message)
        {
            m_label = CreateWindow("static", "label",
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                20, 20, 600, 20,
                m_window, reinterpret_cast<HMENU>(501),
                nullptr, nullptr);
        }
        else if (WM_PAINT == message)
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_window, &ps);
            FillRect(hdc, &ps.rcPaint, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
            SetWindowText(m_label, m_info.c_str());
            EndPaint(m_window, &ps);
        }

        return DefWindowProc(m_window, message, wparam, lparam);
    }

    void SetInfo(const std::string& info)
    {
        m_info = info;
    }

protected:

    using base_type = DesktopWindow<T>;
    HWND m_window = nullptr;
    HWND m_label = nullptr;
    std::string m_info;
};

struct Window : DesktopWindow<Window>
{
    Window() noexcept
    {
        WNDCLASS wc{};
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
        wc.lpszClassName = "ConfigInfo";
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        RegisterClass(&wc);
        WINRT_ASSERT(!m_window);

        WINRT_VERIFY(CreateWindow(wc.lpszClassName,
            "ConfigInfo",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            nullptr, nullptr, wc.hInstance, this));

        WINRT_ASSERT(m_window);
    }

    LRESULT MessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
    {
        return base_type::MessageHandler(message, wparam, lparam);
    }
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    init_apartment();
    auto controller = CreateDispatcherQueueController();
    Window window;

    int nArgs;
    const auto szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (nullptr != szArglist)
    {
        std::wstring wstr{ szArglist[0] };
        const auto size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wstr[0], static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string ret = std::string(size, 0);
        WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wstr[0], static_cast<int>(wstr.size()), &ret[0], size, nullptr, nullptr);
        ret.replace(ret.end()-3, ret.end(), "xml");
        window.SetInfo(ret);
    }
    LocalFree(szArglist);



    MSG message;
    while (GetMessage(&message, nullptr, 0, 0))
    {
        DispatchMessage(&message);
    }

    return 0;
}