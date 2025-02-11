// YOUR NAME: Sonpaorong Muchhim
//
// CS 4318, spring 2024
// Agent Challenge 4: Hunt the wumpus

//    nice bash htwBuild.bash
//    nice ./htwRunSim

#include "htw.h"
#include <stack>
#include <vector>
#include <algorithm>

namespace
{	
// Knowledge Book 
struct KnowledgeBase {
	// Coordination of current pos X axis,Y axis And the position of shooting arrow
	int x = 0, y = 0, shootX, shootY;
	// KnowledgeBase Map for: dangerMap for moves, Breeze & Stench Maps, visited Map, as well as boolen variables to keep track of modes
	string dangerMap[4][4] = {{"Safe","Safe","Safe","Safe"},{"Safe","Safe","Safe","Safe"},{"Safe","Safe","Safe","Safe"},{"Safe","Safe","Safe","Safe"}};
	bool breezeMap[4][4] = {{false,false,false,false},{false,false,false,false},{false,false,false,false},{false,false,false,false}};
	bool stenchMap[4][4] = {{false,false,false,false},{false,false,false,false},{false,false,false,false},{false,false,false,false}};
	bool visited[4][4] = {{false,false,false,false},{false,false,false,false},{false,false,false,false},{false,false,false,false}};
	bool gold = false, giveUp = false, arrow = true, senseStench = true;
	action lastReturnMove, arrowDir; // Extra variables holding previous GPS when in return mode, and Arrow direction
	
	vector<action> target;		// Vector storing possible movable base on GPS
	stack<action> lifeLine;		// Vector storing the reverse of all moves made --> for safety return
};

// Function to search for unmoveable action base on GPS X, Y
KnowledgeBase getMovable(KnowledgeBase &kb) { 
	kb.target = {moveS, moveW, moveN, moveE}; // Custom Move Pattern, effect exploring pattern
	auto notPossible = find(kb.target.begin(), kb.target.end(), doNothing);
	if (kb.x == 0) {
		notPossible = find(kb.target.begin(), kb.target.end(), moveW);
		kb.target.erase(notPossible);
	}
	if (kb.x == 3) {
		notPossible = find(kb.target.begin(), kb.target.end(), moveE);
		kb.target.erase(notPossible);
	}
	if (kb.y == 0) {
		notPossible = find(kb.target.begin(), kb.target.end(), moveS);
		kb.target.erase(notPossible);
	}
	if (kb.y == 3) {
		notPossible = find(kb.target.begin(), kb.target.end(), moveN);
		kb.target.erase(notPossible);
	}
	return kb;
}

// Function to assist the redundant in rethink the logical analysis
KnowledgeBase logicConclude (KnowledgeBase &kb, int row, int col, string str) {
	if (str == "Wampus") {
		if (kb.dangerMap[row][col] == "Wampus")
			kb.dangerMap[row][col] = "Safe";
		if (kb.dangerMap[row][col] == "Both")
			kb.dangerMap[row][col] = "Pit";
	}
	if (str == "Pit") {
		if (kb.dangerMap[row][col] == "Pit")
			kb.dangerMap[row][col] = "Safe";
		if (kb.dangerMap[row][col] == "Both")
			kb.dangerMap[row][col] = "Wampus";	
	}
	return kb;
}
// Function to check between dangerMap to Breeze Map and Stench Map
KnowledgeBase checkSenseAndDangerLogic(KnowledgeBase &kb) {
	int countLogic;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (kb.dangerMap[i][j] == "Wampus" || kb.dangerMap[i][j] == "Both") {
				countLogic = 0;
				if (i == 0) {
					if (!kb.stenchMap[i+1][j] && kb.visited[i+1][j])
						countLogic++;
				}
				else if (i == 3) {
					if (!kb.stenchMap[i-1][j] && kb.visited[i-1][j])
						countLogic++;
				}
				else {
					if (!kb.stenchMap[i+1][j] && kb.visited[i+1][j])
						countLogic++;
					if (!kb.stenchMap[i-1][j] && kb.visited[i-1][j])
						countLogic++;
				}
				
				if (j == 0) {
					if (!kb.stenchMap[i][j+1] && kb.visited[i][j+1])
						countLogic++;
				}
				else if (j == 3) {
					if (!kb.stenchMap[i][j-1] && kb.visited[i][j-1])
						countLogic++;
				}
				else {
					if (!kb.stenchMap[i][j+1] && kb.visited[i][j+1])
						countLogic++;
					if (!kb.stenchMap[i][j-1] && kb.visited[i][j-1])
						countLogic++;
				}
				if (countLogic > 1)
					logicConclude(kb, i, j, "Wampus");
			}
			if (kb.dangerMap[i][j] == "Pit" || kb.dangerMap[i][j] == "Both") {
				countLogic = 0;
				if (i == 0) {
					if (!kb.breezeMap[i+1][j] && kb.visited[i+1][j])
						countLogic++;
				}
				else if (i == 3) {
					if (!kb.breezeMap[i-1][j] && kb.visited[i-1][j])
						countLogic++;
				}
				else {
					if (!kb.breezeMap[i+1][j] && kb.visited[i+1][j])
						countLogic++;
					if (!kb.breezeMap[i-1][j] && kb.visited[i-1][j])
						countLogic++;
				}
				
				if (j == 0) {
					if (!kb.breezeMap[i][j+1] && kb.visited[i][j+1])
						countLogic++;
				}
				else if (j == 3) {
					if (!kb.breezeMap[i][j-1] && kb.visited[i][j-1])
						countLogic++;
				}
				else {
					if (!kb.breezeMap[i][j+1] && kb.visited[i][j+1])
						countLogic++;
					if (!kb.breezeMap[i][j-1] && kb.visited[i][j-1])
						countLogic++;
				}
				if (countLogic)
					logicConclude(kb, i, j, "Pit");
			}
		}
	}
	return kb;
}

