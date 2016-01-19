L = P1 - P0

if L.x > L.y:
  d = L / L.x
else:
  de = L / L.y

# to draw:
Q = P0

longside = max(L.x, L.y)
for i in range(longside):
  draw(Q) # round Q
  Q = Q + d