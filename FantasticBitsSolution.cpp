#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <sstream>

# define M_PI           3.14159265358979323846  /* pi */

using namespace std;

const int Y_MIDDLE_GOAL = 3750;

/**
* Grab Snaffles and try to throw them through the opponent's goal!
* Move towards a Snaffle and use your team id to determine where you need to throw it.
**/

class Point
{
private:
	int x;
	int y;
public:
	int getX();
	int getY();
	void setXY(int, int);
	float distance(Point);
};

class Unit : public Point
{
private:
	int id;
	float vx;
	float vy;
	int radius;
	float mass;
public:
	float getMass();
	float getVelX();
	float getVelY();
	void setId(int);
	void setMass(float);
	void setRadius(int);
	void setVel(int, int);
	int getId();
};

class Wizard : public Unit
{
private:
	bool state;
	int assignment;
public:
	Wizard();
	Wizard(int);
	int getAssignment();
	bool getState();
	void setAssignment(int);
	void setState(bool);
};

class Snaffle : public Unit
{
private:
	bool updated;
public:
	Snaffle(int);
	bool getUpdated();
	void setUpdated(bool);
};

class Bludger : public Unit
{
private:
	int target;
	int ignore;
public:
	Bludger();
	Bludger(int);
	int getIgnore();
	int getTarget();
	void setIgnore(int);
	void setTarget(int);
	void updateTarget(Wizard[2], Wizard[2]);
};

class Move
{
private:
	string action;
	int xValue;
	int yValue;
	int thurst;
public:
	Move();
	Move(string, int, int, int);
	string getAction();
	int getXValue();
	int getYValue();
	int getThurst();
};

class Game
{
private:
	Wizard myWizards[2];
	Wizard oppWizards[2];
	Bludger bludgers[2];
	vector<Snaffle> snaffles;
	int myMana;
	int teamId;
public:
	Game(Wizard[2], Wizard[2], Bludger[2], vector<Snaffle>, int, int);
	void predictTurn(Move&, int);
	void runCollisions(int);
	Bludger getBludger(int a);
	int getMana();
	Wizard getMyWizard(int);
	Wizard getOppWizard(int);
	Snaffle getSnaffle(int);
	int getSnaffleSize();
	int getTeamId();

	//Modifiers
	void decreaseMana(int);
};

// Main functions

string createOutputString(Wizard, Game&, Point, int&, int&);
void deleteNonexistentSnaffles(vector<Snaffle>&);
void recordDistances(Wizard, vector<Snaffle>&, vector<int>&); // Distance recorder for Wizards to Snaffles
void recordDistances2(Bludger, Wizard[2], Wizard[2], vector<int>&); // Distance recorder for Bludgers to Wizards
int searchSnaffle(int, vector<Snaffle>);
void setAssignments(Wizard[2], vector<Snaffle>&);
void updateBludgers(Bludger[2], Wizard[2], Wizard[2]);

// Spell functions

int accioGoalKeeper(Wizard, Game, Point);
int checkStraightLineGoal(Wizard, Game, Point);
int petrificusGoalKeeper(Wizard, Game);

// Auxiliary functions
float radiansToDegrees(float);
Point predictPosition(Unit);
void flippendoPrediction(Wizard& wiz, Game& sim, Snaffle sn);

