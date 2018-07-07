import random

def miller_rabin_primality_test(n):
	def get_r_d(N):
		N -= 1
		r = 0
		d = N
		while True:
			if N % 2 != 0:
				break
			r += 1
			d = N // 2
			N //= 2
		return r, d

	test_numbers = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97]

	# try division
	for number in test_numbers:
		if n % number == 0:
			return False

	# Miller-Rabin
	r, d = get_r_d(n)
	for k in range(len(test_numbers)):
		a = random.choice(test_numbers)
		test_numbers.remove(a)
		x = pow(a, d, n)
		if x == 1 or x == n - 1:
			continue
		for t in range(r - 1):
			x = pow(x, 2, n)
			if x == 1:
				return False
			if x == n - 1:
				break
		else:
			return False
	return True



# use 100 512-bit-long odd numbers to test
if __name__ == '__main__':
	with open('test.txt', 'w') as f:
		for _ in range(100):
			n = '1'
			for i in range(510):
				n += str(random.randint(0, 1))
			n = n + '1'
			f.write(n + '\n')
			n = int(n, 2)
			f.write(str(n) + '\n')
			f.write(str(miller_rabin_primality_test(n)) + '\n')
