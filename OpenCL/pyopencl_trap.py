import pyopencl as cl
import numpy
import sys
 
class CL(object):
    def __init__(self, divisions=1000, h=0.001):
        self.divisions = divisions
        self.ctx = cl.create_some_context()
        self.queue = cl.CommandQueue(self.ctx)
 
    def load_program(self):
        fstr="""
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
		    __local float wiSums[64];
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
 	
	# We use float32s here to have a fair comparison to the CPU
        self.a = numpy.array(range(self.divisions), dtype=numpy.float32)
        self.b = numpy.array(range(self.divisions), dtype=numpy.float32)
	self.sums = numpy.array(range(self.divisions), dtype=numpy.float32)
	self.sums_buf = cl.Buffer(self.ctx, mf.READ_ONLY | mf.COPY_HOST_PTR,
				hostbuf=self.sums)
 	
        self.a_buf = cl.Buffer(self.ctx, mf.READ_ONLY | mf.COPY_HOST_PTR,
                               hostbuf=self.a)
        self.b_buf = cl.Buffer(self.ctx, mf.READ_ONLY | mf.COPY_HOST_PTR,
                               hostbuf=self.b)
	self.dest_buf = cl.Buffer(self.ctx, mf.WRITE_ONLY, self.sums.nbytes)
 
    def execute(self):
        #self.program.part1(self.queue, self.a.shape, None, self.a_buf, self.b_buf, self.dest_buf)
	
        #self.program.comp_trap_multi(self.queue, None, self.a_buf, self.b_buf, self.sums_buf, self.divisions_buf, self.dest_buf)
	self.program.comp_trap_multi(self.queue, 1, None, self.a_buf, self.b_buf, self.sums_buf, self.dest_buf)
	c = numpy.empty_like(self.a)
        cl.enqueue_read_buffer(self.queue, self.dest_buf, c).wait()
        #print "a", self.a
        #print "b", self.b
        print "Area Under Curve: ", c
 
if __name__ == '__main__':
    matrixmul = CL()
    matrixmul.load_program()
    matrixmul.popCorn()
    matrixmul.execute()
