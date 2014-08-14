//SceneJulia.h  Julia����
#ifndef __SceneJulia_H_
#define __SceneJulia_H_

#include "IGameScene.h"
#include "HColor.h"
#include "ColorMover.h"
#include <vector>
#include "Surface.h"

class CSceneJulia:public IGameScene
{
protected:
    CColorMover        m_ColorMoverIn;
    CColorMover        m_ColorMoverOut;
    std::vector<char> _m_MData;  //Mandelbrot���ϵ�������������  ���õ��Ϻ��ʵ�Julia������
    char*  m_MData; //==&_m_MData[0];
    double m_Mx0;
    double m_My0;
    long   m_UpdateTime;
    CSurface  m_BufBck;
    bool      m_BufBckIsOk;
    void DoDraw(Context32* out_dst);
    void Inti();
public:
    void Scene_Inti(long ScreenWidth,long ScreenHeight) { Scene_Clear(); Inti(); }
    void Scene_Clear();
    void Scene_Update(unsigned long StepTime_ms);
    void Scene_Draw(Context32* dst) { DoDraw(dst); }
    ~CSceneJulia(){}
};



#endif //__SceneJulia_H_