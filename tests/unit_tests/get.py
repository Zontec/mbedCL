import sys

a = sys.argv[1]

for i in range(0, len(a), 2):
	print(f"\\x{a[i]}{a[i+1]}",end='')
print()