int main()
{
	// Creating goals
	Point goals[2];
	goals[0].setXY(0, 3750);
	goals[1].setXY(16000, 3750);

	int myMana = 0;
	Wizard wiz(-1);
	Wizard myWizards[2] = { wiz, wiz }; // Array for my wizards
	Wizard oppWizards[2] = { wiz, wiz }; //Array for enemy wizards

	Bludger blud(-1); // Bludgers
	Bludger bludgers[2] = { blud, blud }; // Array for bludgers

	vector<Snaffle> snaffles; // Vector of snaffles


	int myTeamId; // if 0 you need to score on the right of the map, if 1 you need to score on the left
	cin >> myTeamId; cin.ignore();

	// game loop
	while (1) {
		int entities; // number of entities still in game
		cin >> entities; cin.ignore();
		int currentMyWizard = 0;
		int currentOppWizard = 0;
		int currentBludger = 0;
		for (int i = 0; i < entities; i++) {
			int entityId; // entity identifier
			string entityType; // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" (or "BLUDGER" after first league)
			int x; // position
			int y; // position
			int vx; // velocity
			int vy; // velocity
			int state; // 1 if the wizard is holding a Snaffle, 0 otherwise
			cin >> entityId >> entityType >> x >> y >> vx >> vy >> state; cin.ignore();

			if (entityType.compare("WIZARD") == 0)
			{
				myWizards[currentMyWizard].setXY(x, y);
				myWizards[currentMyWizard].setVel(vx, vy);
				myWizards[currentMyWizard].setState(state);
				if (myWizards[currentMyWizard].getId() == -1)
					myWizards[currentMyWizard].setId(entityId);
				currentMyWizard++;
			}
			if (entityType.compare("OPPONENT_WIZARD") == 0)
			{
				oppWizards[currentOppWizard].setXY(x, y);
				oppWizards[currentOppWizard].setVel(vx, vy);
				oppWizards[currentOppWizard].setState(state);
				if (oppWizards[currentOppWizard].getId() == -1)
					oppWizards[currentOppWizard].setId(entityId);
				currentOppWizard++;
			}
			if (entityType.compare("BLUDGER") == 0)
			{
				bludgers[currentBludger].setXY(x, y);
				bludgers[currentBludger].setVel(vx, vy);
				if (bludgers[currentBludger].getId() == -1)
					bludgers[currentBludger].setId(entityId);
				currentBludger++;
			}
			if (entityType.compare("SNAFFLE") == 0)
			{
				int snaffleIndex = searchSnaffle(entityId, snaffles);
				if (snaffleIndex == -1)
				{
					Snaffle newSnaffle(entityId);
					newSnaffle.setXY(x, y);
					newSnaffle.setVel(vx, vy);
					snaffles.push_back(newSnaffle);
				}
				else
				{
					snaffles[snaffleIndex].setXY(x, y);
					snaffles[snaffleIndex].setVel(vx, vy);
					snaffles[snaffleIndex].setUpdated(1);
				}
			}
		}

		deleteNonexistentSnaffles(snaffles);

		updateBludgers(bludgers, myWizards, oppWizards);
		setAssignments(myWizards, snaffles);

		Game simulator(myWizards, oppWizards, bludgers, snaffles, myMana, myTeamId);


		for (int i = 0; i < snaffles.size(); i++)
		{
			cerr << "Snaffle " << snaffles[i].getId() << " vx: " << snaffles[i].getVelX() << endl;
		}

		int spellTargetIndex = -1; // Used to make sure both wizards won't use their spells on the same target.

		for (int i = 0; i < 2; i++) {

			string output = createOutputString(myWizards[i], simulator, goals[!myTeamId], myMana, spellTargetIndex);
			stringstream ss;
			ss << output;
			string action;
			int xValue;
			int yValue;
			int thurst;
			ss >> action >> xValue >> yValue >> thurst;
			Move move(action, xValue, yValue, thurst);
			if (i == 0)
				simulator.predictTurn(move, 0);
			cout << output << endl;
		}
		myMana++;
	}
}

// Main class functions

