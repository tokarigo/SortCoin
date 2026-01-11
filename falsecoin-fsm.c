#include <stdio.h>
#include <string.h>

typedef enum {
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

/* ---------- weigh ---------- */
Event weigh(int left, int right) {
    if (left > right) return LEFT;
    if (left < right) return RIGHT;
    return BALANCED;
}

int main(void) {

    /* ---------- setup coins ---------- */
    int coins[12];
    for (int i = 0; i < 12; i++) coins[i] = 1;

    int coin_number;
    char wt;

    printf(
        "This program shows how to find one fake coin among 12 coins,\n"
        "without knowing whether the fake is heavier or lighter.\n"
        "Enter the false coin number and H/L (e.g. 3 L, 5h): "
    );
    scanf("%d %c", &coin_number, &wt);

    coins[coin_number - 1] = (wt == 'H' || wt == 'h') ? 2 : 0;

    printf("\nCoin storage array:\n");
    for (int i = 0; i < 12; i++)
        printf("Coin %d: %d\n", i + 1, coins[i]);

    /* ---------- groups ---------- */
    int A[4], B[4], C[4];
    for (int i = 0; i < 4; i++) {
        A[i] = coins[i];
        B[i] = coins[i + 4];
        C[i] = coins[i + 8];
    }

    printf("\nCoins grouped into 3 equal parts, A (1-4), B (5-8) and C (9-12)\n");

    printf("\nGroup A: ");
    for (int i = 0; i < 4; i++) printf("A%d: %d ", i + 1, A[i]);

    printf("\nGroup B: ");
    for (int i = 0; i < 4; i++) printf("B%d: %d ", i + 1, B[i]);

    printf("\nGroup C: ");
    for (int i = 0; i < 4; i++) printf("C%d: %d ", i + 1, C[i]);

    printf("\n\nGroups A and B selected to start with, comparing them in TRY 1...\n");

    /* ---------- FSM ---------- */
    State state = TRY1_AB;
    Event firstresAB = BALANCED;

    char leftrel[8] = "";
    char rightrel[8] = "";
    char fake_coin[4] = "";
    char fake_relation[8] = "unknown";

    while (state != DONE) {

        /* ---------- TRY 1 ---------- */
        if (state == TRY1_AB) {
            printf("\nTRY 1: Weighing A vs B\n");
            Event e = weigh(A[0]+A[1]+A[2]+A[3],
                            B[0]+B[1]+B[2]+B[3]);

            if (e == BALANCED) {
                printf("A equals B → fake is in C\n");
                state = TRY2_C12;
            } else {
                firstresAB = e;
                if (e == LEFT) {
                    strcpy(leftrel, "heavier");
                    strcpy(rightrel, "lighter");
                } else {
                    strcpy(leftrel, "lighter");
                    strcpy(rightrel, "heavier");
                }
                printf(
                    "A is %s than B → C is genuine\n",
                    leftrel
                );
                state = TRY2_SWAP;
            }
        }

        /* ---------- TRY 2: C1+C2 vs A1+A2 ---------- */
        else if (state == TRY2_C12) {
            printf("\nTRY 2: Weighing C1+C2 vs A1+A2\n");
            Event e = weigh(C[0]+C[1], A[0]+A[1]);

            if (e == BALANCED) {
                printf(
                    "C1 and C2 are genuine, looking for fake in C3 or C4\n"
                );
                state = TRY3_C3_A3;
            } else {
                strcpy(leftrel, (e == LEFT) ? "heavier" : "lighter");
                printf(
                    "C1 or C2 is fake and %s, looking for fake...\n",
                    leftrel
                );
                state = TRY3_C1_A1;
            }
        }

        /* ---------- TRY 3: C1 vs A1 ---------- */
        else if (state == TRY3_C1_A1) {
            printf("\nTRY 3: Weighing C1 vs A1\n");
            Event e = weigh(C[0], A[0]);

            if (e == BALANCED) {
                strcpy(fake_coin, "C2");
                strcpy(fake_relation, leftrel);
                printf(
                    "C1 is genuine → fake is %s (%s)\n",
                    fake_coin, fake_relation
                );
            } else {
                strcpy(fake_coin, "C1");
                strcpy(fake_relation, (C[0] > A[0]) ? "heavier" : "lighter");
                printf(
                    "%s is fake (%s)\n",
                    fake_coin, fake_relation
                );
            }
            state = DONE;
        }

        /* ---------- TRY 3: C3 vs A3 ---------- */
        else if (state == TRY3_C3_A3) {
            printf("\nTRY 3: Weighing C3 vs A3\n");
            Event e = weigh(C[2], A[2]);

            if (e == BALANCED) {
                strcpy(fake_coin, "C4");
                strcpy(fake_relation,
                       rightrel[0] ? rightrel : "unknown");
                printf(
                    "C3 is genuine → fake is %s (%s)\n",
                    fake_coin, fake_relation
                );
            } else {
                strcpy(fake_coin, "C3");
                strcpy(fake_relation, (C[2] > A[2]) ? "heavier" : "lighter");
                printf(
                    "%s is fake (%s)\n",
                    fake_coin, fake_relation
                );
            }
            state = DONE;
        }

        /* ---------- TRY 2 SWAP ---------- */
        else if (state == TRY2_SWAP) {
            printf(
                "\nTRY 2: Removing A4 and B4, swapping A2,A3 with B2,B3,\n"
                "and replacing A2 with C2.\n"
                "Weighing A1+B2+B3 vs B1+C2+A3\n"
            );

            Event e = weigh(A[0]+B[1]+B[2],
                            B[0]+C[1]+A[2]);

            if (e == BALANCED) {
                printf(
                    "Scales balanced → fake is among A2, A4, B4\n"
                );
                state = TRY3_A2_A4;
            }
            else if (e == firstresAB) {
                printf(
                    "Scales stayed same → fake is A1 or B1\n"
                );
                state = TRY3_A1_C1;
            }
            else {
                printf(
                    "Scales flipped → fake is B2, B3 or A3\n"
                );
                state = TRY3_B2A3;
            }
        }

        /* ---------- TRY 3: A1 vs C1 ---------- */
        else if (state == TRY3_A1_C1) {
            printf("\nTRY 3: Weighing A1 vs C1\n");
            Event e = weigh(A[0], C[0]);

            if (e == BALANCED) {
                strcpy(fake_coin, "B1");
                strcpy(fake_relation, rightrel);
                printf(
                    "A1 is genuine → fake is %s (%s)\n",
                    fake_coin, fake_relation
                );
            } else {
                strcpy(fake_coin, "A1");
                strcpy(fake_relation, leftrel);
                printf(
                    "%s is fake (%s)\n",
                    fake_coin, fake_relation
                );
            }
            state = DONE;
        }

        /* ---------- TRY 3: B2+A3 vs C2+C3 ---------- */
        else if (state == TRY3_B2A3) {
            printf("\nTRY 3: Weighing B2+A3 vs C2+C3\n");
            Event e = weigh(B[1]+A[2], C[1]+C[2]);

            if (e == BALANCED) {
                strcpy(fake_coin, "B3");
                strcpy(fake_relation, rightrel);
                printf(
                    "B2 and A3 are genuine → fake is %s (%s)\n",
                    fake_coin, fake_relation
                );
            }
            else if (e == LEFT) {
                strcpy(fake_coin, "A3");
                strcpy(fake_relation, leftrel);
                printf(
                    "%s is fake (%s)\n",
                    fake_coin, fake_relation
                );
            }
            else {
                strcpy(fake_coin, "B2");
                strcpy(fake_relation, rightrel);
                printf(
                    "%s is fake (%s)\n",
                    fake_coin, fake_relation
                );
            }
            state = DONE;
        }

        /* ---------- TRY 3: A2 vs A4 ---------- */
        else if (state == TRY3_A2_A4) {
            printf("\nTRY 3: Weighing A2 vs A4\n");
            Event e = weigh(A[1], A[3]);

            if (e == BALANCED) {
                strcpy(fake_coin, "B4");
                strcpy(fake_relation, rightrel);
                printf(
                    "A2 equals A4 → fake is %s (%s)\n",
                    fake_coin, fake_relation
                );
            }
            else if (e == firstresAB) {
                strcpy(fake_coin, "A2");
                strcpy(fake_relation, leftrel);
                printf(
                    "%s is fake (%s)\n",
                    fake_coin, fake_relation
                );
            }
            else {
                strcpy(fake_coin, "A4");
                strcpy(fake_relation, leftrel);
                printf(
                    "%s is fake (%s)\n",
                    fake_coin, fake_relation
                );
            }
            state = DONE;
        }
    }

    printf(
        "\nResult: %s is the fake coin, and it is %s.\n",
        fake_coin, fake_relation
    );

    return 0;
}