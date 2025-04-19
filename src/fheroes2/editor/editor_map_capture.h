#pragma once

#include <cstdint>

namespace Interface
{
    class EditorInterface;

    class MapCapture
    {
    public:
        explicit MapCapture( EditorInterface & interface )
            : _interface( interface )
        {}

        void startCapture();
        void update();
        bool isActive() const
        {
            return _isActive;
        }

        void showMessage();

    private:
        EditorInterface & _interface;
        bool _isActive{ false };
        int32_t _currentScrollX{ 0 };
        int32_t _currentScrollY{ 0 };
    };
}
