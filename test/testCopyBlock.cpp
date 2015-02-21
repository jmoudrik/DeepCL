// Copyright Hugh Perkins 2015 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include "gtest/gtest.h"
#include "test/gtest_supp.h"

#include <iostream>
#include <string>
#include <algorithm>

#include "OpenCLHelper.h"

using namespace std;

namespace testCopyBlock{

CLKernel *makeKernel( OpenCLHelper *cl );

TEST( testCopyBlock, basic ) {
    OpenCLHelper *cl = OpenCLHelper::createForFirstGpuOtherwiseCpu();

    float a[] = { 1,2,3,4,
                  5,6,7,8,
                  9,10,11,12 };
    float b[10];
    memset(b, 0, sizeof(float)*10);

    CLKernel *kernel = makeKernel( cl );
    kernel->in( 12, a )->out( 6, b )->in( ( 3<<10)|4)->in( (0<<10)|1)->in((2<<10)|3);
    kernel->run_1d(12,4);
    cl->finish();
    for( int i = 0; i < 2; i++ ) {
        for( int j = 0; j < 3; j++ ) {
            cout << b[i*3+j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    for( int i = 6; i < 10; i++ ) {
        cout << b[i] << " ";
    }
    cout << endl;

    delete kernel;
    delete cl;
}

CLKernel *makeKernel( OpenCLHelper *cl ) {
    CLKernel *kernel = 0;
    // [[[cog
    // import stringify
    // stringify.write_kernel2( "kernel", "test/testCopyBlock.cl", "run", '""' )
    // ]]]
    // generated using cog, from test/testCopyBlock.cl:
    const char * kernelSource =  
    "// Copyright Hugh Perkins 2015 hughperkins at gmail\n" 
    "//\n" 
    "// This Source Code Form is subject to the terms of the Mozilla Public License,\n" 
    "// v. 2.0. If a copy of the MPL was not distributed with this file, You can\n" 
    "// obtain one at http://mozilla.org/MPL/2.0/.\n" 
    "\n" 
    // including cl/copyBlock.cl:
    "// Copyright Hugh Perkins 2015 hughperkins at gmail\n" 
    "//\n" 
    "// This Source Code Form is subject to the terms of the Mozilla Public License,\n" 
    "// v. 2.0. If a copy of the MPL was not distributed with this file, You can\n" 
    "// obtain one at http://mozilla.org/MPL/2.0/.\n" 
    "\n" 
    "int posToRow( int pos ) {\n" 
    "    return ( pos >> 10 ) & ( 2^11-1);\n" 
    "}\n" 
    "int posToCol( int pos ) {\n" 
    "    return pos & (2^11-1);\n" 
    "}\n" 
    "int rowColToPos( int row, int col ) {\n" 
    "    return ( row << 10 ) | col;\n" 
    "}\n" 
    "int linearIdToPos( int linearId, int base ) {\n" 
    "    return rowColToPos( ( linearId / base ), ( linearId % base )  );\n" 
    "}\n" 
    "int posToOffset( int pos, int rowLength ) {\n" 
    "    return posToRow(pos) * rowLength + posToCol(pos);\n" 
    "}\n" 
    "\n" 
    "// assumes that the block will fit exactly into the target\n" 
    "void copyBlock( local float *target, global float const *source,\n" 
    "    const int sourceSize, const int blockStart, const int blockSize ) {\n" 
    "    const int totalLinearSize = posToRow( blockSize ) * posToCol( blockSize );\n" 
    "    const int numLoops = ( totalLinearSize + get_local_size(0) - 1 ) / get_local_size(0);\n" 
    "    for( int loop = 0; loop < numLoops; loop++ ) {\n" 
    "        const int offset = get_local_id(0) + loop * get_local_size(0);\n" 
    "        if( offset < totalLinearSize ) {\n" 
    "            const int offsetAsPos = linearIdToPos( offset, posToRow( blockSize ) );\n" 
    "            target[ offset ] = source[ posToOffset( blockStart + offsetAsPos, posToCol( sourceSize ) ) ];\n" 
    "        }\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "\n" 
    "\n" 
    // including cl/ids.cl:
    "// Copyright Hugh Perkins 2015 hughperkins at gmail\n" 
    "//\n" 
    "// This Source Code Form is subject to the terms of the Mozilla Public License,\n" 
    "// v. 2.0. If a copy of the MPL was not distributed with this file, You can\n" 
    "// obtain one at http://mozilla.org/MPL/2.0/.\n" 
    "\n" 
    "#define globalId ( get_global_id(0) )\n" 
    "#define localId ( get_local_id(0)  )\n" 
    "#define workgroupId ( get_group_id(0) )\n" 
    "#define workgroupSize ( get_local_size(0) )\n" 
    "\n" 
    "\n" 
    "\n" 
    // including cl/copyLocal.cl:
    "// Copyright Hugh Perkins 2015 hughperkins at gmail\n" 
    "//\n" 
    "// This Source Code Form is subject to the terms of the Mozilla Public License,\n" 
    "// v. 2.0. If a copy of the MPL was not distributed with this file, You can\n" 
    "// obtain one at http://mozilla.org/MPL/2.0/.\n" 
    "\n" 
    "void copyLocal( local float *target, global float const *source, int N ) {\n" 
    "    int numLoops = ( N + get_local_size(0) - 1 ) / get_local_size(0);\n" 
    "    for( int loop = 0; loop < numLoops; loop++ ) {\n" 
    "        int offset = loop * get_local_size(0) + get_local_id(0);\n" 
    "        if( offset < N ) {\n" 
    "            target[offset] = source[offset];\n" 
    "        }\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "void copyGlobal( global float *target, local float const *source, int N ) {\n" 
    "    int numLoops = ( N + get_local_size(0) - 1 ) / get_local_size(0);\n" 
    "    for( int loop = 0; loop < numLoops; loop++ ) {\n" 
    "        int offset = loop * get_local_size(0) + get_local_id(0);\n" 
    "        if( offset < N ) {\n" 
    "            target[offset] = source[offset];\n" 
    "        }\n" 
    "    }\n" 
    "}\n" 
    "\n" 
    "\n" 
    "\n" 
    "kernel void( global const float *source, global float *destination, int sourceSize, int blockPos, int blockSize,\n" 
    "    local float *localBuffer ) {\n" 
    "    copyBlock( localBuffer, source, sourceSize, blockPos, blockSize );\n" 
    "    copyGlobal( destination, localBuffer, posToRow( blockSize ) * posToCol( blockSize ) );\n" 
    "}\n" 
    "\n" 
    "";
    kernel = cl->buildKernelFromString( kernelSource, "run", "", "test/testCopyBlock.cl" );
    // [[[end]]]

    return kernel;
}

}
