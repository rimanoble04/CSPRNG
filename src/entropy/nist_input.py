import os
import time  # To add small delays if needed

# Required bit length for NIST STS (minimum 100,000 bits)
REQUIRED_BITS = 100000

# Paths
OUTPUT_FILE = "CSPRNG\\src\\entropy\\nist\\sts-2.1.2\\experiments\\AlgorithmTesting\\nist_input.data"

# Ensure the STS directory exists before writing
os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)

# Function to generate and append bits
def generate_bits():
    """ Reads binary data, converts to bitstream, writes to nist_input.txt and then to nist_input.data """
    
    # Read binary input
    with open("output.txt", "rb") as f:
        binary_data = f.read()

    # Convert binary data to a bitstream
    bit_string = ''.join(f'{byte:08b}' for byte in binary_data)

    # Ensure bit length is a multiple of 8
    bit_length = len(bit_string)
    if bit_length % 8 != 0:
        bit_string = bit_string[:-(bit_length % 8)]  # Trim extra bits if needed

    # Append bitstream to `nist_input.txt`
    with open(OUTPUT_FILE, "a") as f:
        f.write(bit_string)
        f.flush()  # Force writing to disk
        os.fsync(f.fileno())  # Ensure data is saved

    print(f"Appended {len(bit_string)} bits to '{OUTPUT_FILE}'.")

    
generate_bits()  # Final write operation to `nist_input.data`
