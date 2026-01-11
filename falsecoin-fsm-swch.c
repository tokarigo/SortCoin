#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* ===================== ENUMS ===================== */

typedef enum {
    START,
    TRY1_AB,
    TRY2_C12,
    TRY2_SWAP,
    TRY3_A1_C1,
    TRY3_C1_A1,
    TRY3_C3_A3,
    TRY3_B2A3,
    TRY3_A2_A4,
    DONE
} State;

typedef enum {
    LEFT,
    RIGHT,
    BALANCED
} Event;

typedef enum {
    HEAVIER,
    LIGHTER,
    UNKNOWN
} Relation;

/* ===================== WEIGH FUNCTION ===================== */

Event weigh(int left, int right) {
    if (left > right) return LEFT;
    if (left < right) return RIGHT;
    return BALANCED;
}

/* ===================== MAIN ===================== */

int main(void) {

    int coins[12];
    int i;

    /* Initialize all coins as genuine */
    for (i = 0; i < 12; i++) coins[i] = 1;

    /* ---------------- USER INPUT ---------------- */
    char input[20];
    printf(
        "This program finds one fake coin among 12 coins.\n"
        "Enter fake coin number and weight (e.g. 4L, 7H, 10 l): "
    );
    fgets(input, sizeof(input), stdin);

    int coin_number = 0;
    char weight_char = 0;

    for (i = 0; input[i]; i++) {
        if (isdigit(input[i]))
            coin_number = coin_number * 10 + (input[i] - '0');
        else if (tolower(input[i]) == 'h' || tolower(input[i]) == 'l')
            weight_char = tolower(input[i]);
    }

    if (coin_number < 1 || coin_number > 12 || weight_char == 0) {
        printf("Invalid input.\n");
        return 1;
    }

    coins[coin_number - 1] = (weight_char == 'h') ? 2 : 0;

    /* ---------------- PRINT COINS ---------------- */
    printf("\nCoin storage array:\n");
    for (i = 0; i < 12; i++)
        printf("Coin %d: %d\n", i + 1, coins[i]);

    /* ---------------- GROUPS ---------------- */
    int *A = &coins[0];
    int *B = &coins[4];
    int *C = &coins[8];

    printf("\nCoins grouped into 3 equal parts, A (1-4), B (5-8) and C (9-12)\n");

    printf("Group A: ");
    for (i = 0; i < 4; i++) printf("A%d:%d ", i + 1, A[i]);
    printf("\n");

    printf("Group B: ");
    for (i = 0; i < 4; i++) printf("B%d:%d ", i + 1, B[i]);
    printf("\n");

    printf("Group C: ");
    for (i = 0; i < 4; i++) printf("C%d:%d ", i + 1, C[i]);
    printf("\n");

    /* ===================== FSM ===================== */

    State state = TRY1_AB;
    Event event, firstresAB;
    Relation leftrel = UNKNOWN, rightrel = UNKNOWN;
    char fake_coin[4] = "";
    Relation fake_relation = UNKNOWN;

    while (state != DONE) {

        switch (state) {

        /* ============ TRY 1 ============ */
        case TRY1_AB: {
            printf("\nTRY 1: Weighing A vs B\n");
            event = weigh(A[0]+A[1]+A[2]+A[3], B[0]+B[1]+B[2]+B[3]);

            if (event == BALANCED) {
                printf("A equals B → fake is in C\n");
                state = TRY2_C12;
            } else {
                firstresAB = event;
                leftrel  = (event == LEFT)  ? HEAVIER : LIGHTER;
                rightrel = (event == LEFT)  ? LIGHTER : HEAVIER;
                printf("After TRY 1, group A is %s than group B. "
                       "The fake is in one of these groups. Group C is genuine.\n",
                       leftrel == HEAVIER ? "heavier" : "lighter");
                state = TRY2_SWAP;
            }
            break;
        }

        /* ============ TRY 2 (C PATH) ============ */
        case TRY2_C12: {
            printf("\nTRY 2: Weighing C1+C2 vs A1+A2\n");
            event = weigh(C[0]+C[1], A[0]+A[1]);

            if (event == BALANCED) {
                printf("Coins C1 and C2 are genuine, fake is in C3 or C4. TRY 3.\n");
                state = TRY3_C3_A3;
            } else {
                printf("Fake is in C1 or C2. TRY 3.\n");
                state = TRY3_C1_A1;
            }
            break;
        }

        /* ============ TRY 2 (SWAP PATH) ============ */
        case TRY2_SWAP: {
            printf(
                "\nTRY 2: Removing A4 and B4, swapping coins.\n"
                "Weighing A1+B2+B3 vs B1+C2+A3\n"
            );
            event = weigh(A[0]+B[1]+B[2], B[0]+C[1]+A[2]);

            if (event == BALANCED) {
                printf("Balanced → fake is in A2, A4 or B4. TRY 3.\n");
                state = TRY3_A2_A4;
            } else if (event == firstresAB) {
                printf("Same direction → fake is A1 or B1. TRY 3.\n");
                state = TRY3_A1_C1;
            } else {
                printf("Direction flipped → fake is B2, B3 or A3. TRY 3.\n");
                state = TRY3_B2A3;
            }
            break;
        }

        /* ============ TRY 3 STATES ============ */

        case TRY3_C1_A1:
            printf("\nTRY 3: Weighing C1 vs A1\n");
            event = weigh(C[0], A[0]);
            if (event == BALANCED) {
                sprintf(fake_coin, "C2");
                fake_relation = (C[1] > 1) ? HEAVIER : LIGHTER;
            } else {
                sprintf(fake_coin, "C1");
                fake_relation = (C[0] > A[0]) ? HEAVIER : LIGHTER;
            }
            state = DONE;
            break;

        case TRY3_C3_A3:
            printf("\nTRY 3: Weighing C3 vs A3\n");
            event = weigh(C[2], A[2]);
            if (event == BALANCED) {
                sprintf(fake_coin, "C4");
                fake_relation = UNKNOWN;
            } else {
                sprintf(fake_coin, "C3");
                fake_relation = (C[2] > A[2]) ? HEAVIER : LIGHTER;
            }
            state = DONE;
            break;

        case TRY3_A1_C1:
            printf("\nTRY 3: Weighing A1 vs C1\n");
            event = weigh(A[0], C[0]);
            if (event == BALANCED) {
                sprintf(fake_coin, "B1");
                fake_relation = rightrel;
            } else {
                sprintf(fake_coin, "A1");
                fake_relation = (A[0] > C[0]) ? HEAVIER : LIGHTER;
            }
            state = DONE;
            break;

        case TRY3_B2A3:
            printf("\nTRY 3: Weighing B2+A3 vs C2+C3\n");
            event = weigh(B[1]+A[2], C[1]+C[2]);
            if (event == BALANCED) {
                sprintf(fake_coin, "B3");
                fake_relation = rightrel;
            } else if (event == LEFT) {
                sprintf(fake_coin, "A3");
                fake_relation = HEAVIER;
            } else {
                sprintf(fake_coin, "B2");
                fake_relation = LIGHTER;
            }
            state = DONE;
            break;

        case TRY3_A2_A4:
            printf("\nTRY 3: Weighing A2 vs A4\n");
            event = weigh(A[1], A[3]);
            if (event == BALANCED) {
                sprintf(fake_coin, "B4");
                fake_relation = rightrel;
            } else if (event == firstresAB) {
                sprintf(fake_coin, "A2");
                fake_relation = leftrel;
            } else {
                sprintf(fake_coin, "A4");
                fake_relation = leftrel;
            }
            state = DONE;
            break;

        default:
            printf("FSM error.\n");
            state = DONE;
        }
    }

    /* ===================== RESULT ===================== */

    printf("\nRESULT: Fake coin is %s and it is %s.\n",
           fake_coin,
           fake_relation == HEAVIER ? "heavier" :
           fake_relation == LIGHTER ? "lighter" : "unknown");

    return 0;
}