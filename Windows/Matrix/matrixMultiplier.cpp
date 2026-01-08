#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

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

class BlockMultiplier {
public:
    BlockMultiplier(const Matrix& a, const Matrix& b, Matrix& c, int k)
            : a_(a), b_(b), c_(c), block_size_(k) {}

    void multiplyBlock(int row_block, int col_block) {
        int start_row = row_block * block_size_;
        int start_col = col_block * block_size_;

        int end_row = std::min(start_row + block_size_, a_.getRows());
        int end_col = std::min(start_col + block_size_, c_.getCols());
        int common = b_.getRows();

        for (int i = start_row; i < end_row; ++i) {
            for (int j = start_col; j < end_col; ++j) {
                int sum = 0;
                for (int k = 0; k < common; ++k) {
                    sum += a_.get(i, k) * b_.get(k, j);
                }

                std::lock_guard<std::mutex> lock(mtx_);
                c_.set(i, j, c_.get(i, j) + sum);
            }
        }
    }

    void multiplyMatricesParallel() {
        int num_row_blocks = (a_.getRows() + block_size_ - 1) / block_size_;
        int num_col_blocks = (c_.getCols() + block_size_ - 1) / block_size_;

        std::vector<std::thread> threads;

        for (int i = 0; i < num_row_blocks; ++i) {
            for (int j = 0; j < num_col_blocks; ++j) {
                threads.emplace_back(&BlockMultiplier::multiplyBlock, this, i, j);
            }
        }

        for (auto& t : threads) {
            t.join();
        }
    }

private:
    const Matrix& a_;
    const Matrix& b_;
    Matrix& c_;
    int block_size_;
    std::mutex mtx_;
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
    const int n_dim = 200;

    std::cout << "Matrix size: " << n_dim << "x" << n_dim << "\n";

    Matrix a(n_dim, n_dim);
    Matrix b(n_dim, n_dim);

    a.randomFill();
    b.randomFill();

    std::cout << "Classical algorithm:\n";
    auto start = std::chrono::high_resolution_clock::now();
    Matrix c_simple = multiplyMatricesSimple(a, b);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_simple = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << duration_simple.count() << " ms\n\n";

    std::cout << "Multi-threaded algorithm with different block sizes:\n";
    std::cout << std::setw(10) << "Block size"
              << std::setw(15) << "Num blocks"
              << std::setw(15) << "Num threads"
              << std::setw(15) << "Time (ms)"
              << std::setw(15) << "Speedup" << "\n";
    std::cout << std::string(70, '-') << "\n";

    for (int k = 1; k <= n_dim; ++k) {
        Matrix c_parallel(n_dim, n_dim);
        BlockMultiplier multiplier(a, b, c_parallel, k);

        int num_row_blocks = (n_dim + k - 1) / k;
        int num_col_blocks = (n_dim + k - 1) / k;
        int num_blocks = num_row_blocks * num_col_blocks;

        start = std::chrono::high_resolution_clock::now();
        multiplier.multiplyMatricesParallel();
        end = std::chrono::high_resolution_clock::now();
        auto duration_parallel = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        double speedup = 0.0;
        if (duration_parallel.count() > 0) {
            speedup = static_cast<double>(duration_simple.count()) / duration_parallel.count();
        }

        std::cout << std::setw(10) << k
                  << std::setw(15) << num_blocks
                  << std::setw(15) << num_blocks
                  << std::setw(15) << duration_parallel.count()
                  << std::setw(15) << std::fixed << std::setprecision(2) << speedup << "\n";
    }

    return 0;
}