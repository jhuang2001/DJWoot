
#include "water.h"


namespace water {

NodeGrid::NodeGrid(int rows, int cols) : m_rows(rows), m_cols(cols) {
    grid.assign(cols,std::vector<float>(rows, 0.0f));
}

void NodeGrid::press(int x, int y, float amount) {
    // TODO: potential unsafe read
    // should be OK since it will just result in extra buffer entries
    if (amount != grid[x][y]) writeBuff(x,y,amount);
}

void NodeGrid::tick() {

}

bool NodeGrid::readBuff(int& x, int& y, float& value) {
    if (read_i == write_i) return false;

    std::tie(x,y,value) = input_buffer[read_i];
    
    read_i = (read_i + 1) & (NODEBUFFERSIZE - 1);
    return true;
}

bool NodeGrid::writeBuff(const int& x, const int& y, const float& value){
    auto nextWrite = (write_i+1) & (NODEBUFFERSIZE - 1);
    if (nextWrite == read_i) return false;

    input_buffer[write_i] = {x,y,value};
    write_i = nextWrite;
    return true;
}


} // namespace water