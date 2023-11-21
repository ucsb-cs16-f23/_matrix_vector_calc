#include "my_Vars.h"
#include<utility>
std::list<MyVar> tmp_var_list;

MyVar::MyVar()
{
    my_data.mat = nullptr;
    var_name = NULL;
    var_type = null_var;
}

MyVar::MyVar(_m_type _type, const char* name, void* ptr)
{
    var_type = _type;
    var_name = NULL;
    SetName(name);
    allocate_space();
    if (ptr)copy_data(ptr);
}

MyVar::~MyVar()
{
    discard_data();
    if(var_name)delete[] var_name;
}

const MyVar& MyVar::op_add(const MyVar& b)
{
    if (var_type == null_var) {
        assign_val(b);
        return *this;
    }
    if (b.var_type != var_type)throw 10000;
    switch (var_type)
    {
    case MyVar::number:
        my_data.num->operator+=(*b.my_data.num);
        break;
    case MyVar::vector:
        my_data.vec->operator+=(*b.my_data.vec);
        break;
    case MyVar::matrix:
        my_data.mat->operator+=(*b.my_data.mat);
        break;
    }
    return *this;
}

const MyVar& MyVar::op_mul(const MyVar& b)
{
    switch (var_type)
    {
    case MyVar::number:
    {
        switch (b.var_type)
        {
        case matrix:
            this->assign_val(b.my_data.mat->multipy_val(*my_data.num));
            break;
        case number:
            my_data.num->operator*=(*b.my_data.num);
            break;
        case vector:
            this->assign_val(b.my_data.vec->mutiply_num(*my_data.num));
            break;
        }
    }
    break;
    case MyVar::vector:
    {
        switch (b.var_type)
        {
        case number:
            this->assign_val(my_data.vec->mutiply_num(*b.my_data.num));
            break;
        case matrix:
            this->assign_val(b.my_data.mat->multipy_vec(*my_data.vec));
            break;
        default:
            throw 10000;
            break;
        }
    }
    break;
    case MyVar::matrix:
    {
        switch (b.var_type)
        {
        case matrix:
            this->assign_val(my_data.mat->multipy_mat(*b.my_data.mat));
            break;
        case number:
            this->assign_val(my_data.mat->multipy_val(*b.my_data.num));
            break;
        case vector:
            this->assign_val(my_data.mat->multipy_vec(*b.my_data.vec));
            break;
        }
    }
    break;
    case null_var:
        assign_val (b);
    }
    return *this;
}

void MyVar::SetName(const char* _name)
{
    if(var_name)delete[] var_name;
    var_name = NULL;
    if (_name == NULL)return;
    auto len = strlen(_name) + 1;
    var_name = new char[len];
    memcpy_s(var_name, len, _name, len);
}

MyVar::MyVar(const MyVar& _left)
{
    var_name = NULL;
    SetName(_left.var_name);
    var_type = _left.var_type;
    allocate_space();
    copy_data(_left.my_data.native_ptr);
}

MyVar::MyVar(MyVar&& _right)noexcept
{
    var_name = _right.var_name;
    _right.var_name = NULL;
    my_data.native_ptr = _right.my_data.native_ptr;
    _right.my_data.native_ptr = NULL;
    var_type = _right.var_type;
    _right.var_type = null_var;
}

const MyVar& MyVar::operator=(const MyVar& _left)
{
    SetName(_left.var_name);
    discard_data();
    var_type = _left.var_type;
    allocate_space();
    copy_data(_left.my_data.native_ptr);
    return *this;
}

const MyVar& MyVar::operator=(MyVar&& _right) noexcept
{
    if (var_name)delete[] var_name;
    discard_data();
    var_name = _right.var_name;
    _right.var_name = NULL;
    my_data.native_ptr = _right.my_data.native_ptr;
    _right.my_data.native_ptr = NULL;
    var_type = _right.var_type;
    _right.var_type = null_var;
    return* this;
}

bool MyVar::operator==(const MyVar& b) const
{
    return false;
}

void MyVar::_Not()
{
    if (var_type != number)throw "bad call of not(number)!";
    my_data.num->_up = (my_data.num->_up) ? 0 : 1;
}

void MyVar::set_negetive()
{
    switch (var_type)
    {
    case MyVar::number:
        my_data.num->_up = -my_data.num->_up;
        break;
    case MyVar::vector:
        my_data.vec->_set_v_neg();
        break;
    case MyVar::matrix:
        my_data.mat->_set_v_neg();
        break;
    }
}

MyVar MyVar::_at(const MyVar& _i) const
{
    MyVar ret;
    if (_i.var_type != MyVar::number)throw 113;
    switch (var_type)
    {
    case MyVar::matrix:
        ret.assign_val(my_data.mat->ExtractRowVector((int)_i.my_data.num->_up));
        break;
    case MyVar::vector:
        ret.assign_val<MyNum>(my_data.vec->operator[]((int)_i.my_data.num->_up));
        break;
    default:
        throw "operator \'[]\' is called on a number";
        break;
    }
    return ret;
}

void MyVar::discard_data()
{
    switch (var_type)
    {
    case MyVar::number:
        delete my_data.num;
        break;
    case MyVar::vector:
        delete my_data.vec;
        break;
    case MyVar::matrix:
        delete my_data.mat;
        break;
    default:
        return;
    }
    var_type = null_var;
}

void MyVar::copy_data(const void* Ptr)
{
    switch (var_type)
    {
    case MyVar::number:
        *my_data.num = *((MyNum*)Ptr);
        break;
    case MyVar::vector:
        *my_data.vec = *((Vector*)Ptr);
        break;
    case MyVar::matrix:
        *my_data.mat = *((Matrix*)Ptr);
        break;
    }
}

void MyVar::allocate_space()
{
    switch (var_type)
    {
    case MyVar::number:
        my_data.num = new MyNum;
        break;
    case MyVar::vector:
        my_data.vec = new Vector;
        break;
    case MyVar::matrix:
        my_data.mat = new Matrix(1,1);
        break;
    }
}

MyVar* _create_tmp_var(Matrix&& _mat, const char* _name)
{
    tmp_var_list.push_back(MyVar());
    tmp_var_list.back().SetName(_name);
    tmp_var_list.back().assign_val(::std::move(_mat));
    return &tmp_var_list.back();
}

MyVar* REF(const Matrix& mat)
{
    Matrix mx = mat;
    auto x = _create_tmp_var(std::move(mx), "Func_REF_tmp_ret_Val");
    x->my_data.mat->reduced_row_echolon_form();
    return x;
}
