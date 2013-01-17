#include "image.h"

#include <iostream>
#include <cstdio>
#include <getopt.h>
#include <dirent.h>
#include <unistd.h>

using namespace std;

void create_mosaic(ImageLibrary& archive, Image* target_image, DivergenceMeasure* divergence, int BLOCK_SZ)
{
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
			float div_res = (*divergence).compute(*libIt, *blockIt);
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

void produce_mosaic_for_all_images_in_folder(string input_folder, string output_folder, DivergenceMeasure* divergence, int size)
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
			create_mosaic(archive, &i, divergence, size);
			i.save(output_folder + s);
            cout << "done." << endl;
		}
	}
}

void print_usage()
{
		cout<<"This is the help menu for our mosaic maker. You can reenter this menu by launch the program with the command line options \"-h\" or \"--help\"."<<endl;
		cout<<"If this is your first use of the mosaic maker, you should make sure that you have a good image library, namely images in the folder \"imageLibrary\". If not, use \"make library_generator\" and the library_generator.exe to make one. It has its own help."<<endl;
		cout<<"You can choose tile size and divergence measure via command line options. Default measure is the Monte Carlo mean squared error measure and default tile size is 20."<<endl;
		cout<<"Type \"--mse\" or \"-a\" after the execution command to choose the mean squared error measure."<<endl;
		cout<<"Type \"--meancolor\" or \"-b\" after the execution command to choose the mean color measure."<<endl;
		cout<<"Type \"--mcmes\" or \"-c\" after the execution command to choose the Monte Carlo mean squared error measure."<<endl;
		cout<<"Type \"--tilesize\" or \"-d\" and a positive integer to choose a tilesize."<<endl;
		cout<<"Copyright 2012-2013 Artem Oboturov and Ludwig Brummer. All rights reserved."<<endl;
		cout<<"Images are property of their published and are in public domain, thanks to NASA and other US Administrations."<< endl;
		cout<<"Disclaimer: The authors are not responsible for any damages to persons, objects, animals, files ot other stuff resulting from using this program."<<endl;
}

int main(int argc, char *argv[])
{
	// Default values
	int size=20;
	DivergenceMeasure* divergence = new MCmse;	
	// CLI parameters parsing
	static struct option long_options[] =
    {
        {"mse", no_argument, 0, 'a'},
        {"meancolor",  no_argument, 0, 'b'},
        {"mcmse",  no_argument, 0, 'c'},
		{"tilesize", required_argument, 0, 'd'},
		{"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
	};
	int option_index=0;
	int opts_res = -1;
	while (-1 != (opts_res = getopt_long(argc, argv, "abcdh", long_options, &option_index)))
	{
		switch(opts_res)
		{			
			case 'a':
			case 'b': 
			case 'c':
				if (NULL != divergence) delete divergence;
				switch (opts_res)
				{
					case 'a':
						cout<<"You chose the mean squared error measure."<<endl;
						divergence = new MseDivergence;
						break;
					case 'b': 
						cout<<"You chose the mean color measure."<<endl;	
						divergence = new MeanColor;
						break;
					case 'c':
						cout<<"You chose the Monte Carlo mean squared error measure."<<endl;
						divergence = new MCmse;
						break;					
				}
				break;
			case 'd':
				size=atoi(optarg);
				cout<<"You chose tile size "<< size << "." <<endl;
				if (size<1)
				{
					cout<< "Wrong size. Please use positive integers. Size was set 20"<<endl;
					size=20;
				}
				break;
			case 'h':
				print_usage();
				exit(0);
		}	
	}
	// Program execution
	try {
		produce_mosaic_for_all_images_in_folder("inputImages/", "outputImages/", divergence, size);
	} catch (exception) {
	    cout << "Error opening inputImages/" << endl;
	}
	return 0;
}

