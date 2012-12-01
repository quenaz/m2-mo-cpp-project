#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE M2MO_IMAGE_PROJECT_IMAGE_TEST

#include <boost/test/unit_test.hpp>

#include "image.h"

BOOST_AUTO_TEST_CASE(check_image_height_width_constructor)
{
	img_size_t square_sz = 20;
	Image	empty(square_sz, square_sz);

	// After initialization all color values must be equal to zero.
	for (img_size_t row = 0; row < square_sz; row++)
		for (img_size_t col = 0; col < square_sz; col++)
			for (img_color_layer_t layer = FIRST; layer < LAYER_CNT; layer++) {
				BOOST_REQUIRE_EQUAL(empty(col, row, layer), 0);
			}
}

BOOST_AUTO_TEST_CASE(check_mse_for_monochrome_images_when_both_blocks_has_same_color)
{
	img_size_t	square_sz = 20;
	Image		black(square_sz, square_sz);
	Block		a(&black,  0,  0, 10, 10),
			b(&black, 10, 10, 10, 10);
	// Gives Zero	
	BOOST_REQUIRE_EQUAL(a.mse_divergence(b), 0.f);
}

BOOST_AUTO_TEST_CASE(check_mse_for_monochrome_images_when_one_block_is_empty_and_other_is_255)
{	
	img_size_t	square_sz = 30;
	Image		black(square_sz, square_sz),
			white(square_sz, square_sz);
	fill_in_with_color(white, 255);

	Block		a(&black,  4,  5, 10, 10),
			b(&white,  11, 7, 10, 10);
	
	BOOST_REQUIRE_EQUAL(a.mse_divergence(b), 1.f);
}

BOOST_AUTO_TEST_CASE(check_block_iterator_for_consistency)
{
	img_size_t	square_sz = 30,
			block_sz = 10;
	Image		black(square_sz, square_sz);
	Image::iterator	begin = black.begin(block_sz, block_sz),
			end = black.end(block_sz, block_sz);

	BOOST_REQUIRE_EQUAL(begin == end, false);
	BOOST_REQUIRE_EQUAL(begin != end, true);
	
	int cnt = 0;
	for (Image::iterator it = begin; it != end; ++it) {
		cnt++;
	}
	BOOST_REQUIRE_EQUAL(cnt, 9);

}

BOOST_AUTO_TEST_CASE(check_block_iterator_for_consistency_when_partial_blocks_present)
{
	img_size_t	square_sz = 30,
			block_h_sz = 7,
			block_w_sz = 9;
	Image		black(square_sz, square_sz);
	Image::iterator	begin = black.begin(block_h_sz, block_w_sz),
			end = black.end(block_h_sz, block_w_sz);

	BOOST_REQUIRE_EQUAL(begin == end, false);
	BOOST_REQUIRE_EQUAL(begin != end, true);
	
	int cnt = 0;
	for (Image::iterator it = begin; it != end; ++it) {
		cnt++;
	}
	BOOST_REQUIRE_EQUAL(cnt, 20);

}