string createOutputString(Wizard wiz, Game& sim, Point goal, int& mana, int& spellTargetIndex)
{
	string output;
	bool madeAMove = 0;
	if (wiz.getState())
	{
		output += "THROW ";
		output += to_string(goal.getX()) + " ";
		output += to_string(goal.getY() + 300) + " ";
		output += "500";
		madeAMove = 1;
	}
	if (sim.getMana() >= 10) // Using spell
	{
		if (!madeAMove)
		{
			int petrificusTest = petrificusGoalKeeper(wiz, sim);
			if (petrificusTest != -1 && petrificusTest != spellTargetIndex)
			{
				output += "PETRIFICUS ";
				output += to_string(sim.getSnaffle(petrificusTest).getId());
				sim.decreaseMana(10);
				mana -= 10;
				madeAMove = 1;
				spellTargetIndex = petrificusTest;
			}
		}
		if (!madeAMove && sim.getMana() >= 20)
		{
			int straightLineGoalTest = checkStraightLineGoal(wiz, sim, goal);
			if (straightLineGoalTest != -1 && straightLineGoalTest != spellTargetIndex)
			{
				output += "FLIPENDO ";
				output += to_string(sim.getSnaffle(straightLineGoalTest).getId());
				sim.decreaseMana(20);
				mana -= 20;
				madeAMove = 1;

				flippendoPrediction(wiz, sim, sim.getSnaffle(straightLineGoalTest));
				spellTargetIndex = straightLineGoalTest;
			}
		}
		if (!madeAMove && sim.getMana () >= 20)
		{
			int accioGoalKeeperTest = accioGoalKeeper(wiz, sim, goal);
			if (accioGoalKeeperTest != -1 && accioGoalKeeperTest != spellTargetIndex)
			{
				output += "ACCIO ";
				output += to_string( sim.getSnaffle(accioGoalKeeperTest).getId() );
				sim.decreaseMana(20);
				mana -= 20;
				madeAMove = 1;
				spellTargetIndex = accioGoalKeeperTest;
			}
		}
	}
	if (!madeAMove)
	{
		output += "MOVE ";
		int assignment = wiz.getAssignment();
		if (assignment != -1)
		{
			output += to_string(sim.getSnaffle(assignment).getX()) + " ";
			output += to_string(sim.getSnaffle(assignment).getY()) + " ";
			output += "150";
		}
		else  // If the assignment is to be deffensive (-1) [Not being used]
		{
			int xPosition;
			if (goal.getX() != 0)
			{
				xPosition = 1000;
			}
			else
			{
				xPosition = 15000;
			}
			output += to_string(xPosition) + " ";
			output += to_string(goal.getY()) + " ";
			output += "150";
		}
	}
	return output;
}

void deleteNonexistentSnaffles(vector<Snaffle>& snaffles)
{
	for (int i = 0; i < snaffles.size(); i++)
	{
		if (!snaffles[i].getUpdated())
			snaffles.erase(snaffles.begin() + i);
	}

	//Resetting updated to 0
	for (int i = 0; i < snaffles.size(); i++)
	{
		snaffles[i].setUpdated(0);
	}
}

void recordDistances(Wizard wiz, vector<Snaffle>& snaffles, vector<int>& unitDistance)
{
	for (int i = 0; i < snaffles.size(); i++)
	{
		unitDistance.push_back(wiz.distance(snaffles[i]));
	}
}
void recordDistances2(Bludger bud, Wizard wizards1[2], Wizard wizards2[2], vector<int>& unitDistance)
{
	for (int i = 0; i < 2; i++)
	{
		unitDistance.push_back(bud.distance(wizards1[i]));
	}
	for (int i = 0; i < 2; i++)
	{
		unitDistance.push_back(bud.distance(wizards2[i]));
	}
}
int searchSnaffle(int a, vector<Snaffle> snaffles)
{
	for (int i = 0; i < snaffles.size(); i++)
	{
		if (snaffles[i].getId() == a)
			return i;
	}
	return -1;
}

void setAssignments(Wizard wizards[2], vector<Snaffle>& snaffles)
{
	vector<vector<int>> distances;
	for (int i = 0; i < 2; i++)
	{
		vector<int> unitDistances;
		recordDistances(wizards[i], snaffles, unitDistances);
		distances.push_back(unitDistances);
	}
	int lowestDistance = 99999;
	int lowestWizId;
	int lowestSnaffleId = 99999;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < snaffles.size(); j++)
		{
			if (distances[i][j] < lowestDistance)
			{
				lowestDistance = distances[i][j];
				lowestWizId = i;
				lowestSnaffleId = j;
			}
		}
	}
	wizards[lowestWizId].setAssignment(lowestSnaffleId);
	lowestWizId = !lowestWizId;
	if (snaffles.size() > 1)
	{

		lowestDistance = 999999;
		int lowestSnaffleIdTwo;
		for (int j = 0; j < distances[lowestWizId].size(); j++)
		{
			if (distances[lowestWizId][j] < lowestDistance && j != lowestSnaffleId)
			{
				lowestDistance = distances[lowestWizId][j];
				lowestSnaffleIdTwo = j;
			}
		}
		wizards[lowestWizId].setAssignment(lowestSnaffleIdTwo);
	}
	else // If there's only one snaffle left
	{
		wizards[lowestWizId].setAssignment(lowestSnaffleId); // Will run after the same snaffle as the other guy
	}
}

