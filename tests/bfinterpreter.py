import sys

def interpret(code, input_stream):
    tape = [0] * 30000
    ptr = 0  # puntatore
    pc = 0   # program counter
    input_ptr = 0
    output = []

    # Preprocessing: mappa le parentesi [ ]
    bracket_map = {}
    stack = []

    for i, cmd in enumerate(code):
        if cmd == '[':
            stack.append(i)
        elif cmd == ']':
            start = stack.pop()
            bracket_map[start] = i
            bracket_map[i] = start

    # Esecuzione
    while pc < len(code):
        cmd = code[pc]

        if cmd == '>':
            ptr += 1
        elif cmd == '<':
            ptr -= 1
        elif cmd == '+':
            tape[ptr] = (tape[ptr] + 1) % 256
        elif cmd == '-':
            tape[ptr] = (tape[ptr] - 1) % 256
        elif cmd == '.':
            output.append(chr(tape[ptr]))
        elif cmd == ',':
            if input_ptr < len(input_stream):
                tape[ptr] = ord(input_stream[input_ptr])
                input_ptr += 1
            else:
                tape[ptr] = 0
        elif cmd == '[':
            if tape[ptr] == 0:
                pc = bracket_map[pc]
        elif cmd == ']':
            if tape[ptr] != 0:
                pc = bracket_map[pc]

        pc += 1

    return ''.join(output)

if len(sys.argv)!=3:
    print("Usage: python bfinterpreter.py <filename bf> <filename arg>\n")
    print("Provide a long string as argument to ensure the input isn't enough")
with open(sys.argv[1], "r") as f:
    with open(sys.argv[2],"r") as f2:
        print(interpret(f.read(),f2.read()))