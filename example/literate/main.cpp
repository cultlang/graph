/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/* This document is a literate programming-esque introduction to this graph library. This library
 * is an in memory native graph library designed to be extensible and ergonomic.
 */

#include "main.h"

int main(int argc, char** argv)
{
    int main_ret = 0;
    int part_ret = 0;

    part_ret = part_01_basics();
    if (part_ret < 0)
    {
        if (main_ret == 0) main_ret = part_ret;
        std::cout << "literate part_01_basics failed." << std::endl;
    }
    
    return main_ret;
}
