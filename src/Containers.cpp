
// Containers.cpp

#include "../include/Containers.hpp"
#include "../include/Logging/Logger.hpp"

#include <sstream>


uint8_t encodeKeyStates(const KeyStates& ks) {
    uint8_t data = 0;
    data |= (ks.left_click) ? 0b00100000 : 0;
    data |= (ks.use_button) ? 0b00010000 : 0;
    data |= (ks.w)          ? 0b00001000 : 0;
    data |= (ks.a)          ? 0b00000100 : 0;
    data |= (ks.s)          ? 0b00000010 : 0;
    data |= (ks.d)          ? 0b00000001 : 0;
    return data;
}
void decodeKeyStates(const uint8_t& data, KeyStates& ks) {
    ks.left_click = (data & 0b00100000) ? 1 : 0;
    ks.use_button = (data & 0b00010000) ? 1 : 0;
    ks.w =          (data & 0b00001000) ? 1 : 0;
    ks.a =          (data & 0b00000100) ? 1 : 0;
    ks.s =          (data & 0b00000010) ? 1 : 0;
    ks.d =          (data & 0b00000001) ? 1 : 0;
}

// -------------------------------------------------//
//                                                  //
//                  UDP MESSAGE                     //
//                                                  //
// -------------------------------------------------//

PacketData::PacketData(const Uint8* data, int len) {
    if(data == nullptr || len < 1) {
        return; // ignore empty data
    }
    // kopiramo podatke v vektor
    std::copy(data, data + len, std::back_inserter(this->data));

    // ce je dolzina manjsa od 1, dodamo se en byte
    if(this->data.size() < 1) {
        this->data.push_back(0);
    }
}

std::unique_ptr<Uint8[]> PacketData::getRawData() {
    Uint8* out = new Uint8[data.size()];
    std::memcpy(out, data.data(), data.size());
    return std::unique_ptr<Uint8[]>(out);
}

size_t PacketData::size() const {
    return data.size();
}

void PacketData::clear() {
    data.clear();
}

void PacketData::reset() {
    data.clear();
    data.push_back(0); // vedno imamo vsaj en byte
}

Uint8& PacketData::flags() {
    if(data.size() <= 0)
        data.push_back(0);
    return data[0];
}

Uint8& PacketData::operator[](int i) {
    return data[i];
}

void PacketData::append(const Uint8* data, int len) {
    this->data.insert(this->data.end(), data, data + len);
}

std::string PacketData::dump() {
    std::ostringstream oss;
    oss << "[";

    for (size_t i = 0; i < data.size(); ++i) {
        oss << static_cast<int>(data.data()[i]); // Cast to int for proper representation
        if (i < data.size() - 1) {
            oss << ", ";
        }
    }

    oss << "]";
    return oss.str();
}

template<>
void PacketData::append<float>(float data) {
    static_assert(sizeof(float) == 4, "Unexpected float size.");

    Uint32 asInt;
    std::memcpy(&asInt, &data, sizeof(float));

    Uint32 network_order;
    SDLNet_Write32(asInt, &network_order);

    this->data.insert(this->data.end(),
                      reinterpret_cast<Uint8*>(&network_order),
                      reinterpret_cast<Uint8*>(&network_order) + sizeof(Uint32));
}

template<>
void PacketData::getByOffset(float& target, size_t size, size_t offset) {
    if (offset + sizeof(float) > data.size()) {
        throw std::out_of_range("Offset out of bounds");
    }

    Uint32 network_order;
    std::memcpy(&network_order, &data[offset], sizeof(Uint32));

    Uint32 host_order = SDLNet_Read32(&network_order);
    std::memcpy(&target, &host_order, sizeof(float)); // Reinterpret as float
}
