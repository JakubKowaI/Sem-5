# Solve the quadratic equation ax**2 + bx + c = 0

# import complex math module
import cmath

def test():
    print("hahahahhaha")


"""
TURBO KOMENTARZ

"""

a = 1
b = 5
c = 6

# calculate the discriminant
d = (b**2) - (4*a*c)

# find two solutions
sol1 = (-b-cmath.sqrt(d))/(2*a)
sol2 = (-b+cmath.sqrt(d))/(2*a)

kom="test # to jest test a to jest cudzyslow \" "

print('The solution are {0} and {1}'.format(sol1,sol2))