// Function assist the redundant in adding danger in Danger Map
KnowledgeBase dangerMapPit(KnowledgeBase &kb, int row, int col) { 
	if (!kb.visited[row][col]) {
		if (kb.dangerMap[row][col] == "Wampus")
			kb.dangerMap[row][col] = "Both";
		else
			kb.dangerMap[row][col] = "Pit";
	}
	return kb;
}

// Function assist the redundant in adding danger in Danger Map
KnowledgeBase dangerMapWampus(KnowledgeBase &kb, int row, int col) { 
	if (!kb.visited[row][col]) {
		if (kb.dangerMap[row][col] == "Pit")
			kb.dangerMap[row][col] = "Both";
		else
			kb.dangerMap[row][col] = "Wampus";
	}
	return kb;
}

// Function updateing dangerMap for immediate danger base on senses
KnowledgeBase update(KnowledgeBase &kb, string str) { 
	int x = kb.x, y = kb.y;
	if (str == "Stench") {
		if (x == 0)
			dangerMapWampus(kb, x + 1, y);
		else if (x == 3)
			dangerMapWampus(kb, x - 1, y);
		else {
			dangerMapWampus(kb, x - 1, y);
			dangerMapWampus(kb, x + 1, y);
		}
		if (y == 0)
			dangerMapWampus(kb, x, y + 1);
		else if (y == 3)
			dangerMapWampus(kb, x, y - 1);
		else {
			dangerMapWampus(kb, x, y - 1);
			dangerMapWampus(kb, x, y + 1);
		}
	}
	if (str == "Breeze") {
		if (x == 0)
			dangerMapPit(kb, x + 1, y);
		else if (kb.x == 3)
			dangerMapPit(kb, x - 1, y);
		else {
			dangerMapPit(kb, x - 1, y);
			dangerMapPit(kb, x + 1, y);
		}
		if (y == 0)
			dangerMapPit(kb, x, y + 1);
		else if (y == 3)
			dangerMapPit(kb, x, y - 1);
		else {
			dangerMapPit(kb, x, y - 1);
			dangerMapPit(kb, x, y + 1);
		}
	}
	return kb;
}

