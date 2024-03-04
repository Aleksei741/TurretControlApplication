//******************************************************************************
//include
//******************************************************************************
#include "main.h"
/*#include "video_H264_decode.h"
#include <Windows.h>
#include <strsafe.h>
#include <chrono>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>*/
#include <iostream>
#include <queue>
#include <set>

extern "C" {
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libpostproc/postprocess.h"
#include "libavcodec/avcodec.h"
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"postproc.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")

#pragma warning(disable: 4996)
//******************************************************************************
// Section for determining the variables used in the module
//******************************************************************************
//------------------------------------------------------------------------------
// Global
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local
//------------------------------------------------------------------------------
const int inteval = 0x01000000;
BOOL FlagRTPActive = TRUE;

HANDLE hMutexQPacket;
HANDLE hMutexRTPRecive;
HANDLE hSemaphoreRTP;
HANDLE hTreadRTPRecive;
HANDLE hTreadRTPDecode;


SOCKET RTPSocket; //socket UDP RTP
//RTPData_DType packet;
RTPData_DType FU_buffer = { 0 };

std::queue<RTPData_DType> q;
std::queue<RTPData_DType> QPacket;
std::set<int> seq;

AVFormatContext* pAVFormatContext;
AVCodecContext* pAVCodecContext;
const AVCodec* pAVCodec;
AVFrame* pAVFrame;
AVFrame* AVFrameRGG;
SwsContext* pSwsContext;
AVPacket *pAVPacket;
AVDictionary *dict = NULL;
AVCodecParserContext* pAVCodecParserContext;
//******************************************************************************
// Section of prototypes of local functions
//******************************************************************************
DWORD WINAPI rtp_H264_recive_Procedure(CONST LPVOID lpParam);
DWORD WINAPI rtp_decode_Procedure(CONST LPVOID lpParam);
char RTPSocketInit(void);
void RTPPacketParser(unsigned char* buffer, int buffer_size);
char RTPSocketRecive(void);
void Decode_NaluToAVFrameRGG();
//******************************************************************************
// Section of the description of functions
//******************************************************************************
UCHAR rtp_H264_recive_init(void)
{
    hSemaphoreRTP = CreateSemaphore(
        NULL,           // default security attributes
        0,				// initial count
        1,				// maximum count
        NULL);          // unnamed semaphore

    hMutexRTPRecive = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

    hMutexQPacket = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

    hTreadRTPRecive = CreateThread(NULL, NULL, rtp_H264_recive_Procedure, NULL, NULL, NULL);
    hTreadRTPDecode = CreateThread(NULL, NULL, rtp_decode_Procedure, NULL, NULL, NULL);

    return 0;
}
//------------------------------------------------------------------------------
UCHAR RTPStop(void)
{
    FlagRTPActive = FALSE;

    if (hSemaphoreRTP) CloseHandle(hSemaphoreRTP);
    if (hMutexRTPRecive) CloseHandle(hMutexRTPRecive);
    if (hTreadRTPRecive) CloseHandle(hTreadRTPRecive);

    closesocket(RTPSocket);  

    return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI rtp_H264_recive_Procedure(CONST LPVOID lpParam)
{
    UINT port = param.VideoOption.VideoPort; //

    while (RTPSocketInit() == 0)
        Sleep(2000);
       
    while (1)
    {
        RTPSocketRecive();

        if (!FlagRTPActive)
            break;
        
        ReleaseSemaphore(hSemaphoreRTP, 1, NULL);
    }

    return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI rtp_decode_Procedure(CONST LPVOID lpParam)
{
    RTPData_DType packet;
    uint8_t* packed_metadata;
    size_t metadata_len;

    pAVPacket = av_packet_alloc();
    if (!pAVPacket)
    {
        MessageBox(NULL, L"ERROR Could not allocate pAVPacket", L"Init decoder error", MB_OK | MB_ICONERROR);
        exit(1);
    }
    av_init_packet(pAVPacket);

    /* find the MPEG-1 video decoder */
    pAVCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!pAVCodec)
    {
        MessageBox(NULL, L"ERROR Codec not found", L"Init decoder error", MB_OK | MB_ICONERROR);
        exit(1);
    }

    pAVCodecParserContext = av_parser_init(pAVCodec->id);
    if (!pAVCodecParserContext)
    {
        MessageBox(NULL, L"ERROR Parser not found", L"Init decoder error", MB_OK | MB_ICONERROR);
        exit(1);
    }

    av_dict_set(&dict, "tune", "zerolatency", 0);
    av_dict_set(&dict, "preset", "ultrafast", 0);
    //packed_metadata = av_packet_pack_dictionary(dict, &metadata_len);
    //av_packet_add_side_data(pAVPacket, AV_PKT_DATA_STRINGS_METADATA, packed_metadata, metadata_len);

    pAVCodecContext = avcodec_alloc_context3(pAVCodec);
    if (!pAVCodecContext) 
    {
        MessageBox(NULL, L"ERROR Could not allocate video codec context", L"Init decoder error", MB_OK | MB_ICONERROR);
        exit(1);
    }
               
    if (avcodec_open2(pAVCodecContext, pAVCodec, &dict) < 0)
    {
        MessageBox(NULL, L"ERROR Could not open codec", L"Init decoder error", MB_OK | MB_ICONERROR);
        exit(1);
    }

    pAVFrame = av_frame_alloc();
    if (!pAVFrame) 
    {
        MessageBox(NULL, L"ERROR Could not allocate video frame", L"Init decoder error", MB_OK | MB_ICONERROR);
        exit(1);
    }    
    
    while (FlagRTPActive)
    {
        while (1)
        {
            WaitForSingleObject(hMutexRTPRecive, INFINITE);
            if (QPacket.empty())
            {
                ReleaseMutex(hMutexRTPRecive);
                break;                
            }
            packet = QPacket.front();
            QPacket.pop();
            ReleaseMutex(hMutexRTPRecive);

            RTPPacketParser(packet.data, packet.size);
            free(packet.data);
            
        }
        Decode_NaluToAVFrameRGG();
    }

    //av_freep(&packed_metadata);
    av_dict_free(&dict);
    avformat_free_context(pAVFormatContext);
    av_frame_free(&pAVFrame);
    avcodec_close(pAVCodecContext);
    av_packet_free(&pAVPacket);

    
    if (hTreadRTPDecode) CloseHandle(hTreadRTPDecode);

    return 0;
}

//------------------------------------------------------------------------------
char RTPSocketInit(void)
{    
    sockaddr_in RTPSocketAddr;
    
    RTPSocketAddr.sin_family = AF_INET;
    RTPSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    RTPSocketAddr.sin_port = htons(param.VideoOption.VideoPort);

    RTPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (RTPSocket == INVALID_SOCKET)
    {
        MessageBox(NULL, L"ERROR Invalid RTP Socket", L"UDP Socket", MB_OK | MB_ICONERROR);
        return 0;
    }

    int VideoOption = 200000;
    if (setsockopt(RTPSocket, SOL_SOCKET, SO_RCVBUF, (char*)&VideoOption, sizeof(VideoOption)) < 0)
    {
        printf("setsockopt failed\n");
    }

    VideoOption = TRUE;
    if (setsockopt(RTPSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (char*)&VideoOption, sizeof(VideoOption)) < 0)
    {
        printf("setsockopt failed\n");
    }

    if (bind(RTPSocket, (sockaddr*)&RTPSocketAddr, sizeof(RTPSocketAddr)) == SOCKET_ERROR)
    {
        MessageBox(NULL, L"ERROR bind", L"UDP Socket", MB_OK | MB_ICONERROR);
        closesocket(RTPSocket);
        return 0;
    }
    return 1;
}

//------------------------------------------------------------------------------
char RTPSocketRecive(void)
{
    RTPData_DType packet;
    static char RecvBuf[2000];
    static int BufLen = 2000;

    int iResult = 0;
    struct sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof(SenderAddr);
    TCHAR szErrorMsg[100];

    iResult = recvfrom(RTPSocket, RecvBuf, BufLen, 0, NULL, NULL);
    if(iResult == SOCKET_ERROR)
        return -1;
    else if (iResult == SOCKET_ERROR && FlagRTPActive == TRUE)
    {
        StringCchPrintf(szErrorMsg, 100, L"ERROR recvfrom Ошибка:%d", WSAGetLastError());
        MessageBox(NULL, szErrorMsg, L"UDP Socket", MB_OK | MB_ICONERROR);
        return -1;
    }
    
    packet.data = (unsigned char*)malloc(iResult);
    memcpy(packet.data, &RecvBuf, iResult);
    packet.size = iResult;

    WaitForSingleObject(hMutexRTPRecive, INFINITE);
    QPacket.push(packet);
    ReleaseMutex(hMutexRTPRecive);

    return 1;
}

//------------------------------------------------------------------------------
void RTPPacketParser(unsigned char* buffer, int buffer_size)
{   
    RTPHeader_DType RTPHeader;    
    RTPData_DType NALU;
    
    int pos = 0;
    static int count = 0;
    static short lastSequenceNumber = 0xFFFF;
    short type;
    char payload_header;

    //read rtp header
    RTPHeader.V = buffer[0] >> 6;
    RTPHeader.P = buffer[0] >> 5;
    RTPHeader.X = buffer[0] >> 4;
    RTPHeader.CC = buffer[0];

    RTPHeader.M = buffer[1] >> 7;
    RTPHeader.PT = buffer[1];
    
    RTPHeader.sequence_number = buffer[2] << 8 | buffer[3];    
    if (seq.count(RTPHeader.sequence_number)) 
        return;
    seq.insert(RTPHeader.sequence_number);
    if (seq.size() > 1000)
    {
        seq.clear();
    }

    RTPHeader.time_stamp = buffer[4] << 24 | buffer[5] << 16 | buffer[6] << 8 | buffer[7];
    RTPHeader.SSRC = buffer[8] << 24 | buffer[9] << 16 | buffer[10] << 8 | buffer[11] ;
    pos += 12;

    if (RTPHeader.X) {
        //profile extension
        short define;
        short length;
        length = buffer[pos + 3];//suppose not so long extension
        pos += 4;
        pos += (length * 4);
    }

    if (RTPHeader.PT == 96)
    {
        payload_header = buffer[pos];
        type = payload_header & 0x1f; //Тип полезной нагрузки RTP
        pos++;


        //STAP-A
        if (type == 24)
        {
            while (pos < buffer_size)
            {
                unsigned short NALU_size;
                memcpy(&NALU_size, buffer + pos, 2);
                NALU_size = BYTE2_SWAP(NALU_size);
                pos += 2;
                char NAL_header = buffer[pos];
                short NAL_type = NAL_header & 0x1f;

                if (NAL_type == 7)
                {
                    count++;
                    //cout<<"SPS, sequence number: "<<seq_num<<endl;
                }
                else if (NAL_type == 8)
                {
                    //cout<<"PPS, sequence number: "<<seq_num<<endl;
                }
                else if (NAL_type == 10)
                {
                    //cout<<"end of sequence, sequence number: "<<seq_num<<endl;
                }

                if (count > 0)
                {
                    NALU.data = (unsigned char*)malloc(NALU_size + 4);
                    NALU.size = NALU_size + 4;
                    memcpy(NALU.data, &inteval, 4);
                    memcpy(NALU.data + 4, &buffer[pos], NALU_size);

                    //WaitForSingleObject(hMutexRTPRecive, INFINITE);
                    q.push(NALU);
                    //ReleaseMutex(hMutexRTPRecive);
                }

                pos += NALU_size;
            }
        }
        //FU-A Fragmentation unit
        else if (type == 28)
        {
            //FU header
            char FU_header = buffer[pos];
            bool fStart = FU_header & 0x80;
            bool fEnd = FU_header & 0x40;

            //NAL header
            char NAL_header = (payload_header & 0xe0) | (FU_header & 0x1f);
            short NAL_type = FU_header & 0x1f;
            if (NAL_type == 7)
            {
                count++;
                //SPS
            }
            else if (NAL_type == 8)
            {
                //PPS
            }
            else if (NAL_type == 10)
            {
                //end of sequence
            }
            pos++;

            int size = buffer_size - pos;

            if (count > 0)
            {
                if (fStart)
                {
                    if (FU_buffer.size != 0)
                    {
                        free(FU_buffer.data);
                        FU_buffer.size = 0;
                    }
                    FU_buffer.data = (unsigned char*)malloc(size + 5);
                    if (FU_buffer.data == NULL)
                        return;
                    FU_buffer.size = size + 5;
                    memcpy(FU_buffer.data, &inteval, 4);
                    memcpy(FU_buffer.data + 4, &NAL_header, 1);
                    memcpy(FU_buffer.data + 5, buffer + pos, size);
                }
                else
                {
                    unsigned char* temp = (unsigned char*)malloc(FU_buffer.size + size);
                    memcpy(temp, FU_buffer.data, FU_buffer.size);
                    memcpy(temp + FU_buffer.size, buffer + pos, size);
                    if (FU_buffer.size != 0) free(FU_buffer.data);
                    FU_buffer.data = temp;
                    FU_buffer.size += size;
                }

                if (fEnd)
                {
                    NALU.data = (unsigned char*)malloc(FU_buffer.size);
                    NALU.size = FU_buffer.size;
                    memcpy(NALU.data, FU_buffer.data, FU_buffer.size);

                    //WaitForSingleObject(hMutexRTPRecive, INFINITE);
                    q.push(NALU);
                    //ReleaseMutex(hMutexRTPRecive);

                    free(FU_buffer.data);
                    FU_buffer.size = 0;
                }
            }

        }
        else
        {
            //other type
            short NAL_type = type;
            if (NAL_type == 7)
            {
                count++;
                //SPS
            }
            else if (NAL_type == 8)
            {
                //PPS
            }
            else if (NAL_type == 10)
            {
                //end of sequence
            }

            int size = buffer_size - pos + 1;
            if (count > 0)
            {
                NALU.data = (unsigned char*)malloc(size + 4);
                NALU.size = size + 4;
                memcpy(NALU.data, &inteval, 4);
                memcpy(NALU.data + 4, &buffer[12], size);

                //WaitForSingleObject(hMutexRTPRecive, INFINITE);
                q.push(NALU);
                //ReleaseMutex(hMutexRTPRecive);
            }
        }
    }
    else
    {
        RTPHeader.PT = 0;
        return;
    }
}

//------------------------------------------------------------------------------
void Decode_NaluToAVFrameRGG()
{
    unsigned char cntNALU;
    int len = 0;
    static int size = 0;
    unsigned char* data = NULL;
    int ret;
    RTPData_DType NALU;
    
    av_frame_unref(pAVFrame);
    av_packet_unref(pAVPacket);

    while(1)
    {
        //WaitForSingleObject(hMutexRTPRecive, INFINITE);
        if (q.empty())
        {
            //ReleaseMutex(hMutexRTPRecive);
            break;
        }
        NALU = q.front();
        q.pop();
        //ReleaseMutex(hMutexRTPRecive);

        data = NALU.data;
        size = NALU.size;

        while(size)
        {                        
            len = av_parser_parse2(pAVCodecParserContext, pAVCodecContext, &pAVPacket->data, &pAVPacket->size,
                (uint8_t*)data, size,
                AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

            data = len ? data + len : data;
            size -= len;
            
            if (pAVPacket->size)
            {
                ret = avcodec_send_packet(pAVCodecContext, pAVPacket);

                if(ret == AVERROR_EOF)
                    MessageBox(NULL, L"the codec has been fully flushed, and there will be no more output frames", L"avcodec_send_packet", MB_OK | MB_ICONERROR);

                if (ret < 0)
                {
                    MessageBox(NULL, L"ERROR sending a packet for decoding", L"Decode", MB_OK | MB_ICONERROR);
                    //exit(1);
                }

                while (ret >= 0) 
                {
                    ret = avcodec_receive_frame(pAVCodecContext, pAVFrame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    else if (ret < 0) {
                        MessageBox(NULL, L"ERROR during decoding", L"Decode", MB_OK | MB_ICONERROR);
                        //exit(1);
                    }
                    
                    AVFrameRGG = av_frame_alloc();                                       
                    //pSwsContext = sws_getContext(pAVCodecContext->width, pAVCodecContext->height, pAVCodecContext->pix_fmt,
                    pSwsContext = sws_getContext(pAVCodecContext->width, pAVCodecContext->height, (AVPixelFormat) pAVFrame->format,
                        pAVCodecContext->width, pAVCodecContext->height, AV_PIX_FMT_RGB24, SWS_SPLINE,
                        0, 0, 0);

                    sws_scale_frame(pSwsContext, AVFrameRGG, pAVFrame);
                                        
                    ImgBufferSet(AVFrameRGG->data[0], pAVCodecContext->height, pAVCodecContext->width, AVFrameRGG->linesize[0]);

                    sws_freeContext(pSwsContext);
                    av_frame_free(&AVFrameRGG);
                }
            }
        }
        free(NALU.data);
        NALU.size = 0;
    }   
    
}

