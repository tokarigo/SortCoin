from enum import Enum, auto

class State(Enum):
    START = auto()
    TRY1_AB = auto()
    TRY2_C12 = auto()
    TRY2_SWAP = auto()
    TRY3_A1_C1 = auto()
    TRY3_C1_A1 = auto()
    TRY3_C4_A4 = auto()
    TRY3_B2A3 = auto()
    TRY3_A2_A4 = auto()
    DONE = auto()


class Event(Enum):
    LEFT = auto()
    RIGHT = auto()
    BALANCED = auto()


def weigh(left, right):
    if left > right:
        return Event.LEFT
    elif left < right:
        return Event.RIGHT
    else:
        return Event.BALANCED


def solve_12_coin_fsm(A, B, C):
    state = State.TRY1_AB
    firstresAB = None
    fake_coin = None
    fake_relation = None  # 'heavier' / 'lighter' / 'unknown'

    while state != State.DONE:

        # ---------------- TRY 1 ----------------
        if state == State.TRY1_AB:
            print("\nTRY 1: Weighing A vs B")
            event = weigh(sum(A), sum(B))

            if event == Event.BALANCED:
                print("A equals B → fake is in C")
                state = State.TRY2_C12
            else:
                firstresAB = event
                leftrel = "heavier" if event == Event.LEFT else "lighter"
                rightrel = "lighter" if event == Event.LEFT else "heavier"
                print(f"A is {leftrel} than B → C is genuine")
                state = State.TRY2_SWAP

        # ---------------- TRY 2 (C1+C2+C3 vs A1+A2+A3) ----------------
        elif state == State.TRY2_C12:
            print("\nTRY 2: Weighing C1+C2+C3 vs A1+A2+C3")
            event = weigh(C[0] + C[1]+C[2], A[0] + A[1]+A[3])

            if event == Event.BALANCED:
                print("C1+C2+C3 are genuine, the fake is C4")
                state = State.TRY3_C4_A4
  #              c_path = "34"
            else:
                leftrel = "heavier" if event == Event.LEFT else "lighter"
                print(f"C1/C2/C3 is fake and {leftrel}, looking for fake...")
                rightrel = "lighter" if event == Event.LEFT else "heavier"
                state = State.TRY3_C1_A1
#                c_path = "12"
#               c_event = event
                
######## chexk above


        # ---------------- TRY 2 SWAP ----------------
        elif state == State.TRY2_SWAP:
            print("\nTRY 2: Rwmoving A4 and B4, swapping A2, A3 with B2, B3, and replacing A2 with C2.\nAs a result, weighing A1+B2+B3 vs B1+C2+A3")
            event = weigh(A[0] + B[1] + B[2], B[0] + C[1] + A[2])

            same_direction = (event == firstresAB)

            if event == Event.BALANCED:
                print("\nAfter TRY 2 (weighing A1+B2+B3 vs B1+C2+A3) the scles became balanced so fake coin is within those removed (A2, A4, B4)")
                state = State.TRY3_A2_A4
            elif same_direction:
                print("\nAfter TRY 2 (weighing A1+B2+B3 vs B1+C2+A3) the scles stayed in the same posituon so fake coin is either A1 or B1")
                state = State.TRY3_A1_C1
            else:
                print("\nAfter TRY 2 (weighing A1+B2+B3 vs B1+C2+A3) the scles flipped to the other side so fake coin is either B2, B3 or A3 (coins which flipped sides)")
                state = State.TRY3_B2A3

        # ---------------- TRY 3: A1 vs C1 ----------------
        elif state == State.TRY3_A1_C1:
            print("\nTRY 3: Weighing A1 vs C1")
            event = weigh(A[0], C[0])

            if event == Event.BALANCED:
                fake_coin = "B1"
                fake_relation = rightrel if 'rightrel' in locals() else "unknown"
                print(f"A1 is genuine → fake is {fake_coin} ({fake_relation})")
            else:
                fake_coin = "A1"
                fake_relation = "heavier" if A[0] > C[0] else "lighter"
                print(f"{fake_coin} is fake ({fake_relation})")

            state = State.DONE


        # ---------------- TRY 3: C1/C2/C3 (v3)  ----------------
        elif state == State.TRY3_C1_A1:
            print("\nTRY 3: Weighing C1 vs C2 (C3 put aside)")
            event = weigh(C[0], C[1])

            if event == Event.BALANCED:
                fake_coin = "C3"
                fake_relation = leftrel
                print(f"C1 and C2 are genuine → fake is {fake_coin} ({fake_relation})")
            elif (event == Event.LEFT and leftrel == "heavier") or (event == Event.RIGHT and leftrel == "lighter"):
                fake_coin = "C1"
                fake_relation = leftrel
                print(f"{fake_coin} is fake ({fake_relation})")
            else:
                fake_coin = "C2"
                fake_relation = leftrel
                print(f"{fake_coin} is fake ({fake_relation})")



            state = State.DONE

     # ---------------- TRY 3: C4 vs A4 (v3) ----------------
        elif state == State.TRY3_C4_A4:
            print("\nTRY 3: Weighing C4 vs A4")
            event = weigh(C[3], A[3])
            
            fake_coin = "C4"
            if event == Event.LEFT:
                fake_relation = "heavier"
            else: fake_relation = "lighter"
            print(f"{fake_coin} is fake ({fake_relation})")

            state = State.DONE


        # ---------------- TRY 3: B2+A3 vs C2+C3 ----------------
        elif state == State.TRY3_B2A3:
            print("\nTRY 3: Weighing B2+A3 vs C2+C3")
            event = weigh(B[1] + A[2], C[1] + C[2])

            if event == Event.BALANCED:
                fake_coin = "B3"
                fake_relation = rightrel if 'rightrel' in locals() else "unknown"
                print(f"Coins B2 and A3 are genuine → fake is {fake_coin} ({fake_relation})")
            elif event == Event.LEFT:
                fake_coin = "A3"
                fake_relation = "heavier" if leftrel == "heavier" else "lighter"
                print(f"{fake_coin} is fake ({fake_relation})")
            else:
                fake_coin = "B2"
                fake_relation = rightrel if 'rightrel' in locals() else "unknown"
                print(f"{fake_coin} is fake ({fake_relation})")

            state = State.DONE

        # ---------------- TRY 3: A2 vs A4 ----------------
        elif state == State.TRY3_A2_A4:
            print("\nTRY 3: Weighing A2 vs A4")
            event = weigh(A[1], A[3])

            if event == Event.BALANCED:
                fake_coin = "B4"
                fake_relation = rightrel if 'rightrel' in locals() else "unknown"
                print(f"A2 equals A4 → fake is {fake_coin} ({fake_relation})")
            elif event == firstresAB:
                fake_coin = "A2"
                fake_relation = leftrel
                print(f"{fake_coin} is fake ({fake_relation})")
            else:
                fake_coin = "A4"
                fake_relation = leftrel
                print(f"{fake_coin} is fake ({fake_relation})")

            state = State.DONE

    return fake_coin, fake_relation
    

