
import numpy as np
import matplotlib.pyplot as plt
import codecs, json 

Y = 64 # number of days per "year"
NY = 1 # number of years
n = Y * NY # total number of points
t = np.arange(n) # [0, 1, ..., n-1]

rng = np.random.default_rng()
noise = rng.standard_normal(size=n) # generate some noise using a standard distribution

def f(t):
  #return 100 * (np.cos(t / 100) + 2) - 25 * (t / 365) + noise
  return 100 + 200 * ((n - t) / n) + 50 * (np.sin(0.5 * t / n) + 1) + 75 * noise

values = f(t) # generate the values

# print(values)

# write the generated numbers in a file as a json array 
json.dump(values.astype(int).tolist(), codecs.open('numbers.json', 'w', encoding='utf-8'), 
          separators=(',', ':'), 
          sort_keys=True, 
          indent=4) 

# plot the graph
fig, ax = plt.subplots()
ax.plot(t, values)
plt.draw()
plt.savefig('numbers.png')
