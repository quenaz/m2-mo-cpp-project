#include "image.h"

#include <jpeglib.h>
#include <cstdlib>
#include <algorithm>

using namespace std;

Image::Image(img_size_t width, img_size_t height)
{
	this->width = width;
	this->height = height;
	data = new img_color_t[LAYER_CNT * width * height];
	for(int i=0; i < LAYER_CNT*width*height; i++)
		data[i] = 0;
}

Image::Image(string filename)
{
	load(filename);
}

Image::~Image()
{
	delete[] data;
}

Image::Image(Image &img)
{
	this->width = img.width;
	this->height = img.height;
	this->data = new img_color_t[LAYER_CNT*width*height];
	for(int i=0; i < LAYER_CNT*width*height; i ++)
		this->data[i] = img.data[i];
}

int Image::get_width()
{
	return width;
}

int Image::get_height()
{
	return height;
}


void Image::load(string filename)
{
	FILE *infile;
	struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;
	JSAMPARRAY buffer;

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);

        if ((infile = fopen(filename.c_str(), "rb")) == NULL) 
	{
            cout << "Can't open " << filename << endl;
            exit(EXIT_FAILURE);
        }

	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);

	width = cinfo.image_width;
	height = cinfo.image_height;
	
	// TODO: this is a memory leak: if already initialized.
	data = new img_color_t[LAYER_CNT*width*height];

	if(data == NULL)
	{
		cout << "Error in Memory allocation. " << endl;
		exit(EXIT_FAILURE);
	}

	jpeg_start_decompress(&cinfo);

	int i = 0;

	buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, width*LAYER_CNT, 1);

	while (cinfo.output_scanline < cinfo.output_height) 
	{
		jpeg_read_scanlines(&cinfo, buffer, 1);

		for(int j=0; j < LAYER_CNT*width; j++)
			data[(cinfo.output_scanline-1)*LAYER_CNT*width + j] = (*buffer)[j];
	}
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);
}

void Image::save(string filename)
{
	FILE *outfile;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer;          

	if((outfile = fopen(filename.c_str(), "wb")) == NULL)
	{
		cout << "Can't open " << filename << " for saving image." << endl;
		exit(EXIT_FAILURE);
        }

 
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);
  
	cinfo.image_width      = width;
	cinfo.image_height     = height;
	cinfo.input_components = LAYER_CNT;
	cinfo.in_color_space   = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality (&cinfo, 96, true);	  /*set the quality [0..100]  */
	jpeg_start_compress(&cinfo, true);

  
	while (cinfo.next_scanline < cinfo.image_height) 
	{
		row_pointer = (JSAMPROW) &data[cinfo.next_scanline*width*LAYER_CNT];
		jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);
}

img_color_t & Image::operator()(img_coord_t x, img_coord_t y, img_color_layer_t layer)
{
	return data[y*LAYER_CNT*width + x*LAYER_CNT + layer];
}

void Image::flip_horizontally()
{
	for(int y=0; y < height / 2; y ++)
		for(int x = 0; x < width; x ++)
			for(img_color_layer_t c =0; c < LAYER_CNT; c++)
			{
				img_color_t d = (*this)(x,y, c);
				(*this)(x,y, c) = (*this)(x,height-y-1, c);
				(*this)(x,height-y-1, c) = d;
			}
}

Block::Block(Image& iimage, const img_coord_t lleft, const img_coord_t ttop, const img_size_t hheight, const img_size_t wwidth)
	: img(iimage), left(lleft), top(ttop), height(hheight), width(wwidth)
{ }

// The MSE implementation.
float Block::mse_proximity(const Block& rhs) const {
	// Determine top/bottom coordinates of blocks in images.
	img_coord_t	lhs_t = this->top,
			lhs_b = min(this->top+this->height, this->img.get_height()),
			rhs_t = rhs.top,
			rhs_b = min(rhs.top+rhs.height, rhs.img.get_height());
	// Determine left/right coordinates of blocks in images.
	img_coord_t	lhs_l = this->left,
			lhs_r = min(this->left+this->width, this->img.get_width()),
			rhs_l = rhs.left,
			rhs_r = min(rhs.left+rhs.width, rhs.img.get_width());

	float score = .0f;
	for (; lhs_t <= lhs_b && rhs_t <= rhs_b; lhs_t++, rhs_t++)
		for (; lhs_l <= lhs_r && rhs_l <= rhs_r; lhs_l++, rhs_l++)
			for (int layer = FIRST; layer < LAYER_CNT; layer++) {
				int colors_diff = ((this->img)(lhs_l, lhs_t, layer)-(rhs.img)(rhs_l, rhs_t, layer))/255.f;
				score += colors_diff*colors_diff;
			}
	return score;
}

