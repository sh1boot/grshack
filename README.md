Something something golden ratio line-filling blah...

This attempts to measure the minimum distance between visited points in a
discrete interval as it's filled in steps of the golden ratio, and determine
how close that method comes to evenly dividing the interval evenly by n as n
approaches the square root of the length of the internal.

Since the spaces I want to measure are 128 bits (more would be ideal)
optimisations were necessary, and the line between empirical and theoretical
started to get a bit blurry... but I learned stuff.  Probably stuff I could
have found in a book if I looked, but this was more fun.

Optimisations include trimming the test window to something much smaller than
the full range, and using a better-informed search for the next point that will
fall within that window (it's normally a Fibonacci number of steps away).
