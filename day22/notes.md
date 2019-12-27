# Simple modular linear congruence

y = x + 2 (mod 5)

y(0) = 2
y(1) = 3
y(2) = 4
y(3) = 0
y(4) = 1
y(5) = 2
y(6) = 3
y(7) = 4
y(8) = 0
y(9) = 1
...

In our case, we have (for the `cut` method): y(position) = (position - (n - size)) (mod size)

## Inverting

Modular arithmetic satisfies some of the same properties as "regular" arithmetic
(https://en.wikipedia.org/wiki/Modular_arithmetic#Properties), specially "compatibility
with addition/subtraction", which lets us invert congruences like y = x + a (mod N) by
subtracting `a` from both sides:

`x = y - a (mod N)`

Particularly:
```
x = y - 2 (mod 5)
x(0) = -2 = 3*
x(1) = -1 = 4
x(2) = 0
x(3) = 1
x(4) = 2
x(5) = 3
x(6) = 4
x(7) = 0
x(8) = 1
...
```

\* Negative numbers: in (mod N) world, we are free to add `N` to a number infinitely, and the result
will still be congruent with the original value. For example in (mod 5):
-1 + 5 = 4 + 5 =  9 + 5 = 14 + 5 = 4 (mod 5)

In our case, both `cut` and `deal_with_new_stack` have this shape, so we know how to invert
them.

# Multiplicative congruences

What about congruences in the shape of y = 3x (mod 5)?

```
y(0) = 0
y(1) = 3
y(2) = 1
y(3) = 4
y(4) = 2
y(5) = 0
y(6) = 3
y(7) = 1
...
```

In "regular" math, we can find the inverse of `y = 3x` by dividing both sides by 3:

`x = 1/3 * y`

But in `(mod N)` world, there are no `1/k` values, only integers from `{0,1,2,3,...,n-1}` (and their congruences like `6 == 1` or `7 == 2` in `(mod 5)`).
So even though division is not defined, we can still find a number `b` that acts like `1/a`, in the sense that:

`a * b = 1`

In particular, if we are interested in finding the inverse of:

`y = 3x (mod 5)`

We need to find b such that:

`3 * b = 1`

If we have this value `b`, we can multiply both sides by it:

```
by = b3x = 1x (mod 5)
=> x = by (mod 5)
```

### Questions

#### Does such b always exist?

https://en.wikipedia.org/wiki/Modular_multiplicative_inverse#Multiplicative_group_of_integers_modulo_m

Apparently the multiplicative inverse of `a (mod N)` exists if `N` and `a` are relatively prime. This is
always the case when `N` itself is prime.

#### What if a == 0?

## How do we find `b` (the "multiplicative inverse")?

One approach is to use the Extended Euclidean Algorithm.
(https://en.wikipedia.org/wiki/Modular_multiplicative_inverse#Extended_Euclidean_algorithm):

To find the inverse of `a (mod N)` using the Extended Euclidian Algorithm, we solve the equation:

`ax + yN = 1`

If we find `x` and `y`, the term `yN` becomes congruent to `0` in `(mod N)` (since `yN` is necessarily a multiple of `N`), and we get:

```
ax + yN = 1
=> ax = 1
```

Here, `x` is the multiplicative inverse, exactly like we wanted.

\* Note that if `N` is small, we can precompute a table of inverses by doing only the "forward" multiplication. That is,
we can loop for all possible values of x twice and built the map:

```python
for i in range(N):
    for j in range(N):
        table[i][(i * j) % N] = j
```
By doing so, the multiplicative inverse of `i` is in `table[i][1]`. Many applications in computer science use `N = 8 bits = 1 byte`
(where results of operations on a byte are guaranteed to also fit into a byte). Examples are the [AES block cipher](https://csrc.nist.gov/csrc/media/projects/cryptographic-standards-and-guidelines/documents/aes-development/rijndael-ammended.pdf) (see chap. 4).
I also borrowed the idea when implementing the multiplicative inverse in my barebones [Shamir Secret Sharing scheme implementation](https://github.com/rbaron/secret/blob/master/secret.c#L44).

In our case, N is big, so the approach of precomputing doesn't pay off. We need to use something like the Extended Euclidean Algorithm.

# Composition of modular congruences

Say we have the two congruences:

```
f(x) = a1x + b1 (mod N)
g(x) = a2x + b2 (mod N)
```

We can compose `fog(x)`:

`fog(x) = a1(a2x + b2) + b1 = a1a2x + a1b2 + b1 (mod N)`

## What about integer overflow when multiplying potentially big terms `a1` and `a2` when composing many functions?

For the terms without multiplication by `x`, we can apply `(mod N)` and "truncate" each term as we compose pairs of functions:

```
a1b2 = a1b1 % N
b1 = b1 % N
```

For the term `a1a2x`, not so much, since we **cannot** do this:

```
// THIS IS NOT CORRECT!
a1a2x = (a1a2x % N)x
```

So I think we'll need a "bigint" class for arbitrary precision integers (boost::multiprecision::cpp_int, for example).

At this point, we can invert all shuffling operations and compose all of them from our input into a single modular function. We
now need to answer the question: how can we apply this composed function a huge number of times over and over again (precisely
`101741582076661` times)?

# How can we apply the same modular function over and over again?

Let `f(x) = ax + b (mod N)`.

## Twice
```
f(f(x)) = a(ax + b) + b = a^2 + ab + b
```

## Trice
```
f(f(f(x))) = a^2(ax + b) + ab + b = a^3x + a^2b + ab + b
```

## Four times
```
f^4(x) = a^3(ax + b) + a^2b + ab + b = a^4x + a^3b + a^2b + ab + b
       = a^4 + b(a^3 + a^2 + a + 1)
```

## n times
```
f^n(x) = a^nx + a^(n-1)b + ... + ab + b
       = a^nx + b(a^(n-1) + a^(n-2) + ... + a + 1)
```

The part multiplying `b` is a geometric progression with factor `a`, and its sum is:

`(a^n - 1)/(a - 1)`

So to compute `f^n(x)` we need to calculate two things:
1. `a^n`
2. `inverse of (a - 1)`

In our case:
`n (number of times to run the full inversion) = 101741582076661 (prime)`
`N (deck size) = 119315717514047 (prime)`

### Calculating `a^n`

Maybe there's a trick calculating `a^n` when `n` is prime, and Fermat's little theorem has something to
say when that's the case (https://en.wikipedia.org/wiki/Fermat%27s_little_theorem). If `p` is prime:
`a^(p-1) = 1 (mod p)`. That's interesting but doesn't seem to help us very much here, since `N != n`.

So we will need to calculate `a^n` properly. Instead of multiplying `a` by itself `n` times (when `n` is huge like in here),
we can use a trick from SICP (repeated squaring, https://en.wikipedia.org/wiki/Exponentiation_by_squaring), with which we
can calculate `a^n` in `log-ish(n)` time:

`a^n = (a^2)^(n/2) = ((a^2)^2)^(n/4) = ...`

### Calculating the inverse of `(a - 1)`

We can use the same technique (Extended Euclidean Algorithm) we used before for finding multiplicative inverses.

### Building `f^(n)`

With `a^n` and `inv(a - 1)`, we can build it:

`f^n(x) = a^nx + b(inv(a-1))`

# Finalizing

Now we know how to:

1. Invert each shuffling operation (cut, deal with increment, deal into new stack)
2. Compose multiple shuffling operations into a single operation that is equivalent to a single pass over all the reversed shuffling operations from the input
3. Apply the composed shuffling operations many times over

So we can finally answer:

```
f^n(2020) = (a^n)*2020 + b(inv(a-1))
```