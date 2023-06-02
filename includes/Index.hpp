#pragma once

class Index{
    public:
        Index();
        ~Index();
        bool taken;
    private:
        Index(const Index &other);
        Index &operator = (const Index &other);
};