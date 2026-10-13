#include <iostream>
#include <iomanip>
double GetAverage(const int* arr, int n) {
  double ans = 0;
  for (int i = 0; i  < n; i++) {
    ans += arr[i];
  }
  ans = ans / n;
  return ans;
}
void GetMinMax(const int* arr, int n, int& min_temp, int&max_temp) {
  min_temp = arr[0];
  max_temp = arr[0];
  for (int i = 1; i < n; i++) {
    min_temp = std::min(min_temp, arr[i]);
    max_temp = std::max(max_temp, arr[i]);
  }
}
int GetCold(const int* arr, int n) {
  int cnt = 0;
  for (int i = 0; i < n; i++) {
    if (arr[i] < 0) {
      cnt += 1;
    }
  }
  return cnt;
}
int main() {
  int n;
  std::cin >> n;
  if (n <= 0) {
    return 0;
  }
  int* arr = new int[n];
  for (int i = 0; i < n; i++) {
    std::cin >> arr[i];
  }
  int min_temp;
  int max_temp;
  double average = GetAverage(arr, n);
  GetMinMax(arr, n, min_temp, max_temp);
  int cold = GetCold(arr, n);
  std::cout << std::fixed << std::setprecision(1) << average << "\n";
  std::cout << min_temp << " " << max_temp << "\n" << cold << "\n";
  delete[] arr;
  return 0;
}
