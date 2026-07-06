#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>
using namespace std;

struct TreeNode {
	string value;      
	TreeNode* left;
	TreeNode* right;

	TreeNode(string val) : value(val), left(nullptr), right(nullptr) {}
};

class Calculator {
private:
	TreeNode* root;
	string expression;
	vector<string> tokens;

	// 第1步：分词
	void tokenize() {
		tokens.clear();

		for (size_t i = 0; i < expression.length(); i++) {
			if (isspace(expression[i])) continue;  

			// 处理数字
			if (isdigit(expression[i]) || expression[i] == '.') {
				string num;
				while (i < expression.length() && (isdigit(expression[i]) || expression[i] == '.')) {
					num += expression[i];
					i++;
				}
				i--;
				tokens.push_back(num);
			}
			// 处理负号（在开头或左括号后）
			else if (expression[i] == '-' && (i == 0 || expression[i - 1] == '(')) {
				string num = "-";
				i++;
				// 负号后面跟数字或小数点
				while (i < expression.length() && (isdigit(expression[i]) || expression[i] == '.')) {
					num += expression[i];
					i++;
				}
				i--;
				tokens.push_back(num);
			}
			else {
				tokens.push_back(string(1, expression[i]));
			}
		}
	}

	// 第2步：合法性检查
	bool isValid() {
		// 检查空表达式
		if (tokens.empty()) {
			cout << "错误：表达式为空" << endl;
			return false;
		}

		// 检查括号匹配
		int balance = 0;
		int pos = 0;
		for (string t : tokens) {
			if (t == "(") balance++;
			if (t == ")") balance--;
			if (balance < 0) {
				cout << "错误：在第" << pos + 1 << "个位置有多余的右括号" << endl;
				return false;
			}
			pos++;
		}
		if (balance != 0) {
			cout << "错误：括号不匹配，缺少 " << balance << " 个右括号" << endl;
			return false;
		}

		// 检查空括号 ()
		for (size_t i = 0; i < tokens.size() - 1; i++) {
			if (tokens[i] == "(" && tokens[i + 1] == ")") {
				cout << "错误：发现空括号 '()'" << endl;
				return false;
			}
		}

		// 检查非法字符
		for (size_t i = 0; i < tokens.size(); i++) {
			string t = tokens[i];
			if (t.length() == 1) {
				char c = t[0];
				if (!isdigit(c) && c != '+' && c != '-' &&
					c != '*' && c != '/' && c != '(' && c != ')') {
					cout << "错误：在第" << i + 1 << "个位置发现非法字符 '" << c << "'" << endl;
					return false;
				}
			}
			else {
				// 检查数字或负数是否合法
				for (char c : t) {
					if (c != '-' && !isdigit(c) && c != '.') {
						cout << "错误：在第" << i + 1 << "个位置发现非法数字格式" << endl;
						return false;
					}
				}
				// 检查小数点个数
				int dotCount = 0;
				for (char c : t) {
					if (c == '.') dotCount++;
				}
				if (dotCount > 1) {
					cout << "错误：数字 '" << t << "' 中有多个小数点" << endl;
					return false;
				}
				// 检查负号位置（负数只能有一个负号在开头）
				if (t[0] == '-') {
					for (size_t j = 1; j < t.length(); j++) {
						if (t[j] == '-') {
							cout << "错误：数字 '" << t << "' 中有多个负号" << endl;
							return false;
						}
					}
				}
			}
		}

		// 检查连续运算符
		for (size_t i = 0; i < tokens.size() - 1; i++) {
			string t1 = tokens[i];
			string t2 = tokens[i + 1];

			// 检查运算符连续
			if ((t1 == "+" || t1 == "-" || t1 == "*" || t1 == "/") &&
				(t2 == "+" || t2 == "-" || t2 == "*" || t2 == "/")) {
				cout << "错误：在第" << i + 1 << "个位置发现连续运算符 '" << t1 << t2 << "'" << endl;
				return false;
			}
		}

		// 检查以运算符开头
		string first = tokens[0];
		if ((first == "+" || first == "*" || first == "/")) {
			cout << "错误：表达式不能以运算符 '" << first << "' 开头" << endl;
			return false;
		}

		// 检查以运算符结尾
		string last = tokens.back();
		if (last == "+" || last == "-" || last == "*" || last == "/") {
			cout << "错误：表达式不能以运算符结尾" << endl;
			return false;
		}

		// 检查括号后是否紧跟着数字或左括号
		for (size_t i = 0; i < tokens.size() - 1; i++) {
			if (tokens[i] == "(") {
				string next = tokens[i + 1];
				if (next == "+" || next == "*" || next == "/") {
					cout << "错误：左括号后不能直接跟运算符 '" << next << "'" << endl;
					return false;
				}
			}
		}

		return true;
	}

