//Base64���뺯���Ż����Գ���
//  HouSisong@GMail.com  2007.07.27����
//  ��ӭ���ʼ������ύ��������ĵ����ϵĲ��Գɼ�

#include <iostream>
#include <vector>
#include <string>

#define _MY_TIME_CLOCK

#ifdef _MY_TIME_CLOCK
    #include <windows.h>
    #define clock_t __int64
    clock_t CLOCKS_PER_SEC=0;
    inline clock_t clock() {
        __int64 result;
        if (CLOCKS_PER_SEC==0)
        {
            QueryPerformanceFrequency((LARGE_INTEGER *)&result);
            CLOCKS_PER_SEC=(clock_t)result;
        }
        QueryPerformanceCounter((LARGE_INTEGER *)&result);
        return (clock_t)result;
    }
#else
  #include <time.h>
#endif

#define asm __asm

    const unsigned char BASE64_PADDING='=';  //�������ݲ���3�ı���ʱ ����ַ��������'='��
    typedef void (*Tbase64_encode_proc)(const void* pdata,const unsigned long data_size,void* out_pcode);

    //��ñ���������ַ���С(ԭ���ݴ�С)
    inline unsigned long base64_code_size(const unsigned long data_size)
    {
        return (data_size+2)/3*4;
    }

    inline unsigned long max_base64_data_size(const unsigned long code_size)
    {
        //assert((code_size/4*4)==code_size);
        return code_size/4*3;
    }
    inline unsigned long min_base64_data_size(const unsigned long code_size)
    {
        unsigned long result=max_base64_data_size(code_size);
        if (result>0)
            return result-2;
        else
            return 0;
    }

    enum B64ReultType{  
        b64Result_OK=0, 
        b64Result_CODE_SIZE_ERROR, 
        b64Result_DATA_SIZE_SMALLNESS,
        b64Result_CODE_ERROR
    };


// 445MB/s
B64ReultType base64_decode(const void* pcode,const unsigned long code_size,void* out_pdata,const unsigned long data_size,unsigned long* out_pwrited_data_size)
{
    const unsigned char DECODE_DATA_MAX = 64-1;
    const unsigned char DECODE_PADDING  = DECODE_DATA_MAX+2;
    const unsigned char DECODE_ERROR    = DECODE_DATA_MAX+3;

    static unsigned char BASE64_DECODE[256];
    static bool initialized=false;
    if(!initialized)
    {
        unsigned long i;
        for(i=0;i<256;++i) BASE64_DECODE[i]=DECODE_ERROR;
        for(i='A';i<='Z';++i) BASE64_DECODE[i]=(unsigned char)(i-'A');
        for(i='a';i<='z';++i) BASE64_DECODE[i]=(unsigned char)(i-'a'+26);
        for(i='0';i<='9';++i) BASE64_DECODE[i]=(unsigned char)(i-'0'+26*2);
        BASE64_DECODE['+']=26*2+10;
        BASE64_DECODE['/']=26*2+10+1;
        BASE64_DECODE['=']=DECODE_PADDING;
        initialized=true;
    }

    *out_pwrited_data_size=0;
    unsigned long code_node=code_size/4;
    if ((code_node*4)!=code_size) 
        return b64Result_CODE_SIZE_ERROR;
    else if (code_node==0) 
        return b64Result_OK;
    //code_node>0

    const unsigned char* input=(const unsigned char*)pcode;
    unsigned char*       output=(unsigned char*)out_pdata;
    unsigned long output_size=code_node*3;
    if (input[code_size-2]==BASE64_PADDING) 
    {
        if (input[code_size-1]!=BASE64_PADDING)
            return b64Result_CODE_ERROR;
        output_size-=2;
    }
    else if (input[code_size-1]==BASE64_PADDING) 
        --output_size;
    if (output_size>data_size) return b64Result_DATA_SIZE_SMALLNESS;

    const unsigned char* input_last_fast_node=&input[output_size/3*4];

    for(;input<input_last_fast_node;input+=4,output+=3)
    {
        unsigned int code0=BASE64_DECODE[input[0]];
        unsigned int code1=BASE64_DECODE[input[1]];
        unsigned int code2=BASE64_DECODE[input[2]];
        unsigned int code3=BASE64_DECODE[input[3]];
        if ( ((code0|code1)|(code2|code3)) <= DECODE_DATA_MAX )
        {
            output[0]=(unsigned char)((code0<<2) + (code1>>4));
            output[1]=(unsigned char)((code1<<4) + (code2>>2));
            output[2]=(unsigned char)((code2<<6) + code3);
        }
        else
            return b64Result_CODE_ERROR;
    }

    unsigned long bord_width=output_size%3;
    if (bord_width==1)
    {
        unsigned int code0=BASE64_DECODE[input[0]];
        unsigned int code1=BASE64_DECODE[input[1]];
        if ((code0|code1) <= DECODE_DATA_MAX)
        {
            output[0]=(unsigned char)((code0<<2) + (code1>>4));
        }
        else
            return b64Result_CODE_ERROR;
    }
    else if (bord_width==2)
    {
        unsigned int code0=BASE64_DECODE[input[0]];
        unsigned int code1=BASE64_DECODE[input[1]];
        unsigned int code2=BASE64_DECODE[input[2]];
        if ((code0|code1|code2) <= DECODE_DATA_MAX)
        {
            output[0]=(unsigned char)((code0<<2) + (code1>>4));
            output[1]=(unsigned char)((code1<<4) + (code2>>2)); 
        }
        else
            return b64Result_CODE_ERROR;
    }
    *out_pwrited_data_size=output_size;
    return b64Result_OK;
}

        bool check_base64(Tbase64_encode_proc base64_encode,const std::vector<unsigned char>& data)
        {
            const long code_size=base64_code_size(data.size());
            std::string code_str;
            code_str.resize(code_size,' ');

            if (data.size()==0) 
                base64_encode(0,data.size(),&code_str[0]);//��&data[0]��STL�ᱨ��û��Ҫ�ɣ��� 
            else
                base64_encode(&data[0],data.size(),&code_str[0]);

            std::vector<unsigned char> data_new(data.size());
            unsigned long data_size_new;
            B64ReultType result;
            if (data.size()==0)
                result=base64_decode(0,code_str.size(),0,data_new.size(),&data_size_new);
            else
                result=base64_decode(&code_str[0],code_str.size(),&data_new[0],data_new.size(),&data_size_new);

            if (result!=b64Result_OK)
                return false;
            if (data_size_new!=data.size())
                return false;
            for (unsigned long i=0;i<data_size_new;++i)
            {
                if (data_new[i]!=data[i])
                    return false;
            }
            return true;
        }
    bool unit_test(Tbase64_encode_proc base64_encode)//������뺯���Ƿ���ȷ
    {
        try
        {
            long test_count=5000; 
            for(long i=0;i<test_count;++i)
            {
                long data_size=rand()%4000;
                std::vector<unsigned char> data_buf(data_size);
                for (long r=0;r<data_size;++r) data_buf[r]=rand();
                if (!check_base64(base64_encode,data_buf))
                    return false;
            }
        }
        catch(...)
        {
            return false;
        }
        return true;
    }


    //��6bit���ݰ�����ӳ����ַ�(6bit����)
    inline unsigned char to_base64char(const unsigned char code6bit)
    {
        if (code6bit<26)         //[ 0..25] => ['A'..'Z']
            return code6bit+'A';
        else if (code6bit<52)    //[26..51] => ['a'..'z']
            return code6bit+('a'-26);
        else if (code6bit<62)    //[52..61] => ['0'..'9']
            return code6bit+('0'-52);
        else if (code6bit==62)   //62 => '+'
            return '+';
        else //if (code6bit==63) //63 => '/'
            return '/';
    }

