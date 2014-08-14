
#include "DDrawImport.h"
#include "hDibImport.h"

#include <ddraw.h>
#include "ColorMover.h"
#include "MMTime.h"

class CDDraw
{
private:
    // DirectDraw stuff
    LPDIRECTDRAW4         lpdd4        ;   // DD3 ����
    LPDIRECTDRAWSURFACE4  lpddsprimary ;   // dd ������
    LPDIRECTDRAWSURFACE4  lpddsback    ;   // dd �������
    DDSURFACEDESC2        ddsd;            // һ�� Direct Draw ���� ���� struct
    CDibImport            m_DibSurface;      //DIB���ƻ���

    //֡������
    bool                  m_IsShowFPS;
    long                  m_OldDrawTime;
    double                m_FPS;
    CColorMover           m_FPSColor;
public:
    CDDraw():lpdd4(0),lpddsprimary(0),lpddsback(0),m_IsShowFPS(false) { }
    ~CDDraw()  { Clear(); }
    void IsShowFPS(bool isShowFPS) { m_IsShowFPS=isShowFPS; }

    void  Clear()
    {
        if (lpddsback!=0) // �ͷŻ������
        {
            lpddsback->Release();
            lpddsback = 0;
        }
        if (lpddsprimary!=0)//�ͷ�������
        {
            lpddsprimary->Release();
            lpddsprimary = 0;
        }
        if (lpdd4!=0)// �ͷ� IDirectDraw4 ����
        {
            lpdd4->Release();
            lpdd4 = 0;
        }
        m_DibSurface.Clear();
    }

    bool setScreenMode(long ScreenWidth,long ScreenHeight,long ScreenColorBit)
    {
        HRESULT result=lpdd4->SetDisplayMode(ScreenWidth,ScreenHeight, ScreenColorBit,0,0);
        return !(FAILED(result));
    }
    bool trySetScreenMode(long ScreenWidth,long ScreenHeight,long ScreenColorBit)
    {
        //����32,24,16������ɫģʽ
        if ( (ScreenColorBit!=32)&&(setScreenMode(ScreenWidth,ScreenHeight, 32)))  return true;
        if ( (ScreenColorBit!=24)&&(setScreenMode(ScreenWidth,ScreenHeight, 24)))  return true;
        if ( (ScreenColorBit!=16)&&(setScreenMode(ScreenWidth,ScreenHeight, 16)))  return true;
        return false;
    }

    bool IntiScreen(long ScreenWidth,long ScreenHeight,long ScreenColorBit,void * main_window_handle)
    {
        Clear();

        // �ú����ڳ��δ����괰��ͽ����¼�ѭ��ʱ����
        // ����DDraw�ĳ�ʼ��

        LPDIRECTDRAW lpdd_temp;

        // �������� IDirectDraw ����
        if (FAILED(DirectDrawCreate(0, &lpdd_temp, 0)))
            return false;

        // ��ѯ IDirectDraw4 ����
        if (FAILED(lpdd_temp->QueryInterface(IID_IDirectDraw4,(LPVOID *)&lpdd4)))
            return false;

        // ����Э���ȼ�Ϊȫ��Ļ
        if ( FAILED(lpdd4->SetCooperativeLevel((HWND)main_window_handle, 
            DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE|
            DDSCL_ALLOWREBOOT  
            //|DDSCL_FPUSETUP  
            )) )
            return false;

        //������ʾģʽ
        if (!setScreenMode(ScreenWidth,ScreenHeight, ScreenColorBit))//����Ĭ��ģʽ
        {
            if (!trySetScreenMode(ScreenWidth,ScreenHeight, ScreenColorBit))
                return false;
        }

        // ���Direct Draw ���� ���������ô�С
        memset(&ddsd,0,sizeof(ddsd)); 
        ddsd.dwSize=sizeof(ddsd); 
        // ������ȷ�����Ա��
        ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        // ���û��������Ŀ�������������棩
        ddsd.dwBackBufferCount = 1;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
        // ����������
        if (FAILED(lpdd4->CreateSurface(&ddsd,&lpddsprimary, 0)))
            return false;

        // ��������������ĸ��ӱ���
        ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
        // �õ����ӵĻ������  ���Ǳ����
        if (FAILED(lpddsprimary->GetAttachedSurface(&ddsd.ddsCaps, &lpddsback)))
            lpddsback=0; //���ӱ������벻�ɹ� ��������ִ��

        m_DibSurface.Inti(ScreenWidth,ScreenHeight);
        m_FPSColor.Inti(20,100,10.0/1000,80.0/1000);

        return true;
    }

