// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
// Copyright (c) Keiya Chinen


#include "SimpleDirect2dApplication.h"
#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <winsock2.h>
#include <vector>
#include <string>
#include <iostream>
#include "msgpack.hpp"
#include "nuiapi.h"
#include "NuiSensor.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>


float angle;
HANDLE hFind;
WIN32_FIND_DATA fd;
std::vector<std::string> files;
int vec_idx;

std::string DemoApp::wchar2string(WCHAR *wc) {
    //convert from wide char to narrow char array
	char ch[260];
    char DefChar = ' ';
    WideCharToMultiByte(CP_ACP,0,wc,-1, ch,260,&DefChar, NULL);
    
    //A std:string  using the char* constructor.
    std::string ss(ch);

	return ss;
}

void DemoApp::string2wchar(WCHAR *pWbuffer,std::string &str) {
	size_t ReturnValue;
	mbstowcs_s(&ReturnValue, pWbuffer,500,str.c_str(),500);
}

void calcArmDegree(std::vector<std::vector<std::vector<float>>> &humans) {
	std::vector<std::vector<std::vector<float>>>::iterator v_itr = humans.begin();
	
	for (; v_itr != humans.end(); ++v_itr) {
		//std::cout << *(v_itr) << std::endl;
		std::vector<std::vector<float>> w_itr = *v_itr;
		
		printf("%04.3f %04.3f | %04.3f %04.3f\r",
		w_itr.at(NUI_SKELETON_POSITION_ELBOW_RIGHT).at(0),
		w_itr.at(NUI_SKELETON_POSITION_SHOULDER_RIGHT).at(0),
		w_itr.at(NUI_SKELETON_POSITION_ELBOW_LEFT).at(0),
		w_itr.at(NUI_SKELETON_POSITION_SHOULDER_LEFT).at(0)
		);
	if (w_itr.at(NUI_SKELETON_POSITION_WRIST_RIGHT).at(1) > w_itr.at(NUI_SKELETON_POSITION_ELBOW_RIGHT).at(1)) {
		// 手の角度
		angle = atan2(
			w_itr.at(NUI_SKELETON_POSITION_WRIST_RIGHT).at(1) - w_itr.at(NUI_SKELETON_POSITION_ELBOW_RIGHT).at(1),
			w_itr.at(NUI_SKELETON_POSITION_WRIST_RIGHT).at(0) - w_itr.at(NUI_SKELETON_POSITION_ELBOW_RIGHT).at(0)
		);
		printf("R:ANGL: %04.3f                  \r",angle*180/M_PI);
		angle = angle*180/M_PI;
	}
	else if (w_itr.at(NUI_SKELETON_POSITION_WRIST_LEFT).at(1) > w_itr.at(NUI_SKELETON_POSITION_ELBOW_LEFT).at(1)) {
		// 手の角度
		angle = atan2(
			w_itr.at(NUI_SKELETON_POSITION_WRIST_LEFT).at(1) - w_itr.at(NUI_SKELETON_POSITION_ELBOW_LEFT).at(1),
			w_itr.at(NUI_SKELETON_POSITION_WRIST_LEFT).at(0) - w_itr.at(NUI_SKELETON_POSITION_ELBOW_LEFT).at(0)
		);

		printf("L:ANGL: %04.3f                  \r",angle*180/M_PI+180);
		angle = angle*180/M_PI+180;
	}
	else {
		angle = 180;
	}
	
	}

}

