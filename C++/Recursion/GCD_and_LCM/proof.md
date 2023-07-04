If two integers A and B are NOT coprime, let's say their greatest common divisor is gcd.

A = gcd * a

B = gcd * b

integers a and b are coprime.

A - B = gcd * a - gcd * b = gcd * (a - b)


assume (a - b) is not coprime with b, so assume k is their greatest common divisor

	(a - b) = k * i

	b       = k * j

then A - B = A - gcd * b = A - gcd * k * j = gcd * (a - b) = gcd * k * i

then A = gcd * k * i + gcd * k * j = gcd * k * (i + j)

     B = gcd * k * j

that contradict gcd is the greatest common divisor of A and B

So (a - b) must coprime with a and b.
