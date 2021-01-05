#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <chrono>
#include <thread>
#include <time.h>

using namespace std;

typedef pair<int, int> Point;
typedef vector<Point> Path;
typedef map<Point, pair<Point, double>> Parent;
typedef pair<Point, double> CostPair;
typedef vector<vector<int>> Matrix;

#define NO_PARENT -1;


Point source;
Point destination;

struct comp {
	bool operator()(const CostPair& p1, const CostPair& p2) const {
		if (p1.second == p2.second) {
			if (p1.first.first == p2.first.first) {
				return p1.first.second > p2.first.second;
			}
			return p1.first.first > p2.first.first;
		}
		return p1.second > p2.second;
	}
};
typedef set<CostPair, comp> PQue;

Matrix genMatrix(int row, int col) {
	srand(time(0));
	Matrix maze;
	for (int i = 0; i < row; i++) {
		vector<int> v;
		for (int j = 0; j < col; j++) {
			int temp = rand() % 2;
			v.push_back(temp);
		}
		maze.push_back(v);
	}
	return maze;
}
/* Return Euclidean distance since diagonal movement is allowed */
static double getDistance(const Point& p1, const Point& p2) {
	return sqrt(pow((p1.first - p2.first), 2) + pow((p1.second - p2.second), 2));
}
bool validPosition(int size, int p) {
	if ((p < 0) || (p >= size)) {
		return false;
	}
	return true;
}
Path genValidNeighbours(const Matrix& maze, Point p) {
	Path valid_neigh;
	int row_size = maze.size();
	int col_size = maze[0].size();
	p.first = p.first - 1;
	p.second = p.second - 1;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			int x_pos = p.first + i;
			int y_pos = p.second + j;
			if (i == 1 && j == 1) {
				continue;
			}
			if ((validPosition(row_size, x_pos)) && (validPosition(col_size, y_pos))) {
				if (maze[x_pos][y_pos]) {
					valid_neigh.push_back({ x_pos,y_pos });
				}
			}
		}
	}
	//cout << "Valid Neighbours: \n";
	for (auto x : valid_neigh) {
		//cout << maze[x.first][x.second] << " at : " << x.first << ", " << x.second << endl;
	}
	//cout << endl;
	return valid_neigh;
}

PQue::iterator find(PQue& st, Point p) {
	for (auto i = st.begin(); i != st.end(); i++) {
		if (i->first.first == p.first) {
			if (i->first.second == p.second) {
				return i;
			}
		}
	}
	return st.end();
}

