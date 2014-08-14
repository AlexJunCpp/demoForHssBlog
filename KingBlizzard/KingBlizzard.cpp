//���ζ�������  HouSisong 2008.01.15

//#define WIN32_LEAN_AND_MEAN  // ��ʹ�� MFC

#include <windows.h>
#include "resource.h"
#include "GameSys.h"
#include "MMTime.h"
#include "stdlib.h"

#include "SceneKing.h"
#include "SceneMira.h"
#include "SceneSpotFormat.h"
#include "SceneJulia.h"
#include "SceneNewton.h"

void AddScenes(CGameSys& GameSys)
{
    //��ӳ���
#ifdef OUT_SCR_Newton
    GameSys.AddGameScene(new CSceneNewton());
#else
    GameSys.AddGameScene(new CSceneKing());
    GameSys.AddGameScene(new CSceneMira());
    GameSys.AddGameScene(new CSceneSpotFormat());
    GameSys.AddGameScene(new CSceneJulia());
    GameSys.AddGameScene(new CSceneNewton());
#endif
}


bool g_WindowProc_isRunAsDebug=false;

//ϵͳ��Ϣ������
LRESULT CALLBACK WindowProc(HWND main_window_handle, UINT msg,  WPARAM wparam,  LPARAM lparam)
{
    switch(msg)
	{	
	case WM_CREATE: 
        {
		    //��ʼ��
            //todo: 
		    return 0;
		} break;
	case WM_PAINT: 
		{
            /*PAINTSTRUCT	ps;		// WM_PAINT��Ϣ��
   	        HDC	hdc = BeginPaint(main_window_handle,&ps);	 // ��ͼ���
            TextOut(hdc,rand()*800/RAND_MAX,rand()*600/RAND_MAX,"afsdgfergdgdf",10);
            EndPaint(main_window_handle,&ps);*/
		    return 0;
   		} break;
	case WM_DESTROY: 
		{
		    PostQuitMessage(0);// �������򣬷��� WM_QUIT ��Ϣ
		    return 0;
		} break;
    case WM_KILLFOCUS: 
        {
            if (!g_WindowProc_isRunAsDebug)
                PostMessage(main_window_handle,WM_SYSCOMMAND,SC_MAXIMIZE,0);//��ֹ����С��
        } break;
	default:
        break;
    } 

    //����δ������Ϣ
    return (DefWindowProc(main_window_handle, msg, wparam, lparam));
} 


///////////////////////////////////////////////////////////

bool DisposeCommonline(HINSTANCE hinstance,LPSTR lpcmdline,const char* Caption,bool& out_IsRunAsDebug)
{
    //�������в���
    //MessageBox(0,lpcmdline,"",0);
    //return false;

    out_IsRunAsDebug=false;
    if (strlen(lpcmdline)>0)
    {
	    
	    if (strcmp(lpcmdline,"/debug")==0)
	    {
		    //��������
            out_IsRunAsDebug=true;
            return true;
	    }
        else if (strcmp(lpcmdline,"/S")==0)
	    {
		    //��������\��������
            return true;
	    }
	    else if (strcmp(lpcmdline,"/s")==0)
	    {
		    //Ԥ������\��������
		    //��Ĭ��ֵ����
            return true;
	    }
	    else if ((strcmp(lpcmdline,"/c    ")>0)&&
		    (strcmp(lpcmdline,"/czzzz")<0))
	    {
		    //����
		    MessageBox(0,"\n no setting !\n",Caption,MB_OK+MB_ICONINFORMATION);
		    return false;
	    }
	    else if ((strcmp(lpcmdline,"/p   ")>0)&&
		         (strcmp(lpcmdline,"/pzzz")<0))
	    {
		    //��װ\С��ĻԤ��
		    char * str=lpcmdline+2;
		    //MessageBox(0,str,Caption,MB_OK+MB_ICONINFORMATION);
		    HWND shwnd=(HWND)atoi(str);
		    HDC shdc,phdc;
		    shdc=::GetWindowDC(shwnd);
		    phdc=::GetWindowDC(GetDesktopWindow());

		    RECT rect;
		    ::GetWindowRect(shwnd,&rect);
		    RECT rectp;
		    ::GetWindowRect(GetDesktopWindow(),&rectp);
		    ::StretchBlt(shdc,0,0,rect.right-rect.left,rect.bottom-rect.top,
			             phdc,0,0,rectp.right-rectp.left,rectp.bottom-rectp.top,
					     SRCCOPY);
    		
		    ::DrawIcon(shdc,(rect.right-rect.left-32)/2,(rect.bottom-rect.top-32)/2,
			    ::LoadIcon(hinstance,MAKEINTRESOURCE(IDI_MAINICON)) );
		    ::TextOut(shdc,0,0,"KingBlizzard",12);
		    ::ReleaseDC(GetDesktopWindow(),phdc);	
		    ::ReleaseDC(shwnd,shdc);	
		    return false;
	    }
	    else  
	    {
		    // �����в�������
		    MessageBox(0,"\nThe Command Line is Error!\n",Caption,MB_OK+MB_ICONERROR);
		    return false;
	    }
    }
    else
    {
        return true;
    }
}



inline bool KEYDOWN(int vk_code)  {  return ( 0!=(GetAsyncKeyState(vk_code) & 0x8000) );  }

inline long GetScreenColorBit()
{
    HWND dHWND=GetDesktopWindow();
    HDC dc=GetDC(dHWND);
    long result=GetDeviceCaps(dc,BITSPIXEL);
    ReleaseDC(dHWND,dc);
    return result;
}