// udp server thread
unsigned __stdcall server(void *p) {
	HANDLE hEvent;
    /* イベントをシグナル状態にする */
    hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME);
    SetEvent(hEvent);
    CloseHandle(hEvent);

	WSAData wsaData;

	SOCKET sock;
	struct sockaddr_in addr;
	char text[9];
	
	char buf[2048];
	
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
	    printf("Failed. Error Code : %d",WSAGetLastError());
		MessageBoxA(NULL,"WSAStartup() error\nWSAStartup()に失敗","Server",MB_ICONERROR|MB_OK);
	    exit(EXIT_FAILURE);
	}
	
	if((sock = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET)
	{
	    printf("Could not create socket : %d" , WSAGetLastError());
		MessageBoxA(NULL,"Socket socket() error\nソケット／socket()に失敗","Server",MB_ICONERROR|MB_OK);
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(65487);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	
	if( bind(sock ,(struct sockaddr *)&addr , sizeof(addr)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d" , WSAGetLastError());
		MessageBoxA(NULL,"Socket bind() error\nソケット／bind()に失敗","Server",MB_ICONERROR|MB_OK);
	    exit(EXIT_FAILURE);
	}
	
	memset(buf, 0, sizeof(buf));
	int res;
	
	std::vector<std::vector<std::vector<float>>> humans;
	//char *eob;
	while (1) {
		res = recv(sock, buf, sizeof(buf), 0);
		if (res > 0) {
			msgpack::unpacked msg;
			msgpack::unpack(&msg,buf,res);
			msgpack::object obj = msg.get();
			obj.convert(&humans);
			calcArmDegree(humans);
		}
	}

	closesocket(sock);

	WSACleanup();

	_endthreadex(0);

	return 0;
}

// viewer thread
unsigned __stdcall viewer(void *p)
{
	// Ignore the return value because we want to continue running even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize (NULL)))
    {
        {
            DemoApp app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }
    
    _endthreadex(0);
    return 0;
}

//http://stackoverflow.com/questions/306533/how-do-i-get-a-list-of-files-in-a-directory-in-c
std::vector<std::string> DemoApp::GetDirectories(std::string directory)
{
    HANDLE dir;
    WIN32_FIND_DATA file_data;
	
	std::vector<std::string> out;

	WCHAR wSuffix[] = L"\\*.*";
	std::string sSuffix = wchar2string(wSuffix);

	WCHAR pWbuffer[512];
	string2wchar(pWbuffer,directory+sSuffix);
	//printf("%s\n",(LPCWSTR)(directory + sSuffix).c_str());
    //if ((dir = FindFirstFile((LPCWSTR)(directory + sSuffix).c_str(), &file_data)) == INVALID_HANDLE_VALUE) {
	if ((dir = FindFirstFile((LPCWCHAR)pWbuffer, &file_data)) == INVALID_HANDLE_VALUE) {
		printf("er:%d\n",GetLastError ());
		MessageBoxA(NULL,"FindFirstFile() failed","GetDirectories",MB_ICONERROR|MB_OK);
    	return out; /* No files found */
	}


    do {
		WCHAR wDelim[] = L"\\";
		std::string sDelim = wchar2string(wDelim);
		std::string file_name = wchar2string(file_data.cFileName);

    	//const std::string file_name = file_data.cFileName;
    	const std::string full_file_name = directory + sDelim + file_name;

    	const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    	if (file_name[0] == '.')
    		continue;

    	//if (!is_directory)
    	//	continue;
    	out.push_back(full_file_name);
    } while (FindNextFile(dir, &file_data));

    FindClose(dir);
	return out;
}

//
// Provides the entry point to the application.
//
int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
    )
{
		setlocale(LC_ALL,"ja_JP.UTF-8");
	if (AllocConsole())
	{
		freopen("CONOUT$", "w", stdout);
		//printf("KineticSignage@MissingLink\n");
	}
	else
	{
		MessageBox(NULL,L"Failed to init a console.\n",L"",0);
	}

    HANDLE hThread[2];

    hThread[0] = (HANDLE)_beginthreadex(NULL, 0, viewer, NULL, 0, NULL);
    hThread[1] = (HANDLE)_beginthreadex(NULL, 0, server, NULL, 0, NULL);

    WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
    CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
    return 0;
}


//
// Initialize members.
//
DemoApp::DemoApp() :
    m_hwnd(NULL),
    m_pD2DFactory(NULL),
    m_pWICFactory(NULL),
    m_pDWriteFactory(NULL),
    m_pRenderTarget(NULL),
    m_pBitmap(NULL)
{
}

//
// Release resources.
//
DemoApp::~DemoApp()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pWICFactory);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBitmap);
}

