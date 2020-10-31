import random
import datetime

# SHUFFLE FUNC
def shuffle_once(lista):
    first = lista[0]
    lista = lista[1:]
    lista.append(first)
    return lista

# INVERSE SHUFFLE FUNC
def ishuffle(lista):
    tmp = lista[:-1]
    last = lista[len(lista)-1]
    tmp.insert(0, last)
    return tmp

# FLIP LAST BIT
def change_last_bit(in_current):
    output = []
    if in_current[len(in_current) - 1] == 0:
            last = 1
    else:
        last = 0
    output = in_current[:-1]
    output.append(last)
    return output

# GET THE OUTPUT FROM A BLOCK BASED ON THE CONNECTION TYPE (D/I)
def out_block(in_current, leg):
    output = []
    last = -1
    if leg == "DIRECT":
        # direct => in_current = output from block
        output = in_current
    else:
        output = change_last_bit(in_current)
    return output

# GET THE INPUT IN THE CURRENT BLOCK BASED ON THE SHUFFLE ALGORITM
def in_block(out_previous, shuffle_type):
    in_current = []
    if shuffle_type == "SHUFFLE":
        in_current = shuffle_once(out_previous)
    else:
        in_current = ishuffle(out_previous)
    return in_current

def print_level(leg):
    print(f"{leg}")

# BENES 4X4 FUNCTIONS
def get_leg(in_current, out):
    if in_current[len(in_current) - 1] == out[0]:
        leg = "DIRECT"
    else:
        leg = "INVERS"
    return leg

def get_last_leg(in_current, out):
    if in_current[len(in_current) - 1] == out[len(out)-1]:
        leg = "DIRECT"
    else:
        leg = "INVERS"
    return leg

def random_choose():
    a =  random.uniform(0, 1)
    if a < 0.5:
        return "DIRECT"
    return "INVERS"

# RECURSIVE BENES FUNCTION
def benes(in_current, output, leg):
    if len(in_current) == 2:
        print_level(leg)

        out_b0 = out_block(in_current, leg)
        in_b1 = in_block(out_b0, "ISHUFFLE")
        leg = get_leg(in_b1, output)
        print_level(leg)

        out_b1 = out_block(in_b1, leg)
        in_b2 = in_block(out_b1, "SHUFFLE")
        leg = get_last_leg(in_b2, output)
        print_level(leg)
        return

    print(leg)

    out_b0 = out_block(in_current, leg)
    in_b1 = in_block(out_b0, "ISHUFFLE")

    block_index = in_b1[0]
    in_next_benes = in_b1[1:]
    out_next_benes = output[:-1]
    
    # obs aici trb introdusa si varianta cu INVERS 
    benes(in_next_benes, out_next_benes, random_choose())

    out_next_benes.insert(0, block_index)
    in_b2 = in_block(out_next_benes, "SHUFFLE")
    leg = get_last_leg(in_b2, output)
    print_level(leg)

if __name__ == '__main__':

    # ADD INPUT AND OUTPUT TO TEST
    input = [0, 0, 1, 0]
    out = [0, 0, 0, 1]
    print(f"INPUT: {input}, OUTPUT: {out}")

    print("The algoritm makes random choices between DIRECT/INVERS at each step")
    print("Two consecutive runs could produce different (but correct) output")
    benes(input, out, random_choose())