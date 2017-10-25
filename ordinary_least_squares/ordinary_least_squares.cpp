// Lab2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <utility>
#include <iostream>
#include "ordinary_least_squares.h"

struct Function {
	std::vector<double> x;
	double y;
};

double sumOfValues(const std::vector<double>& xs, size_t degree) {
	if (degree == 0) {
		return 1.0;
	}

	double sum = 0.0;

	for (auto& x : xs) {
		sum += pow(x, degree);
	}
	return sum;
}

double sumOfValuesWithFunctions(const std::vector<double>& xs, const std::vector<double>& ys, size_t degree) {
	double sum = 0.0;

	for (size_t i = 0; i < xs.size(); i++) {
		sum += pow(xs[i], degree) * ys[i];
	}
	return sum;
}

std::vector<Function> makeSlu(const std::vector<double>& xs, const std::vector<double>& ys, int m) {

	std::vector<Function> slu(m, { std::vector<double>(m, 0.0), 0.0 });


	for (size_t line = 0; line < slu.size(); line++) {
		for (size_t i = 0; i < slu[line].x.size(); i++) {
			slu[line].x[i] = sumOfValues(xs, line + i);
					
		}
		slu[line].y = sumOfValuesWithFunctions(xs, ys, line);
	}

	slu[0].x[0] = xs.size();

	return std::move(slu);
}


int gauss(std::vector<std::vector<double>> a, std::vector<double>& ans) {
	int n = (int)a.size();
	int m = (int)a[0].size() - 1;

	const double EPS = 0.000001;

	std::vector<int> where(m, -1);
	for (int col = 0, row = 0; col<m && row<n; ++col) {
		int sel = row;
		for (int i = row; i<n; ++i)
			if (abs(a[i][col]) > abs(a[sel][col]))
				sel = i;
		if (abs(a[sel][col]) < EPS)
			continue;
		for (int i = col; i <= m; ++i)
			std::swap(a[sel][i], a[row][i]);
		where[col] = row;

		for (int i = 0; i<n; ++i)
			if (i != row) {
				double c = a[i][col] / a[row][col];
				for (int j = col; j <= m; ++j)
					a[i][j] -= a[row][j] * c;
			}
		++row;
	}

	ans.assign(m, 0);
	for (int i = 0; i<m; ++i)
		if (where[i] != -1)
			ans[i] = a[where[i]][m] / a[where[i]][i];
	for (int i = 0; i<n; ++i) {
		double sum = 0;
		for (int j = 0; j<m; ++j)
			sum += ans[j] * a[i][j];
		if (abs(sum - a[i][m]) > EPS)
			return 0;
	}

	for (int i = 0; i<m; ++i)
		if (where[i] == -1)
			return 2;
	return 1;
}

std::vector<double> solveSlu(std::vector<Function> slu) {

	std::vector<double> ans;

	std::vector<std::vector<double>> a;
	for (size_t j = 0; j < slu.size(); j++) {
		a.push_back(std::vector<double>{});
		for (size_t i = 0; i < slu[j].x.size(); i++) {
			a[j].push_back(slu[j].x[i]);
		}
		a[j].push_back(slu[j].y);
	}

	gauss(std::move(a), ans);

	return std::move(ans);
}

std::vector<double> RunOrdinaryLeastSquares(std::vector<double> xs, std::vector<double> fs, int order) {

    std::vector<Function> slu = makeSlu(std::move(xs), std::move(fs), order);

    std::vector<double> a = solveSlu(std::move(slu));

    return std::move(a);
}
