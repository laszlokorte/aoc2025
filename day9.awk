#!/usr/bin/awk -f

# call with
# ./day9.awk inputs/day9/prod.txt > day9.obj
BEGIN {
    FS = ","    # set CSV separator
}
{
    x[NR-1] = $1
    y[NR-1] = $2
    n++
    sumx += $1
    sumy += $2
}
END {
    # compute means to center
    mx = sumx / n
    my = sumy / n

    # compute max distance from origin after centering
    maxd = 0
    for(i=1; i<=n; i++) {
        xc = x[i] - mx
        yc = y[i] - my
        d = sqrt(xc*xc + yc*yc)
        if(d > maxd) maxd = d
    }

    # print vertices scaled and centered
    for(i=1; i<=n; i++) {
        xc = x[i]
        yc = y[i]
        print "v", xc, yc, 0
    }

    # print lines connecting consecutive vertices
    for(i=1; i<n; i++) {
        print "l", i-1, i
    }
    print "l", n-1, 1
}