void updateBludgers(Bludger bludgers[2], Wizard myWizards[2], Wizard oppWizards[2])
{
	for (int i = 0; i < 2; i++)
	{
		bludgers[i].updateTarget(myWizards, oppWizards);
	}
}

// Point class

int Point::getX()
{
	return x;
}
int Point::getY()
{
	return y;
}

void Point::setXY(int j, int k)
{
	x = j;
	y = k;
}

float Point::distance(Point p)
{
	float distX = x - p.x;
	distX = distX * distX;
	float distY = y - p.y;
	distY = distY * distY;
	return sqrt(distX + distY);
}

// Unit class

float Unit::getMass()
{
	return mass;
}
float Unit::getVelX()
{
	return vx;
}
float Unit::getVelY()
{
	return vy;
}
int Unit::getId()
{
	return id;
}
void Unit::setId(int a)
{
	id = a;
}
void Unit::setMass(float a)
{
	mass = a;
}
void Unit::setRadius(int a)
{
	radius = a;
}
void Unit::setVel(int a, int b)
{
	vx = a;
	vy = b;
}

// Wizard class
Wizard::Wizard()
{
	setRadius(400);
	setMass(1);
	setId(-1);
}
Wizard::Wizard(int a)
{
	setRadius(400);
	setMass(1);
	setId(a);
}
int Wizard::getAssignment()
{
	return assignment;
}
bool Wizard::getState()
{
	return state;
}
void Wizard::setAssignment(int a)
{
	assignment = a;
}
void Wizard::setState(bool a)
{
	state = a;
}

// Snaffle class

Snaffle::Snaffle(int a)
{
	setRadius(150);
	setMass(0.5);
	setId(a);
	updated = 1;
}
bool Snaffle::getUpdated()
{
	return updated;
}
void Snaffle::setUpdated(bool a)
{
	updated = a;
}

//Bludger class

Bludger::Bludger()
{
	setRadius(200);
	setMass(8);
	setId(-1);
	ignore = -1; // Unkown
	target = -1; // Unkown
}
Bludger::Bludger(int a)
{
	setRadius(200);
	setMass(8);
	setId(a);
	ignore = -1; // Unkown
	target = -1; // Unkown
}
int Bludger::getIgnore() {
	return ignore;
}
int Bludger::getTarget()
{
	return target;
}
void Bludger::setIgnore(int a) {
	ignore = a;
}
void Bludger::setTarget(int a)
{
	target = a;
}
void Bludger::updateTarget(Wizard wizards1[2], Wizard wizards2[2])
{
	vector<int> unitDistances;
	recordDistances2(*this, wizards1, wizards2, unitDistances);
	int lowestDistance = 99999;
	int lowestIndex;
	for (int i = 0; i < unitDistances.size(); i++)
	{
		if (unitDistances[i] < lowestDistance)
		{
			lowestDistance = unitDistances[i];
			lowestIndex = i;
		}
		else if (unitDistances[i] == lowestDistance)
		{
			lowestDistance = 99999;
			break;
		}
	}
	if (lowestDistance != 99999)
	{
		if (lowestIndex < 2)
			target = wizards1[lowestIndex].getId();
		else
			target = wizards2[lowestIndex - 2].getId();
	}
}
// Move class

Move::Move()
{
	action = "none";
	xValue = -1;
	yValue = -1;
	thurst = -1;
}
Move::Move(string a, int b, int c, int d)
{
	action = a;
	xValue = b;
	yValue = c;
	thurst = d;
}
string Move::getAction()
{
	return action;
}
int Move::getXValue()
{
	return xValue;
}
int Move::getYValue()
{
	return yValue;
}
int Move::getThurst()
{
	return thurst;
}

