#include "../includes/ft_int_to_string.hpp"


std::string ft_int_to_string(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}