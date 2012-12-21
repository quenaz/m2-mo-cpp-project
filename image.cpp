#include "image.h"

#include <jpeglib.h>

#include <cstdlib>
#include <algorithm>
#include <cstring>

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

int Image::get_width() const
{
	return width;
}

int Image::get_height() const
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

img_color_t& Image::operator()(img_coord_t x, img_coord_t y, img_color_layer_t layer) const
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

void fill_in_with_color(Image &img, const img_color_t color)
{
	img_size_t	height = img.get_height(),
			width = img.get_width();
	for(img_coord_t y =0; y < height; y++)
		for(img_coord_t x = 0; x < width; x++)
			for(img_color_layer_t layer =0; layer < LAYER_CNT; layer++)
			{
				img(x, y, layer) = color;
			}
}

inline DivergenceMeasure::~DivergenceMeasure() {};

float MseDivergence::compute(const Block& lhs, const Block& rhs) const
{
        // Determine top/bottom coordinates of blocks in images.
        const img_coord_t     lhs_t = lhs.get_top(),
                        lhs_b = min(lhs.get_top()+lhs.get_height(), lhs.get_img()->get_height()),
                        rhs_t = rhs.get_top(),
                        rhs_b = min(rhs.get_top()+rhs.get_height(), rhs.get_img()->get_height());
        // Determine left/right coordinates of blocks in images.
        const img_coord_t     lhs_l = lhs.get_left(),
                        lhs_r = min(lhs.get_left()+lhs.get_width(), lhs.get_img()->get_width()),
                        rhs_l = rhs.get_left(),
                        rhs_r = min(rhs.get_left()+rhs.get_width(), rhs.get_img()->get_width());
        long samplesCounter = 0l;
        float score = .0f;
		for (img_coord_t lt = lhs_t, rt = rhs_t; lt < lhs_b && rt < rhs_b; lt++, rt++)
            for (img_coord_t ll = lhs_l, rl = rhs_l; ll < lhs_r && rl < rhs_r; ll++, rl++)
                        for (int layer = FIRST; layer < LAYER_CNT; layer++) {
                                int colors_diff = ((*(lhs.get_img()))(ll, lt, layer)-(*(rhs.get_img()))(rl, rt, layer))/255.f;
                                score += colors_diff*colors_diff;
                                samplesCounter++;
                        }
        score /= samplesCounter;
        return 1.f - score;
}

float MeanColor::compute(const Block& lhs, const Block& rhs) const
{
        // Determine top/bottom coordinates of blocks in images.
        const img_coord_t     lhs_t = lhs.get_top(),
                        lhs_b = min(lhs.get_top()+lhs.get_height(), lhs.get_img()->get_height()),
                        rhs_t = rhs.get_top(),
                        rhs_b = min(rhs.get_top()+rhs.get_height(), rhs.get_img()->get_height());
        // Determine left/right coordinates of blocks in images.
        const img_coord_t     lhs_l = lhs.get_left(),
                        lhs_r = min(lhs.get_left()+lhs.get_width(), lhs.get_img()->get_width()),
                        rhs_l = rhs.get_left(),
                        rhs_r = min(rhs.get_left()+rhs.get_width(), rhs.get_img()->get_width());
        long samplesCounter = 0l;
	long meanColors[2][LAYER_CNT];
	memset(meanColors, 0, sizeof(long) * 2 * LAYER_CNT);
		for (img_coord_t lt = lhs_t, rt = rhs_t; lt < lhs_b && rt < rhs_b; lt++, rt++)
            for (img_coord_t ll = lhs_l, rl = rhs_l; ll < lhs_r && rl < rhs_r; ll++, rl++)
                for (int layer = FIRST; layer < LAYER_CNT; layer++) {
				meanColors[0][layer] += (*(lhs.get_img()))(ll, lt, layer);
				meanColors[1][layer] += (*(rhs.get_img()))(rl, rt, layer);
				samplesCounter++;
                        }
	float score = 1.f, tmp_score;
	for (int layer = FIRST; layer < LAYER_CNT; layer++) {
		tmp_score = meanColors[0][layer]/samplesCounter/255.f;
		if (tmp_score < score) score = tmp_score;
		tmp_score = meanColors[1][layer]/samplesCounter/255.f;
		if (tmp_score < score) score = tmp_score;
	}
	return score;
}

Block::Block(const Image* iimage, const img_coord_t ttop, const img_coord_t lleft, const img_size_t hheight, const img_size_t wwidth)
	: img(iimage), top(ttop), left(lleft), height(hheight), width(wwidth)
{ }


void Block::copy_content(Block& to_copy)
{
	Block& lhs = *this, rhs = to_copy;
       // Determine top/bottom coordinates of blocks in images.
        const img_coord_t     lhs_t = lhs.get_top(),
                        lhs_b = min(lhs.get_top()+lhs.get_height(), lhs.get_img()->get_height()),
                        rhs_t = rhs.get_top(),
                        rhs_b = min(rhs.get_top()+rhs.get_height(), rhs.get_img()->get_height());
		// Determine left/right coordinates of blocks in images.
        const img_coord_t     lhs_l = lhs.get_left(),
                        lhs_r = min(lhs.get_left()+lhs.get_width(), lhs.get_img()->get_width()),
                        rhs_l = rhs.get_left(),
                        rhs_r = min(rhs.get_left()+rhs.get_width(), rhs.get_img()->get_width());
		for (img_coord_t lt = lhs_t, rt = rhs_t; lt < lhs_b && rt < rhs_b; lt++, rt++)
                for (img_coord_t ll = lhs_l, rl = rhs_l; ll < lhs_r && rl < rhs_r; ll++, rl++)
                        for (int layer = FIRST; layer < LAYER_CNT; layer++) {
							(*lhs.get_img())(ll, lt, layer) = (*(rhs.get_img()))(rl, rt, layer);
                        }
}

