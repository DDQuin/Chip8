import sys
if len(sys.argv) == 1:
    print("No file provided")
    exit()
f = open(sys.argv[1], "r")
hex_list = []
for line in f:
    for word in line.split():
        hex_byte = word.lower()[2:]
        hex_list.append(hex_byte)
hex_string = "".join(hex_list)
with open(f'{sys.argv[1]}.ch8', "wb") as bin_file:
    bin_file.write(bytes.fromhex(hex_string))
f.close()
