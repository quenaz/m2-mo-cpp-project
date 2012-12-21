/**
 * C++ course project.
 * Copyright (C) 2012, Artem Oboturov, Ludwig Brummer 
 */
#ifndef M2MO_IMAGE_PROJECT_H
#define M2MO_IMAGE_PROJECT_H

#include <iostream>
#include <cstdio>
#include <vector>

#include <dirent.h>

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
	const Image*	img;
	img_coord_t	left;
	img_coord_t	top;
	img_size_t	height;
	img_size_t	width;
public:
	Block(const Image* image, const img_coord_t top, const img_coord_t left, const img_size_t height, const img_size_t width);
	// Compare blocks with the Mean Square Error proximity measure and returns a value in [0,1].
	float mse_divergence(const Block& rhs) const;
};

class reached_end {};

class BlockIterator
{
private:
	Image* img;
	img_size_t block_height;
	img_size_t block_width;
	img_size_t cur_height_offset;
	img_size_t cur_width_offset;
	bool is_at_the_end;
	Block cur_block;
	Block end_block;
public:
	BlockIterator(Image* image, const img_size_t block_height, const img_size_t block_width);
	BlockIterator& end();
	Block& operator*();
	Block* operator->();
	BlockIterator& operator++();
	bool operator==(const BlockIterator&) const;
	bool operator!=(const BlockIterator&) const;
};

class Image
{
private:
	img_color_t*	data;
	img_size_t	width;
	img_size_t	height;

	void load(string filename);
public: 
	typedef BlockIterator iterator;

	Image(string filename);
	Image(img_size_t width, img_size_t height);
	Image(Image &img);
	~Image();
	img_size_t get_width() const;
	img_size_t get_height() const;

	iterator begin(const img_size_t height, const img_size_t width);
	iterator end(const img_size_t height, const img_size_t width);

	void save(string filename);

	void flip_horizontally();

	img_color_t& operator()(img_coord_t x, img_coord_t y, img_color_layer_t layer) const;
	Image& scale_to(Image& downscaled) const; 
};

void fill_in_with_color(Image &img, const img_color_t color);

class DirectoryListing;

class DirectoryListingIterator
{
	friend class DirectoryListing;
	DIR *open_dir;
	struct dirent *cur_file;
	string cur_file_name;
	string folder;
	static DirectoryListingIterator it_to_end;
public:
	string& operator*();
	DirectoryListingIterator& operator++();
	bool operator==(const DirectoryListingIterator&) const;
	bool operator!=(const DirectoryListingIterator&) const;
private:
	DirectoryListingIterator(string& dir) throw (exception);
	DirectoryListingIterator() : open_dir(NULL), cur_file(NULL) {}
};

class DirectoryListing
{
public:
	typedef DirectoryListingIterator iterator;
private:
	string folder;
public:
	DirectoryListing(string& dir);
	iterator begin();
	iterator end();
};

class ImageLibrary
{
public:
	typedef vector<Image*> storage;
private:
	storage images;
public:
	// Reload images from the folder "libraryImages"
	void reload(string inputfolder);
	storage::const_iterator begin();
	storage::const_iterator end();
	int size();
	~ImageLibrary();
};
#endif
