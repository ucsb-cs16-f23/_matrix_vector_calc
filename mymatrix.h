#pragma once
#ifndef _MY_MATRIX_H_
#define _MY_MATRIX_H_
#include<iostream>

long long gcd(long long a, long long b);

long long lcm(long long a, long long b);

class MyNum {
public:
	long long int _up;
	long long int _down;
	constexpr explicit MyNum():_up(0),_down(1){}
	constexpr MyNum(long long _Up, long long _Down = 1):_up(_Up),_down(_Down){
	}
	MyNum(const MyNum& b);
	constexpr MyNum(int _Up):_up(_Up),_down(1){
	}
	~MyNum(){}

	const MyNum& operator *= (const MyNum& b);
	MyNum operator * (const MyNum& b)const;
	const MyNum& operator /= (const MyNum& b);
	MyNum operator / (const MyNum& b)const;

	const MyNum& operator+= (const MyNum & b);
	MyNum operator+ (const MyNum& b)const;
	const MyNum& operator -=(const MyNum& b);
	MyNum operator -(const MyNum& b)const;
	void simplefied();
	bool operator==(const MyNum& b)const;
	bool operator!=(const MyNum& b)const;
	bool operator<=(const MyNum& b)const;
	bool operator>=(const MyNum& b)const;
	bool operator<(const MyNum& b)const;
	bool operator>(const MyNum& b)const;
	MyNum& operator-();

	MyNum reverse()const;
};

std::ostream& operator<<(std::ostream& _stream,const MyNum& _val);
std::istream& operator>>(std::istream& _stream, MyNum& _val);

class Vector {
	using iterator = MyNum*;
	using const_iterator = const MyNum*;
	using value_type = MyNum;
protected:
	MyNum* _vec;
	void _allocate_space(MyNum* initialize_data = nullptr);
	void _copy_data(MyNum* _src);
	void _discard_data();
public:
	int length;
	Vector(int _size, MyNum* iptr = nullptr);
	constexpr Vector() :_vec(nullptr), length(0){}
	Vector(Vector&& _Right)noexcept;
	Vector(const Vector& _Left);
	~Vector();
	MyNum* data() {
		return _vec;
	}
	const MyNum* data()const {
		return _vec;
	}

	iterator begin() {
		return _vec;
	}
	const_iterator begin()const {
		return _vec;
	}
	iterator end() {
		return _vec + length;
	}
	const_iterator end()const {
		return _vec + length;
	}
	
	MyNum& operator[](int indix) {
		return _vec[indix];
	}
	const MyNum& operator[](int indix)const {
		return _vec[indix];
	}

	const Vector& operator=(const Vector& _Left);
	const Vector& operator=(Vector&& _Right)noexcept;

	const Vector& operator+=(const Vector& _b);
	const Vector& operator-=(const Vector& _b);

	const Vector& operator*=(const MyNum& _val);

	Vector mutiply_num(MyNum _val);

	void _set_v_neg();
	MyNum dot_mutipy(const Vector& vec)const;
};

class Solution {
protected:
	void _discard_data();
	void _allocate_space();
	//void _copy_data();
public:
	int cnt_free_var;
	int cnt_solution;
	Vector solution;
	Vector* span;
	int* free_var_id;
	bool flag_inconsistance;
	Solution();
	Solution(const Solution& _left);
	~Solution();
	void _set_varable_cnt(int cnt);
	int get_free_var_indix(int id);
	void set_free_var_number(int num);
	void print_result()const;
};

class Matrix {
protected:
	MyNum** mat;
	static void format_output_val(std::ostream& _stream, const MyNum& _val);
public:
	int width;
	int height;
	//reference to a matrix line
	class Mat_line {
	protected:
		Mat_line(int wid);
	public:
		MyNum* ptr;
		int length;
		bool flag_temp_line;
		constexpr Mat_line(MyNum* _p, int _l):ptr(_p),length(_l),flag_temp_line(false) {}
		Mat_line(const Mat_line& _r) :ptr(_r.ptr), length(_r.length) {
			flag_temp_line = false;
		}
		Mat_line(Mat_line&& _r)noexcept :ptr(_r.ptr), length(_r.length) {
			flag_temp_line = _r.flag_temp_line;
			_r.flag_temp_line = false; 
		}
		~Mat_line() {
			if (flag_temp_line)delete[] ptr;
		}
		MyNum& operator [](int indix) { return ptr[indix]; }
		const MyNum& operator[](int indix)const { return ptr[indix]; }

		Mat_line& scale(const MyNum& _val);
		Mat_line& add(const Mat_line& _val);
		Mat_line& substract(const Mat_line& _val);
		Mat_line create_temp_line();
	};
protected:
	void _allocate_space();
	void copy_data(MyNum** _src);
	void _free_space();
	int find_nozero_start_indix(int start_at, int line_indix);
	void _copy_data_higher_dimision(MyNum** _src, int ignore_row, int ignore_colum);
public:
	int seperate_index;
	Matrix(int wid, int hei);
	Matrix(const Matrix& b);
	Matrix(Matrix&& _right)noexcept;
	~Matrix();
	const Matrix& operator=(const Matrix& b);
	const Matrix& operator=(Matrix&& _right)noexcept;
	Mat_line operator [](int _line) { return Mat_line(mat[_line], width); };
	void exchange_line(int l1, int l2);
	void row_echolon_form();
	void reduced_row_echolon_form();
	void read_matrix_cin();
	void print_matrix()const;
	int nozero_entry(int _at_l,int start_indix = 0);
	Matrix inverse_mat();
	Solution analyze_result();
	Vector extract_vector(int col);
	Matrix multipy_mat(const Matrix& b)const;
	Vector multipy_vec(const Vector& _vec)const;
	Matrix multipy_val(const MyNum _val)const;
	void _set_v_neg();
	Matrix& operator+=(const Matrix& b);
	MyNum det()const;
	Matrix MinorsMatrix();
	Matrix CofactorsMatrix();
	void SetDimision(int _Width, int _Height);
	Vector ExtractRowVector(int i);
	void _assign_c_vector(const Vector& _vec, int _i);
	MyNum* lval_ref(int _i);
};

#endif // !_MY_MATRIX_H_