//���뺯��(ԭ���ݵ�ַ��ԭ�����ֽڴ�С�����������ַ)
void base64_encode0(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    const unsigned char* input=(const unsigned char*)pdata;
    const unsigned char* input_end=&input[data_size];
    unsigned char*       output=(unsigned char*)out_pcode;

    for(;input+2<input_end;input+=3,output+=4)
    {
        output[0]=to_base64char(   input[0] >> 2 );
        output[1]=to_base64char( ((input[0] << 4) | (input[1] >> 4)) & 0x3F );
        output[2]=to_base64char( ((input[1] << 2) | (input[2] >> 6)) & 0x3F );
        output[3]=to_base64char(   input[2] & 0x3F);
    }

    unsigned long bord_width=input_end-input;
    if (bord_width==1)
    {
        output[0]=to_base64char(   input[0] >> 2 );
        output[1]=to_base64char(  (input[0] << 4) & 0x3F );
        output[2]=BASE64_PADDING;
        output[3]=BASE64_PADDING;
    }
    else if (bord_width==2)
    {
        output[0]=to_base64char(   input[0] >> 2 );
        output[1]=to_base64char( ((input[0] << 4) | (input[1] >> 4)) & 0x3F );
        output[2]=to_base64char(  (input[1] << 2) & 0x3F );
        output[3]=BASE64_PADDING;
    }
}



const unsigned char BASE64_CODE[]=
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//ʹ��64�ֽڱ�
void base64_encode0_table(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    const unsigned char* input=(const unsigned char*)pdata;
    const unsigned char* input_end=&input[data_size];
    unsigned char*       output=(unsigned char*)out_pcode;

    for(;input+2<input_end;input+=3,output+=4)
    {
        output[0]=BASE64_CODE[   input[0] >> 2 ];
        output[1]=BASE64_CODE[ ((input[0] << 4) | (input[1] >> 4)) & 0x3F ];
        output[2]=BASE64_CODE[ ((input[1] << 2) | (input[2] >> 6)) & 0x3F ];
        output[3]=BASE64_CODE[   input[2] & 0x3F];
    }

    unsigned long bord_width=input_end-input;
    if (bord_width==1)
    {
        output[0]=BASE64_CODE[   input[0] >> 2 ];
        output[1]=BASE64_CODE[  (input[0] << 4) & 0x3F ];
        output[2]=BASE64_PADDING;
        output[3]=BASE64_PADDING;
    }
    else if (bord_width==2)
    {
        output[0]=BASE64_CODE[   input[0] >> 2 ];
        output[1]=BASE64_CODE[ ((input[0] << 4) | (input[1] >> 4)) & 0x3F ];
        output[2]=BASE64_CODE[  (input[1] << 2) & 0x3F ];
        output[3]=BASE64_PADDING;
    }
}


    inline void base64_addpaing(const unsigned char* input,const unsigned int bord_width,unsigned char* output)
    {
        if (bord_width==1)
        {
            unsigned int input0=input[0];
            unsigned int output0=BASE64_CODE[   input0 >> 2 ];
            unsigned int output1=BASE64_CODE[  (input0 << 4) & 0x3F ];
            *(unsigned long*)(&output[0])=output0 | (output1<<8) | ((BASE64_PADDING<<16) | (BASE64_PADDING<<24)); 
        }
        else if (bord_width==2)
        {
            unsigned int input0=input[0];
            unsigned int input1=input[1];
            unsigned int output0=BASE64_CODE[   input0 >> 2 ];
            unsigned int output1=BASE64_CODE[ ((input0 << 4) | (input1 >> 4)) & 0x3F ];
            unsigned int output2=BASE64_CODE[  (input1 << 2) & 0x3F ];
            *(unsigned long*)(&output[0])=output0 | (output1<<8) | (output2<<16) | (BASE64_PADDING<<24); 
        }
    }

