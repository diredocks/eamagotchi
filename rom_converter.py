data = open("rom.bin", "rb").read()

bits = ''.join(f'{int.from_bytes(data[i:i+2], "big") & 0xfff:012b}'
               for i in range(0, len(data) - 1, 2))

out = bytes(int(bits[i:i+8], 2) for i in range(0, len(bits) - 7, 8))

print("#ifndef _ROM_H_")
print("#define _ROM_H_")
print("#include <pgmspace.h>\n")
print("static const unsigned char g_program[] PROGMEM = {")
for i in range(0, len(out), 12):
    print('  ', end="")
    print(', '.join(f'0x{x:02x}' for x in out[i:i+12]), end=",\n")
print("};\n")
print("#endif")
