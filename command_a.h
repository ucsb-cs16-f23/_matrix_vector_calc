#ifndef _COMMAND_A_H_
#define _COMMAND_A_H_

#include<new>
#include<utility>
#include"my_Vars.h"
#include<type_traits>

class cmNode {
protected:
	cmNode* pPrev;
	cmNode* pNext;
	static char op_table[256];
	static char ret_char_buffer[128];
	constexpr static  char chBound = 3;
	constexpr static  char chOp_1 = 1;
	constexpr static  char chOP_2 = 2;
	static inline void op_set(char ind, char val) {
		op_table[ind] = val;
	}
	static int mov_to_next(char*& _str);
	static int op_length(char* str);
	cmNode(const cmNode&);
public:
	char* str;
	int length;
	enum mType
	{
		mOPERATOR,
		mIDENTIFIER,
		mVALUE,
		idVAR,
		idFUNC,
		mExprEND,
		mOPCMP,
		mOPLOG,
		idUNDEFINED
	};
	mType type;
	void clean_up();
	cmNode();
	cmNode(char* _str, int);
	~cmNode();
	void append(cmNode* nd);
	cmNode* find_back();
	cmNode* find_head();
	cmNode* replace(cmNode* _x, bool clear_this = true);

	static cmNode* create_list(char* _str);
	void print_node();

	bool is_end() {
		return pNext == nullptr;
	}
	cmNode* get_next() {
		return pNext;
	}
	cmNode* get_prev() {
		return pPrev;
	}
	bool is_head() {
		return pPrev == nullptr;
	}

	void _break_list();
	static void table_init();

	//static int word_chk(cmNode* _Node);
	//TODO
	const char* get_cstr()const;
	void insert(cmNode* _Node);
};

extern bool fCallerUSRdefFUNC;

#define STACK_PUSHPOP_CHECKLEVEL 1

extern char* _My_Stack_Poiner;
extern char* _m_stack_end;
extern char* _m_stack_begin;

template<class T>
void PUSH(const T& _val) {
	_My_Stack_Poiner -= sizeof(T);
#if STACK_PUSHPOP_CHECKLEVEL == 1
	if (_m_stack_end > _My_Stack_Poiner)throw - 1;
#endif
	new(_My_Stack_Poiner) T(_val);
}

template<class T>
void PUSH(T&& _val) {
	_My_Stack_Poiner-= sizeof(T);
#if STACK_PUSHPOP_CHECKLEVEL == 1
	if (_m_stack_end > _My_Stack_Poiner)throw - 1;
#endif
	new(_My_Stack_Poiner) T(::std::move(_val));
}

template<class T>
typename std::remove_const<typename std::remove_reference<T>::type>::type 
POP() {
	using Type = typename std::remove_const<typename std::remove_reference<T>::type>::type;
	Type ret = ::std::move(*reinterpret_cast<Type*>(_My_Stack_Poiner));
#if STACK_PUSHPOP_CHECKLEVEL == 1
	if (_My_Stack_Poiner+sizeof(Type) > _m_stack_begin)throw - 1;
#endif
	if(std::is_pointer<Type>::value == false )reinterpret_cast<Type*>(_My_Stack_Poiner)->~Type();
	_My_Stack_Poiner += sizeof(Type);
	return ret;
}

template<int _i, class Ft>
struct _At;

template<class R, class A1, class... Ar>
struct _At<0, R(A1, Ar...)>
{
	using type = A1;
};

template<int _i, class R, class A1, class... Ar>
struct _At<_i, R(A1, Ar...)> {
	using type = typename _At<_i - 1, R(Ar...)>::type;
};

template<class F>
struct _Rt;

template<class _Ret, class... Arg>
struct _Rt<_Ret(Arg...)> {
	using type = typename _Ret;
};

class FuncBase {
public:
	char* name;
	static void* ret_area;
	FuncBase();
	FuncBase(const char* _name);
	FuncBase(const FuncBase& b);
	virtual ~FuncBase() {
		delete name;
		delete[] is_ptr_list;
		delete[] m_type_list;
	}
	virtual void _call_func_ret_on_retarea()const {
	}
	virtual bool _is_va_func()const {
		return false;
	}
	int get_arguments_cnts();
	MyVar::_m_type get_arg_type(int a_at);
	bool arg_is_pointer_at(int at) {
		return is_ptr_list[at];
	}
	const char* pr_name() {
		return name;
	}
	bool f_return_pointer;
	virtual void record_stack_pointer() {
	}
protected:
	int cnt;
	MyVar::_m_type* m_type_list;
	bool* is_ptr_list;
};