// class Game

Game::Game(Wizard myWiz[2], Wizard oppWiz[2], Bludger blud[2], vector<Snaffle> snaf, int mana, int myTeamId)
{
	snaffles.clear();
	for (int i = 0; i < 2; i++)
		myWizards[i] = myWiz[i];
	for (int i = 0; i < 2; i++)
		oppWizards[i] = oppWiz[i];
	for (int i = 0; i < 2; i++)
		bludgers[i] = blud[i];
	for (int i = 0; i < snaf.size(); i++)
		snaffles.push_back(snaf[i]);
	myMana = mana;
	teamId = myTeamId;
}

void Game::predictTurn(Move& move, int unitIndex)
{
	if (move.getAction().compare("MOVE") == 0)
	{
		/* Angle is not being used but might be useful code later

		Point p;
		p.setXY(move.getXValue(), move.getYValue());
		float d = unit.distance(p);
		float distX = (move.getXValue() - unit.getX()) / d;
		float distY = (move.getYValue() - unit.getY()) / d;
		float angle = acos(distX);
		if (distY < 0)
		{
		angle = 2 * M_PI - angle;
		}


		cerr << "currentAngle: " << radiansToDegrees(angle) << endl;
		*/

		float normX = (move.getXValue() - myWizards[unitIndex].getX());
		float normY = (move.getYValue() - myWizards[unitIndex].getY());
		float norm_v = sqrt((normX * normX) + (normY * normY));
		normX = normX / norm_v;
		normY = normY / norm_v;


		float nextVX = move.getThurst() * normX * myWizards[unitIndex].getMass() + myWizards[unitIndex].getVelX();
		float nextVY = move.getThurst() * normY * myWizards[unitIndex].getMass() + myWizards[unitIndex].getVelY();
	}
}

void Game::runCollisions(int id)  // Working on
{/*
	vector<Unit&> entities;
	Unit* unit;
	for (int i = 0; i < 2; i++)
	{
		if (myWizards[i].getId() != id)
			entities.push_back(myWizards[i]);
		else
			unit = &myWizards[i];
		if (oppWizards[i].getId() != id)
			entities.push_back(oppWizards[i]);
		else
			unit = &oppWizards[i];
		if (bludgers[i].getId() != id)
			entities.push_back(bludgers[i]);
		else
			unit = &oppWizards[i];
	}
	// Unit that will check for collisions is saved as a pointer unit and all other entities saved in the entities vector
	*/
}

Bludger Game::getBludger(int a)
{
	return bludgers[a];
}
int Game::getMana()
{
	return myMana;
}
Wizard Game::getMyWizard(int a)
{
	return myWizards[a];
}

Wizard Game::getOppWizard(int a)
{
	return oppWizards[a];
}
Snaffle Game::getSnaffle(int a)
{
	return snaffles[a];
}
int Game::getSnaffleSize()
{
	return snaffles.size();
}
int Game::getTeamId()
{
	return teamId;
}

void Game::decreaseMana(int a)
{
	myMana -= a;
}

// Spell functions

int accioGoalKeeper(Wizard wiz, Game sim, Point goal)
{
	for (int i = 0; i < sim.getSnaffleSize(); i++)
	{
		Snaffle snaffle = sim.getSnaffle(i);
		
		// First checking to make sure that we're not snatching from my other wizard
		int wizId = wiz.getId();
		int myOtherWizIndex;
		for (int j = 0; j < 2; j++)
		{
			if (sim.getMyWizard(j).getId() != wizId)
				myOtherWizIndex = j;
		}
		if (snaffle.getX() == sim.getMyWizard(myOtherWizIndex).getX() && snaffle.getY() == sim.getMyWizard(myOtherWizIndex).getY())
		{
			//Do nothing
		}
		else // If my other wizard doesnt have that snaffle
		{
			if (goal.getX() == 0)
			{
				if (snaffle.getX() > wiz.getX() && wiz.distance(snaffle) < 2500 && wiz.distance(snaffle) > 1500)
					return i;
			}
			else
			{
				if (snaffle.getX() < wiz.getX() && wiz.distance(snaffle) < 2500 && wiz.distance(snaffle) > 1500)
					return i;
			}
		}
	}
	return -1;
}