// WINMAIN ����////////////////////////////////////////////////

int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{
    const char* Caption="King Blizzard";  //���ڱ���

    bool isRunAsDebug=false;
    if (!DisposeCommonline(hinstance,lpcmdline,Caption,isRunAsDebug)) return 1;

    g_WindowProc_isRunAsDebug=isRunAsDebug;
    
    //�����ĻĬ������  Ҳ����Ĭ�� 1024x768 32bitRGB;
    long SCREEN_WIDTH=GetSystemMetrics(SM_CXSCREEN);   
    long SCREEN_HEIGHT=GetSystemMetrics(SM_CYSCREEN); 
    long SCREEN_COLORBIT=GetScreenColorBit(); 

    //����fps�Զ�������������
    //todo:
    /*long MIN_FPS=20;
    long RUN_Quality=100;
    */

    const char* const WINDOW_CLASS_NAME="WINCLASS_King_Blizzard";

    WNDCLASSEX winclass;
    //���WNDCLASSEX�ṹ
    memset(&winclass,0,sizeof(winclass)); 
    winclass.cbSize         = sizeof(WNDCLASSEX);
    winclass.style			= CS_DBLCLKS | CS_OWNDC | 
                              CS_HREDRAW | CS_VREDRAW;
    winclass.lpfnWndProc	= WindowProc;
    winclass.cbClsExtra		= 0;
    winclass.cbWndExtra		= 0;
    winclass.hInstance		= hinstance;
    winclass.hIcon			= LoadIcon(hinstance, MAKEINTRESOURCE(IDI_MAINICON));
    winclass.hCursor		= LoadCursor(hinstance, MAKEINTRESOURCE(IDC_ARROW)); 
    winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
    winclass.lpszMenuName	= 0;
    winclass.lpszClassName	= WINDOW_CLASS_NAME;
    winclass.hIconSm        = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_MAINICON));

    // ע�ᴰ����
    if (!RegisterClassEx(&winclass))
	    return 1;

    HWND	   main_window_handle=0;	 // ��������
    // ��������
    if (!(main_window_handle = CreateWindowEx(0,                     // ��չ���
                                WINDOW_CLASS_NAME,     // ����
						        Caption,               // ����
						        WS_POPUP | WS_VISIBLE,
					 	        0,0,	               // ��ʼ����
						        SCREEN_WIDTH,SCREEN_HEIGHT,  // ��ʼ��С
						        0,	                   // �����ھ�� 
						        0,	                   // �˵����
						        hinstance,             // ��������
						        0)))	               // ��չ��������
    {
        return 1;
    }


    //�������
	ShowCursor(false);

    CGameSys GameSys;

    // ��ʼ����Ϸ
    if (!GameSys.Game_Init(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_COLORBIT,main_window_handle,isRunAsDebug)) return 1;
    AddScenes(GameSys);//��Ӳ�ͬ�ĳ��� Ȼ���������һ�����л�

    GameSys.Game_Loop();//Ԥ��ִ��һ����Ϸѭ��
    unsigned long Game_runStartTime=mmGetTime();

    //�������¼�ѭ��
    while(true)
    {
        MSG	msg; // ��Ϣ
        // �����Ϣ���У�����Ϣ��õ���
        if (PeekMessage(&msg,0,0,0,PM_REMOVE))
        { 
            if (msg.message == WM_QUIT)  break; // �����˳���Ϣ
            
            if (!isRunAsDebug)
            {   //������ͼ����¼�
                unsigned long Game_runNowTime=mmGetTime();
                if ((msg.message>= WM_MOUSEFIRST)&&(msg.message <= WM_MOUSELAST))
                {
                    //���������Ϣ�����Ѿ�������һ��ʱ��� �˳�����
                    if (Game_runNowTime-Game_runStartTime>500) 
                    {
                        static POINT msPos0;
                        static bool IsInti_msPos0=false;
                        if (!IsInti_msPos0)
                        {
                            GetCursorPos(&msPos0);
                            IsInti_msPos0=true;
                        }
                        else
                        {
                            if (msg.message==WM_MOUSEMOVE)
                            {
                                POINT msPos;
                                GetCursorPos(&msPos);
                                if (abs(msPos.x-msPos0.x)+abs(msPos.y-msPos0.y)>10)
                                    break;
                            }
                            else
                                break;
                        }
                    }
                }
                else if ((msg.message >= WM_KEYFIRST)&&(msg.message <= WM_KEYLAST))
                {
                    //����������,���������Ϣʱ�˳�����
                    unsigned long Game_runNowTime=mmGetTime();
                        //���������Ϣ�����Ѿ�������һ��ʱ��� �˳�����
                    if (Game_runNowTime-Game_runStartTime>500) 
                        break; 
                }
            }

            // ת��������ټ�
            TranslateMessage(&msg);

            // ������Ϣ�� window proc
            DispatchMessage(&msg);
        } // end if


        static bool is_window_closed=false;
        if (!is_window_closed)
        {
            //���� ѹ ESC ���� WM_CLOSE ��Ϣ��ʹ�����˳�
            if (KEYDOWN(VK_ESCAPE))
            {
                is_window_closed = true;
                PostMessage(main_window_handle,WM_CLOSE,0,0);
            }

            GameSys.Game_Loop();//��Ϸѭ��
        }
        mmSleep(0);
    }

    GameSys.Game_Shutdown();// �˳���Ϸ
    
    return 0;// �˳�
}

///////////////////////////////////////////////////////////





