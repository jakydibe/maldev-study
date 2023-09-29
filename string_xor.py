def XOR(data, key):
    """XORs two byte arrays."""
    result = bytearray(len(data))
    for i in range(len(data)):
        result[i] = data[i] ^ key[i % len(key)]
    return result

def add_null_byte_terminator(string):
    """Adds a null byte terminator to a string."""
    return string + b"\0"

def main():
    string = input("Write your string here (max 1023 char)----------------------->  ")
    key = input("Write your key here (max 1023 char)----------------------->  ")

    encrypted_string = XOR(string.encode(), key.encode())

    encrypted_string = add_null_byte_terminator(encrypted_string)

    decrypted_string = XOR(encrypted_string, key.encode())

    print("Decrypted string:", decrypted_string.decode())

    print('\nunsigned char payload[] = { 0x' + ', 0x'.join(hex(x)[2:] for x in encrypted_string) + ' };')


if __name__ == "__main__":
    main()