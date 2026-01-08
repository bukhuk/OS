#include <windows.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#undef min
#undef max

class Matrix {
public:
    Matrix(int r, int c) : rows_(r), cols_(c) {
        data_.resize(rows_, std::vector<int>(cols_, 0));
    }

    void randomFill(int min_val = 0, int max_val = 10) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(min_val, max_val);

        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                data_[i][j] = dist(gen);
            }
        }
    }

    int get(int i, int j) const { return data_[i][j]; }
    void set(int i, int j, int value) { data_[i][j] = value; }

    int getRows() const { return rows_; }
    int getCols() const { return cols_; }

    void print() const {
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                std::cout << std::setw(4) << data_[i][j];
            }
            std::cout << "\n";
        }
    }

private:
    std::vector<std::vector<int>> data_;
    int rows_;
    int cols_;
};

struct ThreadData {
    const Matrix* a;
    const Matrix* b;
    Matrix* c;
    int block_size;
    int row_block;
    int col_block;
    CRITICAL_SECTION* cs;
};

DWORD WINAPI MultiplyBlock(LPVOID lp_param) {
auto* data = static_cast<ThreadData*>(lp_param);

int start_row = data->row_block * data->block_size;
int start_col = data->col_block * data->block_size;

int end_row = std::min(start_row + data->block_size, data->a->getRows());
int end_col = std::min(start_col + data->block_size, data->c->getCols());
int common = data->b->getRows();

for (int i = start_row; i < end_row; ++i) {
for (int j = start_col; j < end_col; ++j) {
int sum = 0;
for (int k = 0; k < common; ++k) {
sum += data->a->get(i, k) * data->b->get(k, j);
}

EnterCriticalSection(data->cs);
data->c->set(i, j, data->c->get(i, j) + sum);
LeaveCriticalSection(data->cs);
}
}

delete data;
return 0;
}



class BlockMultiplier {
public:
    BlockMultiplier(const Matrix& a, const Matrix& b, Matrix& c, int k)
            : a_(a), b_(b), c_(c), block_size_(k) {
        InitializeCriticalSection(&cs_);
    }

    ~BlockMultiplier() {
        DeleteCriticalSection(&cs_);
    }

    void multiplyMatricesParallel() {
        int num_row_blocks = (a_.getRows() + block_size_ - 1) / block_size_;
        int num_col_blocks = (c_.getCols() + block_size_ - 1) / block_size_;

        std::vector<HANDLE> thread_handles;
        thread_handles.reserve(num_row_blocks * num_col_blocks);

        for (int i = 0; i < num_row_blocks; ++i) {
            for (int j = 0; j < num_col_blocks; ++j) {
                auto* data = new ThreadData{&a_, &b_, &c_, block_size_, i, j, &cs_};

                HANDLE h_thread = CreateThread(nullptr, 0, MultiplyBlock, data, 0, nullptr);

                if (h_thread != nullptr) {
                    thread_handles.push_back(h_thread);
                } else {
                    delete data;
                }
            }
        }

        if (!thread_handles.empty()) {
            WaitForMultipleObjects(static_cast<DWORD>(thread_handles.size()),
                                   thread_handles.data(), TRUE, INFINITE);
        }

        for (HANDLE h_thread : thread_handles) {
            CloseHandle(h_thread);
        }
    }

private:
    const Matrix& a_;
    const Matrix& b_;
    Matrix& c_;
    int block_size_;
    CRITICAL_SECTION cs_;
};

Matrix multiplyMatricesSimple(const Matrix& a, const Matrix& b) {
    int n = a.getRows();
    int m = a.getCols();
    int p = b.getCols();

    Matrix c(n, p);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            int sum = 0;
            for (int k = 0; k < m; ++k) {
                sum += a.get(i, k) * b.get(k, j);
            }
            c.set(i, j, sum);
        }
    }

    return c;
}

int main() {
    const int n_size = 200;

    std::cout << "Matrix size: " << n_size << "x" << n_size << "\n";

    Matrix a(n_size, n_size);
    Matrix b(n_size, n_size);

    a.randomFill();
    b.randomFill();

    std::cout << "classical algorithm:\n";
    auto start = std::chrono::high_resolution_clock::now();
    Matrix c_simple = multiplyMatricesSimple(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_simple = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "time: " << duration_simple.count() << " ms\n\n";

    std::cout << std::setw(10) << "Block size"
              << std::setw(15) << "Num blocks"
              << std::setw(15) << "Num threads"
              << std::setw(15) << "Time (ms)"
              << std::setw(15) << "Speedup" << "\n";
    std::cout << std::string(70, '-') << "\n";

    for (int k = 1; k <= n_size; k++) {
        Matrix c_parallel(n_size, n_size);
        BlockMultiplier multiplier(a, b, c_parallel, k);

        int num_row_blocks = (n_size + k - 1) / k;
        int num_col_blocks = (n_size + k - 1) / k;
        int num_blocks = num_row_blocks * num_col_blocks;

        auto p_start = std::chrono::high_resolution_clock::now();
        multiplier.multiplyMatricesParallel();
        auto p_end = std::chrono::high_resolution_clock::now();
        auto duration_parallel = std::chrono::duration_cast<std::chrono::milliseconds>(p_end - p_start);

        double speedup = static_cast<double>(duration_simple.count()) /
                         (duration_parallel.count() > 0 ? duration_parallel.count() : 1);

        std::cout << std::setw(10) << k
                  << std::setw(15) << num_blocks
                  << std::setw(15) << num_blocks
                  << std::setw(15) << duration_parallel.count()
                  << std::setw(15) << std::fixed << std::setprecision(2) << speedup << "\n";
    }

    return 0;
}