//ʹ��64�ֽڵı�
void base64_encode1(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    const unsigned char* input=(const unsigned char*)pdata;
    const unsigned char* input_end=&input[data_size];
    unsigned char*       output=(unsigned char*)out_pcode;

    for(;input+2<input_end;input+=3,output+=4)
    {
        //unsigned int output0=BASE64_CODE[   input[0] >> 2 ];
        //unsigned int output1=BASE64_CODE[ ((input[0] << 4) | (input[1] >> 4)) & 0x3F ];
        //unsigned int output2=BASE64_CODE[ ((input[1] << 2) | (input[2] >> 6)) & 0x3F ];
        //unsigned int output3=BASE64_CODE[   input[2] & 0x3F ];
        //*(unsigned long*)(&output[0])=output0 | (output1<<8) | (output2<<16) | (output3<<24);
        // ���Բ���һ�������ڴ��ȡ��ʽ���ٶ�

        unsigned int input0=input[0];
        unsigned int input1=input[1];
        unsigned int input2=input[2];
        unsigned int output0=BASE64_CODE[   input0 >> 2 ];
        unsigned int output1=BASE64_CODE[ ((input0 << 4) | (input1 >> 4)) & 0x3F ];
        unsigned int output2=BASE64_CODE[ ((input1 << 2) | (input2 >> 6)) & 0x3F ];
        unsigned int output3=BASE64_CODE[   input2 & 0x3F ];
        *(unsigned long*)(&output[0])=output0 | (output1<<8) | (output2<<16) | (output3<<24);
    }

    base64_addpaing(input,input_end-input,output);
}


//ʹ��64�ֽڵı�
void base64_encode1_bswap(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    const unsigned char* input=(const unsigned char*)pdata;
    const unsigned char* input_end=&input[data_size];
    unsigned char*       output=(unsigned char*)out_pcode;

    for(;input+2<input_end;input+=3,output+=4)
    {
        unsigned long input_all=(input[0]<<16) | (input[1]<<8) | (input[2]);
        //���ָ������һ��BSWAPָ����ֽ�˳�򣬿����ܼӿ�ô�����ٶ�
        unsigned int  output0=BASE64_CODE[   input_all >> 18 ];
        unsigned int  output1=BASE64_CODE[  (input_all >> 12) & 0x3F ];
        unsigned int  output2=BASE64_CODE[  (input_all >>  6) & 0x3F ];
        unsigned int  output3=BASE64_CODE[   input_all & 0x3F ];
        *(unsigned long*)(&output[0])=output0 | (output1<<8) | (output2<<16) | (output3<<24);
    }

    base64_addpaing(input,input_end-input,output);
}



    void  __declspec(naked) __stdcall _base64_encode1_line_asm(const unsigned char* input,unsigned char* output,const long line_count)
    {
        asm
        {
            push    esi
            push    edi
            push    ebx
            push    ebp
            mov  esi,[esp+4+16]//input
            mov  edi,[esp+8+16]//output
            mov  ebp,[esp+12+16]//line_count
            lea  edi,[edi+ebp*4]
            neg  ebp

            align 4
         loop_begin:

                mov     ebx,dword ptr [esi]
                bswap   ebx
                mov     ecx,ebx
                mov     edx,ebx
                mov     eax,ebx
                shr     ecx,14
                shr     edx,8
                shr     eax,26
                and     ecx,0x3F
                shr     ebx,20
                and     edx,0x3F
                and     eax,0x3F
                movzx   ecx, BYTE PTR [BASE64_CODE+ecx]
                and     ebx,0x3F
                mov     ch , BYTE PTR [BASE64_CODE+edx]
                movzx   eax, BYTE PTR [BASE64_CODE+eax]
                shl     ecx,16
                mov     ah , BYTE PTR [BASE64_CODE+ebx]
                
                add     esi,3
                or      ecx,eax
                mov     [edi+ebp*4],ecx    

                add     ebp,1
                jnz   loop_begin          

            pop     ebp
            pop     ebx
            pop     edi
            pop     esi
            ret     12
        }
    }

//ʹ��64�ֽڵı�
void base64_encode1_asm(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    const unsigned char* input=(const unsigned char*)pdata;
    unsigned char*       output=(unsigned char*)out_pcode;

    unsigned long fast_cell=data_size/3;
    if (fast_cell>0)
    {
        unsigned long fast_cell_safe=fast_cell-1;
        if (fast_cell_safe>0)
        {
            _base64_encode1_line_asm(input,output,fast_cell_safe);
            input+=(fast_cell_safe*3);
            output+=(fast_cell_safe*4);
        }
        unsigned long input_all=(input[0]<<16) | (input[1]<<8) | (input[2]);
        unsigned int  output0=BASE64_CODE[   input_all >> 18 ];
        unsigned int  output1=BASE64_CODE[  (input_all >> 12) & 0x3F ];
        unsigned int  output2=BASE64_CODE[  (input_all >>  6) & 0x3F ];
        unsigned int  output3=BASE64_CODE[   input_all & 0x3F ];
        *(unsigned long*)(&output[0])=output0 | (output1<<8) | (output2<<16) | (output3<<24);
        input+=3;
        output+=4;
    }

    base64_addpaing(input,data_size-(fast_cell*3),output);
}


  // |------------------|------------------|------------------|
  // |      a[0..7]     |      b[0..7]     |     c[0..7]      |
  // |------------------|------------------|------------------|
  //
  // |------------------|------------------|------------------|------------------|
  // |      a[0..7]     |b[4..7]+a[0..7]<<4|c[6..7]+b[0..7]<<2|     c[0..7]      |
  // |------------------|------------------|------------------|------------------|
