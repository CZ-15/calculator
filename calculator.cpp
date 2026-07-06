#include <iostream>
#include <stack>   //使用栈需要
#include <cctype>  //使用isdigit需要这个头文件
#include <string>
#include <vector> //提供容器
using namespace std;

class calculator4 {
private:
	vector<string> tokens; //存储分词结果

	void fenci(string expr) { //假如分词中存着“12”（在tokens里的第一位置），那在分词中的t[0]长度为2(t[0]取tokens里的第一位置)
		tokens.clear();  //tokens作为fenci方法中的动态数组
		for (size_t i = 0;i < expr.length();i++) { //size_t i = 0 的意思是：定义一个无符号整数变量 i 并初始化为 0
			if (isspace(expr[i])) {
				continue;
			}

			if (isdigit(expr[i])) {
				string num;
				while (i < expr.length() && isdigit(expr[i])) {
					num += expr[i];
					i++;
				}
				i--; //因为外层会i++，所以这里需要先减1，等到再经过外层后再加回来
				tokens.push_back(num);
			}
			else {
				tokens.push_back(string(1, expr[i]));
			}
		}
	}

	bool isValid() {
		// 检查1：空表达式
		if (tokens.empty()) {
			cout << "错误！原因：表达式为空" << endl;
			return false;
		}

		// 检查2：括号匹配
		int balance = 0;
		for (string t : tokens) {
			if (t == "(") balance++;
			if (t == ")") balance--;
			if (balance < 0) {
				cout << "错误！原因：括号不匹配" << endl;
				return false;
			}
		}
		if (balance != 0) {
			cout << "错误！原因：括号不匹配" << endl;
			return false;
		}

		// 检查3：非法字符
		for (string t : tokens) {
			if (t.length() == 1) {
				char c = t[0];
				if (!isdigit(c) && c != '+' && c != '-' &&
					c != '*' && c != '/' && c != '(' && c != ')') {
					cout << "错误！原因：非法字符 '" << c << "'" << endl;
					return false;
				}
			}
			else {
				// 多位数，检查是否全是数字
				for (char c : t) {
					if (!isdigit(c)) {
						cout << "错误！原因：非法字符 '" << c << "'" << endl;
						return false;
					}
				}
			}
		}

		return true;
	}


	int getpriority(char op) {
		if (op == '+' || op == '-')return 1;
		if (op == '*' || op == '/')return 2;
		return 0;
	}
	double jisuan(double a, double b, char op) {
		if (op == '+')return a + b;
		if (op == '-')return a - b;
		if (op == '*')return a * b;
		if (op == '/')return a / b;
		return 0;
	}
public:
	double calculate(string expr) {  //arr是字符型数字
		stack<double>nums;
		stack<char>ops;

		for (size_t i = 0; i < expr.length(); i++) {
			char c = expr[i];
			if (c == ' ')continue;
			if (isdigit(c)) {
				nums.push(c - '0');
			}
			else if(c == '(') {
				ops.push(c);
			}
			else if (c == ')') {
				while (ops.top() != '(') {
					double b = nums.top();
					nums.pop();
					double a = nums.top();
					nums.pop();
					char op = ops.top();
					ops.pop();
					nums.push(jisuan(a, b, op));
				}
				ops.pop();  //因为在入栈时就已经排好顺序了，所以可以直接计算
			}
			else if (c == '+' || c == '-' || c == '*' || c == '/') {
				while (!ops.empty() && ops.top()!='(' && getpriority(ops.top()) >= getpriority(c)) {
					double b = nums.top();
					nums.pop();
					double a = nums.top();
					nums.pop();
					char op = ops.top();
					ops.pop();
					nums.push(jisuan(a, b, op));
				}
				ops.push(c);
			}
		}

		while (!ops.empty()) {
			double b = nums.top();
			nums.pop();
			double a = nums.top();
			nums.pop();
			char op = ops.top();
			ops.pop();
			nums.push(jisuan(a, b, op));
		}
		
		return nums.top();
			   
	}
};

int main() {
	calculator4 ce4;
	cout << "1+2 = " << ce4.calculate("1+2") << endl;
	cout << "1+2*5+6 = " << ce4.calculate("1+2*5+6") << endl;

	return 0;
}
