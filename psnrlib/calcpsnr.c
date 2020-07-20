#include <libavutil/motion_vector.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include "extractfts.h"
#include "calcpsnr.h"

static const char *filename = NULL;

int calc_bitrate_qpi(float *fbitrate, float* fqp1)
{
	if (filename == NULL) return -1;
	AVFormatContext 	*ic = NULL;
	AVCodec 			*decoder = NULL;
	AVCodecContext 		*dx = NULL;
	AVStream			*video = NULL;
    AVFrame             *frame = NULL;
	int 				ret, video_stream;
	AVPacket			pkt = { 0 };
	ret = -1;

	*fbitrate = 0.0;
	*fqp1 = 0.0;

	if (avformat_open_input(&ic, filename, NULL, NULL) != 0) {
		fprintf(stderr, "Cannot open input file '%s'\n", filename);
		return ret;
	}

	if (avformat_find_stream_info(ic, NULL) < 0) {
		fprintf(stderr, "Cannot find input stream information.\n");
		return ret;
	}

	/* find the video stream information */
	ret = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
	if (ret < 0) {
		fprintf(stderr, "Cannot find a video stream in the input file\n");
		return ret;
	}

	video_stream = ret;
	video = ic->streams[video_stream];

	if (!(dx = avcodec_alloc_context3(decoder))) return AVERROR(ENOMEM);
	if (avcodec_parameters_to_context(dx, video->codecpar) < 0) return ret;

	if ((ret = avcodec_open2(dx, decoder, NULL)) < 0) {
		fprintf(stderr, "Failed to open codec for stream #%u\n", video_stream);
		return -1;
	}
	frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate frame\n");
		ret = AVERROR(ENOMEM);
		return -1;
	}


	int cur_gop = -1;
	int cur_pos = 0;

	while (av_read_frame(ic, &pkt) >= 0) {

		if (pkt.stream_index == video_stream)
		{
			//ret = decode_packet(&pkt);
			ret = avcodec_send_packet(dx, &pkt);
			if (ret < 0) {
				fprintf(stderr, "Error while sending a packet to the decoder: %s\n", av_err2str(ret));
				return ret;
			}

			while (ret >= 0) {
				ret = avcodec_receive_frame(dx, frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
					ret = 0;
					break;
				}
				else if (ret < 0) {
					fprintf(stderr, "Error while receiving a frame from the decoder: %s\n", av_err2str(ret));
					return ret;
				}
				if (frame->pict_type == AV_PICTURE_TYPE_I) {
					++cur_gop;

					AVFrameSideData *sd = 0;
					H264Context *hcontext = 0;
					H264Picture *pic = 0;
					hcontext = (H264Context*)dx->priv_data;
					if(hcontext)
						pic = hcontext->next_output_pic;

					if (ret >= 0 && hcontext && hcontext->mb_width * hcontext->mb_height > 0) {
						int i;

						int h = frame->height;
						int w = frame->width;

						int nsum = 0;

						if (pic && pic->qscale_table)
						{
							for (int j = 0; j < hcontext->mb_height; j++) {
								int8_t* prow = pic->qscale_table + j * hcontext->mb_stride;

								for (int i = 0; i < hcontext->mb_width; i++) {
									nsum += prow[i];
								}
							}
						}

						*fqp1 = (float)nsum / (float)(hcontext->mb_width * hcontext->mb_height);
						break;
					}
				}

				cur_pos++;
				av_frame_unref(frame);
			}

			if (*fqp1 > 0.0) break;
		}
	}

	*fbitrate = dx->bit_rate / (float)1000.0;


	avcodec_free_context(&dx);
	avformat_close_input(&ic);
	av_frame_free(&frame);

	return 0;
}


float getbitrate(const char* fname)
{
	
	if (fname == 0) return 0.0;
	filename = fname;

	if (filename == NULL) return -1;
	AVFormatContext 	*ic = NULL;
	AVCodec 			*decoder = NULL;
	AVCodecContext 		*dx = NULL;
	AVStream			*video = NULL;
	int 				ret, video_stream;	
	ret = -1;

	if (avformat_open_input(&ic, filename, NULL, NULL) != 0) {
		fprintf(stderr, "Cannot open input file '%s'\n", filename);
		return ret;
	}

	if (avformat_find_stream_info(ic, NULL) < 0) {
		fprintf(stderr, "Cannot find input stream information.\n");
		return ret;
	}

	/* find the video stream information */
	ret = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
	if (ret < 0) {
		fprintf(stderr, "Cannot find a video stream in the input file\n");
		return ret;
	}

	video_stream = ret;
	video = ic->streams[video_stream];

	if (!(dx = avcodec_alloc_context3(decoder))) return AVERROR(ENOMEM);
	if (avcodec_parameters_to_context(dx, video->codecpar) < 0) return ret;

	if ((ret = avcodec_open2(dx, decoder, NULL)) < 0) {
		fprintf(stderr, "Failed to open codec for stream #%u\n", video_stream);
		return -1;
	}

	float fbitrate = dx->bit_rate / (float)1000.0;

	avcodec_free_context(&dx);
	avformat_close_input(&ic);	

	return fbitrate;
}

float getqpi(const char* fname)
{
	int ret = -1;
	if (fname == 0) return 0.0;
	filename = fname;

	float bitrate, qpi;
	bitrate = 0.0;
	qpi = 0.0;

	calc_bitrate_qpi(&bitrate, &qpi);

	return qpi;
}

float calc_norefpsnr(char* fname)
{
    if (fname == 0) return 0.0;
	filename = fname;

    float bitrate, qpi, fpsnr;
	bitrate = 0.0;
	qpi = 0.0;

	int ret = calc_bitrate_qpi(&bitrate, &qpi);
    if(ret == 0)
        fpsnr = 74.791 - 2.215 * log10(bitrate) - 0.975 * qpi + 0.00001708 * bitrate * qpi;
    else 
        fpsnr = 0.0;
    return fpsnr;
}