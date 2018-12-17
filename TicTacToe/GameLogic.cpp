// A game of 'Tic tac toe' written by Akshay M Chavan

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

int getRandomNumber(int min, int max) // A function to calculate random numbers between min and max (both inclusive)
{
	static const double fraction = 1.0 / (RAND_MAX + 1.0);
	return min + static_cast<int>((max - min + 1) * (rand() * fraction));
}

class TicTacToe
{
private:
	char mSquareContents[3][3]; //  3 X 3 maze representing 9 squares of the grid
	char playerChoice; // Move made by player
	char computerChoice; // Move made by computer
	char playerChar; // Character representing player. It is 'X'
	char computerChar; // Character representing computer. It is 'O'
	char digitArr[9] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' }; // An array of 1-9 digits as characters. Used by the isFull() function to check if the array is full
	int vacancyCount; // Number of vacant (unmarked) squares on the grid. 
	vector<int> outerIndexes; // The outer indexes (rows) of vacant squares [X][]
	vector<int> innerIndexes; // The inner indexes (columns) of vacant squares [][X]
	enum playerName { COMPUTER, PLAYER }; // An enum which indicates which player is being referred to
	enum outcome { WIN, LOSE, DRAW }; //WIN: player wins, LOSE: player loses
	enum chosenDifficulty { EASY, MEDIUM, IMPOSSIBLE }; // An enum which indicates which difficulty is being referred to
	chosenDifficulty m_Difficulty; // Difficulty chosen by the player
	vector<int> playerMovesOuterIndexes; // Store the outer indexes (rows) of moves made by player
	vector<int> playerMovesInnerIndexes; // Store the inner indexes (columns) of moves made by player

public:
	TicTacToe(int difficulty = 1)
		: m_Difficulty(static_cast<chosenDifficulty>(difficulty))
	{
		int squareValue = 1;
		/*
		 * Iterate from 1 to 9
		 * Each time convert the digit to a character
		 * Save the character in the right square of the grid
		 */
		for (int indexOuter = 0; indexOuter < 3; indexOuter++)
		{
			for (int indexInner = 0; indexInner < 3; indexInner++)
			{
				ostringstream os;
				os << squareValue;
				mSquareContents[indexOuter][indexInner] = (os.str())[0];// Since it is a null-terminated string, the first index has the character
				squareValue++;
			}
		}

		playerChar = 'X';
		computerChar = 'O';
	}
	friend ostream& operator<<(ostream& out, const TicTacToe& t);
	void updateGrid(char character) // Puts 'X' or 'O' at the right square
	{ /*
	   * Find which character has been selected
	   * Find the square in which this character (digit) is located
	   * Replace the character (digit) with 'X' or 'O', depending on whose turn it is
	   */
		for (int indexOuter = 0; indexOuter < 3; indexOuter++)
		{
			for (int indexInner = 0; indexInner < 3; indexInner++)
			{
				if (playerChoice == mSquareContents[indexOuter][indexInner])
					mSquareContents[indexOuter][indexInner] = character;
			}
		}
	}
	void takePlayerChoice() // Ask the player to choose which square to mark by entering the specific number in that square
	{
		int count = 0;

		cout << "Enter appropriate choice: ";
		cin >> playerChoice;
		cin.ignore(32767, '\n');

		/* Increment count if the square chosen by player exists (is empty) */
		for (int indexOuter = 0; indexOuter < 3; indexOuter++)
		{
			for (int indexInner = 0; indexInner < 3; indexInner++)
			{
				if (playerChoice == mSquareContents[indexOuter][indexInner])
					count++;
			}
		}
		/* If count is not incremented, player entered incorrect digit*/
		if (count == 0)
		{
			cout << "Error: Invalid input.\n";
			this->takePlayerChoice();
		}
	}
	void countVacancies()
	{ /*
	   * Count unmarked squares
	   * Save co-ordinates of squares in outerIndexes and innerIndexes
	   */
		outerIndexes.clear();
		innerIndexes.clear();
		vacancyCount = 0;// Number of vacant squares in grid
		for (int indexOuter = 0; indexOuter < 3; indexOuter++)
		{
			for (int indexInner = 0; indexInner < 3; indexInner++)
			{
				if (mSquareContents[indexOuter][indexInner] != 'X' && mSquareContents[indexOuter][indexInner] != 'O')
				{
					vacancyCount++;
					outerIndexes.push_back(indexOuter);
					innerIndexes.push_back(indexInner);
				}
			}
		}
	}
	bool checkPotentialVictory(playerName pn)
	{ /*
	   * Check if victory is possible in ONE MOVE for player pn
	   * If called with player and victory is possible, block it
	   * If called with player and neither player nor computer can win, don't mark any square
	   * If called with computer and victory is possible, mark the winning square
	   * If called with computer and neither player nor computer can win, don't mark any square
	   * Return true if player is blocked from winning, or computer's winning move is made
	   * Else return false
	   */
		char currentChar;
		char temp;

		if (pn == COMPUTER)
			currentChar = computerChar;
		else
			currentChar = playerChar;

		for (int index = 0; index < outerIndexes.size(); index++)
		{
			temp = mSquareContents[outerIndexes[index]][innerIndexes[index]];
			mSquareContents[outerIndexes[index]][innerIndexes[index]] = currentChar;
			switch (checkVictory())
			{
			case WIN:
				mSquareContents[outerIndexes[index]][innerIndexes[index]] = computerChar;
				return true;
			case LOSE:
				return true; // If the marked square results in a win or loss, let the marked square be... 
			default:
				mSquareContents[outerIndexes[index]][innerIndexes[index]] = temp; // ...else reverse the change
			}
		}
		return false;
	}
	bool markOnSameLine()
	{ /*
	   * If computer has marked one square in earlier turn and two vacant squares exist on same line, mark the second square
	   * Return true if a square is marked
	   * Return false if victory is not possible by marking another square on the same line, so no square is marked
	   */
		vector<int> allVacanciesOuterIndexes; // Outer indexes of all vacancies  with which a straight line can be formed. These vacancies may be on multiple lines
		vector<int> allVacanciesInnerIndexes; // Inner  indexes of all vacancies  with which a straight line can be formed. These vacancies may be on multiple lines
		int earlierMarked[2]{ 100, 100 }; // 100 is a magic value to indicate array is empty
		char temp1, temp2;
		for (int outer = 0; outer < 3; outer++)
		{
			for (int inner = 0; inner < 3; inner++)
			{
				if (mSquareContents[outer][inner] == computerChar)
				{
					earlierMarked[0] = outer; //Save the outer index of square marked earlier by the computer
					earlierMarked[1] = inner; //Save the inner index of square marked earlier by the computer
					break;
				}
			}
			if (earlierMarked[0] != 100) break;
		}

		for (int outer = 0; outer < outerIndexes.size(); outer++) //Iterate through all available vacancies
		{
			for (int compareWith = outer + 1; compareWith < outerIndexes.size(); compareWith++)
			{
				if (outerIndexes[outer] == outerIndexes[compareWith] && outerIndexes[outer] == earlierMarked[0]) //Check if a horizontal line can be formed from left to right
				{
					allVacanciesOuterIndexes.push_back(outerIndexes[outer]);
					allVacanciesInnerIndexes.push_back(innerIndexes[outer]);
					allVacanciesOuterIndexes.push_back(outerIndexes[compareWith]);
					allVacanciesInnerIndexes.push_back(innerIndexes[compareWith]);
				}
				else if (outerIndexes[compareWith] == outerIndexes[outer] + 1 && outerIndexes[compareWith] == earlierMarked[0] + 2)// Check if vertical or diagonal lines can be formed from top to bottom
				{
					temp1 = mSquareContents[outerIndexes[outer]][innerIndexes[outer]];
					temp2 = mSquareContents[outerIndexes[compareWith]][innerIndexes[compareWith]];
					mSquareContents[outerIndexes[outer]][innerIndexes[outer]] =
						mSquareContents[outerIndexes[compareWith]][innerIndexes[compareWith]] = computerChar;
					if (checkVictory() == LOSE)
					{
						allVacanciesOuterIndexes.push_back(outerIndexes[outer]);
						allVacanciesInnerIndexes.push_back(innerIndexes[outer]);
						allVacanciesOuterIndexes.push_back(outerIndexes[compareWith]);
						allVacanciesInnerIndexes.push_back(innerIndexes[compareWith]);
					}
					mSquareContents[outerIndexes[outer]][innerIndexes[outer]] = temp1;
					mSquareContents[outerIndexes[compareWith]][innerIndexes[compareWith]] = temp2;
				}
				else if (outerIndexes[compareWith] == outerIndexes[outer] + 2 && outerIndexes[compareWith] == earlierMarked[0] + 1)// Vertical and diagonal vacancies from top to bottom with earlied marked square in middle
				{
					temp1 = mSquareContents[outerIndexes[outer]][innerIndexes[outer]];
					temp2 = mSquareContents[outerIndexes[compareWith]][innerIndexes[compareWith]];
					mSquareContents[outerIndexes[outer]][innerIndexes[outer]] =
						mSquareContents[outerIndexes[compareWith]][innerIndexes[compareWith]] = computerChar;
					if (checkVictory() == LOSE)
					{
						allVacanciesOuterIndexes.push_back(outerIndexes[outer]);
						allVacanciesInnerIndexes.push_back(innerIndexes[outer]);
						allVacanciesOuterIndexes.push_back(outerIndexes[compareWith]);
						allVacanciesInnerIndexes.push_back(innerIndexes[compareWith]);
					}
					mSquareContents[outerIndexes[outer]][innerIndexes[outer]] = temp1;
					mSquareContents[outerIndexes[compareWith]][innerIndexes[compareWith]] = temp2;
				}
			}

		}
		for (int outer = outerIndexes.size() - 1; outer >= 0; outer--)
		{
			for (int compareWith = outer - 1; compareWith >= 0; compareWith--)
			{
				if (outerIndexes[compareWith] == outerIndexes[outer] - 1 && outerIndexes[compareWith] == earlierMarked[0] - 2)// Vertical and diagonal vacancies from bottom to top
				{
					temp1 = mSquareContents[outerIndexes[outer]][innerIndexes[outer]];
					temp2 = mSquareContents[outerIndexes[compareWith]][innerIndexes[compareWith]];
					mSquareContents[outerIndexes[outer]][innerIndexes[outer]] =
						mSquareContents[outerIndexes[compareWith]][innerIndexes[compareWith]] = computerChar;
					if (checkVictory() == LOSE)
					{
						allVacanciesOuterIndexes.push_back(outerIndexes[outer]);
						allVacanciesInnerIndexes.push_back(innerIndexes[outer]);
						allVacanciesOuterIndexes.push_back(outerIndexes[compareWith]);
						allVacanciesInnerIndexes.push_back(innerIndexes[compareWith]);
					}
					mSquareContents[outerIndexes[outer]][innerIndexes[outer]] = temp1;
					mSquareContents[outerIndexes[compareWith]][innerIndexes[compareWith]] = temp2;
				}
			}
		}
		if (!allVacanciesOuterIndexes.empty())
		{ /*
		   * Mark a random square from allVacanciesOuterIndexes and allVacanciesInnerIndexes
		   */
			int randomIndex = getRandomNumber(0, allVacanciesOuterIndexes.size() - 1);
			mSquareContents[allVacanciesOuterIndexes[randomIndex]][allVacanciesInnerIndexes[randomIndex]] = computerChar;
			return true;
		}
		return false;
	}
	bool blockWinningStrategy()
	{
		/*
		 * If the player has the chance to make a fork in future turns, block him
		 * Return true if computer marks a square to block/slow player's move
		 * Return false if player has no chance of making a fork, so computer marks no square
		 */
		for (int outer = 0; outer < 3; outer++) // First scan the grid to find out how many moves the player has made and save these moves in a vector
		{
			for (int inner = 0; inner < 3; inner++)
			{
				if (mSquareContents[outer][inner] == playerChar)
				{
					playerMovesOuterIndexes.push_back(outer);
					playerMovesInnerIndexes.push_back(inner);
				}
			}
		}

		for (int index = 0; index < playerMovesOuterIndexes.size(); index++) // Scan the vector and remove any moves that have been stored repeatedly
		{
			for (int compareWith = index + 1; compareWith < playerMovesOuterIndexes.size(); compareWith++)
			{
				if (playerMovesOuterIndexes[index] == playerMovesOuterIndexes[compareWith] &&
					playerMovesInnerIndexes[index] == playerMovesInnerIndexes[compareWith])
				{
					playerMovesOuterIndexes.erase(playerMovesOuterIndexes.begin() + compareWith);
					playerMovesInnerIndexes.erase(playerMovesInnerIndexes.begin() + compareWith);
				}
			}
		}

		if (playerMovesOuterIndexes.size() > 2) //If more than 2 moves have been made by player, he/she cannot implement strategy anymore, or his strategy cannot be blocked anymore, so exit this function
		{
			playerMovesOuterIndexes.clear();
			playerMovesInnerIndexes.clear();
			return false;
		}

		/* STRATEGY ONE: When player first move is the central square */
		/*
		 * Most winning strategies can be formed starting from central square of maze
		 * Simple way to block this is by marking a corner square
		 */
		if (playerMovesOuterIndexes[0] == 1 && playerMovesInnerIndexes[0] == 1)
		{/* If player has marked the central square, mark a corner square... */
			int cornerSquares[][2] = { {0, 0}, {2, 2}, {0, 2}, {2, 0} }; // Holds the co-ordinates of 4 corner squares of the maze
			int randomIndex = getRandomNumber(0, 3);
			if (playerMovesOuterIndexes.size() == 1)
			{ /* Mark a corner square only if the player has marked 1 square (central square)*/
				mSquareContents[cornerSquares[randomIndex][0]][cornerSquares[randomIndex][1]] = computerChar;
				return true;
			}

			if (playerMovesOuterIndexes.size() == 2)
			{/* ...If player marks the other corner square of the same diagonal in the next turn, mark any remaining corner square */
				if ((playerMovesOuterIndexes[1] == 2 && playerMovesInnerIndexes[1] == 2) &&
					mSquareContents[0][0] == computerChar)
				{
					randomIndex = getRandomNumber(2, 3);
					mSquareContents[cornerSquares[randomIndex][0]][cornerSquares[randomIndex][1]] = computerChar;
					return true;
				}
				if (playerMovesOuterIndexes[1] == 2 && playerMovesInnerIndexes[1] == 0 && mSquareContents[0][2] == computerChar)
				{
					randomIndex = getRandomNumber(0, 1);
					mSquareContents[cornerSquares[randomIndex][0]][cornerSquares[randomIndex][1]] = computerChar;
					return true;
				}
				if (playerMovesOuterIndexes[1] == 0 && playerMovesInnerIndexes[1] == 0 && mSquareContents[2][2] == computerChar)
				{
					randomIndex = getRandomNumber(2, 3);
					mSquareContents[cornerSquares[randomIndex][0]][cornerSquares[randomIndex][1]] = computerChar;
					return true;
				}
				if (playerMovesOuterIndexes[1] == 0 && playerMovesInnerIndexes[1] == 2 && mSquareContents[2][0] == computerChar)
				{
					randomIndex = getRandomNumber(0, 1);
					mSquareContents[cornerSquares[randomIndex][0]][cornerSquares[randomIndex][1]] = computerChar;
					return true;
				}
			}
		}

		/* STRATEGY TWO: When player's first move is a corner square */
		/* When player starts a winning strategy by first marking a corner square, it can be slowed down by marking the central square*/
		if (playerMovesOuterIndexes.size() == 1 && playerMovesOuterIndexes[0] != 1 && playerMovesInnerIndexes[0] != 1) // Corner squares cannot have 1 as an outer or inner index
		{/* If player has marked only one square (corner square), mark the central square... */
			mSquareContents[1][1] = computerChar;
			return true;
		}
		else if (playerMovesOuterIndexes.size() == 2 && playerMovesOuterIndexes[1] != 1 && playerMovesInnerIndexes[1] != 1 && mSquareContents[1][1] == computerChar && playerMovesOuterIndexes[0] != 1 && playerMovesInnerIndexes[0] != 1)
		{/* ...If player marks diagonally opposite corner square in next turn, mark the central square of any side or... */
			int centralSquaresOfSides[][2] = { {0, 1}, {1, 0}, {1, 2}, {2, 1} };
			int randomIndex = getRandomNumber(0, 3);
			mSquareContents[centralSquaresOfSides[randomIndex][0]][centralSquaresOfSides[randomIndex][1]] = computerChar;
			return true;
		}
		else if (playerMovesOuterIndexes.size() == 2 && mSquareContents[1][1] == computerChar && playerMovesOuterIndexes[0] != 1 && playerMovesInnerIndexes[0] != 1)
		{/* ...If player marks the central square of a side 2 rows or 2 columns away from player's first move, mark any remaining square of that side*/
			if (playerMovesOuterIndexes[1] == playerMovesOuterIndexes[0] + 2 && playerMovesInnerIndexes[1] == playerMovesInnerIndexes[0] + 1) // 2 rows down and one column right
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's second move (central square of a side)
				for (size_t index = 0; index < outerIndexes.size(); index++) // Is a vacancy available in the same row as player's second move?
				{
					if (outerIndexes[index] == playerMovesOuterIndexes[1])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same row the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}
			}
			else if (playerMovesOuterIndexes[1] == playerMovesOuterIndexes[0] - 2 && playerMovesInnerIndexes[1] == playerMovesInnerIndexes[0] - 1) // 2 rows up and 1 column left
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's second move (central square of a side)
				for (size_t index = 0; index < outerIndexes.size(); index++) //Is there a vacancy on the same row as player's second move?
				{
					if (outerIndexes[index] == playerMovesOuterIndexes[1])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same row the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}
			}