// Function to updating logic map for dropping the false believed
KnowledgeBase updateLogic(KnowledgeBase &kb, string str) {
	getMovable(kb);
	while (kb.target.back()) {
		if (kb.target.back() == moveN) {
			if (kb.dangerMap[kb.x][kb.y+1] == str)
				kb.dangerMap[kb.x][kb.y+1] = "Safe";
			else {
				if (kb.dangerMap[kb.x][kb.y+1] == "Both") {
					if (str == "Wampus")
						kb.dangerMap[kb.x][kb.y+1] = "Pit";
					if (str == "Pit")
						kb.dangerMap[kb.x][kb.y+1] = "Wampus";
				}
			}
		}
		if (kb.target.back() == moveS) {
			if (kb.dangerMap[kb.x][kb.y-1] == str)
				kb.dangerMap[kb.x][kb.y-1] = "Safe";
			else {
				if (kb.dangerMap[kb.x][kb.y-1] == "Both") {
					if (str == "Wampus")
						kb.dangerMap[kb.x][kb.y-1] = "Pit";
					if (str == "Pit")
						kb.dangerMap[kb.x][kb.y-1] = "Wampus";
				}
			}
		}
		if (kb.target.back() == moveE) {
			if (kb.dangerMap[kb.x+1][kb.y] == str)
				kb.dangerMap[kb.x+1][kb.y] = "Safe";
			else {
				if (kb.dangerMap[kb.x+1][kb.y] == "Both") {
					if (str == "Wampus")
						kb.dangerMap[kb.x+1][kb.y] = "Pit";
					if (str == "Pit")
						kb.dangerMap[kb.x+1][kb.y] = "Wampus";
				}
			}
		}
		if (kb.target.back() == moveW) {
			if (kb.dangerMap[kb.x-1][kb.y] == str)
				kb.dangerMap[kb.x-1][kb.y] = "Safe";
			else {
				if (kb.dangerMap[kb.x-1][kb.y] == "Both") {
					if (str == "Wampus")
						kb.dangerMap[kb.x-1][kb.y] = "Pit";
					if (str == "Pit")
						kb.dangerMap[kb.x-1][kb.y] = "Wampus";
				}
			}
		}
		kb.target.pop_back();
	}
	checkSenseAndDangerLogic(kb);
	return kb;
}

// Function to clear all Wampus danger from Map && turn off senseStench
KnowledgeBase updateScream(KnowledgeBase &kb) {
	kb.senseStench = false;
	for (int i = 0; i < 4; ++i){
		for (int j = 0; j < 4; ++j) {
			if (kb.dangerMap[i][j] == "Wampus")
				kb.dangerMap[i][j] = "Safe";
			if (kb.dangerMap[i][j] == "Both")
				kb.dangerMap[i][j] = "Pit";
		}
	}
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j)
			kb.stenchMap[i][j] = false;
	}
	return kb;
}

// Function to update cooridination in GPS, with move about to return
KnowledgeBase updateCoordinate(KnowledgeBase &kb, action move) {
	// Update the cooridination
	if (move == moveN)
		kb.y++;
	if (move == moveS)
		kb.y--;
	if (move == moveE)
		kb.x++;
	if (move == moveW)
		kb.x--;
	return kb;
}
}

