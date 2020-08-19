#!/usr/bin/env python3
from random import choice

# Costanti
MAXLEN = 1024
# BRUH AGGIUNGI 'd', 'u', 'r' SE VUOI DELETE, UNDO E REDO
letters = ['c', 'p']

maxfilelen = [0]

index = 0

# Numero di comandi del test
length = int(input("Quanti comandi vuoi brutto gay? "))

# Crea un array con le frasi da usare nelle Change
quotes = []
f = open("quotes.txt", "r")
for line in f:
    quotes.append(line)
f.close()

# Crea il test
f = open("test.txt", "w")

# Ciclo principale
for i in range(length - 1):
    # sceglie a caso una lettera tra le 5
    letter = choice(letters)

    # Undo o Redo
    if letter == 'u' or letter == 'r':
        # sceglie un numero a caso tra 0 e 1024
        num = choice(range(MAXLEN))
        # crea il comando e lo mette nella stringa s
        s = '{}{}'.format(num, letter)

        # tiene traccia dell'indice: per le Undo si sposta
        # indietro, per le Redo avanti
        if letter == 'u':
            index -= num
            if index < 0:
                index = 0
        else:
            index += num
            if index > len(maxfilelen) - 1:
                index = len(maxfilelen) - 1

    # Delete o Print
    elif letter == 'd' or letter == 'p':
        # sceglie due numeri a caso tra 0 e 1024, dove il secondo
        # è maggiore del primo
        num0 = choice(range(MAXLEN))
        num1 = choice(range(num0, MAXLEN))
        # crea il comando e lo mette nella stringa s
        s = '{},{}{}'.format(num0, num1, letter)

        # se è una Delete, tiene traccia della nuova lunghezza del file
        if letter == 'd':
            # se non è alla fine della lista maxfilelen, vuol dire che
            # c'è stata un'Undo precedentemente, ma ora elimina tutto
            # quello che viene dopo l'indice attuale
            if index != len(maxfilelen) - 1:
                maxfilelen = maxfilelen[:index + 1]

            # se non viene eliminato niente (il file era di lunghezza
            # 0 precedentemente o sta cercando di eliminare qualcosa che
            # viene dopo il file), la lunghezza rimane invariata
            if (maxfilelen[index] == 0 or num0 > maxfilelen[index]):
                maxfilelen.append(maxfilelen[index])
            # se sta eliminando tutto il file, la lunghezza è 0
            elif maxfilelen[index] <= num1 and (num0 == 1 or num0 == 0):
                maxfilelen.append(0)
            # altrimenti, scrive la lunghezza, quella precedente meno
            # il numero di righe cancellate
            else:
                num1 = min(maxfilelen[index], num1)
                maxfilelen.append(maxfilelen[index] - (num1 - num0 + 1))
            index += 1

    # Change
    else:
        # se la lunghezza attuale del file è 0, il comando deve per forza
        # iniziare da 1
        if maxfilelen[index] == 0:
            num0 = 1
        # altrimenti, numero casuale tra 1 e la lunghezza attuale + 1
        else:
            num0 = choice(range(1, maxfilelen[index] + 1))
        num1 = choice(range(num0, MAXLEN))

        # crea il comando e lo mette nella stringa s
        s = '{},{}{}\n'.format(num0, num1, letter)
        # aggiunge alla stringa il numero stabilito di frasi, e il . finale
        for j in range(num1 - num0 + 1):
            s += choice(quotes)
        s += '.'

        # se non è alla fine della lista maxfilelen, vuol dire che
        # c'è stata un'Undo precedentemente, ma ora elimina tutto
        # quello che viene dopo l'indice attuale e aggiunge il nuovo
        # valore di lunghezza del file
        if index != len(maxfilelen) - 1:
            maxfilelen = maxfilelen[:index + 1]
        maxfilelen.append(max(num1, maxfilelen[index]))
        index += 1

    # scrive il comando s nel test
    f.write(s + '\n')

# Scrive il q finale nel test e chiude il file di test
f.write('q\n')
f.close()