int checkStraightLineGoal(Wizard wiz, Game sim, Point goal) // Working on
{
	for (int i = 0; i < sim.getSnaffleSize(); i++)
	{
		Snaffle snaffle = sim.getSnaffle(i);
		if (wiz.distance(snaffle) <= 5000)
		{
			float deltaY = wiz.getY() - snaffle.getY();
			float deltaX = wiz.getX() - snaffle.getX();
			float m = deltaY / deltaX;
			float b = float(wiz.getY()) - m * float(wiz.getX());
			float y = m*goal.getX() + b;
			if (y < goal.getY() + 400 && y > goal.getY() - 400)
			{
				if (!sim.getTeamId() && wiz.getX() < snaffle.getX())
				{
					//Making sure it won't collide with bludgers or enemy wizards
					bool collision = 0;
					for (int j = 0; j < 4; j++)
					{
						Point p;
						p.setXY(snaffle.getX() + (200 * j+1), snaffle.getY());
						for (int k = 0; k < 2; k++)
						{
							if (p.distance(sim.getOppWizard(k)) < 550)
								collision = 1;
							if (p.distance(sim.getBludger(k)) < 350)
								collision = 1;
						}
					}
					if (!collision)
						return i;
				}
				if (sim.getTeamId() && wiz.getX() > snaffle.getX())
				{
					//Making sure it won't collide with bludgers or enemy wizards
					bool collision = 0;
					for (int j = 0; j < 4; j++)
					{
						Point p;
						p.setXY(snaffle.getX() - (200 * j+1), snaffle.getY());
						for (int k = 0; k < 2; k++)
						{
							if (p.distance(sim.getOppWizard(k)) < 550)
								collision = 1;
							if (p.distance(sim.getBludger(k)) < 350)
								collision = 1;
						}
					}
					return i;
				}
			}
		}
	}
	return -1;
}

int petrificusGoalKeeper(Wizard wiz, Game sim)
{/*
	for (int i = 0; i < sim.getSnaffleSize(); i++)
	{
		Snaffle snaffle = sim.getSnaffle(i);
		if (!sim.getTeamId() && snaffle.getVelX() <= -1000 && snaffle.getX() + snaffle.getVelX() <= 8000 && snaffle.getX() + snaffle.getVelX() >= 0)
			return i;
		if (sim.getTeamId() && snaffle.getVelX() >= 1000 && snaffle.getX() + snaffle.getVelX() >= 8000 && snaffle.getX() + snaffle.getVelX() <= 16000)
			return i;
	}*/
	return -1;
}

// Auxiliary functions

Point predictPosition(Unit entity)
{
	float nextX = entity.getX() + entity.getVelX();
	float nextY = entity.getY() + entity.getVelY();
	Point p;
	p.setXY(nextX, nextY);
	return p;
}
float radiansToDegrees(float r)
{
	return r * 180.0 / M_PI;
}



// Flipendo prediction function

void flippendoPrediction(Wizard& wiz, Game& sim, Snaffle sn)	// Working on
{
	float normX = (sn.getX() - wiz.getX());
	float normY = (sn.getY() - wiz.getY());
	float norm_v = sqrt((normX * normX) + (normY * normY));
	normX = normX / norm_v;
	normY = normY / norm_v;

	float power = 6000.0 / ((wiz.distance(sn) / 1000.0) *  (wiz.distance(sn) / 1000.0));
	if (power > 1000)
		power = 1000.0;
	float nextVX = (normX * power / 0.5) + sn.getVelX();
	float nextVY = (normY * power / 0.5) + sn.getVelY();
	cerr << "Power: " << power << endl;
	cerr << "Next VX: " << nextVX << endl;
	cerr << "Next VY: " << nextVY << endl;
	cerr << "Next X: " << sn.getX() + nextVX;
}
