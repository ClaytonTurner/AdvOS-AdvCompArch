"""
 1) Get numerical integration kernel to work
    (in Python, C, or C++)

 2) Determine "optimal" workgroup size

 3) Complete Host program and run

 4) Time (see perf counter / time)
"""

import pyopencl as cl
import numpy
import sys
import time
 
class CL(object):
    def __init__(self):
        self.ctx = cl.create_some_context()
        self.queue = cl.CommandQueue(self.ctx)
 
    def load_program(self):
        fstr="""
        /*
        numerical-integration.cl
        */

        float f(float x)
        {
                    return x*x;
                    }

        float simple_trap(float a, float b)
        {
                    float fA, fB;
                    fA = f(a);
                    fB = f(b);
                    return ((fA + fB) * (b-a)) / 2;
        }

        __kernel void comp_trap(
                    float a,
                    float b,
                    __global float* sums)
        {
                /*
                - assumes 1D global and local work dimensions
                - each work unit will calculate 1/get_global_size of the total sum
                - the 0th work unit of each group then accumulates the sum for the
                group and stores it in __global * sums
                - memory allocation: sizeof(sums) = get_num_groups(0) * sizeof(float)
                - assumes local scratchpad size is at lease 8 bytes per work unit in the group
                ie sizeof(wiSums) = get_local_size(0) * sizeof(float)
                */
                //__local float wiSums[8];
                //__local float wiSums[16];
                //__local float wiSums[32];
                //__local float wiSums[64];
                //__local float wiSums[128];
                //__local float wiSums[256];
                __local float wiSums[512];
                //__local float wiSums[1024];
                //__local float wiSums[2048];
                int l_id = get_local_id(0);

                //compute range for this work item is: a1, b1
                float a1 = a+((b-a)/get_global_size(0))*get_global_id(0);
                float b1 = a1+(b-a)/get_global_size(0);

                wiSums[l_id] = simple_trap(a1,b1);

                barrier(CLK_LOCAL_MEM_FENCE);

                int i;
                if(l_id == 0){
                        for(i=1;i<get_local_size(0);i++){
                            wiSums[0] += wiSums[i];
                        }
                        sums[get_group_id(0)] = wiSums[0];
                }
        }

        __kernel void comp_trap_multi(
                    float a,
                    float b,
                    __global float* sums,
                    int divisions)
        {
            /*
            - same as above, but each work unit further divides its range into
            'divisions' equal parts, yielding a more accurate result
            - work units still store only one sum in the local array, which is
            used later for the final group accumulation
            */
            __local float wiSums[64];
            int l_id = get_local_id(0);

            float a1 = a+((b-a)/get_global_size(0))*get_global_id(0);
            float b1 = a1+(b-a)/get_global_size(0);
            float range;
            if(divisions > 0){
                range = (b1-a1)/divisions;
            }else{
                range = (b1-a1);
            }

            int i;
            wiSums[l_id] = 0;
            for(i=0;i<divisions;i++){
            wiSums[l_id] += simple_trap(a1+range*i,a1+range*(i+1));
            }

            barrier(CLK_LOCAL_MEM_FENCE);

            if(l_id == 0){
                for(i=1;i<get_local_size(0);i++){
                    wiSums[0] += wiSums[i];
                }
                sums[get_group_id(0)] = wiSums[0];
            }
        }
         """
        self.program = cl.Program(self.ctx, fstr).build()
 
    def popCorn(self):
        mf = cl.mem_flags
        # comp_trap parameters: float a, float b, __global float* sums
        self.a = numpy.float32(0.0)
        self.b = numpy.float32(6.28)

        self.dest_buf = cl.Buffer(self.ctx, mf.WRITE_ONLY, self.b.nbytes)
 
    def execute(self):
        # We pass in self.a/self.b instead of buffers as we only want a single value
        #   buffers are for arrays/pointers to the head of arrays
        self.program.comp_trap(self.queue, (1,1), None, self.a, self.b, self.dest_buf)
        c = numpy.empty_like(self.a)
        cl.enqueue_read_buffer(self.queue, self.dest_buf, c).wait()
        print "Starting x", self.a
        print "Ending x", self.b
        print "Area under the curve", c
 
if __name__ == '__main__':
    matrixmul = CL()
    matrixmul.load_program()
    matrixmul.popCorn()
    time.clock()
    matrixmul.execute()
    print "Execution took: "+str(time.clock())+" seconds" # microsecond resolution
    # Using time.clock() since we're in python 2.7 - perf counter is a Python 3 mechanic
    print "512 is the optimal workgroup size through empirical tests (GeForce GTX 550 Ti)"
