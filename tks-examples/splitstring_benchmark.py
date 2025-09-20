import time;

ms = round(time.time()*1000.0)

i = 0
for _ in range(1000):
  for _ in range(1000):
    s = "one two three four_and_a_half";
    for t in s.split():
      i += 1

ms = round(time.time()*1000.0) - ms
print(f"ms={ms}")

print(f"i={i}")
