/**
 * C++ course project.
 * Copyright (C) 2012, Artem Oboturov, Ludwig Brummer 
 */
#ifndef M2MO_IMAGE_PROJECT_H
#define M2MO_IMAGE_PROJECT_H

#include <iostream>
#include <cstdio>

using namespace std;

enum {RED = 0, GREEN, BLUE, FIRST = RED, LAST = BLUE, LAYER_CNT = 3}; 

typedef unsigned char img_color_t;
typedef int img_color_layer_t;
typedef int img_coord_t;
typedef int img_size_t;

class Image;

class Block
{
private:
	Image&		img;
	img_coord_t	left;
	img_coord_t	top;
	img_size_t	height;
	img_size_t	width;
public:
	Block(Image& image, const img_coord_t left, const img_coord_t top, const img_size_t height, const img_size_t width);
	// Compare blocks with the Mean Square Error proximity measure and returns a value in [0,1].
	float mse_divergence(const Block& rhs) const;
};

class Image
{
private:
	img_color_t*	data;
	img_size_t	width;
	img_size_t	height;

	void load(string filename);
public: 
	Image(string filename);
	Image(img_size_t width, img_size_t height);
	Image(Image &img);
	~Image();
	img_size_t get_width();
	img_size_t get_height();

	void save(string filename);

	void flip_horizontally();

	img_color_t & operator()(img_coord_t x, img_coord_t y, img_color_layer_t layer);
};

void fill_in_with_color(Image &img, const img_color_t color);

#endif