# Step 1: Create storage array for 12 coins (indexed 1–12 for user clarity)
coins = [1] * 12  # initialize all as genuine coins (value = 1)


# Step 2: Ask user for false coin location and weight difference
user_input = input(
    "This program shows how to find one fake coin among 12 coins, without knowing wether the fake is heavier or lighter than any genuine coin. Enter the false coin number and whether it is heavier or lighter (e.g., '3 L', '5h'): "
)

# Clean and parse input
user_input = user_input.replace(" ", "").lower()

# Extract coin number and weight indicator
coin_number = int("".join(filter(str.isdigit, user_input)))
weight_type = "".join(filter(str.isalpha, user_input))

# Step 3: Set false coin value
if weight_type == "h":
    false_value = 2  # heavier
elif weight_type == "l":
    false_value = 0  # lighter
else:
    raise ValueError("Invalid input: must specify H/h or L/l")

# Place false coin in array (convert to 0-based index)
coins[coin_number - 1] = false_value

# Step 4: Print the array showing member number and value
print("\nCoin storage array:")
for i, value in enumerate(coins, start=1):
    print(f"Coin {i}: {value}")

# --------------------------------------------------
# Grouping and comparison
# --------------------------------------------------

# Step 1: Create sub-arrays
A = coins[0:4]
B = coins[4:8]
C = coins[8:12]

print("\nCoins grouped into 3 equal parts, A (1-4), B (5-8) and C (9-12)")


# Print subarrays A, B, and C with member number and value
print(
    "\nGroup A: " +
    ", ".join(f"A{i+1}: {A[i]}" for i in range(len(A)))
)

print(
    "Group B: " +
    ", ".join(f"B{i+1}: {B[i]}" for i in range(len(B)))
)

print(
    "Group C: " +
    ", ".join(f"C{i+1}: {C[i]}" for i in range(len(C)))
)


# Step 2: Select A and B
print("Groups A and B selected to start with, comparing them in TRY 1...")

fake_coin, fake_relation = solve_12_coin_fsm(A, B, C)
print(f"\nResult: {fake_coin} is the fake coin, and it is {fake_relation}.")


on}.")

Groups A and B selected to start with, comparing them in TRY 1...")

fake_coin, fake_relation = solve_12_coin_fsm(A, B, C)
print(f"\nResult: {fake_coin} is the fake coin, and it is {fake_relation}.")

