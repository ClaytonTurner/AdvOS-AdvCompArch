import pyopencl as cl
import numpy
import sys
 
class CL(object):
    def __init__(self, size=10):
        self.size = size
        self.ctx = cl.create_some_context()
        self.queue = cl.CommandQueue(self.ctx)
 
    def load_program(self):
        fstr="""
        __kernel void part1(__global float* a, __global float* b, __global float* c)
        {
            unsigned int i = get_global_id(0);
 
           c[i] = a[i] + b[i];
        }
         """
        self.program = cl.Program(self.ctx, fstr).build()
 
    def popCorn(self):
        mf = cl.mem_flags
 	
	# We use float32s here to have a fair comparison to the CPU
        self.a = numpy.array(range(self.size), dtype=numpy.float32)
        self.b = numpy.array(range(self.size), dtype=numpy.float32)
 
        self.a_buf = cl.Buffer(self.ctx, mf.READ_ONLY | mf.COPY_HOST_PTR,
                               hostbuf=self.a)
        self.b_buf = cl.Buffer(self.ctx, mf.READ_ONLY | mf.COPY_HOST_PTR,
                               hostbuf=self.b)
        self.dest_buf = cl.Buffer(self.ctx, mf.WRITE_ONLY, self.b.nbytes)
 
    def execute(self):
        self.program.part1(self.queue, self.a.shape, None, self.a_buf, self.b_buf, self.dest_buf)
        c = numpy.empty_like(self.a)
        cl.enqueue_read_buffer(self.queue, self.dest_buf, c).wait()
        print "a", self.a
        print "b", self.b
        print "c", c
 
if __name__ == '__main__':
    matrixmul = CL(10000000)
    matrixmul.load_program()
    matrixmul.popCorn()
    matrixmul.execute()
