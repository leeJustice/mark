import sys

number = float(sys.argv[1])
error_rate = 1 - float(sys.argv[2])
print(number * error_rate)