// ************************** Main Agent Function *************************
action htwAgentBraxton(WumpusWorldSensor currentSensor, bool shouldReset) {
	// Declare one static variable of type KnowledgeBase.
	// Only that one static variable is allowed!
	static KnowledgeBase kb;
	if (shouldReset) {
		// Reset knowledge base for the new wumpus world
		kb = KnowledgeBase();
		return doNothing;
	}
	// Varibles
	int x = kb.x, y = kb.y;
	action myAction = doNothing;
	kb.visited[x][y] = true; // current GPS turns true
	
	// sense glittering && don't have gold yet, return grab with update knowledge gold
	if (currentSensor.perceiveGlitter()) {
		if (!kb.gold) {
			kb.gold = true;
			kb.giveUp = true;
			return grab; 
		}			
	}
	// If already have gold && in GPS(0,0), climbOut
	if (kb.gold && x == 0 && y == 0)
		return climbOut;
	// If only sense stench at GPS(0,0), gamble shooting E, with update info to KB 
	if (kb.arrow && currentSensor.perceiveStench() && !currentSensor.perceiveBreeze() && kb.x == 0 && kb.y == 0) {
		kb.arrow = false;
		kb.arrowDir = shootE;
		kb.shootX = kb.x;
		kb.shootY = kb.y;
		return shootE;
	}
	// If heard Scream, clear Wampus from anythings in Knowledge Book
	if (currentSensor.perceiveScream() && kb.senseStench) {
		updateScream(kb);
	}
	// If sense Breeze or have gold or giveUp at GPS(0,0), climb out
	if ((currentSensor.perceiveBreeze() || kb.gold || kb.giveUp)&& x==0 && y==0) {
		return climbOut;
	}
	
	// Improvised the traceback Life Line as it wasted more move
	// by taking shortcut via visited map. Visited Map --> Safe
	if (kb.giveUp || kb.gold) {
		getMovable(kb);
		uint i = 0;
		while (i < kb.target.size()) {
			if (kb.target[i] == moveW && kb.visited[kb.x-1][kb.y]) {
				myAction = moveW;
				updateCoordinate(kb, myAction);
				kb.lastReturnMove = myAction;
				return myAction;
			}
			if(kb.target[i] == moveS && kb.visited[kb.x][kb.y-1] && kb.target[i] != kb.lastReturnMove) {
				myAction = moveS;
				updateCoordinate(kb, myAction);
				kb.lastReturnMove = moveN;
				return myAction;
			}
			if (kb.target[i] == moveN && kb.visited[kb.x][kb.y+1] && kb.target[i] != kb.lastReturnMove) {
				myAction = moveN;
				updateCoordinate(kb, myAction);
				kb.lastReturnMove = moveS;
				return myAction;
			}
			if (kb.target[i] == moveE && kb.visited[kb.x+1][kb.y] && kb.target[i] != kb.lastReturnMove) {
				myAction = moveE;
				updateCoordinate(kb, myAction);
				kb.lastReturnMove = moveW;
				return myAction;
			}
			i++;
		}
	}
	// If sense Breeze, update danger Map && breezeMap
	if (currentSensor.perceiveBreeze()) {
		update(kb, "Breeze");
		kb.breezeMap[x][y] = true;
	}
	else
		updateLogic(kb, "Pit");		// Update logic if dont feel breeze, potential clear illusions on map
	
	// If sense stench at any GPS location && still has arrow, prioritize shoot E as base on explore pattern
	// NSEW --> Wampus likely on the E side, accuray %60-%65
	if (currentSensor.perceiveStench() && kb.senseStench) {
		update(kb, "Stench");
		kb.stenchMap[x][y] = true;
		if (kb.arrow) {
			kb.arrow = false;
			if (kb.lifeLine.top() == moveN || kb.lifeLine.top() == moveS) {
				kb.arrowDir = shootE;
				kb.shootX = kb.x;
				kb.shootY = kb.y;
				return shootE;
			}
			if (kb.lifeLine.top() == moveE) {
				kb.arrowDir = shootN;
				kb.shootX = kb.x;
				kb.shootY = kb.y;
				return shootN;
			}
			if (kb.lifeLine.top() == moveW) {
				kb.arrowDir = shootE;
				kb.shootX = kb.x;
				kb.shootY = kb.y;
				return shootE;
			}
		}
		else
			kb.stenchMap[kb.shootX+1][kb.shootY+1] = false;
	}
	else
		updateLogic(kb, "Wampus");		// Update logic, if dont sense stench, potential clear illusion
	
	if (currentSensor.perceiveScream() && kb.senseStench) {
		updateScream(kb);				// Cleaer wampus danger from map, turn off stench sense
	}		
	else if (!currentSensor.perceiveScream() && !kb.arrow){
		// Last added solution, when arrow missed the Wampus, Wampus exactly located, clear the other
		// grid around Wampus location for exploration
		if (!kb.arrow && kb.arrowDir == shootE && kb.shootX+1 <4 && kb.dangerMap[kb.shootX+1][kb.shootY] == "Wampus") {
			kb.dangerMap[kb.shootX+1][kb.shootY] = "Safe";
			kb.stenchMap[kb.shootX+1][kb.shootY+1] = false;
			kb.dangerMap[kb.shootX][kb.shootY+1] = "Wampus";
			if (kb.dangerMap[kb.shootX+1][kb.shootY+1] == "Wampus")
				kb.dangerMap[kb.shootX+1][kb.shootY+1] = "Safe";
		}
		if (!kb.arrow && kb.arrowDir == shootN && kb.shootX+1 <4 && kb.dangerMap[kb.shootX+1][kb.shootY] == "Wampus") {
			kb.dangerMap[kb.shootX][kb.shootY+1] = "Safe";
			kb.stenchMap[kb.shootX+1][kb.shootY+1] = false;
			kb.dangerMap[kb.shootX+1][kb.shootY] = "Wampus";
			if (kb.dangerMap[kb.shootX+1][kb.shootY+1] == "Wampus")
				kb.dangerMap[kb.shootX+1][kb.shootY+1] = "Safe";
		}
	}
	// If sense bump, do nothing (should be never happened, as killed better than starved)
	if (currentSensor.perceiveBump()) {
		myAction = doNothing;
		updateCoordinate(kb, myAction);
	}
	checkSenseAndDangerLogic(kb);
	// Explore Phase: looking for possible move actions on map where unvisited && safe
	getMovable(kb);
	for (int i = 0; i < static_cast<int>(kb.target.size()); i++) {
		myAction = kb.target[i];		
		if (myAction == moveE) {
			if (!kb.visited[x+1][y] && kb.dangerMap[x+1][y] == "Safe") {
				kb.lifeLine.push(moveW);
				updateCoordinate(kb, myAction);
				return myAction;
			}
		}
		else if (myAction == moveN) {
			if (!kb.visited[x][y+1] && kb.dangerMap[x][y+1] == "Safe") {
				kb.lifeLine.push(moveS);
				updateCoordinate(kb, myAction);
				return myAction;
			}
		}
		else if (myAction == moveW) {
			if (!kb.visited[x-1][y] && kb.dangerMap[x-1][y] == "Safe") {
				kb.lifeLine.push(moveE);
				updateCoordinate(kb, myAction);
				return myAction;
			}
		}
		else if (myAction == moveS) {
			if (!kb.visited[x][y-1] && kb.dangerMap[x][y-1] == "Safe") {
				kb.lifeLine.push(moveN);
				updateCoordinate(kb, myAction);
				return myAction;
			}
		}
	}
	
	// Recheck logic (believed to still be flawed, Gap for improvement in iteration II)
	checkSenseAndDangerLogic(kb);
	
	// If no possible move, step back with lifeLine move
	if (!kb.lifeLine.empty()) {
		myAction = kb.lifeLine.top();
		kb.lifeLine.pop();
		updateCoordinate(kb, myAction);
		return myAction;
	}
	// Just to secure the grab gold when GPS at (0,0)
	return (currentSensor.perceiveGlitter() && !kb.gold)? grab:climbOut;
}

