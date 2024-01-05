
import numpy as np
import matplotlib.pyplot as plt
import codecs, json 

filename = "numbers64.json"
y = [154, 159, 172, 212, 215, 231, 240, 289, 302]

filename = "numbers365.json"
y = [123, 149, 154, 161, 171, 174, 184, 186, 208, 217, 228, 233, 234, 256, 260, 266, 269, 274, 276, 279, 282, 294, 360, 380]

values = json.load(codecs.open(filename, 'r', encoding='utf-8'))

def find_indices(values, ys):
  x = []
  i = 0
  for y in ys:
    i = values.index(y, i)
    x.append(i)
  return x

x = find_indices(values, y)

t = np.arange(len(values))

fig, ax = plt.subplots()
ax.plot(t, values)
ax.scatter(x, y, color='red')
plt.draw()
plt.savefig('trader-graph-scatter.png')

plt.clf()
fig, ax = plt.subplots()
ax.plot(x, y, color='red')
plt.draw()
plt.savefig('trader-graph-plot.png')