//
// Creates the application window and initializes
// device-independent resources.
//
HRESULT DemoApp::Initialize()
{
	std::string dir = "C:\\Users\\Keiya\\Pictures\\test";
	files = GetDirectories(dir);

	//for (std::vector<std::string>::iterator vec_it = files.begin();vec_it != files.end();++vec_it) {
	//	std::cout << "found:"<<*vec_it << std::endl;
	//}

    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    HRESULT hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(wcex) };
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = WndProc;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = HINST_THISCOMPONENT;
        wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wcex.lpszClassName = L"D2DDemoApp";

        RegisterClassEx(&wcex);

        // Create the application window.
        //
        // Because the CreateWindow function takes its size in pixels, we
        // obtain the system DPI and use it to scale the window size.
        FLOAT dpiX, dpiY;
        m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        // Create the application window.
        m_hwnd = CreateWindow(
            L"D2DDemoApp",
            L"Direct2D Demo Application",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
        );
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            ShowWindow(m_hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_hwnd);
        }
    }

    return hr;
}


//
// Create resources which are not bound
// to any device. Their lifetime effectively extends for the
// duration of the app. These resources include the Direct2D,
// DirectWrite, and WIC factories; and a DirectWrite Text Format object
// (used for identifying particular font characteristics) and
// a Direct2D geometry.
//
HRESULT DemoApp::CreateDeviceIndependentResources()
{
    static const WCHAR msc_fontName[] = L"Verdana";
    static const FLOAT msc_fontSize = 50;

    // Create a Direct2D factory.
    ID2D1GeometrySink *pSink = NULL;
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
    if (SUCCEEDED(hr))
    {
        // Create WIC factory.
        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pWICFactory)
            );
    }
    if (SUCCEEDED(hr))
    {
        
        // Create a DirectWrite factory.
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(m_pDWriteFactory),
            reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite text format object.
        hr = m_pDWriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            msc_fontSize,
            L"", //locale
            &m_pTextFormat
            );
    }
    if (SUCCEEDED(hr))
    {
        // Center the text horizontally and vertically.
        m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

        m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        
    }

    if (SUCCEEDED(hr))
    {
        // Create a path geometry.
        hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometry); 
    }   
    if (SUCCEEDED(hr))
    {
        // Use the geometry sink to write to the path geometry.
        hr = m_pPathGeometry->Open(&pSink);
    }
    if (SUCCEEDED(hr))
    {
        pSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);

        pSink->BeginFigure(
            D2D1::Point2F(0, 0),
            D2D1_FIGURE_BEGIN_FILLED
            );

        pSink->AddLine(D2D1::Point2F(200, 0));

        pSink->AddBezier(
            D2D1::BezierSegment(
                D2D1::Point2F(150, 50),
                D2D1::Point2F(150, 150),
                D2D1::Point2F(200, 200))
            );

        pSink->AddLine(D2D1::Point2F(0, 200));

        pSink->AddBezier(
            D2D1::BezierSegment(
                D2D1::Point2F(50, 150),
                D2D1::Point2F(50, 50),
                D2D1::Point2F(0, 0))
            );

        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

        hr = pSink->Close();
    }
    

    SafeRelease(&pSink);

    return hr;
}

//
//  This method creates resources which are bound to a particular
//  Direct3D device. It's all centralized here, in case the resources
//  need to be recreated in case of Direct3D device loss (eg. display
//  change, remoting, removal of video card, etc).
//
HRESULT DemoApp::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
            );

        // Create a Direct2D render target.
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
            );

        if (SUCCEEDED(hr))
        {
            ID2D1GradientStopCollection *pGradientStops = NULL;
            // Create a linear gradient.
            static const D2D1_GRADIENT_STOP stops[] =
            {
                {   0.f,  { 0.f, 1.f, 1.f, 0.25f }  },
                {   1.f,  { 0.f, 0.f, 1.f, 1.f }  },
            };
        }

    }

    return hr;
}


//
//  Discard device-specific resources which need to be recreated
//  when a Direct3D device is lost
//
void DemoApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBitmap);
 //   SafeRelease(&m_pLinearGradientBrush);
}

