//WorkThreadPool.h
/////////////////////////////////////////////////////////////
//�����̳߳� CWorkThreadPool
//���ڰ�һ�������ֳɶ���߳�����,�Ӷ�����ʹ�ö��CPU
//HouSisong@263.net
////////////////////////////
//todo:�ĳ�������ȡģʽ
//todo:�޸ĸ����߳����ȼ����̳������߳�
//Ҫ��1.����ָ�ʱ�ָ���������ȽϽӽ�
//      2.����Ҳ��Ҫ̫С�������̵߳Ŀ������ܻ���ڲ��е�����
//      3.�����������CPU���ı���
//      4.���̲߳����Թ������ȼ����У��������������߳̿��ܵò���ʱ��Ƭ

#ifndef _WorkThreadPool_H_
#define _WorkThreadPool_H_

typedef void (*TThreadCallBack)(void * pData);

class CWorkThreadPool
{
public:
    static long best_work_count();  //������ѹ����ָ���,���ڵ�ʵ��Ϊ����CPU����
    static void work_execute(const TThreadCallBack work_proc,void** word_data_list,int work_count);  //����ִ�й��������ȴ����й������    
    static void work_execute_multi(const TThreadCallBack* work_proc_list,void** word_data_list,int work_count); //ͬ�ϣ�����ͬ��work���ò�ͬ�ĺ���
    static void work_execute_single_thread(const TThreadCallBack work_proc,void** word_data_list,int work_count)  //���߳�ִ�й��������ȴ����й������;���ڵ��Ե�  
    {
        for (long i=0;i<work_count;++i)
        work_proc(word_data_list[i]);
    }
    static void work_execute_single_thread_multi(const TThreadCallBack* work_proc_list,void** word_data_list,int work_count)  //���߳�ִ�й��������ȴ����й������;���ڵ��Ե�  
    {
        for (long i=0;i<work_count;++i)
        work_proc_list[i](word_data_list[i]);
    }
};

#endif //_WorkThreadPool_H_