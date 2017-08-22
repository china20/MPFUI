
#include <System/Windows/Object.h>

namespace suic
{

struct UVSize
{
    UVSize(Orientation orientation, int width, int height)
    { 
        U = 0;
        V = 0;
        hori = orientation == Orientation::Horizontal; 
        SetWidth(width); 
        SetHeight(height);
    } 

    UVSize(Orientation orientation)
    {
        U = 0;
        V = 0;
        hori = orientation == Orientation::Horizontal; 
    } 

    int GetWidth() const
    {
        return (hori ? U : V);
    }

    int GetHeight() const
    {
        return (hori ? V : U);
    }

    void SetWidth(int val)
    {
        if (hori) 
        {
            U = val; 
        }
        else 
        {
            V = val;
        }
    }

    void SetHeight(int val)
    {
        if (hori) 
        {
            V = val; 
        }
        else 
        {
            U = val;
        }
    }

    int U;
    int V; 
    bool hori;
};

}
