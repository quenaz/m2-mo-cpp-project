#include "image.h"

#include <iostream>
#include <cstdio>

#include <dirent.h>

using namespace std;

void create_mosaic(ImageLibrary& archive, Image* target_image)
{
	MseDivergence divergence; // TODO: make a parameter of this function (via pointer)
	const img_size_t BLOCK_SZ = 10;
	Image::iterator begin = target_image->begin(BLOCK_SZ, BLOCK_SZ),
		end = target_image->end(BLOCK_SZ, BLOCK_SZ);
	int aa = 0;
	char aaa[30]; 
	// Scale down images from library.
	Image	library(archive.size() * BLOCK_SZ, BLOCK_SZ),
		downscaled_img(BLOCK_SZ, BLOCK_SZ);
	size_t cnt = 0;
	for (ImageLibrary::storage::const_iterator image_in_lib_it = archive.begin(); image_in_lib_it != archive.end(); ++image_in_lib_it, ++cnt)
	{
		(*image_in_lib_it)->scale_to(downscaled_img);
		Block downscaled_block(&downscaled_img, 0, 0, BLOCK_SZ, BLOCK_SZ);
		Block(&library, 0, cnt * BLOCK_SZ, BLOCK_SZ, BLOCK_SZ).copy_content(downscaled_block);
	}
	// Create mosaic.
	for (Image::iterator blockIt = begin; blockIt != end; ++blockIt)
	{
		float prox = 0.f;
		Image::iterator	begin = library.begin(BLOCK_SZ, BLOCK_SZ),
				end = library.end(BLOCK_SZ, BLOCK_SZ);
		Block chosen_tile(NULL, 0, 0, BLOCK_SZ, BLOCK_SZ);
		for (Image::iterator libIt = begin; libIt != end; ++libIt)
		{
			float div_res = divergence.compute(*libIt, *blockIt);
			if (div_res > prox)
			{
				prox = div_res;
				chosen_tile = *libIt;
			}
		}
		(*blockIt).copy_content(chosen_tile);
		cout << "*";
		cout.flush();
	}
}

void produce_mosaic_for_all_images_in_folder(string input_folder, string output_folder)
{
	ImageLibrary archive;
	archive.reload("libraryImages/");
	DirectoryListing		dir_listing(input_folder);
	DirectoryListing::iterator	begin = dir_listing.begin(),
					end = dir_listing.end();
	for (DirectoryListing::iterator it = begin; it != end; ++it)
	{
		string s = *it;
		if(s.length() > 4 && s[0] != '.')
        {
			cout << input_folder + s << "... ";
			cout.flush();
            Image i(input_folder + s);
			create_mosaic(archive, &i);
			i.save(output_folder + s);
            cout << "done." << endl;
		}
	}
}

int main(int argc, char *argv[])
{
	try {
		produce_mosaic_for_all_images_in_folder("inputImages/", "outputImages/");
	} catch (exception) {
	    cout << "Error opening inputImages/" << endl;
	}
	return 0;
}