/*
*******************************************************************************************
Explain Approach:

The main approach of this agent is divided into two mode, exploring vs backtracking.
Explore Phase:
- Before exploring mode, sense anything necessary to update the knowledge book, such as
  glitterig, stench, breeze. (develop knowledge book as much as possible)
- Prioritize the gold first, stench and breeze after.
- If sense only stench, not breeze in one location, shoot the arrowE (most of the time) as
  effective with exploring pattern NSEW.
- Arrow direction, shooting GPS are also key infor when missed as it gives out the Wampus GPS
- If sense breeze, movable direction that not visited mark unsafe
- After building Knowledge book, deciding which moves to take upon danger map && unvisited
- Else, pop lifeLine top(), meaing step back to previos GPS and continue explore

Back-Tracing Phase:
- Development of Life Line act as a guilden rop (like yellow line for underwater cave diving),
it is effective to gurentee a secure safety path for return.
- Life Line also effective for stepping back when sensing danger a head
- However, due to inefficient when returning as it returns the same route it explore, method
searching for movable Westward, Southward on visited map were the priority as cut actions almost
2/3 of lifeline action.
- Yet, another problem occured, back && forth moves starving problem where there're no westward
move, and it kept moveS, moveN, moveS, moveN.
- Solution to the problem, save reverse previous return move, add move Eastward if necessary.
- Through out infinite testing, another bug or illogical move. Due to arrow accuracy somewhere 70%
, taking advantage of missed arrow && Wampus's location, explore the stench location even arrow
missed.

*******************************************************************************************
Strengths, Weaknesses, Expectation:

Strength:
Most of it strengths are from the pattern of exploration, as well as the impliment of Life Line,
as well as the back-tracing shortcut, all of which massively reduce the wasteful action as well
as to assit the arrow accuracy. Besides, logical update && recheck and reupdate, would potentially
help the efficient in exploring and clearing illogic conditions.

Weaknessse:
Taking advantage of missed arrow, still the latest approach, flaw might occur as certain senario
agent action returning weirdly led to being killed. Check logic & illogic still the essential 
improvement as I believe its capability still limited. (Gap for iteration II improvment)

Expectation:
Features developed more and more, the Average point getting better and better from first version:
AVG:260, till the lastest version: AVG:365, AA:60%-65% (Max: 409, Min: 345)-- Big Improvement.
With this result, expectation could be in top3.

*******************************************************************************************

Iteration 2:

Slightly tweak the fcuntions:
- checkSenseAndDangerLogic()
- updateLogic()
- as well as some update map when missing arrow
Slight improvment from AVG: 365, AA: 65 ---> AVG: 375, AA: the same.

The gap for futher improvement should be done on consistenly play the missing arrow situation
without dying. However, due to mininal change causing core dump, so inorder to improve it more,
scratch the entire code and rebuild would be the chocie.
However, I believe at this point I already reaching almost the peak of diminishing return and it
not worth it base on time and AVG.
*******************************************************************************************
*/
