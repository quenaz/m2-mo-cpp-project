/**
 * C++ course project.
 * Copyright (C) 2012, Artem Oboturov, Ludwig Brummer 
 */
#include "image.h"

#include <exception>

#include <dirent.h>

// Define DirectoryListing class.

DirectoryListing::DirectoryListing(string& dir)
{
	folder = dir;
}

DirectoryListing::iterator DirectoryListing::begin()
{
	return DirectoryListingIterator(folder);
}

DirectoryListing::iterator DirectoryListing::end()
{
	return DirectoryListingIterator::it_to_end;
}

// End of DirectoryListing class.

// Define DirectoryListingIterator class.

DirectoryListingIterator DirectoryListingIterator::it_to_end = DirectoryListingIterator::DirectoryListingIterator();

DirectoryListingIterator::DirectoryListingIterator(string& dir) throw (exception)
{
	folder = dir;
	if (NULL == (open_dir = opendir(folder.c_str())))
		throw exception();
	cur_file = readdir(open_dir);
}

string& DirectoryListingIterator::operator*()
{
	if (NULL == cur_file)
		throw exception();
	cur_file_name = string(cur_file->d_name, cur_file->d_namlen);
	return cur_file_name;
}

DirectoryListingIterator& DirectoryListingIterator::operator++()
{
	if (NULL != open_dir)
	{
        	cur_file = readdir(open_dir);
		if (NULL == cur_file)
		{
			closedir(open_dir);
			open_dir = NULL;
		}
	}
	return *this;
}

bool DirectoryListingIterator::operator==(const DirectoryListingIterator& rhs) const
{
	return open_dir == rhs.open_dir && cur_file == rhs.cur_file;
}

bool DirectoryListingIterator::operator!=(const DirectoryListingIterator& rhs) const
{
	return !(*this == rhs);
}

// End of DirectoryListingIterator class.

