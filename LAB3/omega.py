import random
import datetime 

def shuffle_once(lista):
    first = lista[0]
    lista = lista[1:]
    lista.append(first)
    return lista

def show(current, final, level):
    if level == len(current):
        return

    # codul blocului curent (numarul lui in baza 2)
    bloc = list(current[:-1])

    # ultimul bit din numarul curent
    last_from_current = current[len(current) - 1]

    # bitul de pe nivelul curent din output
    current_digit_from_output = final[level]

    # adaugare bit de output la sf current
    bloc.append(current_digit_from_output)

    if last_from_current == current_digit_from_output:
        print(f"LEVEL: {level} DIRECT")
    else:
        print(f"LEVEL: {level} INVERS")

    current = bloc
    print(current)
    show(shuffle_once(current), final, level + 1)

intrare = [0, 0, 1]
iesire = [1, 0, 0]

show(shuffle_once(intrare), iesire, 0)

lists = []

for k in range (1, 6):
    tmp = [i for i in range(0, 2**k)]
    res = map(lambda x : format(x, f'#0{k + 2}b')[2:], tmp)
    tmp = list(res)
    lists.append(tmp)

# print(lists)

for i in range(1, 5):
    print()
    print(f"Random test for {i+1}-bit numbers")
    random.seed(10)
    j = random.randint(1, i)
    intrare = list(lists[i][j])
    k = random.randint(1, i)
    iesire = list(lists[i][k])
    print("Intrare: " + str(intrare))
    print("Iesire: " + str(iesire))
    show(intrare, iesire, 0)

