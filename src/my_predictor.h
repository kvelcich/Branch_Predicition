// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.

class my_update : public branch_update {
public:
	unsigned int index;
};

class my_predictor : public branch_predictor {
public:
#define HISTORY_LENGTH	15
#define TABLE_BITS	15
	my_update u;
	branch_info bi;
	unsigned int history;
	int tab[1<<TABLE_BITS];

	my_predictor (void) : history(0) {
		memset (tab, 0, sizeof (tab));
	}

	branch_update *predict (branch_info & b) {
		bi = b;
		if (b.br_flags & BR_CONDITIONAL) {
			u.index = history;
			int value = tab[u.index];
			if (value > 1) u.direction_prediction (true);
			else u.direction_prediction (false);
		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {
			int *c = &tab[((my_update*)u)->index];
			if (taken) {
				if (*c == 0) *c = 1;
				if (*c == 1) *c = 3;
				if (*c == 2) *c = 3;
			} else {
				if (*c == 1) *c = 0;
				if (*c == 2) *c = 0;
				if (*c == 3) *c = 2;
			}
			history <<= 1;
			history |= taken;
			history &= (1<<HISTORY_LENGTH)-1;
		}
	}
};