char** aStarPathFinder(const Matrix& maze) {
	char** path = new char* [maze.size()];
	Path way;
	Parent parent = {};
	Point def_parent = { -1,-1 };
	for (int i = 0; i < maze.size(); i++) {
		path[i] = new char[maze[0].size() - 1];
		for (int j = 0; j < maze[0].size(); j++) {
			path[i][j] = '.';
			parent[{ i, j }] = { def_parent, 0 };
		}
	}
	path[source.first][source.second] = 1;
	//Point destination = { maze.size() - 1, maze[0].size() -1 };


	Matrix travelled(maze.size(), vector<int>(maze[0].size(), 0));
	//auto res = genValidNeighbours(maze, {1,1});
	/*for (auto x : res) {
		//cout << maze[x.first][x.second] << " at : " << x.first << ", " << x.second << endl;
	}*/
	PQue que;
	Point start = source;
	CostPair p = { start, 0 };
	que.insert(p);
	bool found = false;
	while (!que.empty()) {
		auto top_ptr = que.end();
		top_ptr--;
		CostPair top = { top_ptr->first , top_ptr->second };
		travelled[top.first.first][top.first.second] = 1;
		//cout << " Top : "<<maze[top.first.first][top.first.second]<<" at : "<< top.first.first << ", " << top.first.second<<" Cost : "<<top.second << endl;
		que.erase(top_ptr);
		auto valid_neigh = genValidNeighbours(maze, top.first);
		for (auto x : valid_neigh) {
			if (x.first == top.first.first && x.second == top.first.second) {
				continue;
			}
			if (travelled[x.first][x.second] != 1) {
				double cost = top.second + 1 + getDistance(x, destination);
				if (parent[x].first.first != -1) {
					if (parent[x].second > cost) {
						//cout << "Update lower cost : for " << x.first << ", " << x.second << " to " << cost << endl;
						//cout << "Inserting : {" << x.first << ", " << x.second << "} " << "with cost : " << cost << endl;
						que.insert({ x, cost });
						parent[x] = { top.first, cost };
					}
					else {

						//cout << "Not Inserting : {" << x.first << ", " << x.second << "} " << "with cost : " << cost << endl;
					}
				}
				else {

					//cout << "Inserting : {" << x.first << ", " << x.second << "} " << "with cost : " << cost << endl;
					que.insert({ x, cost });
					parent[x] = { top.first, cost };
				}

				if ((x.first == destination.first) && (x.second == destination.second)) {
					found = true;
					break;
				}
			}
		}
		if (found)
			break;
	}
	Point x = source;
	for (auto x : parent) {
		if (x.second.first.first != -1) {
			//cout << "C: " << x.first.first << "," << x.first.second << " P : " << x.second.first.first << "," << x.second.first.second << endl;
		}
	}
	if (found) {
		path[source.first][source.second] = 'S';
		Point temp = destination;
		while (parent[temp].first.first != -1) {
			way.push_back(temp);
			temp = parent[temp].first;
		}
	}
	for (auto x : way) {
		path[x.first][x.second] = '*';
	}

	path[destination.first][destination.second] = 'D';
	return path;
}

int main() {
	Matrix maze = { { 1,0,1,0,1,0,1,0 },
					{ 0,1,0,1,0,1,0,1 },
					{ 1,0,0,0,0,1,0,1 },
					{ 0,0,0,1,0,0,1,1 },
					{ 1,0,1,0,1,0,1,0 },
					{ 0,1,0,1,0,1,0,0 },
					{ 1,1,0,1,1,1,0,1 },
					{ 0,0,0,1,0,0,1,1 },
					{ 1,0,1,0,1,0,1,0 },
					{ 0,1,0,1,0,1,0,1 },
					{ 1,0,0,1,0,1,0,1 },
					{ 0,0,0,1,0,0,0,1 },
					{ 1,0,1,0,0,0,1,0 },
					{ 0,1,0,1,0,1,0,1 },
					{ 1,1,0,1,1,1,0,1 },
					{ 0,0,0,1,0,0,0,1 } };

	Matrix maze2 = { {1,0,1,0 },
					{ 0,1,0,1 },
					{ 1,1,0,1 },
					{ 0,0,0,1 } };

	Matrix maze1 = genMatrix(7, 8);


	int x1, y1, x2, y2;
	cout << "Enter Source co-ordinates : " << endl;
	cout << "X1 : ";
	cin >> x1;
	cout << "Y1 : ";
	cin >> y1;
	cout << "X2 : ";
	cout << "Enter Destination co-ordinates : " << endl;
	cout << "X1 : ";
	cin >> x2;
	cout << "Y2 : ";
	cin >> y2;
	source = { x1, y1 };
	destination = { x2, y2 };

	auto res = aStarPathFinder(maze);
	for (int i = 0; i < maze.size(); i++) {
		for (int j = 0; j < maze[0].size(); j++) {
			//cout << maze[i][j] << " ";
		}
		//cout << endl;
	}
	//cout << "\n******************** \n";
	for (int i = 0; i < maze.size(); i++) {
		for (int j = 0; j < maze[0].size(); j++) {
			if (res[i][j] == '*')
				std::this_thread::sleep_for(chrono::milliseconds(100));
			else
				std::this_thread::sleep_for(chrono::milliseconds(50));
			cout << res[i][j] << " ";
		}
		cout << endl;
	}
	delete[] res;
	res = NULL;
	return 1;
}