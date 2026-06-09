#include <vector>
#include <atomic>

typedef std::vector<std::vector<float>> vvf;

namespace water {

// for optimization, this must be a power of 2
constexpr int NODEBUFFERSIZE = 256;

class NodeGrid {
public:
    NodeGrid(int rows, int cols);
    void tick();

    
    // press() must be called from the same thread (or ISR) to avoid contention
    void press(int x, int y, float amount);

protected:
    // read and writeBuff are SPSC
    // Only ONE consumer thread should ever read from the buffer
    // Only ONE producer thread (or ISR) should ever write to the buffer
    bool readBuff(int& x, int& y, float& value);
    bool writeBuff(const int& x, const int& y, const float& value);

private:
    int m_rows, m_cols;
    vvf grid;

    std::atomic<int8_t> read_i;
    volatile std::atomic<int8_t> write_i;
    std::tuple<int,int,float> input_buffer[NODEBUFFERSIZE];
    
};


}