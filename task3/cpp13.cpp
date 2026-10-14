#include <iostream>
void Clear(char board[5][10]) {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 10; j++) {
      board[i][j] = ' ';
    }
  }
}
bool Check(int x, int y) {
  return (y >= 0 && y < 5 && x >= 0 && x < 10);
}
bool Put(char board[5][10], int x, int y, char c) {
  if (!Check(x, y)) {
    return false;
  }
  board[y][x] = c;
  return true;
}
int L(const char* ch) {
  int len = 0;
  while (ch[len] != '\0') {
    len++;
  }
  return len;
}
bool Text(char board[5][10], int x, int y, const char* c) {
  int len = L(c);
  if (!(x, y) || !Check(x + len - 1, y)) {
    return false;
  }
  for (int i = 0; i < len; i++) {
    board[y][x + i] = c[i];
  }
  return true;
}
void Print(char board[5][10]) {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 10; j++) {
      std::cout << board[i][j];
    }
    std::cout << "\n";
  }
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
  char board[5][10];
  Clear(board);
  while (true) {
    char cmd[7] = {0};
    int i = 0;
    char ch = '0';
    while (std::cin.get(ch) && (ch == ' ' || ch == '\n')) {
    }
    while (ch != ' ' && ch != '\n') {
      if (i < 6) {
        cmd[i] = ch;
        i++;
      }
      std::cin.get(ch);
    }
    cmd[i] = '\0';
    if (F(cmd, "clear")) {
      Clear(board);
    }
    else if (F(cmd, "put")) {
      int x;
      int y;
      char c;
      std::cin >> x >> y >> c;
      bool mark = Put(board, x, y, c);
      if (!mark) {
        std::cout << "error" << "\n";
      }
    }
    else if (F(cmd, "text")) {
      int x;
      int y;
      char w[12] = {0};
      std::cin >> x >> y;
      char w0 = '0';
      while (std::cin.get(w0) && (w0 == ' ' || w0 == '\n')) {
      }
      int i0 = 0;
      while (w0 != ' ' && w0 != '\n') {
        if (i0 < 11) {
          w[i0] = w0;
          i0++;
        }
        std::cin.get(w0);
      }
      w[i0] = '\0';
      bool mark = Text(board, x, y, w);
      if (!mark) {
        std::cout << "error" << "\n";
      }
    }
    else if (F(cmd, "print")) {
      Print(board);
    }
    else if (F(cmd, "quit")) {
      return 0;
    }
    else {
      std::cout << "error" << "\n";
    }
  }
  return 0;
}
