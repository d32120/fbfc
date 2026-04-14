import sys
import random
import string

BF_CHARS = ['.', ',', '<', '>', '+', '-']
LOOP_START = '['
LOOP_END = ']'


def generate_bf_code(length, max_loop_depth=10):
    code = []
    loop_stack = []

    for _ in range(length):
        choice = random.random()

        # Occasionally open a loop
        if choice < 0.1 and len(loop_stack) < max_loop_depth:
            code.append(LOOP_START)
            loop_stack.append(len(code) - 1)

        # Occasionally close a loop (only if there is one open)
        elif choice < 0.2 and loop_stack:
            code.append(LOOP_END)
            loop_stack.pop()

        else:
            code.append(random.choice(BF_CHARS))

    # Close any remaining open loops
    while loop_stack:
        code.append(LOOP_END)
        loop_stack.pop()

    return ''.join(code)


def generate_random_text(length):
    return ''.join(random.choice(list(map(chr, range(48, 123)))) for _ in range(length))


def main():
    if len(sys.argv) != 4:
        print("Usage: python fuzzy.py <length> <output_file> <mode>")
        print("mode: 0 = Brainfuck only, 1 = random text")
        sys.exit(1)

    try:
        length = int(sys.argv[1])
    except ValueError:
        print("Error: length must be an integer.")
        sys.exit(1)

    output_file = sys.argv[2]

    try:
        mode = int(sys.argv[3])
    except ValueError:
        print("Error: mode must be 0 or 1.")
        sys.exit(1)

    if mode == 0:
        result = generate_bf_code(length)
    elif mode == 1:
        result = generate_random_text(length)
    else:
        print("Error: mode must be 0 or 1.")
        sys.exit(1)

    try:
        with open(output_file, "w") as f:
            f.write(result)
        print(f"Written to '{output_file}'")
    except IOError as e:
        print(f"An error occurred: {e}")


if __name__ == "__main__":
    main()