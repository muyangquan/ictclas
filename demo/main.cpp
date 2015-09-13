/*
 * vi:ts=4:tw=78:shiftwidth=4:expandtab
 * vim600:fdm=marker
 *
 * main.cpp  -  This is an very simple command line interface to ICTCLAS, a
 * current state-of-the-art Chinese Word Segmenter/Tagger.
 *
 * The program accepts input from stdin and outputs tagged results to stdout
 * line by line.
 *
 * Usage: program < stdin > stdout
 *
 * Copyright (C) 2003 by Zhang Le <ejoy@users.sourceforge.net>
 * Begin       : 11-Jun-2003
 * Last Change : 25-Oct-2003.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */
#include <unistd.h>
#include <string.h>

#include <cstdlib>
#include <cassert>
#include <stdexcept> //for std::runtime_error
#include <memory>    //for std::bad_alloc
#include <iostream>
#include <string>
#include <fstream>

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

char * get_exe_path()
{
    static char buf[PATH_MAX];
    int rslt=readlink("/proc/self/exe",buf,PATH_MAX);
    if (rslt<0||rslt>=PATH_MAX) return NULL;
    while (buf[--rslt]!='/') { buf[rslt]='\0'; }
    return buf;
}


#include "Result.h"
using namespace std;

int main(int argc, char* argv[]) {
    try {
        char output[1024*100];
		// the argument is the directory where "Data" is located.  It should be ended with a "/"
        CResult  result(get_exe_path()); 
        string s;
        while (getline(cin, s)) {
            //bool ret = result.ParagraphProcessing(s.c_str(), output);
            //if (ret)
            //    cout << output<< endl;
            //else
            //    cerr << "Error in processing " <<  s << endl;
			const int kNum = s.size();
			CResult::result_t result_arr[kNum];
			int result_num;
			result.ParagraphProcessingA(s.c_str(), result_arr, result_num);
			printf("%s\n", s.c_str());
			printf("==============================================>\n");
			for(int i = 0; i < result_num; ++i){
				memset(output, 0x00, sizeof(output));
				memcpy(output, s.c_str()+result_arr[i].start, result_arr[i].length);
				printf("word : %s\t\t\tpart_of_speech:%d\t\t\tvalue: %f\n", 
						output, result_arr[i].part_of_speech, result_arr[i].value);
			}
			printf("\n");
        }
    } catch (std::bad_alloc& e) {
        cerr << "std::bad_alloc caught: out of memory" << endl;
        return EXIT_FAILURE;
    } catch (std::runtime_error& e) {
        cerr << "std::runtime_error caught:" << e.what() << endl;
        return EXIT_FAILURE;
    } catch (std::exception& e) {
        cerr << "std::exception caught:" << e.what() << endl;
        return EXIT_FAILURE;
    } catch (...) {
        cerr << "unknown exception caught!" << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
