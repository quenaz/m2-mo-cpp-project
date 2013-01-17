#include "image.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <dirent.h>

using namespace std;

void rescale_all_library_images_in_folder(int counter_start, string input_folder, string output_folder, int size)
{
	DirectoryListing		dir_listing(input_folder);
	DirectoryListing::iterator	begin = dir_listing.begin(),
					end = dir_listing.end();
	int cnt = counter_start;
	const int BUF_LN = 20;
	char buffer[BUF_LN];
	for (DirectoryListing::iterator it = begin; it != end; ++it)
	{
		string s = *it;
                if(s.length() > 4)
                {
                        cout << input_folder + s << "... ";
                        cout.flush();
			Image downscaled(size, size);
                        Image i(input_folder + s);
			i.scale_to(downscaled);
			int str_len = snprintf(buffer, BUF_LN, "%03d.jpg", cnt++);
                        downscaled.save(output_folder + buffer);
                        cout << "done." << endl;
                }
	}
}

void print_usage()
{
	cout<<"This is the help menu for the library_generator of our mosaic maker. You can reenter this menu by launch the program with the command line options \"-h\" or \"help\"."<<endl;
	cout<<"You can choose tile size via the command line option \"--size\" and a positive integer. Default tile size is 100. The mosaic maker will work if the size of your library images differs from your later chosen tile size."<<endl;
	cout<<"Before executing this program another time, please make sure you copied all the images you want to be part of your image library into the folder \"inputImages\" which should be found in the mosaic maker program folder. The folder \"libraryImages\" should be empty."<<endl;
	cout<<"By executing the library_generator, all your images will be copied and rescaled in the folder \"libraryImages\". Images cannot be made bigger, only smaller."<<endl;
}

int main(int argc, char *argv[])
{
	// Default value
	int size=100;
	// CLI parameters parsing
	static struct option long_options[] =
    {
		{"size", required_argument, 0, 's'},
		{"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
	};
	int option_index=0;
	int opts_res = -1;
	while (-1 != (opts_res = getopt_long(argc, argv, "sh", long_options, &option_index)))
	{
		switch(opts_res)
		{			
			case 's':
				size=atoi(optarg);
				cout<<"You chose size "<< size << "." <<endl;
				if (size<1)
				{
					cout<< "Wrong size. Please use positive integers. Size was set 100."<<endl;
					size=100;
				}
				break;
			case 'h':
				print_usage();
				exit(0);
		}	
	}
	// Program execution
	try {
		int counter_start = atoi(argv[1]);
		rescale_all_library_images_in_folder(counter_start, "inputImages/", "libraryImages/", size);
	} catch (exception) {
	        cout << "Error opening inputImages/" << endl;
	}
	return 0;
}

