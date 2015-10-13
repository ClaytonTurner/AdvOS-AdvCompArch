def add(size=10):
    a = tuple([float(i) for i in range(size)])
    b = tuple([float(j) for j in range(size)])
    c = [None for i in range(size)]
    for i in range(size):
        c[i] = a[i]+b[i]
 
    #print "a", a
    #print "b", b
    print "c", c[:1000]
 
add(1000000)