template<class FuncTy>class MyFunc;

template<class _ret, class ... Args>
class MyFunc<_ret(Args...)> :public FuncBase{
protected:
	_ret(*function_pointer)(Args...);
	template<int index>
	constexpr void get_func_arg_list() {
		m_type_list[index] = MyVar::_get_type_id< 
			std::remove_const<::std::remove_reference< ::std::remove_pointer<
			_At<index, _ret(Args...)>
			::type>::type>::type>::type>();//get argument type~
		is_ptr_list[index] = ::std::is_pointer< _At<index, _ret(Args...)>::type >::value;
		get_func_arg_list<index - 1>();
	}
	template<>
	constexpr void get_func_arg_list<-1>(){
	}
public:
	MyFunc(_ret(*fptr)(Args...), const char* _name):FuncBase(_name) {
		function_pointer = fptr;
		cnt = sizeof...(Args);
		m_type_list = new MyVar::_m_type[cnt];
		is_ptr_list = new bool[cnt];
		get_func_arg_list<sizeof...(Args) - 1>();
		f_return_pointer = ::std::is_pointer<_ret>::value;
	}
	MyFunc(const MyFunc& b):FuncBase(static_cast<const FuncBase&>(b)) {
	}
	virtual ~MyFunc(){
	}
	virtual void _call_func_ret_on_retarea()const override {
		new(ret_area) _ret(function_pointer(POP<Args>()...));
	}
	virtual bool _is_va_func()const {
		return false;
	}
};

MyVar CalcExpr_R(cmNode* &_expr);

//assume expression on the left hand side of '=' is lvalue
//*before use, check ALL lvaue
MyVar CalcExpr_L(cmNode*& ptr_expr);

void FunctionClassINIT(unsigned dwStackSize, unsigned dwRetAsize);

void determine_sequence(cmNode* _expr);

template<class _Ret>
class MyFunc_VA :public FuncBase {
protected:
	_Ret(*function_pointer) (char*);
	char* _stack_ptr_before_call;
public:
	MyFunc_VA(_Ret(*Ptr)(char*), const char* _name) :FuncBase(_name) {
		function_pointer = Ptr;
		_stack_ptr_before_call = 0;
		f_return_pointer = ::std::is_pointer<_Ret>::value;
	}
	MyFunc_VA(const MyFunc_VA& b):FuncBase(static_cast<const FuncBase&>(b)) {
		function_pointer = b.function_pointer;
		_stack_ptr_before_call = b._stack_ptr_before_call;
		f_return_pointer = b.f_return_pointer;
	}
	virtual ~MyFunc_VA() {
	}
	virtual bool _is_va_func()const {
		return true;
	}
	virtual void _call_func_ret_on_retarea()const override {
		new (ret_area) _Ret(function_pointer(_stack_ptr_before_call));
		_My_Stack_Poiner = _stack_ptr_before_call;
	}
	virtual void record_stack_pointer()override {
		_stack_ptr_before_call = _My_Stack_Poiner;
	}
};

#define MYFUNC(_FUNC_NAME_, _NAME_STR_) MyFunc<decltype(_FUNC_NAME_)>\
(_FUNC_NAME_, _NAME_STR_)

#define MYVA_FUNC(_FUNC_, _STR_NAME_) MyFunc_VA<\
typename _Rt<decltype(_FUNC_)>::type>(&_FUNC_,  _STR_NAME_)

inline bool is_VA_end(char* _va_list) {
	return _va_list == _My_Stack_Poiner;
}

inline void _VA_start(char*& _va_list, char* _a1) {
	_va_list = _a1;
}

template<class T>
inline T& _VA_arg(char*& _va_list) {
	_va_list -= sizeof(T);
	return *reinterpret_cast<T*>(_va_list);
}

MyVar CreateVector(char*  _A1);

void procCommand_1(char* _Str_command);

void implFuncInit();

MyVar read_matrix(Matrix* mat);

void _clean_data_before_quit();

#endif // !_COMMAND_A_H_