	// 优先级处理
	int getPriority(char op) {
		if (op == '+' || op == '-') return 1;
		if (op == '*' || op == '/') return 2;
		return 0;
	}

	bool isOperator(const string& s) {
		return s == "+" || s == "-" || s == "*" || s == "/";
	}

	// 第3步：构建树
	void buildTree() {
		stack<TreeNode*> operands;  // 操作数栈
		stack<string> operators;    // 运算符栈

		for (string token : tokens) {
			// 1：数字
			if (isdigit(token[0]) || (token[0] == '-' && token.length() > 1)) {
				operands.push(new TreeNode(token));
			}
			// 2：左括号
			else if (token == "(") {
				operators.push(token);
			}
			// 3：右括号
			else if (token == ")") {
				while (operators.top() != "(") {
					popAndBuild(operands, operators);
				}
				operators.pop(); // 弹出 '('
			}
			// 4：运算符
			else if (isOperator(token)) {
				while (!operators.empty() && operators.top() != "(" &&
					getPriority(operators.top()[0]) >= getPriority(token[0])) {
					popAndBuild(operands, operators);
				}
				operators.push(token);
			}
		}

		// 处理剩余运算符
		while (!operators.empty()) {
			popAndBuild(operands, operators);
		}

		root = operands.top();
	}

	// 弹出并构建子树
	void popAndBuild(stack<TreeNode*>& operands, stack<string>& operators) {
		string op = operators.top(); operators.pop();
		TreeNode* right = operands.top(); operands.pop();
		TreeNode* left = operands.top(); operands.pop();

		TreeNode* node = new TreeNode(op);
		node->left = left;
		node->right = right;

		operands.push(node);
	}

	// 第4步：计算
	double evaluate(TreeNode* node) {
		// 叶子节点：数字
		if (node->left == nullptr && node->right == nullptr) {
			return stod(node->value);
		}

		double leftVal = evaluate(node->left);
		double rightVal = evaluate(node->right);

		char op = node->value[0];
		switch (op) {
		case '+': return leftVal + rightVal;
		case '-': return leftVal - rightVal;
		case '*': return leftVal * rightVal;
		case '/':
			if (rightVal == 0) {
				throw "除零错误";
			}
			return leftVal / rightVal;
		}
		return 0;
	}

	void deleteTree(TreeNode* node) {
		if (node == nullptr) return;
		deleteTree(node->left);
		deleteTree(node->right);
		delete node;
	}

public:
	Calculator() : root(nullptr) {}

	~Calculator() {
		deleteTree(root);
	}

	void setExpression(const string& expr) {
		expression = expr;
	}

	double calculate() {
		// 第1步：分词
		tokenize();

		// 第2步：合法性检查
		if (!isValid()) {
			return NAN;
		}

		// 第3步：构建树
		try {
			buildTree();
		}
		catch (...) {
			cout << "构建树失败" << endl;
			return NAN;
		}

		// 第4步：计算
		try {
			return evaluate(root);
		}
		catch (const char* msg) {
			cout << msg << endl;
			return NAN;
		}
	}
};


int main() {
	Calculator calc;
	string input;

	cout << "欢迎您使用四则运算计算器" << endl;
	cout << "注意：" << endl;
	cout << "输入 quit 可以退出" << endl;

	while (true) {
		cout << "\n请输入表达式：";
		getline(cin, input);

		if (input == "quit") {
			cout << "再见，欢迎下次使用！" << endl;
			break;
		}

		if (input.empty()) continue;

		calc.setExpression(input);
		double result = calc.calculate();

		if (!isnan(result)) {
			cout << "计算结果：" << result << endl;
		}
	}

	return 0;
}