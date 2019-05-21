
#include "H264.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h" 
};
#include "opencv2/opencv.hpp"

AVCodec* pCodec = NULL;
AVCodecContext* pCodecCtx = NULL;
SwsContext* img_convert_ctx = NULL;
AVFrame* pFrame = NULL;
AVFrame* pFrameBGR = NULL;

int H264_Init(void)
{
	/* register all the codecs */
	avcodec_register_all();

	/* find the h264 video decoder */
	pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!pCodec) {
		fprintf(stderr, "codec not found\n");
	}
	pCodecCtx = avcodec_alloc_context3(pCodec);

	//��ʼ������������Ĳ���Ӧ���ɾ����ҵ�����  
	//pCodecCtx->time_base.num = 1;
	//pCodecCtx->frame_number = 1; //ÿ��һ����Ƶ֡  
	//pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	//pCodecCtx->bit_rate = 0;
	//pCodecCtx->time_base.den = 30;//֡��  
	//pCodecCtx->width = 960;//��Ƶ��  
	//pCodecCtx->height = 544;//��Ƶ��  

	/* open the coderc */
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		fprintf(stderr, "could not open codec\n");
	}
	// Allocate video frame  
	pFrame = av_frame_alloc();
	if (pFrame == NULL)
		return -1;
	// Allocate an AVFrame structure  
	pFrameBGR = av_frame_alloc();
	if (pFrameBGR == NULL)
		return -1;
	return 0;

}

static int dumpCount = 0;
static int first = 0;
static uint8_t * out_buffer = NULL;
int H264_2_RGB(char* inputbuf, int frame_size, unsigned char* outputbuf, unsigned int* outsize)
{

	int             decode_size;
	int             numBytes;
	int             av_result;
	uint8_t* buffer = NULL;

	printf("Video decoding\n");
	int ret, got_picture;
	AVPacket packet;
	av_init_packet(&packet);
	packet.size = frame_size;
	packet.data = (uint8_t*)inputbuf;

	ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
	if (ret < 0)
	{
		printf("Decode Error. ret = %d���������\n", ret);
		return -1;
	}

	if (first == 0)
	{
		out_buffer = (uint8_t*)av_malloc(avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height));
		avpicture_fill((AVPicture*)pFrameBGR, out_buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);

		first = 1;
	}

	struct SwsContext* img_convert_ctx;
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

	sws_scale(img_convert_ctx, (const uint8_t * const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameBGR->data, pFrameBGR->linesize);

	memcpy(outputbuf, pFrameBGR->data[0], pCodecCtx->width * pCodecCtx->height * 3);
	*outsize = pCodecCtx->width * pCodecCtx->height * 3;

	return 0;
}

void H264_Release(void)
{
	avcodec_close(pCodecCtx);
	av_free(pCodecCtx);
	av_free(pFrame);
	av_free(pFrameBGR);
}
int main(int argc, char* argv[])
{
	OpenBitstreamFile("../bytes_write.h264");
	NALU_t* nal;
	char fName[300];
	int Frame = 0;
	nal = AllocNALU(8000000);//Ϊ�ṹ��nalu_t�����Աbuf����ռ䡣����ֵΪָ��nalu_t�洢�ռ��ָ��

	H264_Init();

	unsigned char* outputbuf = (unsigned char*)calloc(1000 * 1000, sizeof(char));
	unsigned int outsize = 0;

	unsigned char* m_pData = (unsigned char*)calloc(1000 * 1000, sizeof(char));

	int sizeHeBing = 0;
	while (!feof(getFile()))
	{
		GetAnnexbNALU(nal);//ÿִ��һ�Σ��ļ���ָ��ָ�򱾴��ҵ���NALU��ĩβ��
						   //��һ��λ�ü�Ϊ�¸�NALU����ʼ��0x000001
		dump(nal);//���NALU���Ⱥ�TYPE

		sprintf(fName, "dump[Len=%d][%d].txt", nal->len, Frame);

		memset(m_pData, 0, 4);
		m_pData[3] = 1;
		memcpy(m_pData + 4, nal->buf, nal->len);
		sizeHeBing = nal->len + 4;
		Frame++;

		int ret = H264_2_RGB((char*)m_pData, sizeHeBing, outputbuf, &outsize);
		if (ret != 0)
			continue;

		cv::Mat  image = cv::Mat(pCodecCtx->height, pCodecCtx->width, CV_8UC3);

		memcpy(image.data, outputbuf, pCodecCtx->height * pCodecCtx->width * 3);

		cv::imshow("xxx", image);

		cv::waitKey(40);



		//DumpChar(fName, nal->buf, nal->len);

		//Sleep(33);
	}

	FreeNALU(nal);
	return 0;
}