//
// The main window message loop.
//
void DemoApp::RunMessageLoop()
{
    MSG msg;
	BOOL b;
	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			b = GetMessage(&msg,NULL,0,0);
			if (b == 0 || b == -1)
		    break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// Right hand ; points left
			if (angle > 90 && angle < 180) {
				//printf("R %d\n",angle);
				--vec_idx;
				if (vec_idx < 0) vec_idx = files.size() - 1;
				render(files.at(vec_idx));
			}
			// Right hand ; points right
			else if (angle > 0 && angle < 90) {
				++vec_idx;
				if (files.size() <= vec_idx) vec_idx = 0;
				render(files.at(vec_idx));
			}
			// Left hand ; points left
			else if (angle < 360 && angle > 270) {
				//printf("L %d\n",angle);
				--vec_idx;
				if (vec_idx < 0) vec_idx = files.size() - 1;
				render(files.at(vec_idx));
			}
			// Left hand ; points right
			else if (angle > 180 && angle < 270) {
				++vec_idx;
				if (files.size() <= vec_idx) vec_idx = 0;
				render(files.at(vec_idx));
			}
		}
    }
}


//
//  Called whenever the application needs to display the client
//  window. This method draws a bitmap a couple times, draws some
//  geometries, and writes "Hello, World"
//
//  Note that this function will automatically discard device-specific
//  resources if the Direct3D device disappears during function
//  invocation, and will recreate the resources the next time it's
//  invoked.
//

void DemoApp::OnRender()
{
	HRESULT hr = CreateDeviceResources();
	    if (SUCCEEDED(hr))
    {
	if (m_pRenderTarget)
    {
	m_pRenderTarget->BeginDraw();
    m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
    m_pRenderTarget->EndDraw();
	}
		}
}

void DemoApp::render(std::string filename) {

		if (m_pRenderTarget) {
			HRESULT hr;

			//static const WCHAR sc_helloWorld[] = L"Hello, World!";
			// Retrieve the size of the render target.
			D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

			m_pRenderTarget->BeginDraw();

			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

			m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));


			WCHAR pWbuffer[512];
			string2wchar(pWbuffer,filename);
			hr = LoadBitmapFromFile(
			    m_pRenderTarget,
			    m_pWICFactory,
				(PCWSTR)pWbuffer,
			    1360,
			    0,
			    &m_pBitmap
			);
			if (SUCCEEDED(hr)) {
				D2D1_SIZE_F size = m_pBitmap->GetSize();
				m_pRenderTarget->DrawBitmap(
				    m_pBitmap,
					D2D1::RectF(0.0f, 0.0f, size.width, size.height)
				);
				SafeRelease(&m_pBitmap);
			}
			else {
				printf("resource trouble?\n");
			}
	

			hr = m_pRenderTarget->EndDraw();

			if (hr == D2DERR_RECREATE_TARGET)
			{
			    hr = S_OK;
			    DiscardDeviceResources();
			}
		}

}

//
//  If the application receives a WM_SIZE message, this method
//  resize the render target appropriately.
//
void DemoApp::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        D2D1_SIZE_U size;
        size.width = width;
        size.height = height;

        // Note: This method can fail, but it's okay to ignore the
        // error here -- it will be repeated on the next call to
        // EndDraw.
        m_pRenderTarget->Resize(size);
    }
}

//
// The window message handler.
//
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pDemoApp)
            );

        result = 1;
    }
    else
    {
        DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
                )));

        bool wasHandled = false;

        if (pDemoApp)
        {
            switch (message)
            {
            case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pDemoApp->OnResize(width, height);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DISPLAYCHANGE:
                {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
                {
                    pDemoApp->OnRender();

                    ValidateRect(hwnd, NULL);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DESTROY:
                {
                    PostQuitMessage(0);
                }
                result = 1;
                wasHandled = true;
                break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}



//
// Creates a Direct2D bitmap from the specified
// file name.
//
HRESULT DemoApp::LoadBitmapFromFile(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR uri,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap **ppBitmap
    )
{
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

    HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
        uri,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
        );
        
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {

        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);

    }
 
 
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                            pSource,
                            destinationWidth,
                            destinationHeight,
                            WICBitmapInterpolationModeCubic
                            );
                }
                if (SUCCEEDED(hr))
                {
                    hr = pConverter->Initialize(
                        pScaler,
                        GUID_WICPixelFormat32bppPBGRA,
                        WICBitmapDitherTypeNone,
                        NULL,
                        0.f,
                        WICBitmapPaletteTypeMedianCut
                        );
                }
            }
        }
        else // Don't scale the image.
        {
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
                );
        }
    }
    if (SUCCEEDED(hr))
    {
    
        // Create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
            );
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}

