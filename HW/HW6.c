#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int N, M, L;
/*store information, identity, solution respectively*/
int statements[100];
/*0 represents human, 1 represents werewolf*/
int werewolves[100];
int solution[100];
bool found = false;

/*check if the solution is reasonable*/
bool check_solution() {
    int liar_count = 0;
    int wolf_liar_count = 0;

    for (int i = 0; i < N; i++) {
        int said = statements[i];
        int target = abs(said) - 1;  // 0-indexed
        bool is_wolf_target = (werewolves[target] == 1);
        
        // judge whether lying
        bool is_lying = false;
        if (said > 0) {  // if said human
            if (is_wolf_target) is_lying = true;
        }
        else {  // if said werewolf
            if (!is_wolf_target) is_lying = true;
        }
        
        if (is_lying) {
            liar_count++;
            if (werewolves[i] == 1) {  // whether a lier
                wolf_liar_count++;
            }
        }
    }
    
    return liar_count == L && wolf_liar_count >= 1 && wolf_liar_count < M;
}

/*backtrack using sequence*/
void backtrack(int pos, int wolves_chosen) {
    if (wolves_chosen == M) {
        if (check_solution()) {
            if (!found) {
                //save solution
                for (int i = 0; i < N; i++) {
                    solution[i] = werewolves[i];
                }
                found = true;
            }
        }
        return;
    }
    
    if (pos < 0) return;
    
    //pruning
    if (wolves_chosen + pos + 1 < M) return;
    int wolves_needed = M - wolves_chosen;
    if (wolves_needed == pos + 1) {
        //set all the ramaining position to werewolf
        for (int i = pos; i >= 0; i--) {
            werewolves[i] = 1;
        }
        backtrack(-1, M);

        //if isn't the solution, undo the setting
        for (int i = pos; i >= 0; i--) {
            werewolves[i] = 0;
        }
        return;
    }
    
    if (wolves_needed == 0) {
        //set all the remaining position to human
        for (int i = pos; i >= 0; i--) {
            werewolves[i] = 0;
        }
        backtrack(-1, M);

        //if isn't the solution, undo the setting
        for (int i = pos; i >= 0; i--) {
            werewolves[i] = 0;
        }
        return;
    }
    
    //branch: werewolf
    werewolves[pos] = 1;
    backtrack(pos - 1, wolves_chosen + 1);
    
    //branch: human
    werewolves[pos] = 0;
    backtrack(pos - 1, wolves_chosen);
}

int main() {
    scanf("%d %d %d", &N, &M, &L);
    
    for (int i = 0; i < N; i++) {
        scanf("%d", &statements[i]);
    }
    
    //initialize
    for (int i = 0; i < N; i++) {
        werewolves[i] = 0;
        solution[i] = 0;
    }
    
    backtrack(N - 1, 0);
    
    if (found) {
        bool first = true;
        for (int i = N - 1; i >= 0; i--) {
            if (solution[i] == 1) {
                if (!first) printf(" ");
                printf("%d", i + 1);
                first = false;
            }
        }
        printf("\n");
    }
    else {
        printf("No Solution\n");
    }
    
    return 0;
}