//ʹ��64+256+4096�ֽڵı�(4K+)
void base64_encode2(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    static unsigned char BASE64_CODE_SHIFT2[256];
    static unsigned char BASE64_CODE_EX[1<<12];
    static bool initialized=false;
    if(!initialized)
    {
        unsigned int i;
        for(i=0;i<256;++i)
            BASE64_CODE_SHIFT2[i]=BASE64_CODE[i/4];
        for(i=0;i<(1<<12);++i)
            BASE64_CODE_EX[i]=BASE64_CODE[i%64];
        initialized=true;
    }

    if (data_size<=0) return;
    const unsigned char* input=(const unsigned char*)pdata;
    const unsigned char* input_end=&input[data_size];
    unsigned char*       output=(unsigned char*)out_pcode;
    for(;input+2<input_end;input+=3,output+=4)
    {        
        //output[0]=BASE64_CODE_SHIFT2[input[0]];
        //output[1]=BASE64_CODE_EX[(input[0]<<4) + (input[1]>>4)];
        //output[2]=BASE64_CODE_EX[(input[1]<<2) + (input[2]>>6)];
        //output[3]=BASE64_CODE_EX[input[2]];
        // ���Բ���һ�������ڴ��ȡ��д�뷽ʽ���ٶ�

        unsigned int input0=input[0];
        unsigned int input1=input[1];
        unsigned int input2=input[2];
        unsigned int output0=BASE64_CODE_SHIFT2[input0];
        unsigned int output1=BASE64_CODE_EX[(input0<<4) + (input1>>4)];
        unsigned int output2=BASE64_CODE_EX[(input1<<2) + (input2>>6)];
        unsigned int output3=BASE64_CODE_EX[input2];
        *(unsigned long*)(&output[0])=output0 | (output1<<8) | (output2<<16) | (output3<<24);
    }

    base64_addpaing(input,input_end-input,output);
}


  // |------------------|------------------|------------------|
  // |      a[0..7]     |     b[0..7]      |     c[0..7]      |
  // |------------------|------------------|------------------|
  //
  // |-------------------------------------|-----------------------------------|
  // |     a[0..7]<<4   +     b[4..7]      |    b[0..3]<<8    +    c[0..7]     |
  // |-------------------------------------|-----------------------------------|
//ʹ��4096x2�ֽڵı�(8K)
void base64_encode3(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    static unsigned short BASE64_WCODE[1<<12]; 

    static bool initialized=false; 
    if(!initialized) 
    { 
        for(unsigned int i=0;i<(1<<12);++i) 
        { 
            BASE64_WCODE[i]=BASE64_CODE[i>>6] | (BASE64_CODE[i & 0x3F] << 8);
        } 
        initialized=true; 
    } 

    const unsigned char* input=(const unsigned char*)pdata;
    unsigned char*       output=(unsigned char*)out_pcode;

    long i=data_size/3;
    output+=(i*4);
    for(i=-i;i;input+=3,++i)
    {
        unsigned int input0=input[0];
        unsigned int input1=input[1];
        unsigned int input2=input[2];
        unsigned int output_0_1=BASE64_WCODE[ (input0<<4) | (input1>>4)];
        unsigned int output_2_3=BASE64_WCODE[((input1 & 0x0F)<<8) |  input2]; 
        *(unsigned long*)(&output[i*4])=output_0_1 | (output_2_3<<16);
    }

    base64_addpaing(input,data_size%3,output);
}

void base64_encode3_bswap(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    static unsigned short BASE64_WCODE[1<<12]; 

    static bool initialized=false; 
    if(!initialized) 
    { 
        for(unsigned int i=0;i<(1<<12);++i) 
        { 
            BASE64_WCODE[i]=BASE64_CODE[i>>6] | (BASE64_CODE[i & 0x3F] << 8);
        } 
        initialized=true; 
    } 

    const unsigned char* input=(const unsigned char*)pdata;
    unsigned char*       output=(unsigned char*)out_pcode;

    long i=data_size/3;
    output+=(i*4);
    for(i=-i;i;input+=3,++i)
    {
        unsigned long input_all=(input[0]<<16) | (input[1]<<8) | (input[2]);
        unsigned int output_0_1=BASE64_WCODE[ input_all >> 12 ];
        unsigned int output_2_3=BASE64_WCODE[ input_all & 0xFFF ]; 
        *(unsigned long*)(&output[i*4])=output_0_1 | (output_2_3<<16);
    }

    base64_addpaing(input,data_size%3,output);
}

  // |------------------|------------------|------------------|
  // |      a[0..7]     |     b[0..7]      |     c[0..7]      |
  // |------------------|------------------|------------------|
  //
  // |-------------------------------------|-------------------------------------|
  // |      a[0..7]     +    b[0..7]<<8    |     b[0..7]      +    c[0..7]<<8    |
  // |-------------------------------------|-------------------------------------|
