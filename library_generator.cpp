#include "image.h"

#include <iostream>
#include <cstdio>

#include <dirent.h>

using namespace std;

void rotate_all_images_in_folder(int counter_start, string input_folder, string output_folder)
{
	DirectoryListing		dir_listing(input_folder);
	DirectoryListing::iterator	begin = dir_listing.begin(),
					end = dir_listing.end();
	int cnt = counter_start;
	const int BUF_LN = 20;
	const int BLOCK_SZ = 100;
	char buffer[BUF_LN];
	for (DirectoryListing::iterator it = begin; it != end;)
	{
		string s = *(++it);
                if(s.length() > 4)
                {
                        cout << input_folder + s << "... ";
                        cout.flush();
			Image downscaled(BLOCK_SZ, BLOCK_SZ);
                        Image i(input_folder + s);
			i.scale_to(downscaled);
			int str_len = snprintf(buffer, BUF_LN, "%03d.jpg", cnt++);
                        downscaled.save(output_folder + buffer);
                        cout << "done." << endl;
                }
	}
}

int main(int argc, char *argv[])
{
	try {
		int counter_start = atoi(argv[1]);
		rotate_all_images_in_folder(counter_start, "inputImages/", "outputImages/");
	} catch (exception) {
	        cout << "Error opening inputImages/" << endl;
	}
	return 0;
}

