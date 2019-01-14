#!/usr/bin/python
import sys

def read_graph(filename):
    inc_vertex = {} # incident vertex
    edge_list = []
    E = {}
    with open(filename) as f:
        for line in f:
            s = line.split()
            v = s[0]
            w = s[1]
            if v not in inc_vertex:
                inc_vertex[v] = set([])
            if w not in inc_vertex:
                inc_vertex[w] = set([])
            inc_vertex[v].add(w)
            inc_vertex[w].add(v)
            count = len(edge_list)
            E[(v,w)] = count
            E[(w,v)] = count
            edge_list.append((v, w))
    return E, inc_vertex, edge_list

def print_tree(E, inc_vertex, edge_list, v):
    nrVertices = len(inc_vertex) + 1 + 1 + len(edge_list)
    print nrVertices - 1, "#edges", v
    print "r", "T"
    for w in inc_vertex[v]:
        print "T", "e_%d" % E[(v,w)]
    for (w1,w2) in edge_list:
        if w1 != v and w2 != v:
            print "y_%s" % v, "e_%d" % E[(w1,w2)]
    for w in inc_vertex:
        print "r", "y_%s" % w


if len(sys.argv) != 2:
    sys.stderr.write("Usage: %s <graph.txt>\n" % sys.argv[0])
    sys.exit(1)

E, inc_vertex, edge_list = read_graph(sys.argv[1])
print "# hello"
print len(inc_vertex), "#trees"
for v in inc_vertex:
    print_tree(E, inc_vertex, edge_list, v)