//ʹ��(1<<16)x2x2�ֽڵı�(256K)
void base64_encode_256K(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    static unsigned short BASE64_WCODE_L[1<<16]; 
    static unsigned short BASE64_WCODE_H[1<<16];

    static bool initialized=false; 
    if(!initialized) 
    { 
        for(unsigned long i=0;i<(1<<16);++i) 
        { 
            unsigned int input0=i&0xFF;
            unsigned int input1=(i>>8)&0xFF;
            unsigned int output0=BASE64_CODE[input0>>2];
            unsigned int output1=BASE64_CODE[((input0 & 3)<<4) + (input1>>4)];
            BASE64_WCODE_L[i]=(unsigned short)( output0 | (output1<<8) );

                          input1=i&0xFF;
            unsigned int input2=(i>>8)&0xFF;
            unsigned int output2=BASE64_CODE[((input1 & 0x0F)<<2) + (input2>>6)];
            unsigned int output3=BASE64_CODE[input2 & 0x3F];
            BASE64_WCODE_H[i]=(unsigned short)( output2 | (output3<<8) );
        } 
        initialized=true; 
    } 

    const unsigned char* input=(const unsigned char*)pdata;
    const unsigned char* input_end=&input[data_size];
    unsigned char*       output=(unsigned char*)out_pcode;
    for(;input+2<input_end;input+=3,output+=4)
    {
        //unsigned int input0=input[0];
        //unsigned int input1=input[1];
        //unsigned int input2=input[2];
        //unsigned int output_0_1=BASE64_WCODE_L[input0+(input1<<8)]; 
        //unsigned int output_2_3=BASE64_WCODE_H[input1+(input2<<8)]; 
        //*(unsigned long*)(&output[0])=output_0_1+(output_2_3<<16);
        // ���Բ���һ�������ڴ��ȡ��ʽ���ٶ�

        unsigned int output_0_1=BASE64_WCODE_L[*(unsigned short*)(&input[0])];
        unsigned int output_2_3=BASE64_WCODE_H[*(unsigned short*)(&input[1])]; 
        *(unsigned long*)(&output[0])=output_0_1+(output_2_3<<16);
    }

    base64_addpaing(input,input_end-input,output);
}


  // |----------------------|----------------------|----------------------|
  // |        a[0..7]       |       b[0..7]        |       c[0..7]        |
  // |----------------------|----------------------|----------------------|
  //
  // |--------------------------------------------------------------------|
  // |        a[0..7]       +      b[0..7]<<8      +     c[0..7]<<16      |
  // |--------------------------------------------------------------------|
void base64_encode_64M(const void* pdata,const unsigned long data_size,void* out_pcode)
{

    static unsigned long BASE64_DWCODE[1<<24]; 

    static bool initialized=false; 
    if(!initialized) 
    { 
        for(unsigned long i=0;i<(1<<24);++i) 
        { 
            unsigned int input0=i & 0xFF;
            unsigned int input1=(i>>8) & 0xFF;
            unsigned int input2=(i>>16) & 0xFF;
            unsigned int output0=BASE64_CODE[input0>>2];
            unsigned int output1=BASE64_CODE[((input0 & 3)<<4) + (input1>>4)];
            unsigned int output2=BASE64_CODE[((input1 & 0x0F)<<2) + (input2>>6)];
            unsigned int output3=BASE64_CODE[input2 & 0x3F];
            BASE64_DWCODE[i]=output0 | (output1<<8) | (output2<<16) | (output3<<24);
        } 
        initialized=true; 
    } 

    const unsigned char* input=(const unsigned char*)pdata;
    const unsigned char* input_end=&input[data_size];
    unsigned char*       output=(unsigned char*)out_pcode;
    for(;input+2<input_end;input+=3,output+=4)
    {
        *(unsigned long*)(&output[0])=BASE64_DWCODE[input[0]+(input[1]<<8)+(input[2]<<16)]; 
    }

    base64_addpaing(input,input_end-input,output);
}


    static unsigned short BASE64_WCODE_asm[1<<12];
    void  __declspec(naked) __stdcall _base64_encode3_line_asm(const unsigned char* input,unsigned char* output,const long line_count_AL2)
    {
        asm
        {
            push    esi
            push    edi
            push    ebx
            push    ebp
            mov  esi,[esp+4+16]//input
            mov  edi,[esp+8+16]//output
            mov  ebp,[esp+12+16]//line_count_AL2
            lea  edi,[edi+ebp*4]
            neg  ebp

            align 4
         loop_begin:
                mov     edx,dword ptr [esi]
                mov     ebx,dword ptr [esi+3]
                bswap   edx
                bswap   ebx
                mov     eax,edx
                mov     ecx,ebx
                shr     edx,8
                shr     ebx,8
                shr     eax,20
                shr     ecx,20
                and     edx,0x0FFF
                and     ebx,0x0FFF
               
                movzx   eax,word ptr [BASE64_WCODE_asm+eax*2]
                movzx   edx,word ptr [BASE64_WCODE_asm+edx*2]
                movzx   ebx,word ptr [BASE64_WCODE_asm+ebx*2]
                movzx   ecx,word ptr [BASE64_WCODE_asm+ecx*2]
                shl     edx,16
                shl     ebx,16
                or      edx,eax
                or      ecx,ebx

                mov     [edi+ebp*4],edx    
                mov     [edi+ebp*4+4],ecx 

                add     esi,3*2
                add     ebp,2
                jnz   loop_begin            

            pop     ebp
            pop     ebx
            pop     edi
            pop     esi
            ret     12
        }
    }

