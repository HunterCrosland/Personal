#the following is a 5-minute experiment to test linear transformations
#as a vehicle for derivation.

import numpy as np

#goal is to get derivative of a mathematical polynomial via
#linear transformation (i.e. matrix multiplication)

#first, we take in our statement:
a_term = int(
    input("enter cubic coefficient (i.e. enter 3 for 3x^3, -5 for -5x^3: "))
b_term = int(input("enter square coefficient: "))
c_term = int(input("enter linear coefficient: "))
d_term = int(input("enter constant: "))

#output for clarity
print(f"Deriving ({a_term})x^3 + ({b_term})x^2 + ({c_term})x + ({d_term})")

#construct polynomial vector
polynomial_vector = np.array([
    [d_term],
    [c_term],
    [b_term],
    [a_term]
])

#construct linear transform array
first_derivative_transform = np.array([
    [0,1,0,0],
    [0,0,2,0],
    [0,0,0,3]
])

#get derivative via transformation
derivative =  np.dot(first_derivative_transform,polynomial_vector)

#print result
print(f"Result of your derivation: ({derivative[2][0]})x^2 + ({derivative[1][0]})x + ({derivative[0][0]})")
