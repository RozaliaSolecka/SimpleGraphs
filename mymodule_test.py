import simple_graphs

g = simple_graphs.AdjacencyMatrix("ICRdlpzxo")

print(g.vertices())
print(g.edges())
print(g.add_vertex(10))
print(g.vertices())
print(g.delete_vertex(9))
print(g.edges())
print(g.vertices())