void base64_encode3_asm(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    static bool initialized=false; 
    if(!initialized) 
    { 
        for(unsigned int i=0;i<(1<<12);++i) 
        { 
            BASE64_WCODE_asm[i]=BASE64_CODE[i>>6] | (BASE64_CODE[i & 0x3F] << 8);
        } 
        initialized=true; 
    } 

    const unsigned char* input=(const unsigned char*)pdata;
    unsigned char*       output=(unsigned char*)out_pcode;

    unsigned long fast_cell=data_size/3;
    if (fast_cell>0)
    {
        unsigned long fast_cell_AL2=((fast_cell-1)>>1)<<1; 
        //Ԥ��һ��cell��֤_base64_encode3_line_asm�ڴ���ʰ�ȫ������ʹcell��С2������

        if (fast_cell_AL2>0)
        {
            _base64_encode3_line_asm(input,output,fast_cell_AL2);
            input+=(fast_cell_AL2*3);
            output+=(fast_cell_AL2*4);
        }
        for(unsigned long i=fast_cell_AL2;i<fast_cell;++i,input+=3,output+=4)
        {
            unsigned int input0=input[0];
            unsigned int input1=input[1];
            unsigned int input2=input[2];
            unsigned int output_0_1=BASE64_WCODE_asm[(input0<<4) | (input1>>4)];
            unsigned int output_2_3=BASE64_WCODE_asm[((input1&0x0F)<<8) | input2];
            *(unsigned long*)(&output[0])=output_0_1 | (output_2_3<<16);
        }
    }

    base64_addpaing(input,data_size-(fast_cell*3),output);
}

    void  __declspec(naked) __stdcall _base64_encode3_line_sse(const unsigned char* input,unsigned char* output,const long line_count_AL2)
    {
        asm
        {
            push    esi
            push    edi
            push    ebx
            push    ebp
            mov  esi,[esp+4+16]//input
            mov  edi,[esp+8+16]//output
            mov  ebp,[esp+12+16]//line_count_AL2
            lea  edi,[edi+ebp*4]
            neg  ebp

            align 4
         loop_begin:
                mov     edx,dword ptr [esi]
                mov     ebx,dword ptr [esi+3]
                bswap   edx
                bswap   ebx
                mov     eax,edx
                mov     ecx,ebx
                shr     edx,8
                shr     ebx,8
                shr     eax,20
                shr     ecx,20
                and     edx,0x0FFF
                and     ebx,0x0FFF
               
                movzx   eax,word ptr [BASE64_WCODE_asm+eax*2]
                movzx   edx,word ptr [BASE64_WCODE_asm+edx*2]
                movzx   ebx,word ptr [BASE64_WCODE_asm+ebx*2]
                movzx   ecx,word ptr [BASE64_WCODE_asm+ecx*2]
                shl     edx,16
                shl     ebx,16
                or      edx,eax
                or      ecx,ebx

                //��ͨд��ָ��
                //mov   [edi+ebp*4],edx    
                //mov   [edi+ebp*4+4],ecx 

                //sse֧�ֵ�д��ָ��
                MOVD        MM0,edx
                MOVD        MM1,ecx
                PUNPCKlDQ   MM0,MM1
                MOVNTQ      [edi+ebp*4],MM0   


                add     esi,3*2
                add     ebp,2
                jnz   loop_begin            

            SFENCE //д��ˢ��
            EMMS   //mmx�Ĵ���ʹ�ý���

            pop     ebp
            pop     ebx
            pop     edi
            pop     esi
            ret     12
        }
    }

void base64_encode3_sse(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    static bool initialized=false; 
    if(!initialized) 
    { 
        for(unsigned int i=0;i<(1<<12);++i) 
        { 
            BASE64_WCODE_asm[i]=BASE64_CODE[i>>6] | (BASE64_CODE[i & 0x3F] << 8);
        } 
        initialized=true; 
    } 

    const unsigned char* input=(const unsigned char*)pdata;
    unsigned char*       output=(unsigned char*)out_pcode;

    unsigned long fast_cell=data_size/3;
    if (fast_cell>0)
    {
        unsigned long fast_cell_AL2=((fast_cell-1)>>1)<<1; 
        //Ԥ��һ��cell��֤_base64_encode3_line_sse�ڴ���ʰ�ȫ������ʹcell��С2������

        if (fast_cell_AL2>0)
        {
            _base64_encode3_line_sse(input,output,fast_cell_AL2);
            input+=(fast_cell_AL2*3);
            output+=(fast_cell_AL2*4);
        }
        for(unsigned long i=fast_cell_AL2;i<fast_cell;++i,input+=3,output+=4)
        {
            unsigned int input0=input[0];
            unsigned int input1=input[1];
            unsigned int input2=input[2];
            unsigned int output_0_1=BASE64_WCODE_asm[(input0<<4) | (input1>>4)];
            unsigned int output_2_3=BASE64_WCODE_asm[((input1&0x0F)<<8) | input2];
            *(unsigned long*)(&output[0])=output_0_1 | (output_2_3<<16);
        }
    }

    base64_addpaing(input,data_size-(fast_cell*3),output);
}


    void  __declspec(naked) __stdcall _base64_encode3_line_sse2(const unsigned char* input,unsigned char* output,const long line_count_AL2)
    {
        asm
        {
            push    esi
            push    edi
            push    ebx
            push    ebp
            mov  esi,[esp+4+16]//input
            mov  edi,[esp+8+16]//output
            mov  ebp,[esp+12+16]//line_count_AL2
            lea  edi,[edi+ebp*4]
            neg  ebp

            align 4
         loop_begin:
                mov     edx,dword ptr [esi]
                mov     ebx,dword ptr [esi+3]
                bswap   edx
                bswap   ebx
                mov     eax,edx
                mov     ecx,ebx
                shr     edx,8
                shr     ebx,8
                shr     eax,20
                shr     ecx,20
                and     edx,0x0FFF
                and     ebx,0x0FFF
               
                movzx   eax,word ptr [BASE64_WCODE_asm+eax*2]
                movzx   edx,word ptr [BASE64_WCODE_asm+edx*2]
                movzx   ebx,word ptr [BASE64_WCODE_asm+ebx*2]
                movzx   ecx,word ptr [BASE64_WCODE_asm+ecx*2]
                shl     edx,16
                shl     ebx,16
                or      edx,eax
                or      ecx,ebx

                //��ͨд��ָ��
                //mov   [edi+ebp*4],edx    
                //mov   [edi+ebp*4+4],ecx 

                //sse2֧�ֵ�д��ָ��
                MOVNTI   [edi+ebp*4],edx    
                MOVNTI   [edi+ebp*4+4],ecx 


                add     esi,3*2
                add     ebp,2
                jnz   loop_begin            

            SFENCE //д��ˢ��

            pop     ebp
            pop     ebx
            pop     edi
            pop     esi
            ret     12
        }
    }