Image::iterator Image::begin(const img_size_t height, const img_size_t width)
{
	return iterator(this, height, width);
}

Image::iterator Image::end(const img_size_t height, const img_size_t width)
{
	iterator it = iterator(this, height, width);
	return it.end();
}

Image& Image::scale_to(Image& downscaled) const
{
	img_size_t	desiredw = downscaled.get_width(),
				desiredh = downscaled.get_height();

	float paceh = height/desiredh;
	float pacew = width/desiredw;
	float norm = paceh*pacew;
	int y = 0;// pixel positions for big image
	int x = 0;
	for(int i = 0; i < desiredw; i++)
	{
		x=i*pacew;
		for(int j = 0; j < desiredh; j++)
		{
		y = j*paceh;
			for(img_color_layer_t layer = 0; layer < LAYER_CNT; layer++)
			{
				float color = 0;
				for (int cnth = 0; cnth < int(paceh); cnth++)
				{
					for (int cntw = 0; cntw < int(pacew); cntw++)
						color += (float)(*this)(x + cntw, y + cnth, layer);
				}
				downscaled(i, j, layer) = color/norm;
			}
		}	
	}
	return downscaled;
}

BlockIterator::BlockIterator(Image* iimage, const img_size_t hheight, const img_size_t wwidth)
	: img(iimage)
	, block_height(hheight)
	, block_width(wwidth)
	, cur_height_offset(0)
	, cur_width_offset(0)
	, is_at_the_end(false)
	, cur_block(Block(iimage, 0, 0, hheight, wwidth))
	, end_block(Block(NULL, -1, -1, -1, -1 ))
{ }

BlockIterator& BlockIterator::end()
{
	cur_block = end_block;
	is_at_the_end = true;
	return *this;
}

Block& BlockIterator::operator*()
{
	if (is_at_the_end) throw reached_end();

	return cur_block;
}

Block* BlockIterator::operator->()
{
	if (is_at_the_end) throw reached_end();

	return &cur_block;
}

BlockIterator& BlockIterator::operator++() // prefix
{
	if (is_at_the_end) throw reached_end();

	if (cur_width_offset + block_width < img->get_width()) // can shift window one right
	{
		cur_width_offset += block_width;
	}
	else
	{
		cur_width_offset = 0;
		if (cur_height_offset + block_height < img->get_height()) // can shift window one bottom
		{
			cur_height_offset += block_height;
		}
		else // reached the end
		{
			is_at_the_end = true;
		}
	}
	if (!is_at_the_end)
	{
		cur_block = Block(img, cur_height_offset, cur_width_offset, block_height, block_width);
	}	
	else
	{
		cur_block = end_block;
	}	
	return *this;
}

bool BlockIterator::operator==(const BlockIterator& rhs) const
{
	if (is_at_the_end == true && rhs.is_at_the_end == true)
	{
		return true;
	}
	return	(
			img == rhs.img &&
			block_height == rhs.block_height &&
			block_width == rhs.block_width &&
			cur_height_offset == rhs.cur_height_offset &&
			cur_width_offset == rhs.cur_width_offset &&
			is_at_the_end == rhs.is_at_the_end
		);
}

bool BlockIterator::operator!=(const BlockIterator& rhs) const
{
	if ((is_at_the_end || rhs.is_at_the_end) && (is_at_the_end != rhs.is_at_the_end))
	{
		return true;
	}
	if (is_at_the_end && rhs.is_at_the_end)
	{
		return false;
	}
	return	!(
			img == rhs.img &&
			block_height == rhs.block_height &&
			block_width == rhs.block_width &&
			cur_height_offset == rhs.cur_height_offset &&
			cur_width_offset == rhs.cur_width_offset
		);
}

void ImageLibrary::reload(string inputfolder)
{
DIR *dp;
	struct dirent *dirp;

	if((dp  = opendir(inputfolder.c_str())) == NULL) 
	{
	        cout << "Error opening " << inputfolder << endl;
        	return;
	}

	while ((dirp = readdir(dp)) != NULL) 
	{
		string s(dirp->d_name);
		if(s.length() > 4)
		{
			cout <<"Loading " << inputfolder + s << " to library... ";
			cout.flush();
			Image* i = new Image(inputfolder + s);
			images.push_back(i);
			cout << "done." << endl;
		}		
	}

	closedir(dp);

}

int ImageLibrary::size()
{
	return images.size();
}

ImageLibrary::storage::const_iterator ImageLibrary::begin()
{
	return images.begin();
}
ImageLibrary::storage::const_iterator ImageLibrary::end()
{
	return images.end();
} 

ImageLibrary::~ImageLibrary()
{
	for (ImageLibrary::storage::const_iterator constIt = begin(); constIt != end(); ++constIt)
	{
		(*constIt)->~Image();
	}
}
