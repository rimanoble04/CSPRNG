import os

# Required bit length for NIST STS (minimum 100,000 bits)
REQUIRED_BITS = 100000

# Path to the output file
OUTPUT_FILE = "nist_input.txt"

# Function to generate and append bits
def generate_bits():
    with open("output.txt", "rb") as f:
        binary_data = f.read()

    # Convert binary data to a bitstream
    bit_string = ''.join(f'{byte:08b}' for byte in binary_data)

    # Ensure the bit length is a multiple of 8
    bit_length = len(bit_string)
    if bit_length % 8 != 0:
        bit_string = bit_string[:-(bit_length % 8)]  # Trim extra bits if needed

    # Append bitstream to `nist_input.txt`
    with open(OUTPUT_FILE, "a") as f:
        f.write(bit_string)

    print(f"Appended {len(bit_string)} bits to '{OUTPUT_FILE}'.")
    
    with open(OUTPUT_FILE, "r") as f:
        full_bitstream = f.read()
        print(f"Size of full_bitstream: {len(full_bitstream)} bits")

    
    with open("sts-2.1.2\\sts-2.1.2\\experiments\\AlgorithmTesting\\nist_input.data","w") as fr:
        fr.write(full_bitstream)

# **Keep generating bits until we reach REQUIRED_BITS**
while True:
    # Check the current size of the file
    if os.path.exists(OUTPUT_FILE):
        current_size = os.path.getsize(OUTPUT_FILE) * 8  # Convert bytes to bits
    else:
        current_size = 0

    if current_size >= REQUIRED_BITS:
        print(f"Enough bits collected ({current_size} bits). Ready for NIST STS!")
        break  # Stop when we have enough bits

    print(f"Current size: {current_size} bits. Generating more...")
    generate_bits()  # Generate and append more bits



# wc -c nist_input.txt