    bool DrawBegin(Context32& out_dst)
    {
        if (lpddsprimary==0) return false;

        if (lpdd4->TestCooperativeLevel()==DD_OK)//�Ƿ���Ҫ�ָ�����
	        lpdd4->RestoreAllSurfaces(); 
        else
            return false;
        return m_DibSurface.lock_Data(out_dst);
    }

    void DrawEnd(Context32& dst)
    {
        long ScreenWidth=dst.width;
        long ScreenHeight=dst.height;
        m_DibSurface.unlock_Data(dst);

        LPDIRECTDRAWSURFACE4  dd;
        if (lpddsback!=0)
            dd=lpddsback;
        else
            dd=lpddsprimary;

        //�������ݵ��Դ�
        HDC dstDC=0;  
        if (FAILED(dd->GetDC(&dstDC))) return;
        HDC srcDC=(HDC)m_DibSurface.lock_DC();
        BitBlt(dstDC,0,0,ScreenWidth,ScreenHeight,srcDC,0,0,SRCCOPY);
        long newDrawTime=mmGetTime();
        if (m_OldDrawTime>0)
        {
            //HDC dstDC=srcDC;
            long stepTime=newDrawTime-m_OldDrawTime;
            if ((this->m_IsShowFPS)&&(stepTime>0))
            {
                this->m_FPS=1000.0/stepTime;
                m_FPSColor.Update(stepTime);
                char str[50];
	            gcvt(m_FPS,4,str);
	            SetBkColor(dstDC,RGB(0,0,0));
                SetTextColor(dstDC,m_FPSColor.getColor32());
                TextOut(dstDC,5,5,str,lstrlen(str));
	            TextOut(dstDC,45,5,"fps",3);
            }
        }
        m_OldDrawTime=newDrawTime;
        m_DibSurface.unlock_DC();
        dd->ReleaseDC(dstDC);
        
        if (lpddsback!=0)
        {
            while (FAILED(lpddsprimary->Flip(0, DDFLIP_WAIT)));// ��������
        }

    }
};

/////////////////////////////////////////////////////////////////////////////////////////


CDDrawImprot::CDDrawImprot() 
: m_Import(0) 
{
}


bool CDDrawImprot::IntiScreen(long ScreenWidth,long ScreenHeight,long ScreenColorBit,void * main_window_handle)
{
    if (m_Import==0)
        m_Import=new CDDraw();
    bool result=((CDDraw*)m_Import)->IntiScreen(ScreenWidth,ScreenHeight,ScreenColorBit,main_window_handle);
    if (!result)
        ClearImportPointer();
    return result;
}

void CDDrawImprot::ClearImportPointer()
{
    if (m_Import!=0)
    {
        CDDraw* Import=((CDDraw*)m_Import);
        m_Import=0;
        delete Import;
    }
}


CDDrawImprot::~CDDrawImprot()
{
    ClearImportPointer();
}

bool CDDrawImprot::DrawBegin(Context32& dst)
{
    if (m_Import==0)
        return false;
    else
        return ((CDDraw*)m_Import)->DrawBegin(dst);

}

void CDDrawImprot::DrawEnd(Context32& dst)
{
    if (m_Import!=0)
        ((CDDraw*)m_Import)->DrawEnd(dst);
}


void  CDDrawImprot::IsShowFPS(bool isShowFPS)
{
    if (m_Import!=0)
        ((CDDraw*)m_Import)->IsShowFPS(isShowFPS);
}

