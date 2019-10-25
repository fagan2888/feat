#include <typeindex>
#include <typeinfo>
#include <Eigen/Dense>
using Eigen::MatrixXf;
using Eigen::VectorXf;
using Eigen::ArrayXf;
using Eigen::ArrayXi;
typedef Eigen::Array<bool,Eigen::Dynamic,1> ArrayXb;

class State 
{
    public:

        ArrayXf float_array;
        ArrayXi int_array;
        ArrayXb bool_array;
        float f;
        int i;
        bool b;

        template <typename T> inline T& get()
        {
            return get<T>();
        }
        template <typename T> inline T& operator [](std::type_info)
        {
            return get<T>();
        }
    
};
template <> inline ArrayXf& State::get(){ return float_array; }
            
template <> inline ArrayXb& State::get(){ return bool_array; }
        
template <> inline ArrayXi& State::get(){ return int_array; }

template <> int& State::get(){ return i; }

template <> bool& State::get(){ return b; }

template <> float& State::get(){ return f; }
