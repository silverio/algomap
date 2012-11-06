import math
import heapq

import numpy as np
import matplotlib.pyplot as plt

class Event:
    def __init__(self, x, y, siteEvent=True):
        self.x = x
        self.y = y
        self.siteEvent = siteEvent

    def __cmp__(self, ev):
        return self.y < ev.y

class Arc:
    def __init__(self, x, y):
        self.x = x
        self.y = y

def parab(x0, y0, yL, x):
    """ the beachfront parabola equation"""
    return ((x - x0)**2 + y0*y0 - yL*yL)*0.5/(y0 - yL)

def breakPoint(x0, y0, x1, y1, yL):
    """ the point where two sites' parabolas meet """
    A = y1 - y0
    B = y1 - yL
    C = y0 - yL
    b = 2.*(C*x1 - B*x0)
    c = B*x0*x0 - C*x1*x1 - A*B*C
    # quadratic equation
    D = b*b - 4.*A*c
    if D < 0.:
        raise ValueError
    sqD = math.sqrt(D)
    r1 = (-b + sqD)*0.5/A
    return r1

def findArc(x, bl, yL):
    """ binary search for the site, corresponding to the given coordinate x on the beachfront """
    n = len(bl)
    l = 0
    r = n - 1
    while r > l:
        k = (l + r)/2
        xbp = breakPoint(bl[k].x, bl[k].y, bl[k + 1].x, bl[k + 1].y, yL)
        if (xbp < x):
            l = k + 1
        else:
            r = k
    return l
    
def circleEventPoint(x0, y0, x1, y1, x2, y2):
    A = x1 - x0
    B = y1 - y0
    C = x2 - x0
    D = y2 - y0
    E = A*(x0 + x1) + B*(y0 + y1)
    F = C*(x0 + x2) + D*(y0 + y2)
    G = 2.0*(A*(y2 - y1) - B*(x2 - x1))
    if math.fabs(G) < 1e-12:
        # all three points on the same line
        raise ValueError
    xc = (D*E - B*F)/G
    yc = (A*F - C*E)/G
    r = math.sqrt((xc - x0)**2 + (yc - y0)**2)
    yb = yc - r
    return (xc, yb), (xc, yc, r)


def plotParab(x0, y0, yL, a=0, b=1):
    xp = np.linspace(a, b, 1000)
    yp = np.array(parab(x0, y0, yL, xp))
    plt.plot(xp, yp, 'r-')

def plotCircle(x0, y0, r, minx=-1, maxx=2, miny=-1, maxy=2):
    xy = [(math.cos(t)*r + x0, math.sin(t)*r + y0) for t in np.linspace(0, 2*math.pi, 1000)]
    xs = [x for x, y in xy if x >= minx and x <= maxx]
    ys = [y for x, y in xy if y >= miny and y <= maxy]
    plt.plot(xs, ys, 'y-')

def drawBeachLine(bl, yL):
    n = len(bl)
    
    # breakpoints
    xbp = np.zeros(n - 1)
    ybp = np.zeros(n - 1)
    for i in range(n - 1):
        xbp[i] = breakPoint(bl[i].x, bl[i].y, bl[i + 1].x, bl[i + 1].y, yL)
        ybp[i] = parab(bl[i].x, bl[i].y, yL, xbp[i])
    
    # arcs
    for i in range(n):
        a = 0
        if i > 0:
            a = xbp[i - 1]
        b = 1
        if i < n - 1:
            b = xbp[i]
        plotParab(bl[i].x, bl[i].y, yL, a, b)
    
    # sites
    plt.plot([b.x for b in bl], [b.y for b in bl], 'bo')
    
    # sweep line        
    xlp = np.array([0, 1])
    ylp = np.array([yL, yL]) 
    plt.plot(xlp, ylp, 'b-')
    
def testBeachLine():
    xc = np.array([0.1, 0.12, 0.3, 0.5, 0.6, 0.8, 0.95])
    yc = np.array([0.4, 0.31, 0.4,  0.7, 0.6, 0.4, 0.35])
    tx = np.array([0.05, 0.2, 0.3, 0.4, 0.65, 1])

    yL = 0.3    
    n = len(xc)
    bl = [Arc(xc[i], yc[i]) for i in range(0, n)]
    
    drawBeachLine(bl, yL)
    
    # circle event points
    for i in range(n - 2):
        try:
            (x, y), (x0, y0, r) = circleEventPoint(xc[i], yc[i], xc[i + 1], yc[i + 1], xc[i + 2], yc[i + 2])
            plt.plot([x], [y], 'ko')
            plotCircle(x0, y0, r)
        except:
            print "%d, %d, %d - collinear points" % (i, i + 1, i + 2)

    # find arc test points
    for x in tx:
        k = findArc(x, bl, yL)
        hy = parab(xc[k], yc[k], yL, x)
        plt.plot([x, x], [yL, hy], 'k-')

    plt.xlabel('x')
    plt.ylabel('y')
    plt.ylim([0, 1])
    plt.show()

def computeVoronoi(pts):
    events = []
    beachLine = []
    # put all the sites into the site events queue
    for x, y in pts:
        heapq.heappush(events, Event(x, y))
    
    while events: 
        ev = heapq.heappop(events)
        if ev.siteEvent:
            # find the corresponding arc
            if beachLine:
                k = findArc(ev.x, beachLine, ev.y)
                # break the arc
                newArc = Arc(ev.x, ev.y)
                if k < len(beachLine) - 1:
                    # split the existing arc in two
                    karc = beachLine[k]
                    beachLine.insert(k, Arc(karc.x, karc.y))
                beachLine.insert(k, newArc);
                
            else:
                beachLine.append(Arc(ev.x, ev.y))
        else:
            pass
        
        print "\nBeachline: ",
        for arc in beachLine:
            print "%.4f %.4f; " % (arc.x, arc.y),
        print "\nEvents: ",
        for e in events:
            print "%.4f %.4f; " % (e.x, e.y),
        print "\n"
        
        drawBeachLine(beachLine, ev.y)
        plt.show()
    return [], []

def testAlgorithm():
    xc = np.array([0.5, 0.8, 0.6, 0.3, 0.7, 0.3, 0.95, 0.2, 0.1])
    yc = np.array([0.7, 0.4, 0.6, 0.75, 0.3, 0.3, 0.35, 0.6, 0.4])
    plt.plot(xc, yc, 'bo')
    plt.show()
    pt = zip(xc, yc)    
    for x, y in pt:
        print "%d, %d, " % (int(x*800), int((1.0 - y)*600)),
    nodes, edges = computeVoronoi(pt)
    
#testAlgorithm()
testBeachLine()