void base64_encode3_sse2(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    static bool initialized=false; 
    if(!initialized) 
    { 
        for(unsigned int i=0;i<(1<<12);++i) 
        { 
            BASE64_WCODE_asm[i]=BASE64_CODE[i>>6] | (BASE64_CODE[i & 0x3F] << 8);
        } 
        initialized=true; 
    } 

    const unsigned char* input=(const unsigned char*)pdata;
    unsigned char*       output=(unsigned char*)out_pcode;

    unsigned long fast_cell=data_size/3;
    if (fast_cell>0)
    {
        unsigned long fast_cell_AL2=((fast_cell-1)>>1)<<1; 
        //Ԥ��һ��cell��֤_base64_encode3_line_sse2�ڴ���ʰ�ȫ������ʹcell��С2������

        if (fast_cell_AL2>0)
        {
            _base64_encode3_line_sse2(input,output,fast_cell_AL2);
            input+=(fast_cell_AL2*3);
            output+=(fast_cell_AL2*4);
        }
        for(unsigned long i=fast_cell_AL2;i<fast_cell;++i,input+=3,output+=4)
        {
            unsigned int input0=input[0];
            unsigned int input1=input[1];
            unsigned int input2=input[2];
            unsigned int output_0_1=BASE64_WCODE_asm[(input0<<4) | (input1>>4)];
            unsigned int output_2_3=BASE64_WCODE_asm[((input1&0x0F)<<8) | input2];
            *(unsigned long*)(&output[0])=output_0_1 | (output_2_3<<16);
        }
    }

    base64_addpaing(input,data_size-(fast_cell*3),output);
}


    void  __declspec(naked) __stdcall _base64_encode3_line_sse2_prefetch(const unsigned char* input,unsigned char* output,const long line_count)
    {
        asm
        {
            push    esi
            push    edi
            push    ebx
            push    ebp

            mov  esi,[esp+4+16]//input
            mov  edi,[esp+8+16]//output
            mov  ebp,[esp+12+16]//line_count
            lea  edi,[edi+ebp*4]
            neg  ebp

            align 4
         loop_begin:
                mov     edx,dword ptr [esi]
                mov     ebx,dword ptr [esi+3]
                bswap   edx
                bswap   ebx
                prefetchnta [esi+64*4*2]
                mov     eax,edx
                mov     ecx,ebx
                shr     edx,8
                shr     ebx,8
                shr     eax,20
                shr     ecx,20
                and     edx,0x0FFF
                and     ebx,0x0FFF
               
                movzx   eax,word ptr [BASE64_WCODE_asm+eax*2]
                movzx   edx,word ptr [BASE64_WCODE_asm+edx*2]
                movzx   ebx,word ptr [BASE64_WCODE_asm+ebx*2]
                movzx   ecx,word ptr [BASE64_WCODE_asm+ecx*2]
                shl     edx,16
                shl     ebx,16
                or      edx,eax
                or      ecx,ebx

                //sse2֧�ֵ�д��ָ��
                MOVNTI   [edi+ebp*4],edx    
                MOVNTI   [edi+ebp*4+4],ecx 


                add     esi,3*2
                add     ebp,2
                jnz   loop_begin            

            pop     ebp
            pop     ebx
            pop     edi
            pop     esi
            ret     12
        }
    }

//ʹ��CPU��ʽprefetchntaԤ��ָ��
void base64_encode3_sse2_prefetch(const void* pdata,const unsigned long data_size,void* out_pcode)
{
    static bool initialized=false; 
    if(!initialized) 
    { 
        for(unsigned int i=0;i<(1<<12);++i) 
        { 
            BASE64_WCODE_asm[i]=BASE64_CODE[i>>6] | (BASE64_CODE[i & 0x3F] << 8);
        } 
        initialized=true; 
    } 

    const unsigned char* input=(const unsigned char*)pdata;
    unsigned char*       output=(unsigned char*)out_pcode;

    unsigned long fast_cell=data_size/3;
    if (fast_cell>0)
    {
        unsigned long fast_cell_AL2=((fast_cell-1)>>1)<<1; 
        //Ԥ��һ��cell��֤_base64_encode3_line_sse2�ڴ���ʰ�ȫ������ʹcell��С2������

        if (fast_cell_AL2>0)
        {
            _base64_encode3_line_sse2_prefetch(input,output,fast_cell_AL2);
            input+=(fast_cell_AL2*3);
            output+=(fast_cell_AL2*4);
        }
        for(unsigned long i=fast_cell_AL2;i<fast_cell;++i,input+=3,output+=4)
        {
            unsigned int input0=input[0];
            unsigned int input1=input[1];
            unsigned int input2=input[2];
            unsigned int output_0_1=BASE64_WCODE_asm[(input0<<4) | (input1>>4)];
            unsigned int output_2_3=BASE64_WCODE_asm[((input1&0x0F)<<8) | input2];
            *(unsigned long*)(&output[0])=output_0_1 | (output_2_3<<16);
        }
    }

    base64_addpaing(input,data_size-(fast_cell*3),output);

    asm SFENCE //д��ˢ��
}


#include "WorkThreadPool.h"

struct TBase64WorkData
{
    Tbase64_encode_proc  base64_encode_proc;
    const unsigned char* input;
    unsigned long         input_size;
    unsigned char*       output;
};

void base64_encode_part_callback(void* wd)
{
    TBase64WorkData* WorkData=(TBase64WorkData*)wd;
    WorkData->base64_encode_proc(WorkData->input,WorkData->input_size,WorkData->output);
}

