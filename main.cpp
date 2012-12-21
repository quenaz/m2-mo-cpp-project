#include "image.h"

#include <iostream>
#include <cstdio>

#include <dirent.h>

using namespace std;

void create_mosaic(ImageLibrary& archive, Image* target_image)
{
	MseDivergence divergence; // TODO: make a parameter of this function (via pointer)
	const int BLOCK_SZ=100;
	Image::iterator begin = target_image->begin(BLOCK_SZ, BLOCK_SZ),
		end = target_image->end(BLOCK_SZ, BLOCK_SZ);
	for (Image::iterator blockIt = begin; blockIt != end; ++blockIt)
	{
		float prox = 0.f;
		Block chosen_tile(NULL ,0 ,0 ,BLOCK_SZ ,BLOCK_SZ);
		for (ImageLibrary::storage::const_iterator image_in_lib_it = archive.begin(); image_in_lib_it != archive.end(); ++image_in_lib_it)
		{
			Block tile_block(*image_in_lib_it, 0, 0, BLOCK_SZ, BLOCK_SZ);
			float div_res = divergence.compute(tile_block, *blockIt);
			cout << div_res << " ";
			if (div_res > prox)
			{
				prox = div_res;
				chosen_tile = tile_block;
			}
		}
		(*blockIt).copy_content(chosen_tile);
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
		if(s.length() > 4)
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

