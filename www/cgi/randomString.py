import random
import string

def generate_string(length=16):
	# chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFIHIJKLMNOPQRSTUVWXYZ"
	chars = string.ascii_letters + string.digits
	res = ""

	for _ in range(length):
		res += random.choice(chars)
	
	return res