void parallel_base64_encode(Tbase64_encode_proc base64_encode,const void* pdata,const unsigned long data_size,void* out_pcode)
{
    const unsigned char* input=(const unsigned char*)pdata;
    unsigned char*       output=(unsigned char*)out_pcode;

    long work_count=CWorkThreadPool::best_work_count();
    std::vector<TBase64WorkData>   work_list(work_count);
    std::vector<TBase64WorkData*>  pwork_list(work_count);

    unsigned long part_cell_count=data_size/3/work_count;

    for (long i=0;i<work_count;++i)
    {
        work_list[i].base64_encode_proc=base64_encode;
        work_list[i].input=input;
        work_list[i].output=output;
        work_list[i].input_size=part_cell_count*3;

        pwork_list[i]=&work_list[i];
        input+=part_cell_count*3;
        output+=part_cell_count*4;
    }
    work_list[work_count-1].input_size=data_size-part_cell_count*3*(work_count-1);
    CWorkThreadPool::work_execute(base64_encode_part_callback,(void**)&pwork_list[0],work_count);   
}



 //���Ա����ٶ�(���������ƣ����뺯��������ԭ���ݴ�С)
 void testSpeed(const char* proc_name_str,Tbase64_encode_proc base64_encode,const long DATA_SIZE,const bool is_parallel=false)
 {
    std::cout<<">> ���뺯��: "<<proc_name_str<<std::endl;
   //*
    if (!unit_test(base64_encode))
    {
        std::cout<<std::endl<<" ERROR! ���뺯��\""<<proc_name_str<<"\"û��ͨ����ȷ����֤��"<<std::endl;
        std::cout<<std::endl;
        return;
    }//*/
    if (is_parallel)
        std::cout<<"  ( "<<CWorkThreadPool::best_work_count()<<"· ����ִ�� ) "<<std::endl;

    const long DATA_SIZE_MAX=DATA_SIZE+12; 

    std::vector<unsigned char> data_buf(DATA_SIZE_MAX); //data_buf������Ҫ���������
    for (long r=0;r<DATA_SIZE_MAX;++r) 
        data_buf[r]=rand(); //data_buf���������� ���Բ���

    const long code_size_MAX=base64_code_size(DATA_SIZE_MAX);
    std::string code_str;//code_str���Դ���������ַ�������
    code_str.resize(code_size_MAX,' ');

    long   RunCount=0;
    double SumSpeed=0;
    for (long data_size=DATA_SIZE;data_size<DATA_SIZE_MAX;++data_size)
    { 

        const long code_size=base64_code_size(data_size);
        double start_time=(double)clock();

        if (is_parallel)
            parallel_base64_encode(base64_encode,&data_buf[0],data_size,&code_str[0]);
        else
            base64_encode(&data_buf[0],data_size,&code_str[0]);//�������

        double run_time=((double)clock()-start_time)*(1.0/CLOCKS_PER_SEC);

        double encode_speed=code_size*(1.0/1024/1024)/run_time;//�����ٶ�(MB/��)
        ++RunCount;
        SumSpeed+=encode_speed;

        //std::cout<<" ����ǰ���ݴ�С(MB): "<<data_size*(1.0/1024/1024)<<"   �����ٶ�(MB/��): "<<encode_speed<<std::endl;
        //if (data_size<=1000) std::cout<<code_str<<std::endl; //
    }
    std::cout<<"    ƽ�������ٶ�(MB/��): "<<SumSpeed/RunCount<<std::endl;

}


int main()
{
    std::cout<<" �����������ַ���ʼ����(���԰ѽ������ȼ�����Ϊ��ʵʱ��)> ";
    getchar();
    std::cout<<std::endl;

    const long DATA_SIZE=200*1024*1024; //�������ݴ�С
    
    //*
    testSpeed("base64_encode0"          ,base64_encode0         ,DATA_SIZE/8);      
    testSpeed("base64_encode0_table"    ,base64_encode0_table   ,DATA_SIZE/4);      
    testSpeed("base64_encode1"          ,base64_encode1         ,DATA_SIZE/2);      
    testSpeed("base64_encode1_bswap"    ,base64_encode1_bswap   ,DATA_SIZE/2);      
    //testSpeed("base64_encode3_bswap"    ,base64_encode3_bswap   ,DATA_SIZE);    //û�ܳ���base64_encode3����������ϵCPU��Ҳ��᲻��  base64_encode3_asm������Ľ�
    testSpeed("base64_encode_256K"      ,base64_encode_256K     ,DATA_SIZE/2);      
    testSpeed("base64_encode_64M"       ,base64_encode_64M      ,DATA_SIZE/8);      
 
    testSpeed("base64_encode1_asm"      ,base64_encode1_asm     ,DATA_SIZE/2);      
    testSpeed("base64_encode2"          ,base64_encode2         ,DATA_SIZE);        
    testSpeed("base64_encode3"          ,base64_encode3         ,DATA_SIZE);        
    testSpeed("base64_encode3_asm"      ,base64_encode3_asm     ,DATA_SIZE);        
    testSpeed("base64_encode3_sse"      ,base64_encode3_sse     ,DATA_SIZE);        
    testSpeed("base64_encode3_sse2"     ,base64_encode3_sse2    ,DATA_SIZE);        
    testSpeed("base64_encode3_sse2_prefetch",base64_encode3_sse2_prefetch,DATA_SIZE);

    //*///��·����ִ�� ����:
    testSpeed("base64_encode1_asm"      ,base64_encode1_asm     ,DATA_SIZE,true);   
    testSpeed("base64_encode2"          ,base64_encode2         ,DATA_SIZE,true);   
    testSpeed("base64_encode3"          ,base64_encode3         ,DATA_SIZE,true);   
    testSpeed("base64_encode3_asm"      ,base64_encode3_asm     ,DATA_SIZE,true);   
    testSpeed("base64_encode3_sse"      ,base64_encode3_sse     ,DATA_SIZE,true);   
    testSpeed("base64_encode3_sse2"     ,base64_encode3_sse2    ,DATA_SIZE,true);   
    testSpeed("base64_encode3_sse2_prefetch",base64_encode3_sse2_prefetch,DATA_SIZE,true); 
    //*/
}
