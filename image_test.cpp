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


