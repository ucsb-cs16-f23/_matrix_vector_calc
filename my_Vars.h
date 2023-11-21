#ifndef _MY_VARS_H_
#define _MY_VARS_H_
#include<list>

#include"mymatrix.h"

class MyVar {
public:
	enum _m_type {
		number,
		vector,
		matrix,
		null_var,
		unknow_type
	};
	template<class T>
	static constexpr _m_type _get_type_id() {
		return unknow_type;
	}
	template<>
	static constexpr _m_type _get_type_id<MyNum>() {
		return number;
	}
	template<>
	static constexpr _m_type _get_type_id<Vector>() {
		return vector;
	}
	template<>
	static constexpr _m_type _get_type_id<Matrix>() {
		return matrix;
	}
	
	_m_type var_type;
	union {
		MyNum* num;
		Vector* vec;
		Matrix* mat;
		void* native_ptr;
	} my_data;
	char* var_name;
	MyVar();
	MyVar(_m_type _type, const char* name, void* ptr);
	~MyVar();

	template<class T>
	const MyVar& assign_val(const T& _val) {
		if (var_type != _get_type_id<T>()) {
			discard_data();
			var_type = _get_type_id<T>();
			allocate_space();
		}
		(reinterpret_cast<T*>(my_data.native_ptr))->operator=(_val);
		return *this;
	}
	template<>
	const MyVar& assign_val<MyVar>(const MyVar& _Val)
	{
		if (var_type != _Val.var_type) {
			discard_data();
			var_type = _Val.var_type;
			allocate_space();
		}
		copy_data(_Val.my_data.native_ptr);
		return *this;
	}

	template<class T>
	const MyVar& assign_val(T &&_val) {
		if (var_type != _get_type_id<T>()) {
			discard_data();
			var_type = _get_type_id<T>();
			allocate_space();
		}
		(reinterpret_cast<T*>(my_data.native_ptr))->operator=(::std::move(_val));
		return *this;
	}
	template<>
	const MyVar& assign_val<MyVar>(MyVar && _right) {
		discard_data();
		my_data.native_ptr = _right.my_data.native_ptr;
		_right.my_data.native_ptr = NULL;
		var_type = _right.var_type;
		_right.var_type = null_var;
		return*this;
	}
	const MyVar& op_add(const MyVar& b);
	const MyVar& op_mul(const MyVar& b);
	void SetName(const char* _name);
	MyVar(const MyVar& _left);
	MyVar(MyVar&& _right)noexcept;
	const MyVar& operator=(const MyVar& b);
	const MyVar& operator=(MyVar&& b)noexcept;
	bool operator==(const MyVar& b)const;
	void _Not();
	void set_negetive();
	MyVar _at(const MyVar& _i)const;
protected:
	void discard_data();
	void copy_data(const void* Ptr);
	void allocate_space();
};

extern ::std::list<MyVar>tmp_var_list;

MyVar* _create_tmp_var(Matrix&& _mat, const char* _name);

MyVar* REF(const Matrix& mat);

#endif // !_MY_VARS_H_