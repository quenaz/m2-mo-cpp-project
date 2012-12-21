#include "image.h"

#include <iostream>
#include <cstdio>

#include <dirent.h>

using namespace std;

void rotate_all_images_in_folder(string input_folder, string output_folder)
{
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
                        i.flip_horizontally();
                        i.save(output_folder + s);
                        cout << "done." << endl;
                }
	}
}

int main(int argc, char *argv[])
{
	try {
		rotate_all_images_in_folder("inputImages/", "outputImages/");
	} catch (exception) {
	        cout << "Error opening inputImages/" << endl;
	}
	return 0;
}

