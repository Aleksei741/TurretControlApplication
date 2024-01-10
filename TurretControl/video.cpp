#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"

#include "main.h"
#include "ControlSend.h"
#include <Windows.h>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>

#define MAX_PKT_SIZE 1500
#define H264_PAYLOAD_TYPE 96

struct sockaddr_in servaddr;
int sockfd;

int init_udp_socket() {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Could not create socket");
        exit(-1);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5000); // Set the port number for RTP transmission
    servaddr.sin_addr.s_addr = htons(INADDR_ANY); // Receive any address
    if (bind(sockfd, (struct sockaddr*)&amp; servaddr, sizeof(servaddr)) < 0) {
        printf("Could not bind socket\
");
        exit(-1);
    }
    return 0;
}

int recv_pkt(char* buf, int max_len) {
    int length = 0;
    struct sockaddr_in recvaddr;
    socklen_t recvlen = sizeof(recvaddr);
    length = recvfrom(sockfd, buf, max_len, 0, (struct sockaddr*)&amp; recvaddr, &amp; recvlen);
    if (length < 0) {
        printf("Packet receiving error");
        exit(-1);
    }
    return length;
}

int main(int argc, char* argv[])
{
    AVCodecContext* codec_ctx = NULL;
    AVCodec* codec = NULL;
    AVPacket* pkt = NULL;
    int ret;
    int64_t last_pts = AV_NOPTS_VALUE;
    int64_t last_time = 0;

    /* Initialize AVFormatContext */
    av_register_all();

    /* Get codec parameters */
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (codec == NULL) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find decoder");
        return AVERROR_DECODER_NOT_FOUND;
    }
    codec_ctx = avcodec_alloc_context3(codec);
    if (codec_ctx == NULL) {
        av_log(NULL, AV_LOG_ERROR, "Cannot allocate codec context");
        return AVERROR(ENOMEM);
    }
    if ((ret = avcodec_open2(codec_ctx, codec, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open codec");
        return ret;
    }

    /* Allocate packet */
    pkt = av_packet_alloc();
    if (pkt == NULL) {
        av_log(NULL, AV_LOG_ERROR, "Cannot allocate packet");
        return AVERROR(ENOMEM);
    }

    init_udp_socket();

    /* Initialize frame and buffer */
    AVFrame* frame = av_frame_alloc();
    if (frame == NULL) {
        av_log(NULL, AV_LOG_ERROR, "Cannot allocate frame");
        return AVERROR(ENOMEM);
    }
    uint8_t* buffer = NULL;
    int buffer_size;
    int bpp;
    bpp = av_get_bytes_per_pixel(AV_PIX_FMT_RGB24);
    buffer_size = codec_ctx->width * codec_ctx->height * bpp;
    ret = av_image_alloc(buffer, buffer_size, codec_ctx->width, codec_ctx->height, AV_PIX_FMT_RGB24, 1);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot allocate image");
        return ret;
    }

    AVFormatContext* ofmt_ctx = NULL;
    AVStream* out_stream = NULL;
    /* Initialize output context */
    if (avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, "output.mp4") < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot allocate output format context");
        return -1;
    }
    out_stream = avformat_new_stream(ofmt_ctx, NULL);
    avcodec_parameters_from_context(out_stream->codecpar, codec_ctx);
    avio_open(&ofmt_ctx->pb, "output.mp4", AVIO_FLAG_WRITE);
    avformat_write_header(ofmt_ctx, NULL);

    SwsContext* img_convert_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
        codec_ctx->width, codec_ctx->height, AV_PIX_FMT_RGB24,
        SWS_BICUBIC, NULL, NULL, NULL);

    while (1) {
        unsigned char buf[MAX_PKT_SIZE];
        int pkt_size = recv_pkt(buf, MAX_PKT_SIZE);
        if ((buf[0] & amp; 0x1F) == H264_PAYLOAD_TYPE) { // Determine whether it is an H.264 video stream
            pkt->data = buf + 12; // RTP header is 12 bytes
            pkt->size = pkt_size - 12;
            /* Send packet to decoder */
            if ((ret = avcodec_send_packet(codec_ctx, pkt)) < 0) {
                av_log(NULL, AV_LOG_ERROR, "Cannot send packet to decoder");
                break;
            }
            while (1) 
            {
                ret = avcodec_receive_frame(codec_ctx, frame);
                if (ret < 0) {
                    break;
                }

                int delayed_frames = (frame->pts - last_pts) * av_q2d(codec_ctx->time_base) * 1000;
                int64_t cur_time = av_gettime_relative() / 1000;
                if (last_time + delayed_frames > cur_time) {
                    av_usleep((last_time + delayed_frames - cur_time) * 1000);
                    cur_time = av_gettime_relative() / 1000;
                }
                last_pts = frame->pts;
                last_time = cur_time;

                sws_scale(img_convert_ctx, frame->data, frame->linesize, 0, codec_ctx->height, buffer, frame->width * bpp);

                /* Write the frame to the file */
                AVPacket out_pkt;
                av_init_packet(&out_pkt);
                out_pkt.stream_index = out_stream->index;
                out_pkt.data = buffer;
                out_pkt.size = buffer_size;
                av_write_frame(ofmt_ctx, &out_pkt);
            }
        }
    }
    avcodec_free_context(&codec_ctx);
    av_packet_free(&pkt);
    av_frame_free(&amp; frame);
    av_free(buffer);
    sws_freeContext(img_convert_ctx);
    av_write_trailer(ofmt_ctx);
    avformat_close_input(&ofmt_ctx);
    avformat_free_context(ofmt_ctx);

    return 0;
}