			else if (playerMovesOuterIndexes[1] == playerMovesOuterIndexes[0] + 1 && playerMovesInnerIndexes[1] == playerMovesInnerIndexes[0] + 2) // 1 row below and 2 columns right  
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's second move (central square of a side)
				for (size_t index = 0; index < innerIndexes.size(); index++) //Is there a vacancy on the same column as player's second move?
				{
					if (innerIndexes[index] == playerMovesInnerIndexes[1])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same column the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}

			}
			else if (playerMovesOuterIndexes[1] == playerMovesOuterIndexes[0] - 1 && playerMovesInnerIndexes[1] == playerMovesInnerIndexes[0] - 2) // 1 row above and 2 columns left
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's second move (central square of a side)
				for (size_t index = 0; index < innerIndexes.size(); index++) //Is there a vacancy on the same column as player's second move?
				{
					if (innerIndexes[index] == playerMovesInnerIndexes[1])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same column the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}

			}
			else if (playerMovesOuterIndexes[1] == playerMovesOuterIndexes[0] - 1 && playerMovesInnerIndexes[1] == playerMovesInnerIndexes[0] + 2) // 1 row above and 2 columns right 
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's second move (central square of a side)
				for (size_t index = 0; index < innerIndexes.size(); index++) //Is there a vacancy on the same column as player's second move?
				{
					if (innerIndexes[index] == playerMovesInnerIndexes[1])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same column the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}

			}
			else if (playerMovesOuterIndexes[1] == playerMovesOuterIndexes[0] - 2 && playerMovesInnerIndexes[1] == playerMovesInnerIndexes[0] + 1) // 2 rows above and 1 column right
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's second move (central square of a side)
				for (size_t index = 0; index < outerIndexes.size(); index++) //Is there a vacancy on the same row as player's second move?
				{
					if (outerIndexes[index] == playerMovesOuterIndexes[1])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same row the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}

			}
			else if (playerMovesOuterIndexes[1] == playerMovesOuterIndexes[0] + 1 && playerMovesInnerIndexes[1] == playerMovesInnerIndexes[0] - 2) // 1 row below and 2 columns left
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's second move (central square of a side)
				for (size_t index = 0; index < innerIndexes.size(); index++) //Is there a vacancy on the same column as player's second move?
				{
					if (innerIndexes[index] == playerMovesInnerIndexes[1])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same column the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}

			}
			else if (playerMovesOuterIndexes[1] == playerMovesOuterIndexes[0] + 2 && playerMovesInnerIndexes[1] == playerMovesInnerIndexes[0] - 1) // 2 rows down and one column left
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's second move (central square of a side)
				for (size_t index = 0; index < outerIndexes.size(); index++) // Is a vacancy available in the same row as player's second move?
				{
					if (outerIndexes[index] == playerMovesOuterIndexes[1])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same row the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}
			}

		}
		/* STRATEGY THREE: When player's first move is central square of any side */
		/* It can be blocked by marking the central square*/
		if (playerMovesOuterIndexes.size() == 1)
		{/* If player has marked a central square of any side, mark the central square... */
			if (playerMovesOuterIndexes[0] == 0 && playerMovesInnerIndexes[0] == 1)
			{
				mSquareContents[1][1] = computerChar;
				return true;
			}
			else if (playerMovesOuterIndexes[0] == 1 && playerMovesInnerIndexes[0] == 0)
			{
				mSquareContents[1][1] = computerChar;
				return true;
			}
			else if (playerMovesOuterIndexes[0] == 1 && playerMovesInnerIndexes[0] == 2)
			{
				mSquareContents[1][1] = computerChar;
				return true;
			}
			else if (playerMovesOuterIndexes[0] == 2 && playerMovesInnerIndexes[0] == 1)
			{
				mSquareContents[1][1] = computerChar;
				return true;
			}
		}
		else if (playerMovesOuterIndexes.size() == 2)
		{/* ...and if player next marks a central square of a side making an L shape with his/her earlier move, mark the corner square between player's two moves OR... */
			if (playerMovesOuterIndexes[0] == 0 && playerMovesInnerIndexes[0] == 1 &&
				playerMovesOuterIndexes[1] == 1 && playerMovesInnerIndexes[1] == 0)
			{
				mSquareContents[0][0] = computerChar;
				return true;
			}
			else if (playerMovesOuterIndexes[0] == 0 && playerMovesInnerIndexes[0] == 1 &&
				playerMovesOuterIndexes[1] == 1 && playerMovesInnerIndexes[1] == 2)
			{
				mSquareContents[0][2] = computerChar;
				return true;
			}
			else if (playerMovesOuterIndexes[0] == 1 && playerMovesInnerIndexes[0] == 0 &&
				playerMovesOuterIndexes[1] == 0 && playerMovesInnerIndexes[1] == 1)
			{
				mSquareContents[0][0] = computerChar;
				return true;
			}
			else if (playerMovesOuterIndexes[0] == 1 && playerMovesInnerIndexes[0] == 0 &&
				playerMovesOuterIndexes[1] == 2 && playerMovesInnerIndexes[1] == 1)
			{
				mSquareContents[2][0] = computerChar;
				return true;
			}
			else if (playerMovesOuterIndexes[0] == 1 && playerMovesInnerIndexes[0] == 2 &&
				playerMovesOuterIndexes[1] == 0 && playerMovesInnerIndexes[1] == 1)
			{
				mSquareContents[0][2] = computerChar;
				return true;
			}
			else if (playerMovesOuterIndexes[0] == 1 && playerMovesInnerIndexes[0] == 2 &&
				playerMovesOuterIndexes[1] == 2 && playerMovesInnerIndexes[1] == 1)
			{
				mSquareContents[2][2] = computerChar;
				return true;
			}
			else if (playerMovesOuterIndexes[0] == 2 && playerMovesInnerIndexes[0] == 1 &&
				playerMovesOuterIndexes[1] == 1 && playerMovesInnerIndexes[1] == 0)
			{
				mSquareContents[2][0] = computerChar;
				return true;
			}
			else if (playerMovesOuterIndexes[0] == 2 && playerMovesInnerIndexes[0] == 1 &&
				playerMovesOuterIndexes[1] == 1 && playerMovesInnerIndexes[1] == 2)
			{
				mSquareContents[2][2] = computerChar;
				return true;
			}

			/* ...if player in second turn marks a corner square 2 rows or 2 columns away from the first move, mark any of the 2 vacant squares on same row/column of player's first move */
			if (playerMovesOuterIndexes[0] == 0 && playerMovesInnerIndexes[0] == 1 &&
				playerMovesOuterIndexes[1] == 2 && playerMovesInnerIndexes[1] == 0)
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's first move (central square of a side)
				for (size_t index = 0; index < outerIndexes.size(); index++) //Is there a vacancy on the same row as player's first move?
				{
					if (outerIndexes[index] == playerMovesOuterIndexes[0])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same row the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}
			}
			else if (playerMovesOuterIndexes[0] == 0 && playerMovesInnerIndexes[0] == 1 &&
				playerMovesOuterIndexes[1] == 2 && playerMovesInnerIndexes[1] == 2)
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's first move (central square of a side)
				for (size_t index = 0; index < outerIndexes.size(); index++) //Is there a vacancy on the same row as player's first move?
				{
					if (outerIndexes[index] == playerMovesOuterIndexes[0])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same row the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}
			}
			else if (playerMovesOuterIndexes[0] == 1 && playerMovesInnerIndexes[0] == 0 &&
				playerMovesOuterIndexes[1] == 0 && playerMovesInnerIndexes[1] == 2)
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's first move (central square of a side)
				for (size_t index = 0; index < innerIndexes.size(); index++) //Is there a vacancy on the same column as player's first move?
				{
					if (innerIndexes[index] == playerMovesInnerIndexes[0])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same column the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}
			}
			else if (playerMovesOuterIndexes[0] == 1 && playerMovesInnerIndexes[0] == 0 &&
				playerMovesOuterIndexes[1] == 2 && playerMovesInnerIndexes[1] == 2)
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's first move (central square of a side)
				for (size_t index = 0; index < innerIndexes.size(); index++) //Is there a vacancy on the same column as player's first move?
				{
					if (innerIndexes[index] == playerMovesInnerIndexes[0])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same column the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}
			}
			else if (playerMovesOuterIndexes[0] == 1 && playerMovesInnerIndexes[0] == 2 &&
				playerMovesOuterIndexes[1] == 0 && playerMovesInnerIndexes[1] == 0)
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's first move (central square of a side)
				for (size_t index = 0; index < innerIndexes.size(); index++) //Is there a vacancy on the same column as player's first move?
				{
					if (innerIndexes[index] == playerMovesInnerIndexes[0])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same column the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}
			}
			else if (playerMovesOuterIndexes[0] == 1 && playerMovesInnerIndexes[0] == 2 &&
				playerMovesOuterIndexes[1] == 2 && playerMovesInnerIndexes[1] == 0)
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's first move (central square of a side)
				for (size_t index = 0; index < innerIndexes.size(); index++) //Is there a vacancy on the same column as player's first move?
				{
					if (innerIndexes[index] == playerMovesInnerIndexes[0])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same column the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}
			}
			else if (playerMovesOuterIndexes[0] == 2 && playerMovesInnerIndexes[0] == 1 &&
				playerMovesOuterIndexes[1] == 0 && playerMovesInnerIndexes[1] == 0)
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's first move (central square of a side)
				for (size_t index = 0; index < outerIndexes.size(); index++) //Is there a vacancy on the same row as player's first move?
				{
					if (outerIndexes[index] == playerMovesOuterIndexes[0])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same row the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}
			}
			else if (playerMovesOuterIndexes[0] == 2 && playerMovesInnerIndexes[0] == 1 &&
				playerMovesOuterIndexes[1] == 0 && playerMovesInnerIndexes[1] == 2)
			{
				vector<vector<int>> vacantSquaresOnSameLine; //Store vacant squares on same line as player's first move (central square of a side)
				for (size_t index = 0; index < outerIndexes.size(); index++) //Is there a vacancy on the same row as player's first move?
				{
					if (outerIndexes[index] == playerMovesOuterIndexes[0])
						vacantSquaresOnSameLine.push_back({ outerIndexes[index], innerIndexes[index] });
				}
				if (vacantSquaresOnSameLine.size() == 2) //If 2 vacancies exist on the same row the player has marked the central square of, mark a random square among them
				{
					int randomIndex = getRandomNumber(0, 1);
					mSquareContents[vacantSquaresOnSameLine[randomIndex][0]][vacantSquaresOnSameLine[randomIndex][1]] = computerChar;
					return true;
				}
			}
		}
		return false;
	}
	void levelImpossible()
	{ /*
	   * Called when player selects impossible difficulty
	   */
	   /* Priority 1: If computer can win by a single move, make that move*/
		if (checkPotentialVictory(COMPUTER))
			return;

		/* Priority 2: If player can win by a single move, block that move*/
		if (checkPotentialVictory(PLAYER))
			return;

		/* Priority 3: If player has started\can start a winning strategy, block it*/
		if (blockWinningStrategy())
			return;


		/* Priority 4: If computer has marked a single square earlier, continue marking on the same line to win in future turns*/
		if (markOnSameLine())
			return;

		/* Priority 5: Mark a random square*/
		int randomIndex = getRandomNumber(0, outerIndexes.size() - 1);
		mSquareContents[outerIndexes[randomIndex]][innerIndexes[randomIndex]] = computerChar;
	}
	void levelMedium()
	{ /*
	   * Called when player selects medium difficulty level
	   */
	   /*First check if computer can win in next turn*/
		if (checkPotentialVictory(COMPUTER))
			return;
		/*If not, check if player can win in next turn. If yes, block his move*/
		if (checkPotentialVictory(PLAYER))
			return;

		/*If computer has played before, mark a square on the same line so that line can be completed in future turns*/
		if (markOnSameLine())
			return;

		/*If none of the above is applicable, just mark a square randomly*/
		int randomIndex = getRandomNumber(0, outerIndexes.size() - 1);
		mSquareContents[outerIndexes[randomIndex]][innerIndexes[randomIndex]] = computerChar;
	}
	void levelEasy() // 'Easy' difficulty
	{
		int randomIndex = getRandomNumber(0, outerIndexes.size() - 1);
		mSquareContents[outerIndexes[randomIndex]][innerIndexes[randomIndex]] = computerChar;
	}
	void takeComputerChoice()
	{ /*
	   * Depending on the difficulty level selected by the player, the appropriate function is called
	   */
		this->countVacancies();

		switch (m_Difficulty)
		{
		case EASY:
			levelEasy();
			break;
		case MEDIUM:
			levelMedium();
			break;
		case IMPOSSIBLE:
			levelImpossible();
		}
	}
	outcome checkVictory() // Checks if the last move has resulted in player victory ( returns WIN ) or player defeat ( returns LOSE ). Returns DRAW if noone has won
	{
		/*
		Victory occurs when:
			1. Horizontal match is found: array1[outer][inner] == array2[outer][inner+1] == array3[outer][inner+2], or
			2. Vertical match is found: array1[outer][inner] == array2[outer+1][inner] == array3[outer+2][inner], or
			3. Right sloping diagonal is found: array1[outer][inner] == array2[outer+1][inner+1] == array3[outer+2][inner+2], or
			4. Left sloping diagonal is found: array1[outer][inner] == array2[outer+1][inner-1] == array3[outer+2][inner-2]
		*/

		for (int outerIndex = 0; outerIndex < 3; outerIndex++) //Checking if horizontal line formed
		{
			if (mSquareContents[outerIndex][0] == mSquareContents[outerIndex][1] && mSquareContents[outerIndex][1] == mSquareContents[outerIndex][2])
			{
				if (mSquareContents[outerIndex][0] == playerChar)
				{
					return WIN;
				}
				else
				{
					return LOSE;
				}
			}
		}

		for (int innerIndex = 0; innerIndex < 3; innerIndex++) //Checking if vertical line formed
		{
			if (mSquareContents[0][innerIndex] == mSquareContents[1][innerIndex] && mSquareContents[1][innerIndex] == mSquareContents[2][innerIndex])
			{
				if (mSquareContents[0][innerIndex] == playerChar)
				{
					return WIN;
				}
				else
				{
					return LOSE;
				}
			}
		}

		if (mSquareContents[0][0] == mSquareContents[1][1] && mSquareContents[1][1] == mSquareContents[2][2]) //Checking if right-sloping diagonal formed
		{
			if (mSquareContents[0][0] == playerChar)
			{
				return WIN;
			}
			else
			{
				return LOSE;
			}
		}

		if (mSquareContents[0][2] == mSquareContents[1][1] && mSquareContents[1][1] == mSquareContents[2][0]) //Checking if left-sloping diagonal formed
		{
			if (mSquareContents[0][2] == playerChar)
			{
				return WIN;
			}
			else
			{
				return LOSE;
			}
		}

		/* If all above conditions fail, i.e., if no one wins and if all vacancies are full, the game is a draw*/
		if (this->isFull())
		{
			return DRAW;
		}
	}
	void play()
	{ /*
	   * Takes player's choice, updates the grid to show marked square and checks if player has won
	   * Then takes computer choice, updates the grid to show marked square and checks if computer has won
	   * Prints the entire grid everytime the function is called
	   * Throws value 0 when game ends
	   */
		cout << *this;

		if (!this->isFull()) //Player's move
		{
			this->takePlayerChoice();
			this->updateGrid(playerChar);
			switch (this->checkVictory())
			{
			case WIN:
				cout << "\n\tYOU WIN!\n";
				throw 0;
			case LOSE:
				cout << "\n\tYOU LOSE!\n";
				throw 0;
			case DRAW:
				cout << "\n\tTHE GAME IS A DRAW!\n";
				throw 0;
			}

		}

		if (!this->isFull()) //Computer's move
		{
			this->takeComputerChoice();
			this->updateGrid(computerChar);
			switch (this->checkVictory())
			{
			case WIN:
				cout << "\n\tYOU WIN!\n";
				throw 0;
			case LOSE:
				cout << "\n\tYOU LOSE!\n";
				throw 0;
			case DRAW:
				cout << "\n\tTHE GAME IS A DRAW!\n";
				throw 0;
			}
		}

	}
	bool isFull()
	{ /*
	   * Returns true if grid is completely full
	   * Returns false if atleast one square is vacant
	   */
		int count = 0;
		for (int indexOuter = 0; indexOuter < 3; indexOuter++)
		{
			for (int indexInner = 0; indexInner < 3; indexInner++)
			{
				for (auto digit : digitArr)
				{
					if (digit == mSquareContents[indexOuter][indexInner])
						count++;
				}
			}
		}
		return (count == 0) ? true : false;
	}
};
ostream& operator<<(ostream& out, const TicTacToe& t)
{ /*
   * Overload operator << to print entire grid
   */
	out << '\n';
	for (int indexOuter = 0; indexOuter < 3; indexOuter++)
	{
		for (int indexInner = 0; indexInner < 3; indexInner++)
		{
			if (indexInner == 0)
			{
				out << ' ' << t.mSquareContents[indexOuter][indexInner] <<
					" | " << t.mSquareContents[indexOuter][indexInner + 1] << " | " <<
					t.mSquareContents[indexOuter][indexInner + 2] << '\n';
				for (int num = 1; num <= 11 && indexOuter != 2; num++)
				{
					if (num % 4 == 0)
						out << '+';
					else
						out << '-';
				}
				out << '\n';
			}
		}
	}
	out << '\n';
	return out;

}
int main()
{
	srand(static_cast<unsigned int>(time(0)));
	char difficulty; //Difficulty selected by user in character format
	int intDifficulty; //Difficulty selected by user converted to int
	while (difficulty != 'E' && difficulty != 'e' && difficulty != 'M' && difficulty != 'm' && difficulty != 'I' && difficulty != 'i')
	{
		cout << "Choose a difficulty level (E-EASY, M-MEDIUM, I-IMPOSSIBLE): ";
		cin >> difficulty;
		cin.ignore(32767, '\n'); // Take only one character input, ignore other 32,767 characters of until newline
		if (cin.fail()) // If cin fails, clear the buffer and ignore until 32,767 characters of a newline
		{
			cin.clear();
			cin.ignore(32767, '\n');
		}
	}
	if (difficulty == 'E' || difficulty == 'e')
		intDifficulty = 0;
	else if (difficulty == 'M' || difficulty == 'm')
		intDifficulty = 1;
	else if (difficulty == 'I' || difficulty == 'i')
		intDifficulty = 2;
	TicTacToe t(intDifficulty);

	try
	{
		while (!t.isFull())
			t.play();
		cout << t;
	}
	catch (int) // Catches exception thrown in TicTacToe::play()
	{
		cout << t;
	}
	return 0;
}
