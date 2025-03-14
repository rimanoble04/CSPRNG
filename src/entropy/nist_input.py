# Read the raw binary data from the file
with open("output.txt", "rb") as f:
    binary_data = f.read()

# Convert binary data to a bitstream (0s and 1s)
bit_string = ''.join(f'{byte:08b}' for byte in binary_data)  # Convert each byte to 8-bit binary

# Ensure the bit length is a multiple of 8 for NIST
bit_length = len(bit_string)
if bit_length % 8 != 0:
    bit_string = bit_string[:-(bit_length % 8)]  # Trim extra bits if needed

# Save bitstream in a text file for NIST STS
with open("nist_input.txt", "w") as f:
    f.write(bit_string)

print(f"Binary data converted to bitstream and saved to 'nist_input.txt' ({len(bit_string)} bits).")
