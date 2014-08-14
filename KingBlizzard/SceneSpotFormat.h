//SceneSpotFormat.h  ���ʽӳ��
#ifndef __SceneSpotFormat_H_
#define __SceneSpotFormat_H_

#include "ScenePointFractal.h"
#include "math.h"

class CSceneSpotFormat:public CScenePointFractal
{
protected:
    long    m_DrawCount;
    long    m_SpotCount;        //�̶������
    long    m_IteratCount;      //��������
    long    m_dataLength;
    std::vector<double> m_S0x;
    std::vector<double> m_S0y;
    std::vector<double> m_S1x;
    std::vector<double> m_S1y;
    void NextPos();
    void IntiData();
protected:
    void Scene_Inti(long ScreenWidth,long ScreenHeight) 
    { 
        CScenePointFractal::Scene_Inti(ScreenWidth,ScreenHeight); 
        m_DrawCount=80000;
        IntiData();
    }
    void IntiMover();
    void DoDraw(Context32* dst);
public:
};

#endif //__SceneSpotFormat_H_