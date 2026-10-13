#include <iostream>
void Deposit(int& bal) {
  int k;
  std::cin >> k;
  if (k <= 0) {
    std::cout << "error" << "\n";
  } else {
    bal += k;
  }
}
void Withdraw(int& bal) {
  int k;
  std::cin >> k;
  if (k <= 0 || bal < k) {
    std::cout << "error" << "\n";
  } else {
    bal -= k;
  }
}
void Balance(int& bal) {
  std::cout << bal << "\n";
}
void Reset(int& bal) {
  bal = 0;
}
bool F(const char* a, const char* b) {
  int i = 0;
  while (a[i] != '\0' && b[i] != '\0') {
    if (a[i] != b[i]) {
      return false;
    }
    i++;
  }
  return a[i] == b[i];
}
int main() {
  int bal = 0;
  const char* names[4] = {"deposit", "withdraw", "balance", "reset"};
  void (*funcs[4])(int&) = {Deposit, Withdraw, Balance, Reset};
  while (true) {
    char cmd[10] = {0};
    int i = 0;
    char ch = '0';
    while (std::cin.get(ch) && (ch == ' ' || ch == '\n')) {
    }
    while (ch != ' ' && ch != '\n') {
      if (ch == ' ' || ch == '\n') {
        break;
      }
      if (i < 9) {
        cmd[i] = ch;
        i++;
      }
      std::cin.get(ch);
    }
    cmd[i] = '\0';
    if (F(cmd, "stop")) {
      return 0;
    }
    bool ok = false;
    for (int i = 0; i < 4; i++) {
      if (F(cmd, names[i])) {
        funcs[i](bal);
        ok = true;
        break;
      }
    }
    if (!ok) {
      std::cout << "error" << "\n";
    }
  }
  return 0;
}

