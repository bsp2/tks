def test_lists():
    Li1 = range(1, 10000 + 1)
    Li2 = Li1[:]
    Li3 = []

    # remove each individual item from left side of Li2 and
    # append to right side of Li3 (preserving order)
    # 
    # popping the first element is *expensive*
    #
    #while Li2:
    #    Li3.append(Li2.pop(0))   
    Li2.reverse()
    while Li2:
        Li3.append(Li2.pop())
    while Li3:
        Li2.append(Li3.pop())
    Li1.reverse()
    if Li1[0] != 10000:
        return 0
    if Li1 == Li2:
        return len(Li1)
    else:
        return 0

def main():
    NUM = 512 
    if NUM < 1:
        NUM = 1
    while NUM > 0:
        result = test_lists()
        NUM = NUM - 1
    print result

main()